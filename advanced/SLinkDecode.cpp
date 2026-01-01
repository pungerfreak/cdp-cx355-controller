#include "SLinkDecode.h"

namespace {
const SLinkPattern kPatterns[] = {
  {"PLAY",          "cmd", 2, 0, 2, {0x00, 0x00}, {0x00, 0xFF}},
  {"STOP",          "cmd", 2, 0, 2, {0x00, 0x01}, {0x00, 0xFF}},
  {"PAUSE",         "cmd", 2, 0, 2, {0x00, 0x02}, {0x00, 0xFF}},
  {"CHANGING_DISC", "cmd", 2, 0, 2, {0x00, 0x06}, {0x00, 0xFF}},
  {"POWER_ON",      "cmd", 2, 0, 2, {0x00, 0x2E}, {0x00, 0xFF}},
  {"POWER_OFF",     "cmd", 2, 0, 2, {0x00, 0x2F}, {0x00, 0xFF}},
  {"READY",         "cmd", 2, 0, 2, {0x00, 0x08}, {0x00, 0xFF}},
  {"DISC_READY",    "cmd", 3, 0, 2, {0x00, 0x52}, {0x00, 0xFF}},
  {"LOADING_DISC",  "cmd", 3, 0, 2, {0x00, 0x54}, {0x00, 0xFF}},
  {"DISC_LOADED",   "cmd", 3, 0, 2, {0x00, 0x58}, {0x00, 0xFF}},
  // Add known codes here as you confirm them from the S-Link spec.
  // {"Example Name", "Optional note", minLen, maxLen, prefixLen,
  //  {0x01, 0x02}, {0xFF, 0xFF}},
  {nullptr, nullptr, 0, 0, 0, {0}, {0}},
};

constexpr uint16_t kPatternCount = sizeof(kPatterns) / sizeof(kPatterns[0]);

bool isDiscCommand(uint8_t cmd) {
  return (cmd == 0x52 || cmd == 0x54 || cmd == 0x58);
}

bool decodeDiscNumber(uint8_t raw, uint8_t unit, uint16_t& disc) {
  disc = 0;
  if (unit == 0x98) {
    if (raw >= 0x37 && raw <= 0xFE) {
      disc = (uint16_t)raw - 0x36;
      return true;
    }
    return false;
  }
  if (unit == 0x9B) {
    if (raw >= 0x01 && raw <= 0x64) {
      disc = (uint16_t)raw + 200;
      return true;
    }
    return false;
  }
  return false;
}
}  // namespace

const SLinkPattern* SLinkTranslator::matchPattern(const uint8_t* data, uint16_t len) const {
  if (!data || !len) return nullptr;
  for (uint16_t i = 0; i < kPatternCount; i++) {
    const SLinkPattern& pat = kPatterns[i];
    if (!pat.name || !pat.name[0]) continue;
    if (len < pat.minLen) continue;
    if (pat.maxLen && len > pat.maxLen) continue;
    if (pat.prefixLen > len || pat.prefixLen > SLinkPattern::PREFIX_MAX) continue;

    bool match = true;
    for (uint8_t j = 0; j < pat.prefixLen; j++) {
      const uint8_t mask = pat.mask[j];
      if ((data[j] & mask) != (pat.prefix[j] & mask)) {
        match = false;
        break;
      }
    }
    if (match) return &pat;
  }
  return nullptr;
}

void SLinkTranslator::printHexByte(uint8_t b, Stream& out) const {
  if (b < 16) out.print('0');
  out.print(b, HEX);
}

void SLinkTranslator::printHex(const uint8_t* data, uint16_t len, Stream& out) const {
  for (uint16_t i = 0; i < len; i++) {
    printHexByte(data[i], out);
    if (i + 1 < len) out.print(' ');
  }
}

void SLinkTranslator::printAscii(const uint8_t* data, uint16_t len, Stream& out) const {
  bool any = false;
  for (uint16_t i = 0; i < len; i++) {
    if (data[i] >= 32 && data[i] <= 126) {
      any = true;
      break;
    }
  }
  if (!any) return;

  out.print(" ascii=\"");
  for (uint16_t i = 0; i < len; i++) {
    const uint8_t b = data[i];
    out.write((b >= 32 && b <= 126) ? (char)b : '.');
  }
  out.print('"');
}

void SLinkTranslator::printChecksumHints(const uint8_t* data, uint16_t len, Stream& out) const {
  if (len < 2) return;

  uint8_t sum = 0;
  uint8_t xr = 0;
  for (uint16_t i = 0; i + 1 < len; i++) {
    sum = (uint8_t)(sum + data[i]);
    xr ^= data[i];
  }

  const uint8_t last = data[len - 1];
  const bool sumMatch = (last == sum);
  const bool sumInvMatch = (last == (uint8_t)~sum);
  const bool xorMatch = (last == xr);
  const bool xorInvMatch = (last == (uint8_t)~xr);

  out.print(" chk_last=0x");
  printHexByte(last, out);
  out.print(" sum=0x");
  printHexByte(sum, out);
  out.print(" xor=0x");
  printHexByte(xr, out);

  if (sumMatch || sumInvMatch || xorMatch || xorInvMatch) {
    out.print(" chk=");
    bool first = true;
    if (sumMatch) {
      out.print("sum");
      first = false;
    }
    if (sumInvMatch) {
      if (!first) out.print('/');
      out.print("~sum");
      first = false;
    }
    if (xorMatch) {
      if (!first) out.print('/');
      out.print("xor");
      first = false;
    }
    if (xorInvMatch) {
      if (!first) out.print('/');
      out.print("~xor");
    }
  }
}

void SLinkTranslator::printMessage(const uint8_t* data, uint16_t len, Stream& out) const {
  if (!data || !len) return;
  if (data[0] >= 0x90 && data[0] <= 0x92) return;

  const SLinkPattern* pat = matchPattern(data, len);

  out.print("raw=");
  printHex(data, len, out);
  out.print(" | msg=");
  out.print(pat ? pat->name : "UNKNOWN");
  out.print(" len=");
  out.print(len);

  out.print(" b0=0x");
  printHexByte(data[0], out);
  if (len >= 2) {
    out.print(" b1=0x");
    printHexByte(data[1], out);
  }
  if (len >= 3 && isDiscCommand(data[1])) {
    const uint8_t rawDisc = data[2];
    out.print(" disc_raw=0x");
    printHexByte(rawDisc, out);
    uint16_t disc = 0;
    if (decodeDiscNumber(rawDisc, data[0], disc)) {
      out.print(" disc=");
      out.print(disc);
    } else {
      out.print(" disc=?");
    }
  }
  if (len > 2) {
    out.print(" data=");
    printHex(data + 2, len - 2, out);
    printAscii(data + 2, len - 2, out);
  }

  printChecksumHints(data, len, out);

  if (pat && pat->note && pat->note[0]) {
    out.print(" note=");
    out.print(pat->note);
  }

  out.println();
}
