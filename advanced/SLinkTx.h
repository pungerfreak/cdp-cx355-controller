#pragma once
#include <Arduino.h>

class SLinkTx {
public:
  explicit SLinkTx(uint8_t pin);

  void begin();
  void sendBytes(const uint8_t* data, uint16_t len);

private:
  void driveLow();
  void releaseLine();
  void sendSync();
  void sendBit(bool one);

  uint8_t _pin;
  uint16_t _low0Us = 600;
  uint16_t _low1Us = 1200;
  uint16_t _lowSyncUs = 2400;
  uint16_t _highGapUs = 600;
  uint16_t _endGapUs = 5000;
};
