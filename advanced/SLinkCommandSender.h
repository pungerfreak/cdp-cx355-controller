#pragma once
#include <Arduino.h>
#include "SLinkCommandSink.h"
#include "SLinkTx.h"

class SLinkCommandSender : public SLinkCommandSink {
public:
  explicit SLinkCommandSender(SLinkTx& tx, uint8_t unit = SLINK_ADDR_CONTROLLER);

  void setUnit(uint8_t unit);
  void sendCommand(SLinkCommandId id) override;
  void sendCommand(const SLinkCommand& cmd);

private:
  SLinkTx& _tx;
  uint8_t _unit;
};
