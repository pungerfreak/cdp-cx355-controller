#include "command/SLinkCommandConsole.h"
#include "debug/SLinkPrettyPrinter.h"
#include "system/SLinkSystem.h"

SLinkSystem slinkSystem(Serial);
SLinkCommandConsole slinkConsole(Serial, slinkSystem.intentSource(), true);
SLinkPrettyPrinter slinkPrinter(Serial);

void setup() {
  Serial.begin(230400);
  slinkSystem.addCommandInput(slinkConsole);
  slinkSystem.addEventOutput(slinkPrinter);
  slinkSystem.begin();
}

void loop() {
  slinkSystem.poll();
}
