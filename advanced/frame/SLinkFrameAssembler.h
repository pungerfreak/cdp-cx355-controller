#pragma once
#include <Arduino.h>

class SLinkFrameAssembler {
public:
  static constexpr uint16_t BYTES_MAX = 64;

  void reset();
  void onSync();
  void onBit(uint8_t bit);
  void abortWithError();

  bool inFrame() const;
  bool hasError() const;
  uint16_t bitCount() const;
  uint16_t messageLength() const;
  void copyMessage(uint8_t* dst, uint16_t maxLen, uint16_t& outLen) const;

private:
  void resetFrame();

  uint8_t _msg[BYTES_MAX];
  uint16_t _msgLen = 0;
  uint8_t _curByte = 0;
  uint8_t _bitCount = 0;
  bool _inFrame = false;
  bool _msgError = false;
};
