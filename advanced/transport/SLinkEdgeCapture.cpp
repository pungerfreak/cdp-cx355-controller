#include "transport/SLinkEdgeCapture.h"

void SLinkEdgeCapture::begin() {
  _head = 0;
  _tail = 0;
  _overflow = false;
}

void IRAM_ATTR SLinkEdgeCapture::recordDelta(uint32_t dt) {
  uint8_t next = (uint8_t)((_head + 1) % BUFFER_SIZE);
  if (next == _tail) {
    _overflow = true;
    return;
  }

  _deltas[_head] = dt;
  _head = next;
}

bool SLinkEdgeCapture::popDelta(uint32_t& dt) {
  bool hasData = false;

  noInterrupts();
  if (_tail != _head) {
    dt = _deltas[_tail];
    _tail = (uint8_t)((_tail + 1) % BUFFER_SIZE);
    hasData = true;
  }
  interrupts();

  return hasData;
}

bool SLinkEdgeCapture::consumeOverflow() {
  bool overflow = false;

  noInterrupts();
  overflow = _overflow;
  _overflow = false;
  interrupts();

  return overflow;
}
