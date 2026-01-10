#pragma once
#include <Arduino.h>
#include "SLinkUnitEvents.h"

class SLinkUnitEventBus {
public:
  static constexpr uint8_t kMaxObservers = 4;

  bool addObserver(SLinkUnitEventObserver& observer);
  void publish(const SLinkUnitEvent& event);

private:
  SLinkUnitEventObserver* _observers[kMaxObservers];
  uint8_t _count = 0;
};
