#include "SLinkTx.h"

SLinkTx::SLinkTx(uint8_t pin, SLinkTxGate& gate) : _pin(pin), _txGate(gate) {}

void SLinkTx::begin() {
  pinMode(_pin, INPUT_PULLDOWN);
  _txGate.begin();
}

void SLinkTx::driveLow() {
  pinMode(_pin, OUTPUT);
  digitalWrite(_pin, HIGH);
}

void SLinkTx::releaseLine() {
  pinMode(_pin, OUTPUT);
  digitalWrite(_pin, LOW);
}

void SLinkTx::sendSync() {
  driveLow();
  delayMicroseconds(_lowSyncUs);
  releaseLine();
  delayMicroseconds(_highGapUs);
}

void SLinkTx::sendBit(bool one) {
  driveLow();
  delayMicroseconds(one ? _low1Us : _low0Us);
  releaseLine();
  delayMicroseconds(_highGapUs);
}

bool SLinkTx::sendBytes(const uint8_t* data, uint16_t len) {
  if (!data || !len) return false;

  uint32_t now = micros();
  if (!_txGate.canTransmit(now)) return false;
  _txGate.beginTx(now);
  sendSync();
  for (uint16_t i = 0; i < len; i++) {
    uint8_t b = data[i];
    for (int8_t bit = 7; bit >= 0; bit--) {
      sendBit((b >> bit) & 1);
      if (_txGate.shouldAbort()) {
        releaseLine();
        _txGate.endTx(micros(), true);
        return false;
      }
    }
  }
  releaseLine();
  delayMicroseconds(_endGapUs);
  _txGate.endTx(micros(), false);
  return true;
}
