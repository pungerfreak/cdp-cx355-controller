#include "SLinkDebugPrinter.h"

namespace {
void printHexByte(uint8_t b, Stream& out) {
  if (b < 16) out.print('0');
  out.print(b, HEX);
}

void printHex(const uint8_t* data, uint16_t len, Stream& out) {
  for (uint16_t i = 0; i < len; i++) {
    printHexByte(data[i], out);
    if (i + 1 < len) out.print(' ');
  }
}

void printAscii(const uint8_t* data, uint16_t len, Stream& out) {
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

void printChecksumHints(const SLinkMessage& msg, Stream& out) {
  if (!msg.hasChecksum) return;

  out.print(" chk_last=0x");
  printHexByte(msg.checksumLast, out);
  out.print(" sum=0x");
  printHexByte(msg.checksumSum, out);
  out.print(" xor=0x");
  printHexByte(msg.checksumXor, out);

  if (msg.checksumSumMatch || msg.checksumSumInvMatch ||
      msg.checksumXorMatch || msg.checksumXorInvMatch) {
    out.print(" chk=");
    bool first = true;
    if (msg.checksumSumMatch) {
      out.print("sum");
      first = false;
    }
    if (msg.checksumSumInvMatch) {
      if (!first) out.print('/');
      out.print("~sum");
      first = false;
    }
    if (msg.checksumXorMatch) {
      if (!first) out.print('/');
      out.print("xor");
      first = false;
    }
    if (msg.checksumXorInvMatch) {
      if (!first) out.print('/');
      out.print("~xor");
    }
  }
}
}  // namespace

void SLinkDebugPrinter::print(const SLinkMessage& msg, Stream& out) const {
  if (!msg.len) return;

  out.print("raw=");
  printHex(msg.raw, msg.len, out);
  out.print(" | msg=");
  out.print(msg.name ? msg.name : kSLinkUnknownName);
  out.print(" len=");
  out.print(msg.len);

  out.print(" b0=0x");
  printHexByte(msg.unit, out);
  if (msg.len >= 2) {
    out.print(" b1=0x");
    printHexByte(msg.cmd, out);
  }

  if (msg.hasDisc) {
    out.print(" disc_raw=0x");
    printHexByte(msg.discRaw, out);
    out.print(" disc=");
    if (msg.discValid) out.print(msg.disc);
    else out.print('?');
  }

  if (msg.hasTrack) {
    out.print(" track_raw=0x");
    printHexByte(msg.trackRaw, out);
    out.print(" track=");
    if (msg.trackValid) out.print(msg.track);
    else out.print('?');
  }

  if (msg.hasTrackLength) {
    out.print(" length_raw=0x");
    printHexByte(msg.trackMinRaw, out);
    out.print(" 0x");
    printHexByte(msg.trackSecRaw, out);
    out.print(" length=");
    if (msg.trackMinValid && msg.trackSecValid) {
      out.print(msg.trackMin);
      out.print(':');
      if (msg.trackSec < 10) out.print('0');
      out.print(msg.trackSec);
    } else {
      out.print('?');
    }
  }

  if (msg.extraLen > 0) {
    out.print(" u0=0x");
    printHexByte(msg.extra[0], out);
    if (msg.extraLen > 1) {
      out.print(" u1=0x");
      printHexByte(msg.extra[1], out);
    }
  }

  if (msg.len > 2) {
    out.print(" data=");
    printHex(msg.raw + 2, msg.len - 2, out);
    printAscii(msg.raw + 2, msg.len - 2, out);
  }

  printChecksumHints(msg, out);

  if (msg.note) {
    out.print(" note=");
    out.print(msg.note);
  }
  if (msg.truncated) {
    out.print(" trunc=1");
  }

  out.println();
}
