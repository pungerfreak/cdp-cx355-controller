const byte SIG_PIN = 21;

volatile uint32_t lastFallUs = 0;
volatile uint32_t lastRiseUs = 0;
volatile bool     haveNewSymbol = false;
volatile bool     lineIsHigh = false;

const uint16_t BUF_MAX = 256;
char buffer[BUF_MAX];
volatile uint16_t bufLen = 0;

inline void bufClearISR() { bufLen = 0; }

inline void bufPushISR(char c) {
  if (bufLen < (BUF_MAX - 1)) buffer[bufLen++] = c;
}

void onChange() {
  uint32_t now = micros();
  bool level = digitalRead(SIG_PIN);

  if (level) {
    // RISING: end of LOW pulse
    lineIsHigh = true;
    lastRiseUs = now;

    uint32_t lowWidth = now - lastFallUs;

    if (lowWidth > 2300) {          // SYNC ~2400us low
      bufClearISR();
      bufPushISR('S');
    } else if (lowWidth > 1100) {   // '1' ~1200us low
      bufPushISR('1');
    } else if (lowWidth > 500) {    // '0' ~600us low
      bufPushISR('0');
    } else {
      bufPushISR('E');
    }

    haveNewSymbol = true;
  } else {
    // FALLING: start of next LOW pulse (end of HIGH delimiter)
    lineIsHigh = false;
    lastFallUs = now;
  }
}

void setup() {
  pinMode(SIG_PIN, INPUT);
  Serial.begin(230400);
  attachInterrupt(digitalPinToInterrupt(SIG_PIN), onChange, CHANGE);
}

// Converts a '0'/'1' char buffer into hex string.
// Returns number of hex chars written.
uint16_t bitsToHex(const char *bits, uint16_t bitLen,
                   char *hexOut, uint16_t hexMax)
{
  uint16_t hexLen = 0;
  uint8_t byte = 0;
  uint8_t count = 0;

  for (uint16_t i = 0; i < bitLen; i++) {
    char c = bits[i];
    if (c != '0' && c != '1') continue;  // skip 'S', 'E', etc

    byte = (byte << 1) | (c == '1');
    count++;

    if (count == 8) {
      if (hexLen + 2 >= hexMax) break;

      uint8_t hi = (byte >> 4) & 0x0F;
      uint8_t lo = byte & 0x0F;

      hexOut[hexLen++] = hi < 10 ? ('0' + hi) : ('A' + hi - 10);
      hexOut[hexLen++] = lo < 10 ? ('0' + lo) : ('A' + lo - 10);
      hexOut[hexLen++] = ' ';

      byte = 0;
      count = 0;
    }
  }

  hexOut[hexLen] = '\0';
  return hexLen;
}


void loop() {
  const uint32_t FLUSH_HIGH_US = 700;  // detect “missing fall” after rise

  uint32_t riseSnap;
  bool readySnap, highSnap;

  noInterrupts();
  riseSnap  = lastRiseUs;
  readySnap = haveNewSymbol;
  highSnap  = lineIsHigh;
  interrupts();

  // Flush ONLY when the line has stayed HIGH too long since the last RISE
  if (readySnap && highSnap && (micros() - riseSnap) > FLUSH_HIGH_US) {
    char line[BUF_MAX];
    uint16_t n;

    noInterrupts();
    n = bufLen;
    if (n >= BUF_MAX) n = BUF_MAX - 1;
    memcpy(line, buffer, n);
    line[n] = '\0';
    bufLen = 0;
    haveNewSymbol = false;
    interrupts();

    char hex[BUF_MAX];

    uint16_t hexLen = bitsToHex(line, strlen(line), hex, sizeof(hex));

    if (hexLen > 0) {
      Serial.println(hex);
    }
  }
}
