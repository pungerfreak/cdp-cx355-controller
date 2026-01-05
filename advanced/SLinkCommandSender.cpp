#include "SLinkCommandSender.h"
#include <string.h>

SLinkCommandSender::SLinkCommandSender(SLinkTx& tx, uint8_t unit)
    : _tx(tx), _unit(unit) {}

void SLinkCommandSender::setUnit(uint8_t unit) {
  _unit = unit;
}

void SLinkCommandSender::setCurrentDisc(uint16_t disc) {
  if (disc == 0 || disc > 300) return;
  _currentDisc = disc;
  _hasDisc = true;
}

void SLinkCommandSender::setTxCallback(TxCallback cb, void* context) {
  _txCallback = cb;
  _txCallbackCtx = context;
}

bool SLinkCommandSender::takeLastFrame(uint8_t* out, uint16_t& len) {
  if (!out) return false;
  if (!_lastReady || _lastLen == 0) return false;
  uint16_t n = _lastLen;
  if (n > sizeof(_lastFrame)) n = sizeof(_lastFrame);
  memcpy(out, _lastFrame, n);
  len = n;
  _lastReady = false;
  return true;
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
  if (!sendChange(disc, 1)) return false;
  _currentDisc = disc;
  _hasDisc = true;
  return true;
}

bool SLinkCommandSender::changeTrack(uint8_t track) {
  if (!_hasDisc) return false;
  if (track == 0) return false;
  return sendChange(_currentDisc, track);
}

bool SLinkCommandSender::sendCommand(SLinkCommandId id) {
  SLinkCommand cmd = {_unit, id};
  return sendCommand(cmd);
}

bool SLinkCommandSender::sendCommand(const SLinkCommand& cmd) {
  uint8_t frame[2];
  encodeCommand(cmd, frame);
  storeLastFrame(frame, sizeof(frame));
  if (_txCallback) _txCallback(frame, sizeof(frame), _txCallbackCtx);
  _tx.sendBytes(frame, sizeof(frame));
  return true;
}

bool SLinkCommandSender::sendChange(uint16_t disc, uint8_t track) {
  static constexpr uint8_t kCmdChangeTrack = 0x50;
  uint8_t unit = 0;
  uint8_t discRaw = 0;
  uint8_t trackRaw = 0;
  if (!encodeDiscUnit(disc, unit)) return false;
  if (!encodeDiscRaw(disc, discRaw)) return false;
  if (!encodeBcd(track, trackRaw)) return false;

  uint8_t frame[4] = {unit, kCmdChangeTrack, discRaw, trackRaw};
  storeLastFrame(frame, sizeof(frame));
  if (_txCallback) _txCallback(frame, sizeof(frame), _txCallbackCtx);
  _tx.sendBytes(frame, sizeof(frame));
  return true;
}

bool SLinkCommandSender::encodeDiscRaw(uint16_t disc, uint8_t& raw) const {
  if (disc >= 1 && disc <= 200) {
    raw = (uint8_t)(disc + 0x36);
    return true;
  }
  if (disc >= 201 && disc <= 300) {
    raw = (uint8_t)(disc - 200);
    return true;
  }
  return false;
}

bool SLinkCommandSender::encodeDiscUnit(uint16_t disc, uint8_t& unit) const {
  if (disc >= 1 && disc <= 200) {
    unit = 0x90;
    return true;
  }
  if (disc >= 201 && disc <= 300) {
    unit = 0x9C;
    return true;
  }
  return false;
}

bool SLinkCommandSender::encodeBcd(uint8_t value, uint8_t& raw) const {
  if (value > 99) return false;
  raw = (uint8_t)(((value / 10) << 4) | (value % 10));
  return true;
}

void SLinkCommandSender::storeLastFrame(const uint8_t* data, uint16_t len) {
  if (!data || len == 0) return;
  uint16_t n = len;
  if (n > sizeof(_lastFrame)) n = sizeof(_lastFrame);
  memcpy(_lastFrame, data, n);
  _lastLen = n;
  _lastReady = true;
}
