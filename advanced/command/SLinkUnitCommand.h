#pragma once
#include <Arduino.h>

enum class SLinkUnitCommandType : uint8_t {
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

struct SLinkUnitCommand {
  SLinkUnitCommandType type;
  uint16_t disc = 0;
  uint8_t track = 0;
  uint8_t unitOverride = 0;
};
