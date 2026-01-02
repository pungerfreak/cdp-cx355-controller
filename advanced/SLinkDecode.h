#pragma once
#include <Arduino.h>

extern const char kSLinkUnknownName[];

struct SLinkPattern {
  static constexpr uint8_t PREFIX_MAX = 8;

  const char* name;
  const char* note;
  uint8_t minLen;
  uint8_t maxLen;     // 0 means "no max"
  uint8_t prefixLen;  // match up to PREFIX_MAX bytes
  uint8_t prefix[PREFIX_MAX];
  uint8_t mask[PREFIX_MAX];
};

struct SLinkMessage {
  static constexpr uint16_t RAW_MAX = 64;

  uint8_t  raw[RAW_MAX];
  uint16_t len = 0;
  bool     truncated = false;

  uint8_t     unit = 0;
  uint8_t     cmd = 0;
  const char* name = nullptr;
  const char* note = nullptr;

  bool     hasDisc = false;
  uint8_t  discRaw = 0;
  bool     discValid = false;
  uint16_t disc = 0;

  bool    hasTrack = false;
  uint8_t trackRaw = 0;
  bool    trackValid = false;
  uint8_t track = 0;
  bool    hasTrackLength = false;
  uint8_t trackMinRaw = 0;
  uint8_t trackSecRaw = 0;
  bool    trackMinValid = false;
  bool    trackSecValid = false;
  uint8_t trackMin = 0;
  uint8_t trackSec = 0;
  uint8_t extraLen = 0;
  uint8_t extra[2] = {0, 0};

  bool    hasChecksum = false;
  uint8_t checksumLast = 0;
  uint8_t checksumSum = 0;
  uint8_t checksumXor = 0;
  bool    checksumSumMatch = false;
  bool    checksumSumInvMatch = false;
  bool    checksumXorMatch = false;
  bool    checksumXorInvMatch = false;
};

class SLinkTranslator {
public:
  bool decode(const uint8_t* data, uint16_t len, SLinkMessage& out) const;

private:
  const SLinkPattern* matchPattern(const uint8_t* data, uint16_t len) const;
};
