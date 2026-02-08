#include "io/SLinkPrettyPrinter.h"

#include "command/SLinkCommands.h"

namespace {
void printPrettyHexByte(uint8_t b, Stream& out) {
  if (b < 16) out.print('0');
  out.print(b, HEX);
}
}  // namespace

SLinkPrettyPrinter::SLinkPrettyPrinter(Stream& out) : _out(out) {}

void SLinkPrettyPrinter::onUnitEvent(const SLinkUnitEvent& event) {
  switch (event.type) {
    case SLinkUnitEventType::TransportStateChanged:
      printTransportEvent(event.transport, event.debug);
      break;
    case SLinkUnitEventType::DiscChanged:
    case SLinkUnitEventType::Ready:
    case SLinkUnitEventType::LoadingDisc:
    case SLinkUnitEventType::NoDisc:
      printDiscEvent(event);
      break;
    case SLinkUnitEventType::TrackChanged:
      printTrackEvent(event);
      break;
    case SLinkUnitEventType::CurrentDiscInfo:
      printCurrentDiscInfo(event.disc, event.track);
      break;
    case SLinkUnitEventType::CurrentDiscBankSwitchNeeded:
      _out.println("CURRENT_DISC_BANK_B");
      break;
    case SLinkUnitEventType::Status:
      printStatus(event.disc, event.track, event.transport);
      break;
    case SLinkUnitEventType::Unknown:
    default:
      break;
  }
}

void SLinkPrettyPrinter::printTransportEvent(SLinkTransportState transport,
                                             const SLinkDebugInfo* debug) {
  const uint8_t cmd = debug ? debug->cmd : 0xFF;
  switch (transport) {
    case SLinkTransportState::Playing:
      _out.println("PLAY");
      break;
    case SLinkTransportState::Paused:
      _out.println("PAUSE");
      break;
    case SLinkTransportState::Stopped:
      if (cmd == static_cast<uint8_t>(SLinkCommandId::POWER_ON)) {
        _out.println("POWER_ON");
      } else {
        _out.println("STOP");
      }
      break;
    case SLinkTransportState::PowerOff:
      _out.println("POWER_OFF");
      break;
    case SLinkTransportState::Unknown:
    case SLinkTransportState::Unchanged:
    default:
      break;
  }
}

void SLinkPrettyPrinter::printDiscEvent(const SLinkUnitEvent& event) {
  const SLinkDebugInfo* debug = event.debug;
  const uint8_t cmd = debug ? debug->cmd : 0;
  const char* label = "CHANGE_DISC";
  switch (cmd) {
    case 0x06:
      label = "CHANGING_DISC";
      break;
    case 0x08:
      label = "READY";
      break;
    case 0x52:
      label = "DISC_READY";
      break;
    case 0x05:
      label = "NO_DISC";
      break;
    case 0x54:
      label = "LOADING_DISC";
      break;
    case 0x58:
      label = "DISC_LOADED";
      break;
    default:
      break;
  }
  _out.print(label);
  printDisc(event.disc);
  _out.println();
}

void SLinkPrettyPrinter::printTrackEvent(const SLinkUnitEvent& event) {
  const bool hasLength = event.track.lengthPresent;
  const bool looksChanging = (event.debug && event.debug->len >= 6) || hasLength;
  const char* label = looksChanging ? "CHANGING_TRACK" : "CHANGE_TRACK";
  _out.print(label);
  printDisc(event.disc);
  printTrack(event.track);
  if (looksChanging) {
    printLength(event.track);
  }
  _out.println();
}

void SLinkPrettyPrinter::printCurrentDiscInfo(const SLinkDiscInfo& disc,
                                              const SLinkTrackInfo& track) {
  _out.print("CURRENT_DISC_INFO");
  printDisc(disc);
  printTrack(track);
  printLength(track);
  _out.println();
}

void SLinkPrettyPrinter::printStatus(const SLinkDiscInfo& disc,
                                     const SLinkTrackInfo& track,
                                     SLinkTransportState transport) {
  _out.print("STATUS");
  _out.print(" state=");
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
  printDisc(disc);
  printTrack(track);
  _out.println();
}

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
