#include "transport/SLinkTxGate.h"

SLinkTxGate::SLinkTxGate(SLinkBusState& bus) : _bus(bus) {}

void SLinkTxGate::begin() {
  _backoffUntilUs = 0;
}

bool SLinkTxGate::canTransmit(uint32_t nowUs) const {
  if (!_protocolAllowsTx) return false;
  if (nowUs < _backoffUntilUs) return false;
  uint32_t lastActivity = _bus.lastActivityUs();
  if (nowUs - lastActivity < _idleGapUs) return false;
  if (nowUs - _bus.lastRxEdgeUs() < _guardTimeUs) return false;
  return true;
}

uint32_t SLinkTxGate::nextTransmitUs(uint32_t nowUs) const {
  if (_protocolAllowsTx && canTransmit(nowUs)) return nowUs;

  uint32_t candidate = nowUs;
  if (nowUs < _backoffUntilUs) {
    candidate = _backoffUntilUs;
  }

  uint32_t lastActivity = _bus.lastActivityUs();
  uint32_t afterIdleGap = lastActivity + _idleGapUs;
  if (afterIdleGap > candidate) candidate = afterIdleGap;

  uint32_t lastRx = _bus.lastRxEdgeUs();
  uint32_t afterGuard = lastRx + _guardTimeUs;
  if (afterGuard > candidate) candidate = afterGuard;

  // If TX is disabled at the protocol level, there isn't a concrete "next"
  // time; return a short delay to avoid busy retries.
  if (!_protocolAllowsTx && candidate == nowUs) {
    candidate = nowUs + 10000u;
  }

  return candidate;
}

bool SLinkTxGate::shouldAbort() const {
  return _bus.rxDuringTx();
}

void SLinkTxGate::beginTx(uint32_t nowUs) {
  _bus.noteTxStart(nowUs);
}

void SLinkTxGate::endTx(uint32_t nowUs, bool aborted) {
  _bus.noteTxEnd(nowUs);
  if (aborted) {
    _backoffUntilUs = nowUs + randomBackoffUs();
  }
}

void SLinkTxGate::setProtocolAllowsTx(bool allow) {
  _protocolAllowsTx = allow;
}

uint32_t SLinkTxGate::randomBackoffUs() const {
  if (_backoffMaxUs <= _backoffMinUs) return _backoffMinUs;
  return (uint32_t)random(_backoffMinUs, _backoffMaxUs + 1);
}
