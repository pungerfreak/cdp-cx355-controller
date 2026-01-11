#pragma once
#include <Arduino.h>

enum class SLinkUnitCommandType : uint8_t {
  Play,
  Stop,
  Pause,
  PowerOn,
  PowerOff,
  ChangeDisc,
  ChangeTrack
};

struct SLinkUnitCommand {
  SLinkUnitCommandType type;
  uint16_t disc = 0;
  uint8_t track = 0;
};
