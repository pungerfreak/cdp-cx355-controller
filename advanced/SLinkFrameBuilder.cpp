#include "SLinkFrameBuilder.h"

bool SLinkFrameBuilder::build(const SLinkUnitCommand& cmd,
                              uint8_t* out,
                              uint16_t& len,
                              uint8_t unit) const {
  if (!out) return false;
  len = 0;

  switch (cmd.type) {
    case SLinkUnitCommandType::Play:
    case SLinkUnitCommandType::Stop:
    case SLinkUnitCommandType::Pause:
    case SLinkUnitCommandType::PowerOn:
    case SLinkUnitCommandType::PowerOff: {
      SLinkCommandId id = PLAY;
      switch (cmd.type) {
        case SLinkUnitCommandType::Play:
          id = PLAY;
          break;
        case SLinkUnitCommandType::Stop:
          id = STOP;
          break;
        case SLinkUnitCommandType::Pause:
          id = PAUSE;
          break;
        case SLinkUnitCommandType::PowerOn:
          id = POWER_ON;
          break;
        case SLinkUnitCommandType::PowerOff:
          id = POWER_OFF;
          break;
        default:
          break;
      }
      uint8_t frame[2];
      encodeCommand({unit, id}, frame);
      out[0] = frame[0];
      out[1] = frame[1];
      len = 2;
      return true;
    }
    case SLinkUnitCommandType::ChangeDisc:
    case SLinkUnitCommandType::ChangeTrack: {
      uint8_t unitByte = 0;
      uint8_t discRaw = 0;
      uint8_t trackRaw = 0;
      if (!encodeDiscUnit(cmd.disc, unitByte)) return false;
      if (!encodeDiscRaw(cmd.disc, discRaw)) return false;
      if (!encodeBcd(cmd.track, trackRaw)) return false;
      out[0] = unitByte;
      out[1] = 0x50;
      out[2] = discRaw;
      out[3] = trackRaw;
      len = 4;
      return true;
    }
  }
  return false;
}

bool SLinkFrameBuilder::encodeDiscRaw(uint16_t disc, uint8_t& raw) const {
  if (disc >= 1 && disc <= 99) {
    uint8_t bcd = 0;
    if (!encodeBcd((uint8_t)disc, bcd)) return false;
    raw = bcd;
    return true;
  }
  if (disc >= 100 && disc <= 200) {
    raw = (uint8_t)(disc + 0x36);
    return true;
  }
  if (disc >= 201 && disc <= 300) {
    raw = (uint8_t)(disc - 200);
    return true;
  }
  return false;
}

bool SLinkFrameBuilder::encodeDiscUnit(uint16_t disc, uint8_t& unit) const {
  if (disc >= 1 && disc <= 200) {
    unit = 0x90;
    return true;
  }
  if (disc >= 201 && disc <= 300) {
    unit = 0x93;
    return true;
  }
  return false;
}

bool SLinkFrameBuilder::encodeBcd(uint8_t value, uint8_t& raw) const {
  if (value > 99) return false;
  raw = (uint8_t)(((value / 10) << 4) | (value % 10));
  return true;
}
