#include "unit/SLinkUnitStateStore.h"

void SLinkUnitStateStore::resetTrackCount() {
  _trackCount = 0;
  _trackCountValid = false;
  _hasTrackCount = false;
}

void SLinkUnitStateStore::resetDiscLength() {
  _discMinutes = 0;
  _discSeconds = 0;
  _discFrames = 0;
  _discLengthValid = false;
  _hasDiscLength = false;
}

void SLinkUnitStateStore::updateDisc(const SLinkDiscInfo& disc) {
  if (!disc.present || !disc.valid) return;
  const bool discChanged = (!_hasDisc || _currentDisc != disc.disc);
  _currentDisc = disc.disc;
  _hasDisc = true;
  if (discChanged) {
    _currentTrack = 0;
    _hasTrack = false;
    resetTrackCount();
    resetDiscLength();
  }
  if (disc.hasTrackCount) {
    _hasTrackCount = true;
    _trackCountValid = disc.trackCountValid;
    _trackCount = disc.trackCount;
  }
  if (disc.hasDiscLength) {
    _hasDiscLength = true;
    _discLengthValid = disc.discLengthValid;
    _discMinutes = disc.discMinutes;
    _discSeconds = disc.discSeconds;
    _discFrames = disc.discFrames;
  }
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
      if (!event.disc.present || !event.disc.valid
          || !_hasDisc || event.disc.disc != _currentDisc) {
        _currentTrack = 0;
        _hasTrack = false;
        resetTrackCount();
        resetDiscLength();
      }
      updateDisc(event.disc);
      break;
    case SLinkUnitEventType::TrackChanged:
      updateDisc(event.disc);
      updateTrack(event.track);
      break;
    case SLinkUnitEventType::CurrentDiscInfo:
      updateDisc(event.disc);
      updateTrack(event.track);
      break;
    case SLinkUnitEventType::Status:
      updateDisc(event.disc);
      if (event.transport == SLinkTransportState::PowerOff) {
        if (event.track.present && event.track.valid) {
          updateTrack(event.track);
        } else {
          _currentTrack = 0;
          _hasTrack = false;
        }
        _transport = SLinkTransportState::Stopped;
        break;
      }
      updateTrack(event.track);
      if (event.transport != SLinkTransportState::Unchanged) {
        _transport = event.transport;
      }
      break;
    case SLinkUnitEventType::TransportStateChanged:
      if (event.transport != SLinkTransportState::Unchanged) {
        _transport = event.transport;
      }
      break;
    case SLinkUnitEventType::CurrentDiscBankSwitchNeeded:
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
    if (_hasTrackCount) {
      disc.hasTrackCount = true;
      disc.trackCountValid = _trackCountValid;
      disc.trackCount = _trackCount;
    }
    if (_hasDiscLength) {
      disc.hasDiscLength = true;
      disc.discLengthValid = _discLengthValid;
      disc.discMinutes = _discMinutes;
      disc.discSeconds = _discSeconds;
      disc.discFrames = _discFrames;
    }
  }
  if (_hasTrack) {
    track.present = true;
    track.valid = true;
    track.track = _currentTrack;
  }
  track.transport = _transport;
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

SLinkTransportState SLinkUnitStateStore::transportState() const {
  return _transport;
}
