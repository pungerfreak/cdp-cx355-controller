#include "SLinkTx.h"

SLinkTx::SLinkTx(uint8_t pin) : _pin(pin) {}

void SLinkTx::begin() {
  pinMode(_pin, INPUT_PULLDOWN);
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

void SLinkTx::sendBytes(const uint8_t* data, uint16_t len) {
  if (!data || !len) return;

  noInterrupts();
  sendSync();
  for (uint16_t i = 0; i < len; i++) {
    uint8_t b = data[i];
    for (int8_t bit = 7; bit >= 0; bit--) {
      sendBit((b >> bit) & 1);
    }
  }
  releaseLine();
  delayMicroseconds(_endGapUs);
  interrupts();
}
