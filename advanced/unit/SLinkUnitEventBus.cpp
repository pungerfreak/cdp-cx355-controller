#include "unit/SLinkUnitEventBus.h"

bool SLinkUnitEventBus::addObserver(SLinkUnitEventObserver& observer) {
  if (_count >= kMaxObservers) return false;
  _observers[_count++] = &observer;
  return true;
}

void SLinkUnitEventBus::publish(const SLinkUnitEvent& event) {
  for (uint8_t i = 0; i < _count; i++) {
    if (_observers[i]) {
      _observers[i]->onUnitEvent(event);
    }
  }
}
