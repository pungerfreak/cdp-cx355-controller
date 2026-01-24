#include "cddb/CddbLookup.h"
#include <stdio.h>

CddbLookup::CddbLookup(SLinkSystem& system, const CddbLookupConfig& config)
    : system_(system), intents_(system.intentSource()), config_(config) {}

void CddbLookup::start() {
  if (started_) return;
  system_.addUnitObserver(*this);
  started_ = true;
}

void CddbLookup::stop() {
  if (!started_) return;
  system_.removeUnitObserver(*this);
  started_ = false;
}

bool CddbLookup::lookup(uint16_t disc) {
  if (busy()) return false;

  resetState_();
  targetDisc_ = disc;

  SLinkDiscInfo discInfo;
  SLinkTrackInfo trackInfo;
  system_.getUnitStateSnapshot(discInfo, trackInfo);
  if (targetDisc_ == 0 && discInfo.present && discInfo.valid) {
    targetDisc_ = discInfo.disc;
  }

  startMs_ = millis();
  lastProgressMs_ = startMs_;

  if (targetDisc_ == 0) {
    state_ = State::WaitingForDisc;
    requestStatus_();
    return true;
  }

  beginCollection_();
  return true;
}

bool CddbLookup::busy() const {
  return state_ == State::WaitingForDisc || state_ == State::Collecting;
}

bool CddbLookup::hasResult() const {
  return result_.ready;
}

const CddbLookup::Result& CddbLookup::result() const {
  return result_;
}

void CddbLookup::tick(uint32_t nowMs) {
  if (state_ == State::Idle || state_ == State::Complete || state_ == State::Failed) return;
  const uint32_t timeoutMs = 8000;
  if ((int32_t)(nowMs - lastProgressMs_) > (int32_t)timeoutMs) {
    finalizeFailure_();
  }
}

void CddbLookup::onUnitEvent(const SLinkUnitEvent& e) {
  if (state_ == State::Idle || state_ == State::Complete || state_ == State::Failed) {
    return;
  }

  if (e.disc.present && e.disc.valid) {
    handleDiscEvent_(e.disc);
  }

  if (state_ == State::WaitingForDisc && targetDisc_ != 0) {
    beginCollection_();
    return;
  }

  switch (e.type) {
    case SLinkUnitEventType::CurrentDiscInfo:
    case SLinkUnitEventType::TrackChanged:
      handleTrackEvent_(e.track);
      break;
    case SLinkUnitEventType::DiscChanged:
      if (state_ == State::Collecting && targetDisc_ != 0 && e.disc.disc != targetDisc_) {
        finalizeFailure_();
      }
      break;
    default:
      break;
  }
}

void CddbLookup::resetState_() {
  state_ = State::Idle;
  targetDisc_ = 0;
  requestedTrack_ = 0;
  highestTrackSeen_ = 0;
  result_ = Result{};
  for (uint8_t i = 0; i < kMaxTracks; ++i) {
    tracks_[i] = TrackLength{};
  }
}

void CddbLookup::beginCollection_() {
  if (targetDisc_ == 0) {
    finalizeFailure_();
    return;
  }
  state_ = State::Collecting;
  requestedTrack_ = 1;
  highestTrackSeen_ = 0;
  lastProgressMs_ = millis();
  if (!requestTrack_(requestedTrack_)) {
    finalizeFailure_();
  }
}

void CddbLookup::handleDiscEvent_(const SLinkDiscInfo& disc) {
  if (!disc.present || !disc.valid) return;
  if (targetDisc_ == 0) {
    targetDisc_ = disc.disc;
  }
}

void CddbLookup::handleTrackEvent_(const SLinkTrackInfo& track) {
  if (state_ != State::Collecting) return;
  if (!track.present || !track.valid) {
    finalizeFailure_();
    return;
  }

  // If the unit wrapped back to track 1 while scanning higher tracks, stop and use what we have.
  if (requestedTrack_ > 1 && track.track == 1) {
    finalizeSuccess_();
    return;
  }

  if (track.track == 0 || track.track > kMaxTracks) {
    finalizeFailure_();
    return;
  }
  if (track.track != requestedTrack_) return;
  if (!track.lengthPresent || !track.lengthValid) {
    finalizeFailure_();
    return;
  }

  const uint8_t index = static_cast<uint8_t>(track.track - 1);
  tracks_[index].present = true;
  tracks_[index].number = track.track;
  tracks_[index].minutes = track.minutes;
  tracks_[index].seconds = track.seconds;
  if (track.track > highestTrackSeen_) {
    highestTrackSeen_ = track.track;
  }
  lastProgressMs_ = millis();

  if (track.track >= kMaxTracks) {
    finalizeSuccess_();
    return;
  }

  const uint8_t nextTrack = static_cast<uint8_t>(track.track + 1);
  if (!requestTrack_(nextTrack)) {
    finalizeFailure_();
  }
}

