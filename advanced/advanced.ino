#include "SLinkSystem.h"

SLinkSystem slinkSystem(Serial);

void setup() {
  slinkSystem.begin();
}

void loop() {
  slinkSystem.poll();
}
