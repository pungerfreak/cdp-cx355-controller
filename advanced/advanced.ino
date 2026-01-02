#include "SLinkDecode.h"
#include "SLinkDebugPrinter.h"
#include "SLinkPrettyPrinter.h"
#include "SLinkRx.h"

const uint8_t SIG_PIN = 21;
SLinkRx slink(SIG_PIN);
SLinkTranslator translator;
SLinkDebugPrinter debugPrinter;
SLinkPrettyPrinter prettyPrinter;
SLinkMessage message;
constexpr bool kUsePretty = true;

void setup() {
  Serial.begin(230400);
  slink.begin(true);        // INPUT_PULLUP
}

void loop() {
  if (slink.poll(5000)) {   // message gap in us
    if (slink.error()) {
      Serial.println("frame error");
      return;
    }
    if (slink.length()) {
      if (translator.decode(slink.data(), slink.length(), message)) {
        if (kUsePretty) {
          prettyPrinter.print(message, Serial);
        } else {
          debugPrinter.print(message, Serial);
        }
      }
    }
  }
}
