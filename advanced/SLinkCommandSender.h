#pragma once
#include <Arduino.h>
#include "SLinkInputInterface.h"
#include "SLinkCommands.h"
#include "SLinkTx.h"

class SLinkCommandSender : public SLinkInputInterface {
public:
  explicit SLinkCommandSender(SLinkTx& tx, uint8_t unit = SLINK_ADDR_CONTROLLER);

  void setUnit(uint8_t unit);
  void setCurrentDisc(uint16_t disc);
  bool play() override;
  bool stop() override;
  bool pause() override;
  bool powerOn() override;
  bool powerOff() override;
  bool changeDisc(uint16_t disc) override;
  bool changeTrack(uint8_t track) override;

private:
  bool sendCommand(SLinkCommandId id);
  bool sendCommand(const SLinkCommand& cmd);
  bool sendChange(uint16_t disc, uint8_t track);
  bool encodeDiscRaw(uint16_t disc, uint8_t& raw) const;
  bool encodeBcd(uint8_t value, uint8_t& raw) const;
  void storeLastFrame(const uint8_t* data, uint16_t len);

  SLinkTx& _tx;
  uint8_t _unit;
  uint16_t _currentDisc = 0;
  bool _hasDisc = false;
  uint8_t _lastFrame[4] = {0};
  uint16_t _lastLen = 0;
  bool _lastReady = false;
};
