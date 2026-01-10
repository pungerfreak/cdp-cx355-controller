#pragma once
#include <Arduino.h>
#include "SLinkIntents.h"

class SLinkIntentArbiter {
public:
  explicit SLinkIntentArbiter(uint32_t dedupeWindowMs = 250);

  bool shouldDispatch(const SLinkCommandIntent& intent);

private:
  static constexpr uint8_t kIntentTypeCount = 7;

  uint32_t _lastDispatchMs[kIntentTypeCount] = {};
  uint32_t _dedupeWindowMs;
};
