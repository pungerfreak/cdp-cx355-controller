#pragma once
#include <Arduino.h>

class SLinkSymbolDecoder {
public:
  enum class SymbolType : uint8_t {
    None,
    Sync,
    Bit0,
    Bit1,
    Error
  };

  void reset();
  SymbolType decodeDelta(uint32_t dt);

private:
  uint32_t _pendingDelta = 0;
};
