#pragma once
#include "SLinkInterface.h"

class SLinkInterfaceFanout : public SLinkInterface {
public:
  static constexpr uint8_t kMaxTargets = 4;

  bool add(SLinkInterface& target);

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
  void forEach(void (*fn)(SLinkInterface& iface, const SLinkDebugInfo* debug),
               const SLinkDebugInfo* debug);

  SLinkInterface* _targets[kMaxTargets];
  uint8_t _count = 0;
};
