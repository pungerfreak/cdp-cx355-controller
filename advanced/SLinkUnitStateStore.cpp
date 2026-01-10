#include "SLinkUnitStateStore.h"

void SLinkUnitStateStore::updateDisc(const SLinkDiscInfo& disc) {
  if (!disc.present || !disc.valid) return;
  if (_hasDisc && _currentDisc == disc.disc) return;
  _currentDisc = disc.disc;
  _hasDisc = true;
}

void SLinkUnitStateStore::play(const SLinkDebugInfo* debug) { (void)debug; }
void SLinkUnitStateStore::stop(const SLinkDebugInfo* debug) { (void)debug; }
void SLinkUnitStateStore::pause(const SLinkDebugInfo* debug) { (void)debug; }
void SLinkUnitStateStore::powerOn(const SLinkDebugInfo* debug) { (void)debug; }
void SLinkUnitStateStore::powerOff(const SLinkDebugInfo* debug) { (void)debug; }
void SLinkUnitStateStore::changeDisc(const SLinkDiscInfo& disc,
                                     const SLinkDebugInfo* debug) {
  (void)disc;
  (void)debug;
}

void SLinkUnitStateStore::changeTrack(const SLinkDiscInfo& disc,
                                      const SLinkTrackInfo& track,
                                      const SLinkDebugInfo* debug) {
  (void)disc;
  (void)track;
  (void)debug;
}

void SLinkUnitStateStore::ready(const SLinkDebugInfo* debug) { (void)debug; }
void SLinkUnitStateStore::changingDisc(const SLinkDebugInfo* debug) { (void)debug; }

void SLinkUnitStateStore::discReady(const SLinkDiscInfo& disc,
                                    const SLinkDebugInfo* debug) {
  (void)debug;
  updateDisc(disc);
}

void SLinkUnitStateStore::discLoaded(const SLinkDiscInfo& disc,
                                     const SLinkDebugInfo* debug) {
  (void)disc;
  (void)debug;
}

void SLinkUnitStateStore::loadingDisc(const SLinkDiscInfo& disc,
                                      const SLinkDebugInfo* debug) {
  (void)disc;
  (void)debug;
}

void SLinkUnitStateStore::changingTrack(const SLinkDiscInfo& disc,
                                        const SLinkTrackInfo& track,
                                        const SLinkDebugInfo* debug) {
  (void)debug;
  updateDisc(disc);
  (void)track;
}

void SLinkUnitStateStore::unknown(const SLinkDebugInfo* debug) { (void)debug; }

void SLinkUnitStateStore::onUnitEvent(const SLinkUnitEvent& event) {
  switch (event.type) {
    case SLinkUnitEventType::DiscReady:
    case SLinkUnitEventType::ChangingTrack:
      updateDisc(event.disc);
      break;
    case SLinkUnitEventType::ChangeDisc:
    case SLinkUnitEventType::ChangeTrack:
    case SLinkUnitEventType::DiscLoaded:
    case SLinkUnitEventType::LoadingDisc:
    case SLinkUnitEventType::Play:
    case SLinkUnitEventType::Stop:
    case SLinkUnitEventType::Pause:
    case SLinkUnitEventType::PowerOn:
    case SLinkUnitEventType::PowerOff:
    case SLinkUnitEventType::Ready:
    case SLinkUnitEventType::ChangingDisc:
    case SLinkUnitEventType::Unknown:
      break;
  }
}

bool SLinkUnitStateStore::hasDisc() const {
  return _hasDisc;
}

uint16_t SLinkUnitStateStore::currentDisc() const {
  return _currentDisc;
}
