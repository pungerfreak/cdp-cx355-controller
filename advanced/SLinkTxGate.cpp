#include "SLinkTxGate.h"

SLinkTxGate::SLinkTxGate(SLinkBusState& bus) : _bus(bus) {}

void SLinkTxGate::begin() {
  _backoffUntilUs = 0;
}

bool SLinkTxGate::canTransmit(uint32_t nowUs) const {
  if (nowUs < _backoffUntilUs) return false;
  uint32_t lastActivity = _bus.lastActivityUs();
  if (nowUs - lastActivity < _idleGapUs) return false;
  if (nowUs - _bus.lastRxEdgeUs() < _guardTimeUs) return false;
  return true;
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

uint32_t SLinkTxGate::randomBackoffUs() const {
  if (_backoffMaxUs <= _backoffMinUs) return _backoffMinUs;
  return (uint32_t)random(_backoffMinUs, _backoffMaxUs + 1);
}
