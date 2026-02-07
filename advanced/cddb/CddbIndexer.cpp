#include "cddb/CddbIndexer.h"
#include <string.h>

CddbIndexer::CddbIndexer(SLinkSystem& system,
                         CddbLookup& lookup,
                         CddbClient& client,
                         CddbStorage& storage,
                         uint16_t maxDiscs)
    : system_(system),
      intents_(system.intentSource()),
      lookup_(lookup),
      client_(client),
      storage_(storage),
      maxDiscs_(maxDiscs) {}

void CddbIndexer::start() {
  if (state_ != State::Idle && state_ != State::Done && state_ != State::Failed) return;
  disc_ = 1;
  attempt_ = 0;
  currentSaved_ = false;
  lastTracksSeen_ = 0;
  unitsDone_ = 0;
  missingCount_ = 0;
  memset(missing_, 0, sizeof(missing_));
  unitsTotal_ = static_cast<uint32_t>(maxDiscs_) * kPlaceholderTracksPerDisc;
  memset(trackCounts_, 0, sizeof(trackCounts_));
  discReady_ = false;
  advanceState_(State::Stopping);
}

void CddbIndexer::abort() {
  if (state_ == State::Idle || state_ == State::Done) return;
  lookup_.cancel();
  state_ = State::Done;
}

void CddbIndexer::tick(uint32_t nowMs) {
  lookup_.tick(nowMs);
  if (state_ != State::Idle && state_ != State::Done && state_ != State::Failed) {
    const uint32_t elapsed = nowMs - stageStartedMs_;
    uint32_t limit = 10000;
    switch (state_) {
      case State::Stopping:
      case State::WaitDiscReady:
        limit = 15000;
        break;
      case State::Collecting:
        limit = 30000;
        break;
      case State::Querying:
        limit = 20000;
        break;
      default:
        break;
    }
    if (elapsed > limit) {
      advanceState_(State::Failed);
      return;
    }
  }
  switch (state_) {
    case State::Idle:
    case State::Done:
    case State::Failed:
      return;
    case State::Stopping: {
      if (intents_.stop()) {
        discReady_ = false;
        advanceState_(State::ChangeDisc);
      }
      break;
    }
    case State::ChangeDisc: {
      if (disc_ > maxDiscs_) {
        advanceState_(State::Done);
        return;
      }
      while (disc_ <= maxDiscs_) {
        lookup_.clearTrackCountHint();
        attempt_ = 0;
        discReady_ = false;
        if (missing_[disc_]) {
          ++disc_;
          continue;
        }
        if (storage_.has(disc_)) {
          uint8_t storedCount = 0;
          if (storage_.trackCount(disc_, storedCount) && storedCount > 0) {
            setTrackCount_(disc_, storedCount);
            unitsDone_ += trackCounts_[disc_] ? trackCounts_[disc_] : kPlaceholderTracksPerDisc;
            ++disc_;
            continue;
          } else {
            // Corrupt or unreadable entry; re-index it.
            storage_.remove(disc_);
            setTrackCount_(disc_, 0);
          }
        }
        break;
      }
      if (disc_ > maxDiscs_) {
        advanceState_(State::Done);
        return;
      }
      if (intents_.changeDisc(disc_)) {
        advanceState_(State::WaitDiscReady);
      } else {
        // If the intent is rejected, treat slot as missing and continue.
        markMissing_(disc_);
        ++disc_;
        advanceState_(State::ChangeDisc);
      }
      break;
    }
    case State::WaitDiscReady: {
      SLinkDiscInfo discInfo;
      SLinkTrackInfo trackInfo;
      system_.getUnitStateSnapshot(discInfo, trackInfo);
      if (discInfo.hasTrackCount && discInfo.trackCountValid && discInfo.trackCount > 0 &&
          discInfo.valid && discInfo.disc == disc_ && trackCounts_[disc_] == 0) {
        setTrackCount_(disc_, discInfo.trackCount);
      }
      if ((discInfo.present && discInfo.valid && discInfo.disc == disc_) ||
          (millis() - stageStartedMs_) > 2000) {
        if (lookup_.lookup(disc_)) {
          advanceState_(State::Collecting);
        } else {
          if (++attempt_ < 3) {
            advanceState_(State::WaitDiscReady);
          } else {
            advanceState_(State::Failed);
          }
        }
      } else if (discInfo.present && discInfo.valid && discInfo.disc > disc_) {
        // Changer skipped some empty slots: mark the gap as missing.
        markMissingRange_(disc_, discInfo.disc);
        disc_ = discInfo.disc;
        // Give the new target a fresh try immediately.
        advanceState_(State::WaitDiscReady);
      }
      break;
    }
    case State::Collecting:
      handleCollecting_(nowMs);
      break;
    case State::Querying:
      handleQuerying_();
      break;
  }
}

