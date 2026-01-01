#pragma once
#include <Arduino.h>

class SLinkRx {
public:
  explicit SLinkRx(uint8_t pin);

  // call from setup()
  void begin(bool usePullup = true);

  // call from loop(); returns true when a complete message is available
  bool poll(uint32_t gap_us = 5000);

  // after poll() returns true:
  uint16_t length() const;
  const uint8_t* data() const;
  bool error() const;

  // optional helper
  void reset();

private:
  uint8_t _pin;

  static constexpr uint16_t BYTES_MAX = 64;

  // ISR-shared state
  volatile uint8_t  _msg[BYTES_MAX];
  volatile uint16_t _msgLen = 0;

  volatile uint32_t _lastRiseUs = 0;
  volatile uint32_t _lastEdgeUs = 0;

  volatile uint8_t  _curByte = 0;
  volatile uint8_t  _bitCount = 0;

  volatile bool _inFrame = false;
  volatile bool _msgReady = false;
  volatile bool _msgError = false;

  // one instance supported (simple + typical for ISR hookup)
  static SLinkRx* _instance;

  static void IRAM_ATTR isrThunk();
  void IRAM_ATTR onRiseISR();

  inline void resetFrameISR();
  inline void pushBitISR(uint8_t b);

  // local copy for safe access after poll()
  uint8_t  _local[BYTES_MAX];
  uint16_t _localLen = 0;
  bool     _localErr = false;
};