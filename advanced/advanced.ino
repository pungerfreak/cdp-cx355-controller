#include "SLinkRx.h"

const uint8_t SIG_PIN = 21;
SLinkRx slink(SIG_PIN);

void setup() {
  Serial.begin(230400);
  slink.begin(true);        // INPUT_PULLUP
}

void loop() {
  if (slink.poll(5000)) {   // message gap in us
    if (!slink.error() && slink.length()) {
      const uint8_t* p = slink.data();
      for (uint16_t i = 0; i < slink.length(); i++) {
        if (p[i] < 16) Serial.print('0');
        Serial.print(p[i], HEX);
        Serial.print(' ');
      }
      Serial.println();
    }
  }
}
