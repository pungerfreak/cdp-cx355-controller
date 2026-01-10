#pragma once
#include <Arduino.h>
#include "command/SLinkCommands.h"
#include "command/SLinkUnitCommand.h"

class SLinkFrameBuilder {
public:
  bool build(const SLinkUnitCommand& cmd,
             uint8_t* out,
             uint16_t& len,
             uint8_t unit = SLINK_ADDR_CONTROLLER) const;

private:
  bool encodeDiscRaw(uint16_t disc, uint8_t& raw) const;
  bool encodeDiscUnit(uint16_t disc, uint8_t& unit) const;
  bool encodeBcd(uint8_t value, uint8_t& raw) const;
};
