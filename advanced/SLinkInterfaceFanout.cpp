#include "SLinkInterfaceFanout.h"

namespace {
void dispatchPlay(SLinkInterface& iface, const SLinkDebugInfo* debug) {
  iface.play(debug);
}
void dispatchStop(SLinkInterface& iface, const SLinkDebugInfo* debug) {
  iface.stop(debug);
}
void dispatchPause(SLinkInterface& iface, const SLinkDebugInfo* debug) {
  iface.pause(debug);
}
void dispatchPowerOn(SLinkInterface& iface, const SLinkDebugInfo* debug) {
  iface.powerOn(debug);
}
void dispatchPowerOff(SLinkInterface& iface, const SLinkDebugInfo* debug) {
  iface.powerOff(debug);
}
void dispatchReady(SLinkInterface& iface, const SLinkDebugInfo* debug) {
  iface.ready(debug);
}
void dispatchChangingDisc(SLinkInterface& iface, const SLinkDebugInfo* debug) {
  iface.changingDisc(debug);
}
void dispatchUnknown(SLinkInterface& iface, const SLinkDebugInfo* debug) {
  iface.unknown(debug);
}
}  // namespace

bool SLinkInterfaceFanout::add(SLinkInterface& target) {
  if (_count >= kMaxTargets) return false;
  _targets[_count++] = &target;
  return true;
}

void SLinkInterfaceFanout::forEach(void (*fn)(SLinkInterface& iface, const SLinkDebugInfo* debug),
                                   const SLinkDebugInfo* debug) {
  for (uint8_t i = 0; i < _count; i++) {
    if (_targets[i]) {
      fn(*_targets[i], debug);
    }
  }
}

void SLinkInterfaceFanout::play(const SLinkDebugInfo* debug) {
  forEach(dispatchPlay, debug);
}

void SLinkInterfaceFanout::stop(const SLinkDebugInfo* debug) {
  forEach(dispatchStop, debug);
}

void SLinkInterfaceFanout::pause(const SLinkDebugInfo* debug) {
  forEach(dispatchPause, debug);
}

void SLinkInterfaceFanout::powerOn(const SLinkDebugInfo* debug) {
  forEach(dispatchPowerOn, debug);
}

void SLinkInterfaceFanout::powerOff(const SLinkDebugInfo* debug) {
  forEach(dispatchPowerOff, debug);
}

void SLinkInterfaceFanout::changeDisc(const SLinkDiscInfo& disc,
                                      const SLinkDebugInfo* debug) {
  for (uint8_t i = 0; i < _count; i++) {
    if (_targets[i]) {
      _targets[i]->changeDisc(disc, debug);
    }
  }
}

void SLinkInterfaceFanout::changeTrack(const SLinkDiscInfo& disc,
                                       const SLinkTrackInfo& track,
                                       const SLinkDebugInfo* debug) {
  for (uint8_t i = 0; i < _count; i++) {
    if (_targets[i]) {
      _targets[i]->changeTrack(disc, track, debug);
    }
  }
}

void SLinkInterfaceFanout::ready(const SLinkDebugInfo* debug) {
  forEach(dispatchReady, debug);
}

void SLinkInterfaceFanout::changingDisc(const SLinkDebugInfo* debug) {
  forEach(dispatchChangingDisc, debug);
}

void SLinkInterfaceFanout::discReady(const SLinkDiscInfo& disc,
                                     const SLinkDebugInfo* debug) {
  for (uint8_t i = 0; i < _count; i++) {
    if (_targets[i]) {
      _targets[i]->discReady(disc, debug);
    }
  }
}

void SLinkInterfaceFanout::discLoaded(const SLinkDiscInfo& disc,
                                      const SLinkDebugInfo* debug) {
  for (uint8_t i = 0; i < _count; i++) {
    if (_targets[i]) {
      _targets[i]->discLoaded(disc, debug);
    }
  }
}

void SLinkInterfaceFanout::loadingDisc(const SLinkDiscInfo& disc,
                                       const SLinkDebugInfo* debug) {
  for (uint8_t i = 0; i < _count; i++) {
    if (_targets[i]) {
      _targets[i]->loadingDisc(disc, debug);
    }
  }
}

void SLinkInterfaceFanout::changingTrack(const SLinkDiscInfo& disc,
                                         const SLinkTrackInfo& track,
                                         const SLinkDebugInfo* debug) {
  for (uint8_t i = 0; i < _count; i++) {
    if (_targets[i]) {
      _targets[i]->changingTrack(disc, track, debug);
    }
  }
}

void SLinkInterfaceFanout::unknown(const SLinkDebugInfo* debug) {
  forEach(dispatchUnknown, debug);
}
