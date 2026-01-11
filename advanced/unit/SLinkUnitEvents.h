#pragma once
#include <Arduino.h>
#include "unit/SLinkUnitEventHandler.h"

enum class SLinkUnitEventType : uint8_t {
  TransportStateChanged,
  DiscChanged,
  TrackChanged,
  Unknown
};

struct SLinkUnitEvent {
  SLinkUnitEventType type;
  SLinkDiscInfo disc;
  SLinkTrackInfo track;
  const SLinkDebugInfo* debug;
};

class SLinkUnitEventObserver {
public:
  virtual ~SLinkUnitEventObserver() = default;
  virtual void onUnitEvent(const SLinkUnitEvent& event) = 0;
};
