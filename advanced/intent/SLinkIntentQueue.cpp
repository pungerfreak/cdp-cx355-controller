#include "intent/SLinkIntentQueue.h"

bool SLinkIntentQueue::push(const SLinkCommandIntent& intent) {
  if (isFull()) return false;
  _queue[_tail] = intent;
  _queuedAt[_tail] = millis();
  _tail = (uint8_t)((_tail + 1) % kMaxIntents);
  _count++;
  return true;
}

bool SLinkIntentQueue::pop(SLinkCommandIntent& intent) {
  if (isEmpty()) return false;
  intent = _queue[_head];
  _queuedAt[_head] = 0;
  _head = (uint8_t)((_head + 1) % kMaxIntents);
  _count--;
  return true;
}

bool SLinkIntentQueue::peekAt(uint8_t offset,
                              SLinkCommandIntent& intent,
                              uint32_t& enqueuedAt) const {
  if (offset >= _count) return false;
  uint8_t index = (uint8_t)((_head + offset) % kMaxIntents);
  intent = _queue[index];
  enqueuedAt = _queuedAt[index];
  return true;
}

bool SLinkIntentQueue::removeAt(uint8_t offset,
                                SLinkCommandIntent& intent,
                                uint32_t& enqueuedAt) {
  if (!peekAt(offset, intent, enqueuedAt)) return false;
  for (uint8_t i = offset; i + 1 < _count; ++i) {
    uint8_t fromIndex = (uint8_t)((_head + i + 1) % kMaxIntents);
    uint8_t toIndex = (uint8_t)((_head + i) % kMaxIntents);
    _queue[toIndex] = _queue[fromIndex];
    _queuedAt[toIndex] = _queuedAt[fromIndex];
  }
  _tail = (uint8_t)((_tail + kMaxIntents - 1) % kMaxIntents);
  _queuedAt[_tail] = 0;
  _count--;
  return true;
}

bool SLinkIntentQueue::isEmpty() const {
  return _count == 0;
}

bool SLinkIntentQueue::isFull() const {
  return _count >= kMaxIntents;
}

uint8_t SLinkIntentQueue::size() const {
  return _count;
}
