#include "SLinkRx.h"

SLinkRx* SLinkRx::_instance = nullptr;

SLinkRx::SLinkRx(uint8_t pin, SLinkBusState& bus) : _pin(pin), _busState(bus) {}

void SLinkRx::begin() {
  if (_instance && _instance != this) {
    // simplest: only allow one active instance
    // (could assert/log if you want)
  }
  _instance = this;
  pinMode(_pin, INPUT);

  noInterrupts();
  _lastEdgeUs = 0;
  _busState.begin();
  _edgeCapture.begin();
  _symbolDecoder.reset();
  _frame.reset();
  _msgReady = false;
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
  if (!_msgReady && _frame.inFrame() && (micros() - lastEdge) > gap_us) {
    _msgReady = true;
  }

  if (!_msgReady) return false;

  _frame.copyMessage(_local, SLinkFrameAssembler::BYTES_MAX, _localLen);
  _localErr = _frame.hasError() || ((_frame.bitCount() % 8) != 0);

  // reset state for next frame
  _msgReady = false;
  _frame.reset();
  _symbolDecoder.reset();

  if (_rxCallback) {
    _rxCallback(_local, _localLen, _localErr, _rxCallbackCtx);
  }
  return true;
}

void IRAM_ATTR SLinkRx::isrThunk() {
  if (_instance) _instance->onEdgeISR();
}

void IRAM_ATTR SLinkRx::onEdgeISR() {
  uint32_t now = micros();
  uint32_t dt  = now - _lastEdgeUs;
  _lastEdgeUs = now;
  _busState.noteRxEdge(now);
  _edgeCapture.recordDelta(dt);
}

void SLinkRx::drainEdgeBuffer() {
  bool overflow = false;

  while (true) {
    uint32_t dt = 0;
    if (!_edgeCapture.popDelta(dt)) {
      overflow = _edgeCapture.consumeOverflow();
      break;
    }
    processEdgeDelta(dt);
  }

  if (overflow) {
    _frame.abortWithError();
    _symbolDecoder.reset();
  }
}

void SLinkRx::processEdgeDelta(uint32_t dt) {
  handleSymbol(_symbolDecoder.decodeDelta(dt));
}

void SLinkRx::handleSymbol(SLinkSymbolDecoder::SymbolType symbol) {
  switch (symbol) {
    case SLinkSymbolDecoder::SymbolType::None:
      return;
    case SLinkSymbolDecoder::SymbolType::Sync:
      _frame.onSync();
      return;
    case SLinkSymbolDecoder::SymbolType::Bit0:
      _frame.onBit(0);
      return;
    case SLinkSymbolDecoder::SymbolType::Bit1:
      _frame.onBit(1);
      return;
    case SLinkSymbolDecoder::SymbolType::Error:
      _frame.abortWithError();
      return;
  }
}
