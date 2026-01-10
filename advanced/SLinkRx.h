#pragma once
#include <Arduino.h>
#include "SLinkEdgeCapture.h"

class SLinkRx {
public:
  explicit SLinkRx(uint8_t pin);

  // call from setup()
  void begin();

  // call from loop(); returns true when a complete message is available
  bool poll(uint32_t gap_us = 5000);

  using RxCallback = void (*)(const uint8_t* data, uint16_t len, bool error, void* context);
  void setRxCallback(RxCallback cb, void* context);

private:
  uint8_t _pin;

  static constexpr uint16_t BYTES_MAX = 64;

  // ISR-shared state
  uint8_t  _msg[BYTES_MAX];
  uint16_t _msgLen = 0;

  volatile uint32_t _lastEdgeUs = 0;
  SLinkEdgeCapture _edgeCapture;

  uint32_t _pendingDelta = 0;
  uint8_t  _curByte = 0;
  uint8_t  _bitCount = 0;

  bool _inFrame = false;
  bool _msgReady = false;
  bool _msgError = false;

  // one instance supported (simple + typical for ISR hookup)
  static SLinkRx* _instance;

  static void IRAM_ATTR isrThunk();
  void IRAM_ATTR onEdgeISR();

  void drainEdgeBuffer();
  void processEdgeDelta(uint32_t dt);
  inline void resetFrame();
  inline void pushBit(uint8_t b);

  // local copy for safe access after poll()
  uint8_t  _local[BYTES_MAX];
  uint16_t _localLen = 0;
  bool     _localErr = false;
  RxCallback _rxCallback = nullptr;
  void* _rxCallbackCtx = nullptr;
};
