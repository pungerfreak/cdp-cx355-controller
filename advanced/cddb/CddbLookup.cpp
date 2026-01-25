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
  const uint32_t timeoutMs = 20000;
  if ((int32_t)(nowMs - lastProgressMs_) > (int32_t)timeoutMs) {
    if (highestTrackSeen_ > 0) {
      finalizeSuccess_();
    } else {
      Serial.println("cddb lookup: timeout with no tracks");
      finalizeFailure_();
    }
  }

  // If we've seen READY/disc and haven't started track requests, start after a short debounce.
  if (state_ == State::Collecting && readySeen_ && !trackRequestsStarted_) {
    const bool waitForDiscInfo =
        (trackCountHint_ == 0 && lastDiscInfoRequestMs_ != 0 &&
         (int32_t)(nowMs - lastDiscInfoRequestMs_) < 500);
    if (!waitForDiscInfo &&
        (int32_t)(nowMs - readySeenMs_) >= (int32_t)kReadyDebounceMs) {
      if (!requestTrack_(requestedTrack_)) {
        // Retry later; keep waiting for the next chance until overall timeout.
        readySeenMs_ = nowMs;
      } else {
        trackRequestsStarted_ = true;
        lastProgressMs_ = nowMs;
      }
    }
  }

  if (state_ == State::Collecting && trackCountHint_ == 0 &&
      (lastDiscInfoRequestMs_ == 0 ||
       (int32_t)(nowMs - lastDiscInfoRequestMs_) >= 1000)) {
    requestDiscInfo_();
  }
}

void CddbLookup::onUnitEvent(const SLinkUnitEvent& e) {
  if (state_ == State::Idle || state_ == State::Complete || state_ == State::Failed) {
    return;
  }

  // READY arrives as a DiscChanged with an empty disc.
  if (state_ == State::Collecting && !trackRequestsStarted_ &&
      e.type == SLinkUnitEventType::DiscChanged && (!e.disc.present || !e.disc.valid)) {
    readySeen_ = true;
    readySeenMs_ = millis();
    return;
  }

  // If we are already collecting and receive another empty-disc READY, treat it as end-of-list.
  if (state_ == State::Collecting && trackRequestsStarted_ && highestTrackSeen_ > 0 &&
      e.type == SLinkUnitEventType::DiscChanged && (!e.disc.present || !e.disc.valid)) {
    finalizeSuccess_();
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
      handleDiscInfoEvent_(e.disc);
      if (e.track.present && e.track.valid) {
        handleTrackEvent_(e.track);
      }
      break;
    case SLinkUnitEventType::TrackChanged:
      handleTrackEvent_(e.track);
      break;
    case SLinkUnitEventType::DiscChanged:
      if (state_ == State::Collecting && targetDisc_ != 0 && e.disc.valid && e.disc.disc != targetDisc_) {
        Serial.println("cddb lookup: disc changed away from target");
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
  trackRequestsStarted_ = false;
  readySeen_ = false;
  readySeenMs_ = 0;
  trackCountHint_ = 0;
  lastDiscInfoRequestMs_ = 0;
  result_ = Result{};
  for (uint8_t i = 0; i < kMaxTracks; ++i) {
    tracks_[i] = TrackLength{};
  }
}

void CddbLookup::clearTrackCountHint() {
  trackCountHint_ = 0;
  lastDiscInfoRequestMs_ = 0;
}

void CddbLookup::beginCollection_() {
  if (targetDisc_ == 0) {
    finalizeFailure_();
    return;
  }
  state_ = State::Collecting;
  requestedTrack_ = 1;
  highestTrackSeen_ = 0;
  trackRequestsStarted_ = false;
  readySeen_ = false;
  readySeenMs_ = 0;
  lastProgressMs_ = millis();
  lastDiscInfoRequestMs_ = 0;
  requestDiscInfo_();
  requestStatus_();  // prompt READY/DISC info
}

void CddbLookup::handleDiscEvent_(const SLinkDiscInfo& disc) {
  if (!disc.present || !disc.valid) return;
  if (targetDisc_ == 0) {
    targetDisc_ = disc.disc;
  }
  if (disc.trackCountValid && disc.trackCount > 0 && disc.disc == targetDisc_) {
    trackCountHint_ = disc.trackCount;
  }
  // Seeing a valid disc means the unit is ready for track requests.
  if (state_ == State::Collecting && !trackRequestsStarted_) {
    readySeen_ = true;
    readySeenMs_ = millis();
    // defer actual track request to tick() to allow debounce
  }
}

void CddbLookup::handleTrackEvent_(const SLinkTrackInfo& track) {
  if (state_ != State::Collecting) return;
  if (!track.present || !track.valid) {
    // Ignore incomplete track notifications; keep waiting until timeout.
    return;
  }

  // If the unit wrapped back to track 1 while scanning higher tracks, stop and use what we have.
  if (requestedTrack_ > 1 && track.track == 1) {
    finalizeSuccess_();
    return;
  }

  if (track.track == 0 || track.track > kMaxTracks) {
    return;
  }
  if (track.track != requestedTrack_) return;
  if (!track.lengthPresent || !track.lengthValid) {
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

  if (trackCountHint_ > 0 && track.track >= trackCountHint_) {
    finalizeSuccess_();
    return;
  }

  if (track.track >= kMaxTracks) {
    finalizeSuccess_();
    return;
  }

  const uint8_t nextTrack = static_cast<uint8_t>(track.track + 1);
  if (!requestTrack_(nextTrack)) {
    // Retry later; mark not started so tick() will retry after debounce.
    trackRequestsStarted_ = false;
    readySeen_ = true;
    readySeenMs_ = millis();
  }
}

void CddbLookup::handleDiscInfoEvent_(const SLinkDiscInfo& disc) {
  if (!disc.present || !disc.valid) return;
  if (targetDisc_ != 0 && disc.disc != targetDisc_) return;
  if (disc.trackCountValid && disc.trackCount > 0) {
    trackCountHint_ = disc.trackCount;
  }
}

void CddbLookup::requestStatus_() {
  intents_.getStatus();
}

void CddbLookup::requestDiscInfo_() {
  if (targetDisc_ == 0) return;
  uint32_t now = millis();
  if (intents_.getDiscInfo()) {
    lastDiscInfoRequestMs_ = now;
  } else if (lastDiscInfoRequestMs_ == 0) {
    lastDiscInfoRequestMs_ = now;
  }
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
  Serial.print("cddb lookup: finalize failure (state=");
  Serial.print(static_cast<int>(state_));
  Serial.print(" requestedTrack=");
  Serial.print(requestedTrack_);
  Serial.print(" highestSeen=");
  Serial.print(highestTrackSeen_);
  Serial.print(" readySeen=");
  Serial.print(readySeen_);
  Serial.print(" trackStarted=");
  Serial.println(trackRequestsStarted_);
  state_ = State::Failed;
  result_.ready = true;
  result_.success = false;
}

bool CddbLookup::buildResult_() {
  if (targetDisc_ == 0 || highestTrackSeen_ == 0) return false;
  for (uint8_t i = 0; i < highestTrackSeen_; ++i) {
    if (!tracks_[i].present) {
      Serial.print("cddb lookup: missing track ");
      Serial.println(i + 1);
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
