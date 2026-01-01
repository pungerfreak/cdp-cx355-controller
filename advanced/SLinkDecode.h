#pragma once
#include <Arduino.h>

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

class SLinkTranslator {
public:
  void printMessage(const uint8_t* data, uint16_t len, Stream& out) const;

private:
  const SLinkPattern* matchPattern(const uint8_t* data, uint16_t len) const;
  void printHexByte(uint8_t b, Stream& out) const;
  void printHex(const uint8_t* data, uint16_t len, Stream& out) const;
  void printAscii(const uint8_t* data, uint16_t len, Stream& out) const;
  void printChecksumHints(const uint8_t* data, uint16_t len, Stream& out) const;
};
