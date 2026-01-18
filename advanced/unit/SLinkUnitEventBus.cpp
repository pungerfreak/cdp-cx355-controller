#include "unit/SLinkUnitEventBus.h"

bool SLinkUnitEventBus::addObserver(SLinkUnitEventObserver& observer) {
  if (_count >= kMaxObservers) return false;
  _observers[_count++] = &observer;
  return true;
}

bool SLinkUnitEventBus::removeObserver(SLinkUnitEventObserver& observer) {
  for (uint8_t i = 0; i < _count; ++i) {
    if (_observers[i] == &observer) {
      for (uint8_t j = i + 1; j < _count; ++j) {
        _observers[j - 1] = _observers[j];
      }
      _observers[_count - 1] = nullptr;
      --_count;
      return true;
    }
  }
  return false;
}

void SLinkUnitEventBus::publish(const SLinkUnitEvent& event) {
  for (uint8_t i = 0; i < _count; i++) {
    if (_observers[i]) {
      _observers[i]->onUnitEvent(event);
    }
  }
}
