#pragma once
#include <Arduino.h>
#include "intent/SLinkIntents.h"
#include "intent/SLinkIntentQueue.h"

class SLinkIntentArbiter {
public:
  SLinkIntentArbiter() = default;

  bool selectNext(SLinkIntentQueue& queue, SLinkCommandIntent& intent);

private:
  static constexpr uint8_t kIntentTypeCount = 7;

  struct Policy {
    uint8_t priority;
    uint32_t throttleMs;
    uint32_t expireMs;
  };

  static const Policy& policyFor(SLinkIntentType type);

  uint32_t _lastDispatchMs[kIntentTypeCount] = {};
};
