#ifndef CDDB_INDEXER_H_
#define CDDB_INDEXER_H_
#include <Arduino.h>
#include "system/SLinkSystem.h"
#include "unit/SLinkUnitEvents.h"
#include "command/SLinkCommandIntentSource.h"
#include "cddb/CddbLookup.h"
#include "cddb/CddbClient.h"
#include "cddb/CddbStorage.h"

struct CddbIndexStatus {
  bool active = false;
  bool complete = false;
  uint16_t currentDisc = 0;
  uint8_t currentTrack = 0;
  uint16_t totalDiscs = 0;
  uint8_t percent = 0;
  uint32_t unitsDone = 0;
  uint32_t unitsTotal = 0;
  const char* stage = "";
};

class CddbIndexer : public SLinkUnitEventObserver {
public:
  CddbIndexer(SLinkSystem& system,
              CddbLookup& lookup,
              CddbClient& client,
              CddbStorage& storage,
              uint16_t maxDiscs = 300);

  void start();
  void abort();
  void tick(uint32_t nowMs);
  CddbIndexStatus status() const;
  void onUnitEvent(const SLinkUnitEvent& event) override;

private:
  enum class State : uint8_t {
    Idle,
    Stopping,
    ChangeDisc,
    WaitDiscReady,
    Collecting,
    Querying,
    Done,
    Failed
  };

  void advanceState_(State next);
  void handleCollecting_(uint32_t nowMs);
  void handleQuerying_();
  void persist_(const CddbMetadata& meta);
  void setTrackCount_(uint16_t disc, uint8_t tracks);
  void markMissing_(uint16_t disc);
  void markMissingRange_(uint16_t fromDisc, uint16_t toDiscExclusive);

  SLinkSystem& system_;
  SLinkCommandIntentSource& intents_;
  CddbLookup& lookup_;
  CddbClient& client_;
  CddbStorage& storage_;
  State state_ = State::Idle;
  uint16_t disc_ = 1;
  uint16_t maxDiscs_ = 300;
  uint32_t stageStartedMs_ = 0;
  bool currentSaved_ = false;
  uint8_t attempt_ = 0;
  uint8_t lastTracksSeen_ = 0;
  static constexpr uint8_t kPlaceholderTracksPerDisc = 12;
  uint8_t trackCounts_[301] = {};
  uint32_t unitsTotal_ = 0;
  uint32_t unitsDone_ = 0;
  bool discReady_ = false;
  bool missing_[301] = {};
  uint16_t missingCount_ = 0;
  uint16_t discAtStageStart_ = 0;
  bool waitSawLoading_ = false;
  bool waitReady_ = false;
  uint32_t lastChangeMs_ = 0;
  uint32_t lastResponseMs_ = 0;
  bool sawResponseInWait_ = false;
};

#endif  // CDDB_INDEXER_H_
