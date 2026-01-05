#pragma once
#include "SLinkCommandSender.h"
#include "SLinkInterface.h"

class SLinkStateTracker : public SLinkInterface {
public:
  explicit SLinkStateTracker(SLinkCommandSender& sender);

  uint16_t currentDisc() const { return _currentDisc; }
  uint8_t currentTrack() const { return _currentTrack; }
  bool hasDisc() const { return _hasDisc; }
  bool hasTrack() const { return _hasTrack; }

  void play(const SLinkDebugInfo* debug) override;
  void stop(const SLinkDebugInfo* debug) override;
  void pause(const SLinkDebugInfo* debug) override;
  void powerOn(const SLinkDebugInfo* debug) override;
  void powerOff(const SLinkDebugInfo* debug) override;
  void changeDisc(const SLinkDiscInfo& disc, const SLinkDebugInfo* debug) override;
  void changeTrack(const SLinkDiscInfo& disc,
                   const SLinkTrackInfo& track,
                   const SLinkDebugInfo* debug) override;
  void ready(const SLinkDebugInfo* debug) override;
  void changingDisc(const SLinkDebugInfo* debug) override;
  void discReady(const SLinkDiscInfo& disc, const SLinkDebugInfo* debug) override;
  void discLoaded(const SLinkDiscInfo& disc, const SLinkDebugInfo* debug) override;
  void loadingDisc(const SLinkDiscInfo& disc, const SLinkDebugInfo* debug) override;
  void changingTrack(const SLinkDiscInfo& disc,
                     const SLinkTrackInfo& track,
                     const SLinkDebugInfo* debug) override;
  void unknown(const SLinkDebugInfo* debug) override;

private:
  void updateDisc(const SLinkDiscInfo& disc);
  void updateTrack(const SLinkTrackInfo& track);

  SLinkCommandSender& _sender;
  uint16_t _currentDisc = 0;
  uint8_t _currentTrack = 0;
  bool _hasDisc = false;
  bool _hasTrack = false;
};
