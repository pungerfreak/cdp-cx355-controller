#pragma once
#include "SLinkInterface.h"
#include "SLinkUnitEventBus.h"

class SLinkUnitEventPublisher : public SLinkInterface {
public:
  explicit SLinkUnitEventPublisher(SLinkUnitEventBus& bus);

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
  void publish(SLinkUnitEventType type,
               const SLinkDiscInfo& disc,
               const SLinkTrackInfo& track,
               const SLinkDebugInfo* debug);

  SLinkUnitEventBus& _bus;
};
