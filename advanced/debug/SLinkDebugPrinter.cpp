#include "debug/SLinkDebugPrinter.h"

namespace {
void printDebugHexByte(uint8_t b, Stream& out) {
  if (b < 16) out.print('0');
  out.print(b, HEX);
}

void printHex(const uint8_t* data, uint16_t len, Stream& out) {
  for (uint16_t i = 0; i < len; i++) {
    printDebugHexByte(data[i], out);
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
}  // namespace

SLinkDebugPrinter::SLinkDebugPrinter(Stream& out) : _out(out) {}

void SLinkDebugPrinter::printDisc(const SLinkDiscInfo& disc) {
  if (!disc.present) return;
  _out.print(" disc_raw=0x");
  printDebugHexByte(disc.raw, _out);
  _out.print(" disc=");
  if (disc.valid) _out.print(disc.disc);
  else _out.print('?');
}

void SLinkDebugPrinter::printTrack(const SLinkTrackInfo& track) {
  if (!track.present) return;
  _out.print(" track_raw=0x");
  printDebugHexByte(track.raw, _out);
  _out.print(" track=");
  if (track.valid) _out.print(track.track);
  else _out.print('?');
}

void SLinkDebugPrinter::printLength(const SLinkTrackInfo& track) {
  if (!track.lengthPresent) return;
  _out.print(" length_raw=0x");
  printDebugHexByte(track.minRaw, _out);
  _out.print(" 0x");
  printDebugHexByte(track.secRaw, _out);
  _out.print(" length=");
  if (track.lengthValid) {
    _out.print(track.minutes);
    _out.print(':');
    if (track.seconds < 10) _out.print('0');
    _out.print(track.seconds);
  } else {
    _out.print('?');
  }
}

void SLinkDebugPrinter::printData(const SLinkDebugInfo& debug) {
  if (!debug.raw || debug.len <= 2) return;
  _out.print(" data=");
  printHex(debug.raw + 2, debug.len - 2, _out);
  printAscii(debug.raw + 2, debug.len - 2, _out);
}

void SLinkDebugPrinter::printChecksumHints(const SLinkDebugInfo& debug) {
  if (!debug.hasChecksum) return;

  _out.print(" chk_last=0x");
  printDebugHexByte(debug.checksumLast, _out);
  _out.print(" sum=0x");
  printDebugHexByte(debug.checksumSum, _out);
  _out.print(" xor=0x");
  printDebugHexByte(debug.checksumXor, _out);

  if (debug.checksumSumMatch || debug.checksumSumInvMatch ||
      debug.checksumXorMatch || debug.checksumXorInvMatch) {
    _out.print(" chk=");
    bool first = true;
    if (debug.checksumSumMatch) {
      _out.print("sum");
      first = false;
    }
    if (debug.checksumSumInvMatch) {
      if (!first) _out.print('/');
      _out.print("~sum");
      first = false;
    }
    if (debug.checksumXorMatch) {
      if (!first) _out.print('/');
      _out.print("xor");
      first = false;
    }
    if (debug.checksumXorInvMatch) {
      if (!first) _out.print('/');
      _out.print("~xor");
    }
  }
}

void SLinkDebugPrinter::printLine(const char* name,
                                 const SLinkDiscInfo* disc,
                                 const SLinkTrackInfo* track,
                                 const SLinkDebugInfo* debug) {
  if (!debug || !debug->raw || !debug->len) {
    _out.print("msg=");
    _out.println(name);
    return;
  }

  _out.print("raw=");
  printHex(debug->raw, debug->len, _out);
  _out.print(" | msg=");
  _out.print(name);
  _out.print(" len=");
  _out.print(debug->len);

  _out.print(" b0=0x");
  printDebugHexByte(debug->unit, _out);
  if (debug->len >= 2) {
    _out.print(" b1=0x");
    printDebugHexByte(debug->cmd, _out);
  }

  if (disc) printDisc(*disc);
  if (track) {
    printTrack(*track);
    printLength(*track);
  }

  printData(*debug);
  printChecksumHints(*debug);
  if (debug->truncated) _out.print(" trunc=1");
  _out.println();
}

void SLinkDebugPrinter::play(const SLinkDebugInfo* debug) {
  printLine("PLAY", nullptr, nullptr, debug);
}

void SLinkDebugPrinter::stop(const SLinkDebugInfo* debug) {
  printLine("STOP", nullptr, nullptr, debug);
}

void SLinkDebugPrinter::pause(const SLinkDebugInfo* debug) {
  printLine("PAUSE", nullptr, nullptr, debug);
}

void SLinkDebugPrinter::powerOn(const SLinkDebugInfo* debug) {
  printLine("POWER_ON", nullptr, nullptr, debug);
}

void SLinkDebugPrinter::powerOff(const SLinkDebugInfo* debug) {
  printLine("POWER_OFF", nullptr, nullptr, debug);
}

void SLinkDebugPrinter::changeDisc(const SLinkDiscInfo& disc,
                                   const SLinkDebugInfo* debug) {
  printLine("CHANGE_DISC", &disc, nullptr, debug);
}

void SLinkDebugPrinter::changeTrack(const SLinkDiscInfo& disc,
                                    const SLinkTrackInfo& track,
                                    const SLinkDebugInfo* debug) {
  printLine("CHANGE_TRACK", &disc, &track, debug);
}

void SLinkDebugPrinter::ready(const SLinkDebugInfo* debug) {
  printLine("READY", nullptr, nullptr, debug);
}

void SLinkDebugPrinter::changingDisc(const SLinkDebugInfo* debug) {
  printLine("CHANGING_DISC", nullptr, nullptr, debug);
}

void SLinkDebugPrinter::discReady(const SLinkDiscInfo& disc,
                                  const SLinkDebugInfo* debug) {
  printLine("DISC_READY", &disc, nullptr, debug);
}

void SLinkDebugPrinter::discLoaded(const SLinkDiscInfo& disc,
                                   const SLinkDebugInfo* debug) {
  printLine("DISC_LOADED", &disc, nullptr, debug);
}

void SLinkDebugPrinter::loadingDisc(const SLinkDiscInfo& disc,
                                    const SLinkDebugInfo* debug) {
  printLine("LOADING_DISC", &disc, nullptr, debug);
}

void SLinkDebugPrinter::changingTrack(const SLinkDiscInfo& disc,
                                      const SLinkTrackInfo& track,
                                      const SLinkDebugInfo* debug) {
  printLine("CHANGING_TRACK", &disc, &track, debug);
}

void SLinkDebugPrinter::currentDiscInfo(const SLinkDiscInfo& disc,
                                        const SLinkTrackInfo& track,
                                        const SLinkDebugInfo* debug) {
  printLine("CURRENT_DISC_INFO", &disc, &track, debug);
}

void SLinkDebugPrinter::currentDiscBankSwitchNeeded(const SLinkDebugInfo* debug) {
  printLine("CURRENT_DISC_BANK_B", nullptr, nullptr, debug);
}

void SLinkDebugPrinter::status(const SLinkDiscInfo& disc,
                               const SLinkTrackInfo& track,
                               SLinkTransportState transport,
                               const SLinkDebugInfo* debug) {
  printLine("STATUS", &disc, &track, debug);
  _out.print("STATUS transport=");
  switch (transport) {
    case SLinkTransportState::Playing:
      _out.print("PLAYING");
      break;
    case SLinkTransportState::Paused:
      _out.print("PAUSED");
      break;
    case SLinkTransportState::Stopped:
      _out.print("STOPPED");
      break;
    case SLinkTransportState::PowerOff:
      _out.print("POWER_OFF");
      break;
    case SLinkTransportState::Unknown:
    case SLinkTransportState::Unchanged:
    default:
      _out.print("UNKNOWN");
      break;
  }
  _out.println();
}

void SLinkDebugPrinter::unknown(const SLinkDebugInfo* debug) {
  printLine("UNKNOWN", nullptr, nullptr, debug);
}
