#include "command/SLinkUnitCommandSender.h"

SLinkUnitCommandSender::SLinkUnitCommandSender(SLinkTx& tx, uint8_t unit)
    : _tx(tx), _unit(unit) {}

void SLinkUnitCommandSender::setCurrentDisc(uint16_t disc) {
  if (disc == 0 || disc > 300) return;
  _currentDisc = disc;
  _hasDisc = true;
}

void SLinkUnitCommandSender::setTxCallback(TxCallback cb, void* context) {
  _txCallback = cb;
  _txCallbackCtx = context;
}

bool SLinkUnitCommandSender::play() {
  return sendSimple(SLinkUnitCommandType::Play);
}

bool SLinkUnitCommandSender::stop() {
  return sendSimple(SLinkUnitCommandType::Stop);
}

bool SLinkUnitCommandSender::pause() {
  return sendSimple(SLinkUnitCommandType::Pause);
}

bool SLinkUnitCommandSender::powerOn() {
  return sendSimple(SLinkUnitCommandType::PowerOn);
}

bool SLinkUnitCommandSender::powerOff() {
  return sendSimple(SLinkUnitCommandType::PowerOff);
}

bool SLinkUnitCommandSender::changeDisc(uint16_t disc) {
  SLinkUnitCommand cmd{SLinkUnitCommandType::ChangeDisc, disc, 1};
  return send(cmd);
}

bool SLinkUnitCommandSender::changeTrack(uint8_t track) {
  SLinkUnitCommand cmd{SLinkUnitCommandType::ChangeTrack, 0, track};
  return send(cmd);
}

bool SLinkUnitCommandSender::send(const SLinkUnitCommand& cmd) {
  SLinkUnitCommand resolved = cmd;
  if (!resolveChange(resolved)) return false;

  uint8_t frame[4] = {};
  uint16_t len = 0;
  if (!_frameBuilder.build(resolved, frame, len, _unit)) return false;
  if (_txCallback) _txCallback(frame, len, _txCallbackCtx);
  if (!_tx.sendBytes(frame, len)) return false;
  if (resolved.type == SLinkUnitCommandType::ChangeDisc) {
    setCurrentDisc(resolved.disc);
  }
  return true;
}

bool SLinkUnitCommandSender::sendSimple(SLinkUnitCommandType type) {
  SLinkUnitCommand cmd{type, 0, 0};
  return send(cmd);
}

bool SLinkUnitCommandSender::resolveChange(SLinkUnitCommand& cmd) const {
  if (cmd.type == SLinkUnitCommandType::ChangeDisc) {
    if (cmd.disc == 0) return false;
    if (cmd.track == 0) cmd.track = 1;
    return true;
  }
  if (cmd.type == SLinkUnitCommandType::ChangeTrack) {
    if (cmd.track == 0) return false;
    if (cmd.disc == 0) {
      if (!_hasDisc) {
        cmd.disc = 1;
      } else {
        cmd.disc = _currentDisc;
      }
    }
    return true;
  }
  return true;
}
