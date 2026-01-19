#pragma once
#include <Arduino.h>
#include "unit/SLinkUnitEventHandler.h"

enum class SLinkTransportState : uint8_t {
  Unknown = 0,
  Unchanged,
  Stopped,
  Playing,
  Paused
};

enum class SLinkUnitEventType : uint8_t {
  TransportStateChanged,
  DiscChanged,
  TrackChanged,
  CurrentDiscInfo,
  CurrentDiscBankSwitchNeeded,
  Unknown
};

struct SLinkUnitEvent {
  SLinkUnitEventType type;
  SLinkDiscInfo disc;
  SLinkTrackInfo track;
  SLinkTransportState transport;
  const SLinkDebugInfo* debug;
};

class SLinkUnitEventObserver {
public:
  virtual ~SLinkUnitEventObserver() = default;
  virtual void onUnitEvent(const SLinkUnitEvent& event) = 0;
};
