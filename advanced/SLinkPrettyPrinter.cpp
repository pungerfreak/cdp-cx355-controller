#include "SLinkPrettyPrinter.h"

namespace {
void printHexByte(uint8_t b, Stream& out) {
  if (b < 16) out.print('0');
  out.print(b, HEX);
}
}  // namespace

void SLinkPrettyPrinter::print(const SLinkMessage& msg, Stream& out) const {
  if (!msg.len) return;
  if (!msg.name || msg.name == kSLinkUnknownName) return;

  out.print(msg.name);

  if (msg.hasDisc) {
    if (msg.discValid) {
      out.print(" disc=");
      out.print(msg.disc);
    } else {
      out.print(" disc=? disc_raw=0x");
      printHexByte(msg.discRaw, out);
    }
  }

  if (msg.hasTrack) {
    if (msg.trackValid) {
      out.print(" track=");
      out.print(msg.track);
    } else {
      out.print(" track=? track_raw=0x");
      printHexByte(msg.trackRaw, out);
    }
  }

  if (msg.hasTrackLength) {
    if (msg.trackMinValid && msg.trackSecValid) {
      out.print(" length=");
      out.print(msg.trackMin);
      out.print(':');
      if (msg.trackSec < 10) out.print('0');
      out.print(msg.trackSec);
    } else {
      out.print(" length=? length_raw=0x");
      printHexByte(msg.trackMinRaw, out);
      out.print(" 0x");
      printHexByte(msg.trackSecRaw, out);
    }
  }

  out.println();
}
