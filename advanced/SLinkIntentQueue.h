#pragma once
#include <Arduino.h>
#include "SLinkIntents.h"

class SLinkIntentQueue {
public:
  static constexpr uint8_t kMaxIntents = 8;

  bool push(const SLinkCommandIntent& intent);
  bool pop(SLinkCommandIntent& intent);
  bool isEmpty() const;
  bool isFull() const;

private:
  SLinkCommandIntent _queue[kMaxIntents];
  uint8_t _head = 0;
  uint8_t _tail = 0;
  uint8_t _count = 0;
};
