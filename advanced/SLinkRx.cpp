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
  _lastEdgeUs = micros();
  _edgeHead = 0;
  _edgeTail = 0;
  _edgeOverflow = false;
  _curByte = 0;
  _bitCount = 0;
  _inFrame = false;
  _msgReady = false;
  _msgError = false;
  interrupts();

  attachInterrupt(digitalPinToInterrupt(_pin), SLinkRx::isrThunk, FALLING);
}

void SLinkRx::setRxCallback(RxCallback cb, void* context) {
  _rxCallback = cb;
  _rxCallbackCtx = context;
}

bool SLinkRx::poll(uint32_t gap_us) {
  drainEdgeBuffer();

  uint32_t lastEdge;
  noInterrupts();
  lastEdge = _lastEdgeUs;
  interrupts();

  // declare ready after a gap
  if (!_msgReady && _inFrame && (micros() - lastEdge) > gap_us) {
    _msgReady = true;
  }

  if (!_msgReady) return false;

  uint16_t n = _msgLen;
  if (n > BYTES_MAX) n = BYTES_MAX;
  memcpy(_local, _msg, n);
  _localLen = n;

  _localErr = _msgError || ((_bitCount % 8) != 0);

  // reset ISR state for next frame
  _msgReady = false;
  _inFrame = false;
  _msgLen = 0;
  _bitCount = 0;
  _curByte = 0;
  _msgError = false;

  if (_rxCallback) {
    _rxCallback(_local, _localLen, _localErr, _rxCallbackCtx);
  }
  return true;
}

void IRAM_ATTR SLinkRx::isrThunk() {
  if (_instance) _instance->onEdgeISR();
}

inline void SLinkRx::resetFrame() {
  _msgLen = 0;
  _curByte = 0;
  _bitCount = 0;
  _inFrame = true;
  _msgError = false;
}

inline void SLinkRx::pushBit(uint8_t b) {
  _curByte = (uint8_t)((_curByte << 1) | (b & 1));
  _bitCount++;
  if ((_bitCount & 7) == 0) {
    if (_msgLen < BYTES_MAX) _msg[_msgLen++] = _curByte;
    else _msgError = true;
    _curByte = 0;
  }
}

void IRAM_ATTR SLinkRx::onEdgeISR() {
  uint32_t now = micros();
  uint32_t dt  = now - _lastEdgeUs;
  _lastEdgeUs = now;

  uint8_t next = (uint8_t)((_edgeHead + 1) % EDGE_BUFFER_SIZE);
  if (next == _edgeTail) {
    _edgeOverflow = true;
    return;
  }

  _edgeDeltas[_edgeHead] = dt;
  _edgeHead = next;
}

void SLinkRx::drainEdgeBuffer() {
  bool overflow = false;

  while (true) {
    uint32_t dt = 0;
    noInterrupts();
    if (_edgeTail == _edgeHead) {
      overflow = _edgeOverflow;
      _edgeOverflow = false;
      interrupts();
      break;
    }
    dt = _edgeDeltas[_edgeTail];
    _edgeTail = (uint8_t)((_edgeTail + 1) % EDGE_BUFFER_SIZE);
    interrupts();

    processEdgeDelta(dt);
  }

  if (overflow) {
    _msgError = true;
    _inFrame = false;
  }
}

void SLinkRx::processEdgeDelta(uint32_t dt) {
  static constexpr uint32_t kGlitchMinUs = 900;
  static constexpr uint32_t kBit0MinUs = 900;
  static constexpr uint32_t kBit1MinUs = 1500;
  static constexpr uint32_t kSyncMinUs = 2100;

  if (dt < kGlitchMinUs) {
    return;
  }

  // ignore first edge after boot / reset
  if (dt == 0) return;

  // thresholds for edge-to-edge classification
  if (dt >= kSyncMinUs) {
    if (_inFrame && (_bitCount % 8) != 0) _msgError = true;
    resetFrame();
    return;
  }

  if (!_inFrame) return;

  if (dt >= kBit1MinUs) {
    pushBit(1);
  } else if (dt >= kBit0MinUs) {
    pushBit(0);
  } else {
    _msgError = true;
    _inFrame = false; // wait for next sync
  }
}
