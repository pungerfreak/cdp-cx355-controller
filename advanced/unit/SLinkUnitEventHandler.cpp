#include "unit/SLinkUnitEventHandler.h"
#include <string.h>

namespace {
bool nameIs(const char* name, const char* target) {
  if (!name || !target) return false;
  return strcmp(name, target) == 0;
}

SLinkDiscInfo makeDiscInfo(const SLinkMessage& msg) {
  SLinkDiscInfo disc;
  disc.present = msg.hasDisc;
  disc.valid = msg.discValid;
  disc.disc = msg.disc;
  disc.raw = msg.discRaw;
  return disc;
}

SLinkTrackInfo makeTrackInfo(const SLinkMessage& msg) {
  SLinkTrackInfo track;
  track.present = msg.hasTrack;
  track.valid = msg.trackValid;
  track.track = msg.track;
  track.raw = msg.trackRaw;
  track.lengthPresent = msg.hasTrackLength;
  track.lengthValid = (msg.trackMinValid && msg.trackSecValid);
  track.minutes = msg.trackMin;
  track.seconds = msg.trackSec;
  track.minRaw = msg.trackMinRaw;
  track.secRaw = msg.trackSecRaw;
  return track;
}
}  // namespace

SLinkDebugInfo SLinkDispatcher::buildDebugInfo(const SLinkMessage& msg) {
  SLinkDebugInfo info;
  info.raw = msg.raw;
  info.len = msg.len;
  info.truncated = msg.truncated;
  info.unit = msg.unit;
  info.cmd = msg.cmd;
  info.hasChecksum = msg.hasChecksum;
  info.checksumLast = msg.checksumLast;
  info.checksumSum = msg.checksumSum;
  info.checksumXor = msg.checksumXor;
  info.checksumSumMatch = msg.checksumSumMatch;
  info.checksumSumInvMatch = msg.checksumSumInvMatch;
  info.checksumXorMatch = msg.checksumXorMatch;
  info.checksumXorInvMatch = msg.checksumXorInvMatch;
  return info;
}

void SLinkDispatcher::dispatch(const SLinkMessage& msg,
                               SLinkUnitEventHandler& iface,
                               const SLinkDebugInfo* debug) {
  if (!msg.len) return;

  if (!msg.name || nameIs(msg.name, kSLinkUnknownName)) {
    iface.unknown(debug);
    return;
  }

  if (nameIs(msg.name, "PLAY")) {
    iface.play(debug);
    return;
  }
  if (nameIs(msg.name, "STOP")) {
    iface.stop(debug);
    return;
  }
  if (nameIs(msg.name, "PAUSE")) {
    iface.pause(debug);
    return;
  }
  if (nameIs(msg.name, "POWER_ON")) {
    iface.powerOn(debug);
    return;
  }
  if (nameIs(msg.name, "POWER_OFF")) {
    iface.powerOff(debug);
    return;
  }
  if (nameIs(msg.name, "CHANGE_DISC")) {
    iface.changeDisc(makeDiscInfo(msg), debug);
    return;
  }
  if (nameIs(msg.name, "CHANGE_TRACK")) {
    iface.changeTrack(makeDiscInfo(msg), makeTrackInfo(msg), debug);
    return;
  }
  if (nameIs(msg.name, "READY")) {
    iface.ready(debug);
    return;
  }
  if (nameIs(msg.name, "CHANGING_DISC")) {
    iface.changingDisc(debug);
    return;
  }
  if (nameIs(msg.name, "DISC_READY")) {
    iface.discReady(makeDiscInfo(msg), debug);
    return;
  }
  if (nameIs(msg.name, "DISC_LOADED")) {
    iface.discLoaded(makeDiscInfo(msg), debug);
    return;
  }
  if (nameIs(msg.name, "LOADING_DISC")) {
    iface.loadingDisc(makeDiscInfo(msg), debug);
    return;
  }
  if (nameIs(msg.name, "CHANGING_TRACK")) {
    iface.changingTrack(makeDiscInfo(msg), makeTrackInfo(msg), debug);
    return;
  }

  iface.unknown(debug);
}
