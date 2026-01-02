#pragma once
#include <Arduino.h>
#include "SLinkDecode.h"

class SLinkPrettyPrinter {
public:
  void print(const SLinkMessage& msg, Stream& out) const;
};
