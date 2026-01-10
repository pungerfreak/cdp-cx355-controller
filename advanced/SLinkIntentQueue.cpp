#include "SLinkIntentQueue.h"

bool SLinkIntentQueue::push(const SLinkCommandIntent& intent) {
  if (isFull()) return false;
  _queue[_tail] = intent;
  _tail = (uint8_t)((_tail + 1) % kMaxIntents);
  _count++;
  return true;
}

bool SLinkIntentQueue::pop(SLinkCommandIntent& intent) {
  if (isEmpty()) return false;
  intent = _queue[_head];
  _head = (uint8_t)((_head + 1) % kMaxIntents);
  _count--;
  return true;
}

bool SLinkIntentQueue::isEmpty() const {
  return _count == 0;
}

bool SLinkIntentQueue::isFull() const {
  return _count >= kMaxIntents;
}
