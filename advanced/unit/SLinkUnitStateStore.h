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
  void resetTrackCount();
  void resetDiscLength();

  uint16_t _currentDisc = 0;
  bool _hasDisc = false;
  uint8_t _currentTrack = 0;
  bool _hasTrack = false;
  SLinkTransportState _transport = SLinkTransportState::Unknown;
  uint8_t _trackCount = 0;
  bool _hasTrackCount = false;
  bool _trackCountValid = false;
  bool _hasDiscLength = false;
  bool _discLengthValid = false;
  uint8_t _discMinutes = 0;
  uint8_t _discSeconds = 0;
  uint8_t _discFrames = 0;
};
