#include "command/SLinkUnitCommandSender.h"

SLinkUnitCommandSender::SLinkUnitCommandSender(SLinkTx& tx, uint8_t unit)
    : _tx(tx), _unit(unit) {}

bool SLinkUnitCommandSender::canSendNow(uint32_t nowUs) const {
  return _tx.canTransmit(nowUs);
}

uint32_t SLinkUnitCommandSender::nextSendUs(uint32_t nowUs) const {
  return _tx.nextTransmitUs(nowUs);
}

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

bool SLinkUnitCommandSender::getCurrentDisc() {
  if (!sendGetCurrentDisc(0x90)) return false;
  setStage(CurrentDiscStage::BankARequested);
  return true;
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
  } else if (resolved.type == SLinkUnitCommandType::GetCurrentDisc) {
    setStage(CurrentDiscStage::BankARequested);
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
      if (!_hasDisc) return false;
      cmd.disc = _currentDisc;
    }
    return true;
  }
  return true;
}

bool SLinkUnitCommandSender::sendGetCurrentDisc(uint8_t unit) {
  SLinkUnitCommand cmd{SLinkUnitCommandType::GetCurrentDisc, 0, 0, unit};
  uint8_t frame[2] = {};
  uint16_t len = 0;
  if (!_frameBuilder.build(cmd, frame, len, unit)) return false;
  if (_txCallback) _txCallback(frame, len, _txCallbackCtx);
  return _tx.sendBytes(frame, len);
}

void SLinkUnitCommandSender::requestCurrentDiscBankB() {
  if (currentDiscStage() != CurrentDiscStage::BankARequested) return;
  if (sendGetCurrentDisc(0x93)) {
    setStage(CurrentDiscStage::BankBRequested);
  }
}

void SLinkUnitCommandSender::completeCurrentDiscRequest() {
  setStage(CurrentDiscStage::Idle);
}

void SLinkUnitCommandSender::setStage(CurrentDiscStage stage) {
  _currentDiscStage = stage;
}

SLinkUnitCommandSender::CurrentDiscStage SLinkUnitCommandSender::currentDiscStage() const {
  return _currentDiscStage;
}
