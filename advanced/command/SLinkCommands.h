#pragma once
#include <Arduino.h>

enum SLinkCommandId : uint8_t {
  PLAY = 0x00,
  STOP = 0x01,
  PAUSE = 0x02,
  GET_CURRENT_DISC = 0x45,
  POWER_ON = 0x2E,
  POWER_OFF = 0x2F,
};

constexpr uint8_t SLINK_ADDR_CONTROLLER = 0x90;

struct SLinkCommand {
  uint8_t unit = SLINK_ADDR_CONTROLLER;
  SLinkCommandId cmd = PLAY;
};

inline void encodeCommand(const SLinkCommand& cmd, uint8_t (&out)[2]) {
  out[0] = cmd.unit;
  out[1] = static_cast<uint8_t>(cmd.cmd);
}
