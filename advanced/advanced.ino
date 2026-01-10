#include "SLinkCommandConsole.h"
#include "SLinkPrettyPrinter.h"
#include "SLinkSystem.h"

SLinkSystem slinkSystem(Serial);
SLinkCommandConsole slinkConsole(Serial, slinkSystem.intentSource(), true);
SLinkPrettyPrinter slinkPrinter(Serial);

void setup() {
  Serial.begin(230400);
  slinkSystem.attachCommandInput(slinkConsole);
  slinkSystem.attachEventOutput(slinkPrinter);
  slinkSystem.begin();
}

void loop() {
  slinkSystem.poll();
}
