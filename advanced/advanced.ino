#include "SLinkCommandConsole.h"
#include "SLinkCommandSender.h"
#include "SLinkDecode.h"
#include "SLinkDebugPrinter.h"
#include "SLinkPrettyPrinter.h"
#include "SLinkRx.h"
#include "SLinkTx.h"

const uint8_t TX_PIN = 7;
const uint8_t RX_PIN = 21;
SLinkRx slinkRx(RX_PIN);
SLinkTx slinkTx(TX_PIN);
SLinkCommandSender commandSender(slinkTx);
SLinkCommandConsole commandConsole(Serial, commandSender);
SLinkTranslator translator;
SLinkDebugPrinter debugPrinter;
SLinkPrettyPrinter prettyPrinter;
SLinkMessage message;
constexpr bool kUsePretty = true;

void setup() {
  Serial.begin(230400);
  slinkRx.begin(true);      // INPUT_PULLUP
  slinkTx.begin(false);
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
        if (kUsePretty) {
          prettyPrinter.print(message, Serial);
        } else {
          debugPrinter.print(message, Serial);
        }
      }
    }
  }
}
