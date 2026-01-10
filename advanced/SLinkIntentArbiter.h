#pragma once
#include <Arduino.h>
#include "SLinkIntents.h"
#include "SLinkIntentQueue.h"

class SLinkIntentArbiter {
public:
  explicit SLinkIntentArbiter(uint32_t dedupeWindowMs = 250);

  bool shouldDispatch(const SLinkCommandIntent& intent);
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
  uint32_t _dedupeWindowMs;
};
