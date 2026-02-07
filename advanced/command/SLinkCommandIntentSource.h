#pragma once
#include <Arduino.h>

class SLinkCommandIntentSource {
public:
  virtual ~SLinkCommandIntentSource() = default;

  // Commands that act on the "current" disc/player state should be parameterless.
  virtual bool play() = 0;
  virtual bool stop() = 0;
  virtual bool pause() = 0;
  virtual bool nextTrack() = 0;
  virtual bool prevTrack() = 0;
  virtual bool powerOn() = 0;
  virtual bool powerOff() = 0;
  virtual bool changeDisc(uint16_t disc) = 0;
  virtual bool changeTrack(uint8_t track) = 0;
  virtual bool getDiscInfo() = 0;
  virtual bool getCurrentDisc() = 0;
  virtual bool getStatus() = 0;
};
