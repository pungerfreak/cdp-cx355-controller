#pragma once
#include <Arduino.h>
#include "system/SLinkSystem.h"
#include "unit/SLinkUnitEvents.h"
#include "cddb/CddbConfig.h"

struct CddbLookupConfig {
  const char* apiSubdomain = nullptr;
  const char* email = nullptr;
  const char* appName = nullptr;
  const char* appVersion = nullptr;
};

class CddbLookup : public SLinkUnitEventObserver {
public:
  static constexpr uint8_t kMaxTracks = 99;
  static constexpr uint16_t kDefaultLeadInFrames = 150;

  struct Result {
    bool ready = false;
    bool success = false;
    uint16_t disc = 0;
    uint8_t trackCount = 0;
    uint32_t discId = 0;
    uint32_t totalSeconds = 0;
    uint16_t leadInFrames = kDefaultLeadInFrames;
    uint16_t trackSeconds[kMaxTracks] = {};
    String cmd;
    String url;
  };

  CddbLookup(SLinkSystem& system, const CddbLookupConfig& config);
  CddbLookup(SLinkSystem& system) : CddbLookup(system, kDefaultCddbConfig) {}

  void start();
  void stop();

  // Begin a lookup using the current disc (or a specific disc if provided).
  // Returns false if a lookup is already in progress.
  bool lookup(uint16_t disc = 0);
  bool busy() const;
  bool hasResult() const;
  const Result& result() const;
  void tick(uint32_t nowMs);

  // SLinkUnitEventObserver
  void onUnitEvent(const SLinkUnitEvent& e) override;

private:
  struct TrackLength {
    bool present = false;
    uint8_t number = 0;
    uint8_t minutes = 0;
    uint8_t seconds = 0;
  };

  enum class State : uint8_t {
    Idle = 0,
    WaitingForDisc,
    Collecting,
    Complete,
    Failed
  };

  void resetState_();
  void beginCollection_();
  void handleDiscEvent_(const SLinkDiscInfo& disc);
  void handleTrackEvent_(const SLinkTrackInfo& track);
  void requestStatus_();
  bool requestTrack_(uint8_t track);
  void finalizeSuccess_();
  void finalizeFailure_();
  bool buildResult_();
  static uint32_t cddbSumDigits_(uint32_t n);
  static uint32_t toSeconds_(uint8_t minutes, uint8_t seconds);
  static uint32_t toFrames_(uint8_t minutes, uint8_t seconds);

  SLinkSystem& system_;
  SLinkCommandIntentSource& intents_;
  CddbLookupConfig config_;

  State state_ = State::Idle;
  bool started_ = false;
  uint16_t targetDisc_ = 0;
  uint8_t requestedTrack_ = 0;
  uint8_t highestTrackSeen_ = 0;
  uint32_t startMs_ = 0;
  uint32_t lastProgressMs_ = 0;
  TrackLength tracks_[kMaxTracks];
  Result result_;
};

constexpr CddbLookupConfig kDefaultCddbConfig{
    kCddbApiSubdomain, kCddbHelloEmail, kCddbHelloAppName, kCddbHelloVersion};
