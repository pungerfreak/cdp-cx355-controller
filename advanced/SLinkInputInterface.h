#pragma once
#include <Arduino.h>

class SLinkInputInterface {
public:
  virtual ~SLinkInputInterface() = default;

  virtual bool play() = 0;
  virtual bool stop() = 0;
  virtual bool pause() = 0;
  virtual bool powerOn() = 0;
  virtual bool powerOff() = 0;
  virtual bool changeDisc(uint16_t disc) = 0;
  virtual bool changeTrack(uint8_t track) = 0;
};
