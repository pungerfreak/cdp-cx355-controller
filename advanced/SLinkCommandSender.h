#pragma once
#include <Arduino.h>
#include "SLinkInputInterface.h"
#include "SLinkCommands.h"
#include "SLinkTx.h"

class SLinkCommandSender : public SLinkInputInterface {
public:
  explicit SLinkCommandSender(SLinkTx& tx, uint8_t unit = SLINK_ADDR_CONTROLLER);

  void setUnit(uint8_t unit);
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

  SLinkTx& _tx;
  uint8_t _unit;
};
