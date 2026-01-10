#include "SLinkCommandSender.h"

SLinkCommandSender::SLinkCommandSender(SLinkTx& tx, uint8_t unit)
    : _tx(tx), _unit(unit) {}

void SLinkCommandSender::setCurrentDisc(uint16_t disc) {
  if (disc == 0 || disc > 300) return;
  _currentDisc = disc;
  _hasDisc = true;
}

void SLinkCommandSender::setTxCallback(TxCallback cb, void* context) {
  _txCallback = cb;
  _txCallbackCtx = context;
}

bool SLinkCommandSender::play() {
  return sendSimple(SLinkUnitCommandType::Play);
}

bool SLinkCommandSender::stop() {
  return sendSimple(SLinkUnitCommandType::Stop);
}

bool SLinkCommandSender::pause() {
  return sendSimple(SLinkUnitCommandType::Pause);
}

bool SLinkCommandSender::powerOn() {
  return sendSimple(SLinkUnitCommandType::PowerOn);
}

bool SLinkCommandSender::powerOff() {
  return sendSimple(SLinkUnitCommandType::PowerOff);
}

bool SLinkCommandSender::changeDisc(uint16_t disc) {
  SLinkUnitCommand cmd{SLinkUnitCommandType::ChangeDisc, disc, 1};
  return send(cmd);
}

bool SLinkCommandSender::changeTrack(uint8_t track) {
  SLinkUnitCommand cmd{SLinkUnitCommandType::ChangeTrack, 0, track};
  return send(cmd);
}

bool SLinkCommandSender::send(const SLinkUnitCommand& cmd) {
  SLinkUnitCommand resolved = cmd;
  if (!resolveChange(resolved)) return false;

  uint8_t frame[4] = {};
  uint16_t len = 0;
  if (!_frameBuilder.build(resolved, frame, len, _unit)) return false;
  if (_txCallback) _txCallback(frame, len, _txCallbackCtx);
  _tx.sendBytes(frame, len);
  if (resolved.type == SLinkUnitCommandType::ChangeDisc) {
    setCurrentDisc(resolved.disc);
  }
  return true;
}

bool SLinkCommandSender::sendSimple(SLinkUnitCommandType type) {
  SLinkUnitCommand cmd{type, 0, 0};
  return send(cmd);
}

bool SLinkCommandSender::resolveChange(SLinkUnitCommand& cmd) const {
  if (cmd.type == SLinkUnitCommandType::ChangeDisc) {
    if (cmd.disc == 0) return false;
    if (cmd.track == 0) cmd.track = 1;
    return true;
  }
  if (cmd.type == SLinkUnitCommandType::ChangeTrack) {
    if (cmd.track == 0) return false;
    if (cmd.disc == 0) {
      if (!_hasDisc) return false;
      cmd.disc = _currentDisc;
    }
    return true;
  }
  return true;
}
