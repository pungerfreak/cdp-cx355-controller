#pragma once
#include <Arduino.h>
#include "command/SLinkCommandInput.h"
#include "command/SLinkCommandIntentSource.h"

class SLinkCommandConsole : public SLinkCommandInput {
public:
  SLinkCommandConsole(Stream& io, SLinkCommandIntentSource& input, bool printTx);

  void poll() override;
  void handleLine(const char* line);
  void printHelp();

private:
  static constexpr uint8_t kBufferSize = 32;

  bool normalizeCommand(const char* in, char* out, uint8_t outSize) const;
  bool isHelpCommand(const char* cmd) const;
  bool parseNumber(const char* in, uint16_t& value) const;
  bool parsePrefixedNumber(const char* cmd, const char* prefix, uint16_t& value) const;
  bool dispatchSimple(const char* cmd);
  bool dispatchDisc(const char* cmd);
  bool dispatchTrack(const char* cmd);
  void printTx(const char* label);
  void printTx(const char* label, uint16_t value);

  Stream& _io;
  SLinkCommandIntentSource& _input;
  bool _printTx = true;
  char _buf[kBufferSize];
  uint8_t _len = 0;
};
