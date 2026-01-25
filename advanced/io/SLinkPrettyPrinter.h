#pragma once
#include <Arduino.h>
#include "unit/SLinkUnitEvents.h"

class SLinkPrettyPrinter : public SLinkUnitEventObserver {
public:
  explicit SLinkPrettyPrinter(Stream& out);

  void onUnitEvent(const SLinkUnitEvent& event) override;

private:
  void printTransportEvent(SLinkTransportState transport, const SLinkDebugInfo* debug);
  void printDiscEvent(const SLinkUnitEvent& event);
  void printTrackEvent(const SLinkUnitEvent& event);
  void printCurrentDiscInfo(const SLinkDiscInfo& disc, const SLinkTrackInfo& track);
  void printStatus(const SLinkDiscInfo& disc,
                   const SLinkTrackInfo& track,
                   SLinkTransportState transport);
  void printDisc(const SLinkDiscInfo& disc);
  void printTrack(const SLinkTrackInfo& track);
  void printLength(const SLinkTrackInfo& track);

  Stream& _out;
};
