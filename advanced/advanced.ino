#include "SLinkDecode.h"
#include "SLinkRx.h"

const uint8_t SIG_PIN = 21;
SLinkRx slink(SIG_PIN);
SLinkTranslator translator;

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
      translator.printMessage(slink.data(), slink.length(), Serial);
    }
  }
}
