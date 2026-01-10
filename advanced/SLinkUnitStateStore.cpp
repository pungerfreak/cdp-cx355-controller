#include "SLinkUnitStateStore.h"

void SLinkUnitStateStore::updateDisc(const SLinkDiscInfo& disc) {
  if (!disc.present || !disc.valid) return;
  if (_hasDisc && _currentDisc == disc.disc) return;
  _currentDisc = disc.disc;
  _hasDisc = true;
}

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
