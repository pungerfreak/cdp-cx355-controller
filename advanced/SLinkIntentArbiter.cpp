#include "SLinkIntentArbiter.h"

SLinkIntentArbiter::SLinkIntentArbiter(uint32_t dedupeWindowMs)
    : _dedupeWindowMs(dedupeWindowMs) {}

bool SLinkIntentArbiter::shouldDispatch(const SLinkCommandIntent& intent) {
  uint8_t index = static_cast<uint8_t>(intent.type);
  if (index >= kIntentTypeCount) return true;

  uint32_t now = millis();
  uint32_t last = _lastDispatchMs[index];
  if (last != 0 && (now - last) < _dedupeWindowMs) {
    return false;
  }

  _lastDispatchMs[index] = now;
  return true;
}
