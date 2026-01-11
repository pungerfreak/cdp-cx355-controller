#include "unit/SLinkUnitStateStore.h"

void SLinkUnitStateStore::updateDisc(const SLinkDiscInfo& disc) {
  if (!disc.present || !disc.valid) return;
  if (_hasDisc && _currentDisc == disc.disc) return;
  _currentDisc = disc.disc;
  _hasDisc = true;
}

void SLinkUnitStateStore::onUnitEvent(const SLinkUnitEvent& event) {
  switch (event.type) {
    case SLinkUnitEventType::DiscChanged:
    case SLinkUnitEventType::TrackChanged:
      updateDisc(event.disc);
      break;
    case SLinkUnitEventType::TransportStateChanged:
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
