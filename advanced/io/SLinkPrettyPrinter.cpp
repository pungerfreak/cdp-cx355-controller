#include "io/SLinkPrettyPrinter.h"

namespace {
void printPrettyHexByte(uint8_t b, Stream& out) {
  if (b < 16) out.print('0');
  out.print(b, HEX);
}
}  // namespace

SLinkPrettyPrinter::SLinkPrettyPrinter(Stream& out) : _out(out) {}

void SLinkPrettyPrinter::printDisc(const SLinkDiscInfo& disc) {
  if (!disc.present) return;
  if (disc.valid) {
    _out.print(" disc=");
    _out.print(disc.disc);
  } else {
    _out.print(" disc=? disc_raw=0x");
    printPrettyHexByte(disc.raw, _out);
  }
}

void SLinkPrettyPrinter::printTrack(const SLinkTrackInfo& track) {
  if (!track.present) return;
  if (track.valid) {
    _out.print(" track=");
    _out.print(track.track);
  } else {
    _out.print(" track=? track_raw=0x");
    printPrettyHexByte(track.raw, _out);
  }
}

void SLinkPrettyPrinter::printLength(const SLinkTrackInfo& track) {
  if (!track.lengthPresent) return;
  if (track.lengthValid) {
    _out.print(" length=");
    _out.print(track.minutes);
    _out.print(':');
    if (track.seconds < 10) _out.print('0');
    _out.print(track.seconds);
  } else {
    _out.print(" length=? length_raw=0x");
    printPrettyHexByte(track.minRaw, _out);
    _out.print(" 0x");
    printPrettyHexByte(track.secRaw, _out);
  }
}

void SLinkPrettyPrinter::play(const SLinkDebugInfo* debug) {
  (void)debug;
  _out.println("PLAY");
}

void SLinkPrettyPrinter::stop(const SLinkDebugInfo* debug) {
  (void)debug;
  _out.println("STOP");
}

void SLinkPrettyPrinter::pause(const SLinkDebugInfo* debug) {
  (void)debug;
  _out.println("PAUSE");
}

void SLinkPrettyPrinter::powerOn(const SLinkDebugInfo* debug) {
  (void)debug;
  _out.println("POWER_ON");
}

void SLinkPrettyPrinter::powerOff(const SLinkDebugInfo* debug) {
  (void)debug;
  _out.println("POWER_OFF");
}

void SLinkPrettyPrinter::changeDisc(const SLinkDiscInfo& disc,
                                    const SLinkDebugInfo* debug) {
  (void)debug;
  _out.print("CHANGE_DISC");
  printDisc(disc);
  _out.println();
}

void SLinkPrettyPrinter::changeTrack(const SLinkDiscInfo& disc,
                                     const SLinkTrackInfo& track,
                                     const SLinkDebugInfo* debug) {
  (void)debug;
  _out.print("CHANGE_TRACK");
  printDisc(disc);
  printTrack(track);
  _out.println();
}

void SLinkPrettyPrinter::ready(const SLinkDebugInfo* debug) {
  (void)debug;
  _out.println("READY");
}

void SLinkPrettyPrinter::changingDisc(const SLinkDebugInfo* debug) {
  (void)debug;
  _out.println("CHANGING_DISC");
}

void SLinkPrettyPrinter::discReady(const SLinkDiscInfo& disc,
                                   const SLinkDebugInfo* debug) {
  (void)debug;
  _out.print("DISC_READY");
  printDisc(disc);
  _out.println();
}

void SLinkPrettyPrinter::discLoaded(const SLinkDiscInfo& disc,
                                    const SLinkDebugInfo* debug) {
  (void)debug;
  _out.print("DISC_LOADED");
  printDisc(disc);
  _out.println();
}

void SLinkPrettyPrinter::loadingDisc(const SLinkDiscInfo& disc,
                                     const SLinkDebugInfo* debug) {
  (void)debug;
  _out.print("LOADING_DISC");
  printDisc(disc);
  _out.println();
}

void SLinkPrettyPrinter::changingTrack(const SLinkDiscInfo& disc,
                                       const SLinkTrackInfo& track,
                                       const SLinkDebugInfo* debug) {
  (void)debug;
  _out.print("CHANGING_TRACK");
  printDisc(disc);
  printTrack(track);
  printLength(track);
  _out.println();
}

void SLinkPrettyPrinter::currentDiscInfo(const SLinkDiscInfo& disc,
                                         const SLinkTrackInfo& track,
                                         const SLinkDebugInfo* debug) {
  (void)debug;
  _out.print("CURRENT_DISC_INFO");
  printDisc(disc);
  printTrack(track);
  printLength(track);
  _out.println();
}

void SLinkPrettyPrinter::currentDiscBankSwitchNeeded(const SLinkDebugInfo* debug) {
  (void)debug;
  _out.println("CURRENT_DISC_BANK_B");
}

void SLinkPrettyPrinter::unknown(const SLinkDebugInfo* debug) {
  (void)debug;
}
