#pragma once
#include <Arduino.h>

class SLinkEdgeCapture {
public:
  static constexpr uint8_t BUFFER_SIZE = 64;

  void begin();

  void IRAM_ATTR recordDelta(uint32_t dt);
  bool popDelta(uint32_t& dt);
  bool consumeOverflow();

private:
  volatile uint32_t _deltas[BUFFER_SIZE];
  volatile uint8_t _head = 0;
  volatile uint8_t _tail = 0;
  volatile bool _overflow = false;
};
