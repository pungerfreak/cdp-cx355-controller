#pragma once
#include <Arduino.h>

enum class SLinkIntentType : uint8_t {
  Play,
  Stop,
  Pause,
  PowerOn,
  PowerOff,
  ChangeDisc,
  ChangeTrack,
  GetCurrentDisc,
  GetStatus
};

struct SLinkCommandIntent {
  SLinkIntentType type;
  uint16_t disc = 0;
  uint8_t track = 0;
};
