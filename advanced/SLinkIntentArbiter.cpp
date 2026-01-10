#include "SLinkIntentArbiter.h"

const SLinkIntentArbiter::Policy& SLinkIntentArbiter::policyFor(
    SLinkIntentType type) {
  static const Policy kPolicies[] = {
      {2, 250, 5000},  // Play
      {3, 250, 5000},  // Stop
      {2, 250, 5000},  // Pause
      {3, 500, 8000},  // PowerOn
      {3, 500, 8000},  // PowerOff
      {1, 750, 8000},  // ChangeDisc
      {1, 750, 5000}   // ChangeTrack
  };
  static_assert(sizeof(kPolicies) / sizeof(kPolicies[0]) ==
                    SLinkIntentArbiter::kIntentTypeCount,
                "Intent policy table size mismatch");
  uint8_t index = static_cast<uint8_t>(type);
  if (index >= kIntentTypeCount) {
    static const Policy kDefault{1, 250, 0};
    return kDefault;
  }
  return kPolicies[index];
}

bool SLinkIntentArbiter::selectNext(SLinkIntentQueue& queue,
                                    SLinkCommandIntent& intent) {
  if (queue.isEmpty()) return false;

  uint32_t now = millis();

  for (uint8_t offset = 0; offset < queue.size();) {
    SLinkCommandIntent candidate;
    uint32_t enqueuedAt = 0;
    if (!queue.peekAt(offset, candidate, enqueuedAt)) break;
    const Policy& policy = policyFor(candidate.type);
    if (policy.expireMs > 0 && (now - enqueuedAt) > policy.expireMs) {
      SLinkCommandIntent dropped;
      uint32_t droppedAt = 0;
      queue.removeAt(offset, dropped, droppedAt);
      continue;
    }
    ++offset;
  }

  if (queue.isEmpty()) return false;

  uint8_t bestOffset = 0;
  bool found = false;
  uint8_t bestPriority = 0;
  uint32_t bestEnqueuedAt = 0;

  for (uint8_t offset = 0; offset < queue.size(); ++offset) {
    SLinkCommandIntent candidate;
    uint32_t enqueuedAt = 0;
    if (!queue.peekAt(offset, candidate, enqueuedAt)) break;
    const Policy& policy = policyFor(candidate.type);
    uint8_t index = static_cast<uint8_t>(candidate.type);
    uint32_t last = (index < kIntentTypeCount) ? _lastDispatchMs[index] : 0;
    if (policy.throttleMs > 0 && last != 0 && (now - last) < policy.throttleMs) {
      continue;
    }
    if (!found || policy.priority > bestPriority ||
        (policy.priority == bestPriority && enqueuedAt < bestEnqueuedAt)) {
      found = true;
      bestPriority = policy.priority;
      bestEnqueuedAt = enqueuedAt;
      bestOffset = offset;
    }
  }

  if (!found) {
    return false;
  }

  uint32_t enqueuedAt = 0;
  if (!queue.removeAt(bestOffset, intent, enqueuedAt)) return false;

  uint8_t index = static_cast<uint8_t>(intent.type);
  if (index < kIntentTypeCount) {
    _lastDispatchMs[index] = now;
  }
  return true;
}
