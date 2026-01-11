#include "io/SLinkCommandConsole.h"
#include "io/SLinkPrettyPrinter.h"
#include "system/SLinkSystem.h"

constexpr bool kDebugToSerial = true;

SLinkSystem slinkSystem(Serial, kDebugToSerial);
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
