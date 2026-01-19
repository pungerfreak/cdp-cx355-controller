#pragma once
#include <Arduino.h>
#include "intent/SLinkIntents.h"
#include "intent/SLinkIntentQueue.h"

class SLinkIntentArbiter {
public:
  SLinkIntentArbiter() = default;

  bool selectNext(SLinkIntentQueue& queue, SLinkCommandIntent& intent, uint8_t& offset);
  void noteDispatched(SLinkIntentType type, uint32_t now = millis());

private:
  static constexpr uint8_t kIntentTypeCount = 8;

  struct Policy {
    uint8_t priority;
    uint32_t throttleMs;
    uint32_t expireMs;
  };

  static const Policy& policyFor(SLinkIntentType type);
  static bool shouldExpire(uint32_t now,
                           const Policy& policy,
                           uint32_t enqueuedAt);
  static bool isThrottled(uint32_t now,
                          const Policy& policy,
                          uint32_t lastDispatched);

  uint32_t _lastDispatchMs[kIntentTypeCount] = {};
};
