#include "SLinkSymbolDecoder.h"

void SLinkSymbolDecoder::reset() {
  _pendingDelta = 0;
}

SLinkSymbolDecoder::SymbolType SLinkSymbolDecoder::decodeDelta(uint32_t dt) {
  static constexpr uint32_t kGlitchMinUs = 900;
  static constexpr uint32_t kBit0MinUs = 900;
  static constexpr uint32_t kBit1MinUs = 1500;
  static constexpr uint32_t kSyncMinUs = 2100;

  uint32_t total = dt + _pendingDelta;
  if (total < kGlitchMinUs) {
    _pendingDelta = total;
    return SymbolType::None;
  }
  _pendingDelta = 0;

  if (total == 0) return SymbolType::None;

  if (total >= kSyncMinUs) return SymbolType::Sync;
  if (total >= kBit1MinUs) return SymbolType::Bit1;
  if (total >= kBit0MinUs) return SymbolType::Bit0;

  return SymbolType::Error;
}