void CddbLookup::requestStatus_() {
  intents_.getStatus();
}

bool CddbLookup::requestTrack_(uint8_t track) {
  requestedTrack_ = track;
  return intents_.changeTrack(track);
}

void CddbLookup::finalizeSuccess_() {
  if (state_ == State::Complete || state_ == State::Failed) return;
  if (!buildResult_()) {
    finalizeFailure_();
    return;
  }
  state_ = State::Complete;
  result_.ready = true;
  result_.success = true;
}

void CddbLookup::finalizeFailure_() {
  if (state_ == State::Complete || state_ == State::Failed) return;
  state_ = State::Failed;
  result_.ready = true;
  result_.success = false;
}

bool CddbLookup::buildResult_() {
  if (targetDisc_ == 0 || highestTrackSeen_ == 0) return false;
  for (uint8_t i = 0; i < highestTrackSeen_; ++i) {
    if (!tracks_[i].present) {
      return false;
    }
  }

  result_.disc = targetDisc_;
  result_.trackCount = highestTrackSeen_;
  result_.leadInFrames = kDefaultLeadInFrames;

  uint32_t offsets[kMaxTracks] = {};
  uint32_t curFrames = result_.leadInFrames;
  uint32_t sumTrackFrames = 0;
  for (uint8_t i = 0; i < result_.trackCount; ++i) {
    offsets[i] = curFrames;
    const TrackLength& t = tracks_[i];
    const uint32_t frames = toFrames_(t.minutes, t.seconds);
    sumTrackFrames += frames;
    curFrames += frames;
    result_.trackSeconds[i] = static_cast<uint16_t>(toSeconds_(t.minutes, t.seconds));
  }
  result_.totalSeconds = sumTrackFrames / 75;

  uint32_t sum = 0;
  for (uint8_t i = 0; i < result_.trackCount; ++i) {
    sum += cddbSumDigits_(offsets[i] / 75);
  }
  result_.discId = ((sum % 255) << 24) | ((result_.totalSeconds & 0xFFFF) << 8)
                   | (result_.trackCount & 0xFF);

  char discHex[9];
  snprintf(discHex, sizeof(discHex), "%08lx", static_cast<unsigned long>(result_.discId));

  String cmd = "cddb+query+";
  cmd += discHex;
  cmd += "+";
  cmd += result_.trackCount;
  for (uint8_t i = 0; i < result_.trackCount; ++i) {
    cmd += "+";
    cmd += offsets[i];
  }
  cmd += "+";
  cmd += result_.totalSeconds;
  result_.cmd = cmd;

  const char* subdomain = (config_.apiSubdomain && config_.apiSubdomain[0])
                              ? config_.apiSubdomain
                              : "freedb";
  const char* email = (config_.email && config_.email[0])
                          ? config_.email
                          : "email@example.com";
  const char* appName = (config_.appName && config_.appName[0])
                            ? config_.appName
                            : "s-link";
  const char* appVersion = (config_.appVersion && config_.appVersion[0])
                               ? config_.appVersion
                               : "0.1";

  result_.url = "http://";
  result_.url += subdomain;
  result_.url += ".gnudb.org/~cddb/cddb.cgi?cmd=";
  result_.url += cmd;
  result_.url += "&hello=";
  result_.url += email;
  result_.url += "+";
  result_.url += appName;
  result_.url += "+";
  result_.url += appVersion;
  result_.url += "&proto=6";

  return true;
}

uint32_t CddbLookup::cddbSumDigits_(uint32_t n) {
  uint32_t sum = 0;
  while (n > 0) {
    sum += n % 10;
    n /= 10;
  }
  return sum;
}

uint32_t CddbLookup::toSeconds_(uint8_t minutes, uint8_t seconds) {
  return static_cast<uint32_t>(minutes) * 60u + static_cast<uint32_t>(seconds);
}

uint32_t CddbLookup::toFrames_(uint8_t minutes, uint8_t seconds) {
  return toSeconds_(minutes, seconds) * 75u;
}
