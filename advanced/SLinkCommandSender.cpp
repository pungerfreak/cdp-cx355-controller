#include "SLinkCommandSender.h"

SLinkCommandSender::SLinkCommandSender(SLinkTx& tx, uint8_t unit)
    : _tx(tx), _unit(unit) {}

void SLinkCommandSender::setUnit(uint8_t unit) {
  _unit = unit;
}

void SLinkCommandSender::sendCommand(SLinkCommandId id) {
  SLinkCommand cmd = {_unit, id};
  sendCommand(cmd);
}

void SLinkCommandSender::sendCommand(const SLinkCommand& cmd) {
  uint8_t frame[2];
  encodeCommand(cmd, frame);
  _tx.sendBytes(frame, sizeof(frame));
}
