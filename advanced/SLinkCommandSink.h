#pragma once
#include <Arduino.h>
#include "SLinkCommands.h"

class SLinkCommandSink {
public:
  virtual void sendCommand(SLinkCommandId id) = 0;
};
