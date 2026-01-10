#pragma once
#include <Arduino.h>
#include "intent/SLinkIntents.h"

class SLinkIntentQueue {
public:
  static constexpr uint8_t kMaxIntents = 8;

  bool push(const SLinkCommandIntent& intent);
  bool pop(SLinkCommandIntent& intent);
  bool peekAt(uint8_t offset, SLinkCommandIntent& intent, uint32_t& enqueuedAt) const;
  bool removeAt(uint8_t offset, SLinkCommandIntent& intent, uint32_t& enqueuedAt);
  bool isEmpty() const;
  bool isFull() const;
  uint8_t size() const;

private:
  SLinkCommandIntent _queue[kMaxIntents];
  uint32_t _queuedAt[kMaxIntents] = {};
  uint8_t _head = 0;
  uint8_t _tail = 0;
  uint8_t _count = 0;
};
