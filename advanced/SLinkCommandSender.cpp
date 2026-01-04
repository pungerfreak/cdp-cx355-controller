#include "SLinkCommandSender.h"

SLinkCommandSender::SLinkCommandSender(SLinkTx& tx, uint8_t unit)
    : _tx(tx), _unit(unit) {}

void SLinkCommandSender::setUnit(uint8_t unit) {
  _unit = unit;
}

bool SLinkCommandSender::play() {
  return sendCommand(PLAY);
}

bool SLinkCommandSender::stop() {
  return sendCommand(STOP);
}

bool SLinkCommandSender::pause() {
  return sendCommand(PAUSE);
}

bool SLinkCommandSender::powerOn() {
  return sendCommand(POWER_ON);
}

bool SLinkCommandSender::powerOff() {
  return sendCommand(POWER_OFF);
}

bool SLinkCommandSender::changeDisc(uint16_t disc) {
  // TODO: implement once the change-disc command encoding is known.
  (void)disc;
  return false;
}

bool SLinkCommandSender::changeTrack(uint8_t track) {
  // TODO: implement once the change-track command encoding is known.
  (void)track;
  return false;
}

bool SLinkCommandSender::sendCommand(SLinkCommandId id) {
  SLinkCommand cmd = {_unit, id};
  return sendCommand(cmd);
}

bool SLinkCommandSender::sendCommand(const SLinkCommand& cmd) {
  uint8_t frame[2];
  encodeCommand(cmd, frame);
  _tx.sendBytes(frame, sizeof(frame));
  return true;
}
