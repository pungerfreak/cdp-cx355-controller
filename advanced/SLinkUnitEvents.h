#pragma once
#include <Arduino.h>
#include "SLinkInterface.h"

enum class SLinkUnitEventType : uint8_t {
  Play,
  Stop,
  Pause,
  PowerOn,
  PowerOff,
  ChangeDisc,
  ChangeTrack,
  Ready,
  ChangingDisc,
  DiscReady,
  DiscLoaded,
  LoadingDisc,
  ChangingTrack,
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
