#include "unit/SLinkUnitStateStore.h"

void SLinkUnitStateStore::updateDisc(const SLinkDiscInfo& disc) {
  if (!disc.present || !disc.valid) return;
  if (_hasDisc && _currentDisc == disc.disc) return;
  _currentDisc = disc.disc;
  _hasDisc = true;
}

void SLinkUnitStateStore::updateTrack(const SLinkTrackInfo& track) {
  if (!track.present || !track.valid) return;
  if (_hasTrack && _currentTrack == track.track) return;
  _currentTrack = track.track;
  _hasTrack = true;
}

void SLinkUnitStateStore::onUnitEvent(const SLinkUnitEvent& event) {
  switch (event.type) {
    case SLinkUnitEventType::DiscChanged:
      updateDisc(event.disc);
      break;
    case SLinkUnitEventType::TrackChanged:
      updateDisc(event.disc);
      updateTrack(event.track);
      break;
    case SLinkUnitEventType::TransportStateChanged:
    case SLinkUnitEventType::Unknown:
      break;
  }
}

void SLinkUnitStateStore::setInitialState(uint16_t disc, uint8_t track) {
  if (disc > 0) {
    _currentDisc = disc;
    _hasDisc = true;
  }
  if (track > 0) {
    _currentTrack = track;
    _hasTrack = true;
  }
}

void SLinkUnitStateStore::stateInfo(SLinkDiscInfo& disc, SLinkTrackInfo& track) const {
  disc = {};
  track = {};
  if (_hasDisc) {
    disc.present = true;
    disc.valid = true;
    disc.disc = _currentDisc;
  }
  if (_hasTrack) {
    track.present = true;
    track.valid = true;
    track.track = _currentTrack;
  }
}

bool SLinkUnitStateStore::hasDisc() const {
  return _hasDisc;
}

uint16_t SLinkUnitStateStore::currentDisc() const {
  return _currentDisc;
}

bool SLinkUnitStateStore::hasTrack() const {
  return _hasTrack;
}

uint8_t SLinkUnitStateStore::currentTrack() const {
  return _currentTrack;
}
