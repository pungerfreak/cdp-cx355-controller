#pragma once
#include <Arduino.h>
#include "unit/SLinkUnitEvents.h"

class SLinkUnitEventBus {
public:
  static constexpr uint8_t kMaxObservers = 4;

  bool addObserver(SLinkUnitEventObserver& observer);
  bool removeObserver(SLinkUnitEventObserver& observer);
  void publish(const SLinkUnitEvent& event);

private:
  SLinkUnitEventObserver* _observers[kMaxObservers] = {};
  uint8_t _count = 0;
};
