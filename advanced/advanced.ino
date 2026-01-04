#include "SLinkCommandConsole.h"
#include "SLinkCommandSender.h"
#include "SLinkDecode.h"
#include "SLinkDebugPrinter.h"
#include "SLinkInterface.h"
#include "SLinkPrettyPrinter.h"
#include "SLinkRx.h"
#include "SLinkTx.h"

const uint8_t TX_PIN = 2;
const uint8_t RX_PIN = 21;
SLinkRx slinkRx(RX_PIN);
SLinkTx slinkTx(TX_PIN);
SLinkCommandSender commandSender(slinkTx);
SLinkCommandConsole commandConsole(Serial, commandSender);
SLinkTranslator translator;
SLinkDebugPrinter debugPrinter(Serial);
SLinkPrettyPrinter prettyPrinter(Serial);
SLinkMessage message;
constexpr bool kUsePretty = true;

void setup() {
  Serial.begin(230400);
  slinkRx.begin();
  slinkTx.begin();
}

void loop() {
  commandConsole.poll();
  if (slinkRx.poll(5000)) { // message gap in us
    if (slinkRx.error()) {
      Serial.println("frame error");
      return;
    }
    if (slinkRx.length()) {
      if (translator.decode(slinkRx.data(), slinkRx.length(), message)) {
        SLinkDebugInfo debugInfo = SLinkDispatcher::buildDebugInfo(message);
        if (kUsePretty) {
          SLinkDispatcher::dispatch(message, prettyPrinter, &debugInfo);
        } else {
          SLinkDispatcher::dispatch(message, debugPrinter, &debugInfo);
        }
      }
    }
  }
}
