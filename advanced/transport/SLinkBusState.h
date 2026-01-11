#pragma once
#include <Arduino.h>

class SLinkBusState {
public:
  void begin();
  void noteRxEdge(uint32_t nowUs);
  void noteTxStart(uint32_t nowUs);
  void noteTxEnd(uint32_t nowUs);

  uint32_t lastRxEdgeUs() const;
  uint32_t lastTxEndUs() const;
  uint32_t lastActivityUs() const;
  bool rxDuringTx() const;
  bool txActive() const;

private:
  volatile uint32_t _lastRxEdgeUs = 0;
  volatile uint32_t _lastTxEndUs = 0;
  volatile bool _txActive = false;
  volatile bool _rxDuringTx = false;
};
