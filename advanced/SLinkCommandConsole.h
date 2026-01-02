#pragma once
#include <Arduino.h>
#include "SLinkCommandSink.h"

class SLinkCommandConsole {
public:
  SLinkCommandConsole(Stream& io, SLinkCommandSink& sink);

  void poll();
  void handleLine(const char* line);
  void printHelp();

private:
  static constexpr uint8_t kBufferSize = 32;

  bool normalizeCommand(const char* in, char* out, uint8_t outSize) const;
  bool parseCommand(const char* cmd, SLinkCommandId& id) const;
  bool isHelpCommand(const char* cmd) const;

  Stream& _io;
  SLinkCommandSink& _sink;
  char _buf[kBufferSize];
  uint8_t _len = 0;
};
