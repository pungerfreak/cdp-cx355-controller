#include "SLinkCommandConsole.h"
#include "SLinkCommandSender.h"
#include "SLinkDecode.h"
#include "SLinkDebugPrinter.h"
#include "SLinkIntentAdapter.h"
#include "SLinkIntentProcessor.h"
#include "SLinkIntentQueue.h"
#include "SLinkInterface.h"
#include "SLinkInterfaceFanout.h"
#include "SLinkPrettyPrinter.h"
#include "SLinkRx.h"
#include "SLinkSerialCallbacks.h"
#include "SLinkSenderStateSync.h"
#include "SLinkTx.h"
#include "SLinkUnitStateStore.h"

const uint8_t TX_PIN         = 2;
const uint8_t RX_PIN         = 21;
constexpr bool debugToSerial = true;

SLinkRx slinkRx(RX_PIN);
SLinkTx slinkTx(TX_PIN);
SLinkCommandSender commandSender(slinkTx);
SLinkIntentQueue intentQueue;
SLinkIntentAdapter intentAdapter(intentQueue);
SLinkIntentProcessor intentProcessor(intentQueue, commandSender);
SLinkCommandConsole commandConsole(Serial, intentAdapter, true);
SLinkTranslator translator;
SLinkDebugPrinter debugPrinter(Serial);
SLinkPrettyPrinter prettyPrinter(Serial);
SLinkUnitStateStore unitStateStore;
SLinkSenderStateSync senderStateSync(commandSender);
SLinkInterfaceFanout stateFanout;
SLinkSerialCallbacks serialCallbacks(Serial, translator, stateFanout, debugPrinter,
                                     prettyPrinter, debugToSerial);

void setup() {
  Serial.begin(230400);
  stateFanout.add(unitStateStore);
  stateFanout.add(senderStateSync);
  commandSender.setTxCallback(SLinkSerialCallbacks::onTxFrame, &serialCallbacks);
  slinkRx.setRxCallback(SLinkSerialCallbacks::onRxFrame, &serialCallbacks);
  slinkRx.begin();
  slinkTx.begin();
}

void loop() {
  commandConsole.poll();
  intentProcessor.poll();
  slinkRx.poll(5000);
}
