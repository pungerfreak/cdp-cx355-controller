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
  if (state_ != State::Idle) return;
  disc_ = 1;
  attempt_ = 0;
  currentSaved_ = false;
  lastTracksSeen_ = 0;
  unitsDone_ = 0;
  unitsTotal_ = static_cast<uint32_t>(maxDiscs_) * kPlaceholderTracksPerDisc;
  memset(trackCounts_, 0, sizeof(trackCounts_));
  advanceState_(State::Stopping);
  Serial.println("cddb indexer: start");
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
      Serial.println("cddb indexer: stage timeout");
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
        advanceState_(State::ChangeDisc);
      }
      break;
    }
    case State::ChangeDisc: {
      if (disc_ > maxDiscs_) {
        Serial.print("cddb indexer: reached max discs (");
        Serial.print(maxDiscs_);
        Serial.println(")");
        advanceState_(State::Done);
        return;
      }
      lookup_.clearTrackCountHint();
      attempt_ = 0;
      if (storage_.has(disc_)) {
        Serial.print("cddb indexer: will overwrite disc ");
        Serial.println(disc_);
      }
      if (intents_.changeDisc(disc_)) {
        Serial.print("cddb indexer: sent changeDisc ");
        Serial.println(disc_);
        advanceState_(State::WaitDiscReady);
      } else {
        Serial.println("cddb indexer: changeDisc rejected");
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
          Serial.print("cddb indexer: lookup start disc ");
          Serial.println(disc_);
          advanceState_(State::Collecting);
        } else {
          Serial.println("cddb indexer: lookup start failed");
          if (++attempt_ < 3) {
            advanceState_(State::WaitDiscReady);
          } else {
            advanceState_(State::Failed);
          }
        }
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
  s.totalDiscs = maxDiscs_;
  uint32_t doneUnits = unitsDone_;
  uint32_t totalUnits = unitsDone_;
  uint8_t seen = (state_ == State::Collecting || state_ == State::Querying) ? lookup_.tracksSeen() : 0;
  for (uint16_t d = disc_; d <= maxDiscs_; ++d) {
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
      s.stage = "idle";
      break;
    case State::Stopping:
      s.stage = "stopping";
      break;
    case State::ChangeDisc:
      s.stage = "change disc";
      break;
    case State::Collecting:
      s.stage = "collecting";
      break;
    case State::Querying:
      s.stage = "querying";
      break;
    case State::WaitDiscReady:
      s.stage = "waiting disc";
      break;
    case State::Done:
      s.stage = "done";
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
      Serial.println("cddb indexer: stopping playback");
      break;
    case State::ChangeDisc:
      Serial.print("cddb indexer: change disc ");
      Serial.println(disc_);
      break;
    case State::WaitDiscReady:
      Serial.print("cddb indexer: wait disc ready ");
      Serial.println(disc_);
      break;
    case State::Collecting:
      attempt_ = 0;
      Serial.print("cddb indexer: collecting disc ");
      Serial.println(disc_);
      break;
    case State::Querying:
      attempt_ = 0;
      Serial.print("cddb indexer: querying disc ");
      Serial.println(disc_);
      break;
    case State::Done:
      Serial.println("cddb indexer: done");
      break;
    case State::Failed:
      Serial.println("cddb indexer: failed");
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
      Serial.print("cddb indexer: lookup complete disc ");
      Serial.println(disc_);
      advanceState_(State::Querying);
    } else {
      Serial.println("cddb indexer: lookup failed");
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
  }
}

void CddbIndexer::handleQuerying_() {
  const auto& res = lookup_.result();
  CddbMetadata meta{};
  Serial.print("cddb indexer: query cmd ");
  Serial.println(res.cmd);
  Serial.print("cddb indexer: query url ");
  Serial.println(res.url);
  setTrackCount_(disc_, res.trackCount);
  if (!client_.queryAndRead(res.cmd, meta)) {
    Serial.println("cddb indexer: query/read failed");
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
    Serial.print("cddb indexer: saved disc ");
    Serial.println(disc_);
  } else {
    Serial.print("cddb indexer: save failed for disc ");
    Serial.println(disc_);
  }
  currentSaved_ = true;
}

void CddbIndexer::setTrackCount_(uint16_t disc, uint8_t tracks) {
  if (disc == 0 || disc > 300) return;
  uint8_t prev = trackCounts_[disc];
  uint8_t prevAssumed = (prev > 0) ? prev : kPlaceholderTracksPerDisc;
  uint8_t newCount = (tracks > 0) ? tracks : kPlaceholderTracksPerDisc;
  trackCounts_[disc] = newCount;
  // Update total to reflect the new real count.
  if (unitsTotal_ >= prevAssumed) unitsTotal_ -= prevAssumed;
  unitsTotal_ += newCount;
}
