#include "frame/SLinkFrameAssembler.h"

void SLinkFrameAssembler::reset() {
  _msgLen = 0;
  _curByte = 0;
  _bitCount = 0;
  _inFrame = false;
  _msgError = false;
}

void SLinkFrameAssembler::resetFrame() {
  _msgLen = 0;
  _curByte = 0;
  _bitCount = 0;
  _inFrame = true;
  _msgError = false;
}

void SLinkFrameAssembler::onSync() {
  if (_inFrame && (_bitCount % 8) != 0) {
    _msgError = true;
  }
  resetFrame();
}

void SLinkFrameAssembler::onBit(uint8_t bit) {
  if (!_inFrame) return;
  _curByte = (uint8_t)((_curByte << 1) | (bit & 1));
  _bitCount++;
  if ((_bitCount & 7) == 0) {
    if (_msgLen < BYTES_MAX) _msg[_msgLen++] = _curByte;
    else _msgError = true;
    _curByte = 0;
  }
}

void SLinkFrameAssembler::abortWithError() {
  _msgError = true;
  _inFrame = false;
}

bool SLinkFrameAssembler::inFrame() const {
  return _inFrame;
}

bool SLinkFrameAssembler::hasError() const {
  return _msgError;
}

uint16_t SLinkFrameAssembler::bitCount() const {
  return _bitCount;
}

uint16_t SLinkFrameAssembler::messageLength() const {
  return _msgLen;
}

void SLinkFrameAssembler::copyMessage(uint8_t* dst, uint16_t maxLen, uint16_t& outLen) const {
  uint16_t n = _msgLen;
  if (n > maxLen) n = maxLen;
  memcpy(dst, _msg, n);
  outLen = n;
}
