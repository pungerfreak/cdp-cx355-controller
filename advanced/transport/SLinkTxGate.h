#pragma once
#include <Arduino.h>
#include "transport/SLinkBusState.h"

class SLinkTxGate {
public:
  explicit SLinkTxGate(SLinkBusState& bus);

  void begin();
  bool canTransmit(uint32_t nowUs) const;
  uint32_t nextTransmitUs(uint32_t nowUs) const;
  bool shouldAbort() const;
  void beginTx(uint32_t nowUs);
  void endTx(uint32_t nowUs, bool aborted);
  void setProtocolAllowsTx(bool allow);

private:
  uint32_t randomBackoffUs() const;

  SLinkBusState& _bus;
  uint32_t _idleGapUs = 2500;
  uint32_t _guardTimeUs = 600;
  uint32_t _backoffMinUs = 4000;
  uint32_t _backoffMaxUs = 12000;
  uint32_t _backoffUntilUs = 0;
  bool _protocolAllowsTx = true;
};
