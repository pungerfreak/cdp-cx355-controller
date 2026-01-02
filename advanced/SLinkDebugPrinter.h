#pragma once
#include <Arduino.h>
#include "SLinkDecode.h"

class SLinkDebugPrinter {
public:
  void print(const SLinkMessage& msg, Stream& out) const;
};
