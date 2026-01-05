#include "SLinkCommandConsole.h"
#include "SLinkCommandSender.h"
#include "SLinkDecode.h"
#include "SLinkDebugPrinter.h"
#include "SLinkInterface.h"
#include "SLinkPrettyPrinter.h"
#include "SLinkRx.h"
#include "SLinkSerialCallbacks.h"
#include "SLinkStateTracker.h"
#include "SLinkTx.h"

const uint8_t TX_PIN         = 2;
const uint8_t RX_PIN         = 21;
constexpr bool debugToSerial = true;

SLinkRx slinkRx(RX_PIN);
SLinkTx slinkTx(TX_PIN);
SLinkCommandSender commandSender(slinkTx);
SLinkCommandConsole commandConsole(Serial, commandSender);
SLinkTranslator translator;
SLinkDebugPrinter debugPrinter(Serial);
SLinkPrettyPrinter prettyPrinter(Serial);
SLinkStateTracker stateTracker(commandSender);
SLinkSerialCallbacks serialCallbacks(Serial, translator, stateTracker, debugPrinter,
                                     prettyPrinter, debugToSerial);

void setup() {
  Serial.begin(230400);
  commandSender.setTxCallback(SLinkSerialCallbacks::onTxFrame, &serialCallbacks);
  slinkRx.setRxCallback(SLinkSerialCallbacks::onRxFrame, &serialCallbacks);
  slinkRx.begin();
  slinkTx.begin();
}

void loop() {
  commandConsole.poll();
  slinkRx.poll(5000);
}
