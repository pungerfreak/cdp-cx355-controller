#include "SLinkDecode.h"

const char kSLinkUnknownName[] = "UNKNOWN";

namespace {
const SLinkPattern kPatterns[] = {
  {"PLAY",          "cmd", 2, 0, 2, {0x00, 0x00}, {0x00, 0xFF}},
  {"STOP",          "cmd", 2, 0, 2, {0x00, 0x01}, {0x00, 0xFF}},
  {"PAUSE",         "cmd", 2, 0, 2, {0x00, 0x02}, {0x00, 0xFF}},
  {"CHANGING_DISC", "cmd", 2, 0, 2, {0x00, 0x06}, {0x00, 0xFF}},
  {"POWER_ON",      "cmd", 2, 0, 2, {0x00, 0x2E}, {0x00, 0xFF}},
  {"POWER_OFF",     "cmd", 2, 0, 2, {0x00, 0x2F}, {0x00, 0xFF}},
  {"READY",         "cmd", 2, 0, 2, {0x00, 0x08}, {0x00, 0xFF}},
  {"CHANGING_TRACK", "cmd", 6, 0, 2, {0x00, 0x50}, {0x00, 0xFF}},
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
  return (cmd == 0x50 || cmd == 0x52 || cmd == 0x54 || cmd == 0x58);
}

bool isDiscTrackCommand(uint8_t cmd) {
  return (cmd == 0x50);
}

bool decodeBcd(uint8_t raw, uint8_t& value);

bool decodeDiscNumber(uint8_t raw, uint8_t unit, uint16_t& disc) {
  disc = 0;
  if (unit == 0x90 || unit == 0x98) {
    uint8_t bcd = 0;
    if (decodeBcd(raw, bcd) && bcd >= 1 && bcd <= 99) {
      disc = bcd;
      return true;
    }
    if (raw >= 0x9A && raw <= 0xFE) {
      disc = (uint16_t)raw - 0x36;
      return true;
    }
    return false;
  }
  if (unit == 0x9B || unit == 0x9C) {
    if (raw >= 0x01 && raw <= 0x64) {
      disc = (uint16_t)raw + 200;
      return true;
    }
    return false;
  }
  return false;
}

bool decodeBcd(uint8_t raw, uint8_t& value) {
  const uint8_t hi = (raw >> 4) & 0x0F;
  const uint8_t lo = raw & 0x0F;
  if (hi > 9 || lo > 9) return false;
  value = (uint8_t)(hi * 10 + lo);
  return true;
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

bool SLinkTranslator::decode(const uint8_t* data, uint16_t len, SLinkMessage& out,
                             bool allowController) const {
  if (!data || !len) return false;
  if (!allowController && data[0] >= 0x90 && data[0] <= 0x92) return false;

  memset(&out, 0, sizeof(out));
  const uint16_t copyLen = (len > SLinkMessage::RAW_MAX) ? SLinkMessage::RAW_MAX : len;
  memcpy(out.raw, data, copyLen);
  out.len = copyLen;
  out.truncated = (len > copyLen);
  out.unit = out.raw[0];
  if (out.len >= 2) out.cmd = out.raw[1];

  const SLinkPattern* pat = matchPattern(out.raw, out.len);
  out.name = pat ? pat->name : kSLinkUnknownName;
  out.note = (pat && pat->note && pat->note[0]) ? pat->note : nullptr;

  if (out.len >= 3 && isDiscCommand(out.cmd)) {
    out.hasDisc = true;
    out.discRaw = out.raw[2];
    uint16_t disc = 0;
    out.discValid = decodeDiscNumber(out.discRaw, out.unit, disc);
    if (out.discValid) out.disc = disc;
  }

  if (out.len >= 4 && isDiscTrackCommand(out.cmd)) {
    out.hasTrack = true;
    out.trackRaw = out.raw[3];
    uint8_t track = 0;
    out.trackValid = decodeBcd(out.trackRaw, track);
    if (out.trackValid) out.track = track;

    if (out.len >= 6) {
      out.hasTrackLength = true;
      out.trackMinRaw = out.raw[4];
      out.trackSecRaw = out.raw[5];
      uint8_t minutes = 0;
      uint8_t seconds = 0;
      out.trackMinValid = decodeBcd(out.trackMinRaw, minutes);
      out.trackSecValid = decodeBcd(out.trackSecRaw, seconds);
      if (out.trackMinValid) out.trackMin = minutes;
      if (out.trackSecValid) out.trackSec = seconds;
    }
    if (out.len > 6) {
      uint16_t extraLen = out.len - 6;
      if (extraLen > 2) extraLen = 2;
      out.extraLen = (uint8_t)extraLen;
      out.extra[0] = out.raw[6];
      if (out.extraLen > 1) out.extra[1] = out.raw[7];
    }
  }

  if (out.len >= 2) {
    out.hasChecksum = true;
    uint8_t sum = 0;
    uint8_t xr = 0;
    for (uint16_t i = 0; i + 1 < out.len; i++) {
      sum = (uint8_t)(sum + out.raw[i]);
      xr ^= out.raw[i];
    }
    out.checksumLast = out.raw[out.len - 1];
    out.checksumSum = sum;
    out.checksumXor = xr;
    out.checksumSumMatch = (out.checksumLast == sum);
    out.checksumSumInvMatch = (out.checksumLast == (uint8_t)~sum);
    out.checksumXorMatch = (out.checksumLast == xr);
    out.checksumXorInvMatch = (out.checksumLast == (uint8_t)~xr);
  }

  return true;
}
