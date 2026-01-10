#include "SLinkBusState.h"

void SLinkBusState::begin() {
  _lastRxEdgeUs = 0;
  _lastTxEndUs = 0;
  _txActive = false;
  _rxDuringTx = false;
}

void SLinkBusState::noteRxEdge(uint32_t nowUs) {
  _lastRxEdgeUs = nowUs;
  if (_txActive) {
    _rxDuringTx = true;
  }
}

void SLinkBusState::noteTxStart(uint32_t nowUs) {
  _txActive = true;
  _rxDuringTx = false;
  _lastTxEndUs = nowUs;
}

void SLinkBusState::noteTxEnd(uint32_t nowUs) {
  _txActive = false;
  _lastTxEndUs = nowUs;
}

uint32_t SLinkBusState::lastRxEdgeUs() const {
  return _lastRxEdgeUs;
}

uint32_t SLinkBusState::lastTxEndUs() const {
  return _lastTxEndUs;
}

uint32_t SLinkBusState::lastActivityUs() const {
  uint32_t rx = _lastRxEdgeUs;
  uint32_t tx = _lastTxEndUs;
  return (rx > tx) ? rx : tx;
}

bool SLinkBusState::rxDuringTx() const {
  return _rxDuringTx;
}

bool SLinkBusState::txActive() const {
  return _txActive;
}
