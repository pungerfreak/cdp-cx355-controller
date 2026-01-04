#include "SLinkRx.h"

SLinkRx* SLinkRx::_instance = nullptr;

SLinkRx::SLinkRx(uint8_t pin) : _pin(pin) {}

void SLinkRx::begin() {
  if (_instance && _instance != this) {
    // simplest: only allow one active instance
    // (could assert/log if you want)
  }
  _instance = this;
  pinMode(_pin, INPUT);

  noInterrupts();
  _msgLen = 0;
  _lastSymbolUs = 0;
  _lastEdgeUs = micros();
  _curByte = 0;
  _bitCount = 0;
  _inFrame = false;
  _msgReady = false;
  _msgError = false;
  interrupts();

  attachInterrupt(digitalPinToInterrupt(_pin), SLinkRx::isrThunk, FALLING);
}

void SLinkRx::reset() {
  noInterrupts();
  _msgLen = 0;
  _curByte = 0;
  _bitCount = 0;
  _inFrame = false;
  _msgReady = false;
  _msgError = false;
  interrupts();
}

uint16_t SLinkRx::length() const { return _localLen; }
const uint8_t* SLinkRx::data() const { return _local; }
bool SLinkRx::error() const { return _localErr; }

bool SLinkRx::poll(uint32_t gap_us) {
  // snapshot shared ISR values
  uint32_t lastEdge;
  bool inFrame, ready;

  noInterrupts();
  lastEdge = _lastEdgeUs;
  inFrame  = _inFrame;
  ready    = _msgReady;
  interrupts();

  // declare ready after a gap
  if (!ready && inFrame && (micros() - lastEdge) > gap_us) {
    noInterrupts();
    _msgReady = true;
    ready = true;
    interrupts();
  }

  if (!ready) return false;

  // copy out atomically
  noInterrupts();
  uint16_t n = _msgLen;
  if (n > BYTES_MAX) n = BYTES_MAX;
  memcpy(_local, (const void*)_msg, n);
  _localLen = n;

  _localErr = _msgError || ((_bitCount % 8) != 0);

  // reset ISR state for next frame
  _msgReady = false;
  _inFrame = false;
  _msgLen = 0;
  _bitCount = 0;
  _curByte = 0;
  _msgError = false;
  interrupts();

  return true;
}

void IRAM_ATTR SLinkRx::isrThunk() {
  if (_instance) _instance->onEdgeISR();
}

inline void IRAM_ATTR SLinkRx::resetFrameISR() {
  _msgLen = 0;
  _curByte = 0;
  _bitCount = 0;
  _inFrame = true;
  _msgError = false;
}

inline void IRAM_ATTR SLinkRx::pushBitISR(uint8_t b) {
  _curByte = (uint8_t)((_curByte << 1) | (b & 1));
  _bitCount++;
  if ((_bitCount & 7) == 0) {
    if (_msgLen < BYTES_MAX) _msg[_msgLen++] = _curByte;
    else _msgError = true;
    _curByte = 0;
  }
}

void IRAM_ATTR SLinkRx::onEdgeISR() {
  static constexpr uint32_t kGlitchMinUs = 900;

  uint32_t now = micros();
  uint32_t dt  = now - _lastSymbolUs;

  if (dt < kGlitchMinUs) {
    return;
  }

  _lastSymbolUs = now;
  _lastEdgeUs = now;

  // ignore first edge after boot / reset
  if (dt == 0) return;

  // thresholds for edge-to-edge classification
  if (dt > 2700) {
    if (_inFrame && (_bitCount % 8) != 0) _msgError = true;
    resetFrameISR();
    return;
  }

  if (!_inFrame) return;

  if (dt > 1500) {
    pushBitISR(1);
  } else if (dt >= 900) {
    pushBitISR(0);
  }
  else {
    _msgError = true;
    _inFrame = false; // wait for next sync
  }
}