CddbIndexStatus CddbIndexer::status() const {
  CddbIndexStatus s{};
  s.active = (state_ != State::Idle && state_ != State::Done && state_ != State::Failed);
  s.complete = (state_ == State::Done);
  s.currentDisc = disc_;
  s.currentTrack = lookup_.requestedTrack();
  s.totalDiscs = maxDiscs_ - missingCount_;
  uint32_t doneUnits = unitsDone_;
  uint32_t totalUnits = unitsDone_;
  uint8_t seen = (state_ == State::Collecting || state_ == State::Querying) ? lookup_.tracksSeen() : 0;
  for (uint16_t d = disc_; d <= maxDiscs_; ++d) {
    if (missing_[d]) continue;
    uint8_t known = trackCounts_[d];
    uint8_t hint = (state_ == State::Collecting && d == disc_) ? lookup_.trackCountHint() : 0;
    if (known == 0 && hint > 0) {
      known = hint;
    }
    if (d == disc_) {
      uint8_t est = known ? known : (uint8_t)((seen > kPlaceholderTracksPerDisc) ? seen : kPlaceholderTracksPerDisc);
      totalUnits += est;
      if (state_ == State::Collecting || state_ == State::Querying) {
        doneUnits += min<uint32_t>(seen, est);
      }
    } else {
      totalUnits += known ? known : kPlaceholderTracksPerDisc;
    }
  }
  s.unitsDone = doneUnits;
  s.unitsTotal = totalUnits;
  s.percent = (totalUnits == 0)
                  ? 0
                  : static_cast<uint8_t>(min<uint32_t>(100, (doneUnits * 100u) / totalUnits));
  switch (state_) {
    case State::Idle:
    case State::Done:
      s.stage = "";
      break;
    case State::Stopping:
    case State::ChangeDisc:
    case State::WaitDiscReady:
      s.stage = "changing disc";
      break;
    case State::Collecting:
    case State::Querying:
      s.stage = discReady_ ? "reading disc" : "changing disc";
      break;
    case State::Failed:
      s.stage = "failed";
      break;
  }
  return s;
}

void CddbIndexer::advanceState_(State next) {
  state_ = next;
  stageStartedMs_ = millis();
  switch (state_) {
    case State::Stopping:
      break;
    case State::ChangeDisc:
      break;
    case State::WaitDiscReady:
      break;
    case State::Collecting:
      attempt_ = 0;
      break;
    case State::Querying:
      attempt_ = 0;
      break;
    case State::Done:
      break;
    case State::Failed:
      break;
    case State::Idle:
    default:
      break;
  }
}

void CddbIndexer::handleCollecting_(uint32_t nowMs) {
  if (trackCounts_[disc_] == 0) {
    uint8_t hint = lookup_.trackCountHint();
    if (hint > 0) {
      setTrackCount_(disc_, hint);
    }
  }

  if (lookup_.hasResult()) {
    if (lookup_.result().success) {
      setTrackCount_(disc_, lookup_.result().trackCount);
      advanceState_(State::Querying);
    } else {
      if (++attempt_ < 3) {
        advanceState_(State::WaitDiscReady);
      } else {
        advanceState_(State::Failed);
      }
    }
    return;
  }
  uint8_t seen = lookup_.tracksSeen();
  if (seen != lastTracksSeen_) {
    lastTracksSeen_ = seen;
    stageStartedMs_ = nowMs;
    if (seen > 0) {
      discReady_ = true;
    }
  }
}

void CddbIndexer::handleQuerying_() {
  const auto& res = lookup_.result();
  CddbMetadata meta{};
  setTrackCount_(disc_, res.trackCount);
  if (!client_.queryAndRead(res.cmd, meta)) {
    if (++attempt_ < 3) {
      stageStartedMs_ = millis();
      return;
    }
    // Persist lengths-only data so we do not lose progress for this disc.
    persist_(meta);
    unitsDone_ += trackCounts_[disc_] ? trackCounts_[disc_] : kPlaceholderTracksPerDisc;
    disc_++;
    currentSaved_ = false;
    attempt_ = 0;
    advanceState_(State::ChangeDisc);
    return;
  }
  attempt_ = 0;
  persist_(meta);
  uint8_t count = trackCounts_[disc_];
  if (count == 0) count = kPlaceholderTracksPerDisc;
  unitsDone_ += count;
  // Recompute unitsTotal_ in case later discs will be larger than placeholder.
  unitsTotal_ = 0;
  for (uint16_t d = 1; d <= maxDiscs_; ++d) {
    if (missing_[d]) continue;
    uint8_t c = trackCounts_[d];
    unitsTotal_ += (c > 0) ? c : kPlaceholderTracksPerDisc;
  }
  disc_++;
  currentSaved_ = false;
  advanceState_(State::ChangeDisc);
}

void CddbIndexer::persist_(const CddbMetadata& meta) {
  if (currentSaved_) return;
  if (storage_.save(disc_, meta, lookup_.result())) {
    currentSaved_ = true;
  }
}

void CddbIndexer::setTrackCount_(uint16_t disc, uint8_t tracks) {
  if (disc == 0 || disc > 300) return;
  if (missing_[disc]) return;
  uint8_t prev = trackCounts_[disc];
  uint8_t prevAssumed = (prev > 0) ? prev : kPlaceholderTracksPerDisc;
  uint8_t newCount = (tracks > 0) ? tracks : kPlaceholderTracksPerDisc;
  trackCounts_[disc] = newCount;
  // Update total to reflect the new real count.
  if (unitsTotal_ >= prevAssumed) unitsTotal_ -= prevAssumed;
  unitsTotal_ += newCount;
}

void CddbIndexer::markMissing_(uint16_t disc) {
  if (disc == 0 || disc > 300) return;
  if (missing_[disc]) return;
  missing_[disc] = true;
  ++missingCount_;
  uint8_t assumed = trackCounts_[disc] ? trackCounts_[disc] : kPlaceholderTracksPerDisc;
  if (unitsTotal_ >= assumed) unitsTotal_ -= assumed;
}

void CddbIndexer::markMissingRange_(uint16_t fromDisc, uint16_t toDiscExclusive) {
  if (fromDisc == 0) fromDisc = 1;
  if (toDiscExclusive > 301) toDiscExclusive = 301;
  for (uint16_t d = fromDisc; d < toDiscExclusive; ++d) {
    markMissing_(d);
  }
}
