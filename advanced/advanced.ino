#include "SLinkSystem.h"

SLinkSystem slinkSystem(Serial);

void setup() {
  Serial.begin(230400);
  slinkSystem.begin();
}

void loop() {
  slinkSystem.poll();
}
