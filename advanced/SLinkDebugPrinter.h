#pragma once
#include <Arduino.h>
#include "SLinkUnitEventHandler.h"

class SLinkDebugPrinter : public SLinkUnitEventHandler {
public:
  explicit SLinkDebugPrinter(Stream& out);

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
  void printLine(const char* name,
                 const SLinkDiscInfo* disc,
                 const SLinkTrackInfo* track,
                 const SLinkDebugInfo* debug);
  void printDisc(const SLinkDiscInfo& disc);
  void printTrack(const SLinkTrackInfo& track);
  void printLength(const SLinkTrackInfo& track);
  void printData(const SLinkDebugInfo& debug);
  void printChecksumHints(const SLinkDebugInfo& debug);

  Stream& _out;
};
