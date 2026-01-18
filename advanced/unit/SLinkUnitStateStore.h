#pragma once
#include "unit/SLinkUnitEvents.h"

class SLinkUnitStateStore : public SLinkUnitEventObserver {
public:
  void onUnitEvent(const SLinkUnitEvent& event) override;

  void setInitialState(uint16_t disc, uint8_t track);
  void stateInfo(SLinkDiscInfo& disc, SLinkTrackInfo& track) const;
  bool hasDisc() const;
  uint16_t currentDisc() const;
  bool hasTrack() const;
  uint8_t currentTrack() const;
  SLinkTransportState transportState() const;

private:
  void updateDisc(const SLinkDiscInfo& disc);
  void updateTrack(const SLinkTrackInfo& track);

  uint16_t _currentDisc = 0;
  bool _hasDisc = false;
  uint8_t _currentTrack = 0;
  bool _hasTrack = false;
  SLinkTransportState _transport = SLinkTransportState::Unknown;
};
