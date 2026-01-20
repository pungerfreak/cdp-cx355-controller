#pragma once
#include <Arduino.h>
#include "unit/SLinkUnitEventHandler.h"

class SLinkPrettyPrinter : public SLinkUnitEventHandler {
public:
  explicit SLinkPrettyPrinter(Stream& out);

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
  void currentDiscInfo(const SLinkDiscInfo& disc,
                       const SLinkTrackInfo& track,
                       const SLinkDebugInfo* debug) override;
  void currentDiscBankSwitchNeeded(const SLinkDebugInfo* debug) override;
  void status(const SLinkDiscInfo& disc,
              const SLinkTrackInfo& track,
              SLinkTransportState transport,
              const SLinkDebugInfo* debug) override;
  void unknown(const SLinkDebugInfo* debug) override;

private:
  void printDisc(const SLinkDiscInfo& disc);
  void printTrack(const SLinkTrackInfo& track);
  void printLength(const SLinkTrackInfo& track);

  Stream& _out;
};
