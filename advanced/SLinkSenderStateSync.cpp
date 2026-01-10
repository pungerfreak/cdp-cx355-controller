#include "SLinkSenderStateSync.h"

SLinkSenderStateSync::SLinkSenderStateSync(SLinkCommandSender& sender)
    : _sender(sender) {}

void SLinkSenderStateSync::updateDisc(const SLinkDiscInfo& disc) {
  if (!disc.present || !disc.valid) return;
  _sender.setCurrentDisc(disc.disc);
}

void SLinkSenderStateSync::play(const SLinkDebugInfo* debug) { (void)debug; }
void SLinkSenderStateSync::stop(const SLinkDebugInfo* debug) { (void)debug; }
void SLinkSenderStateSync::pause(const SLinkDebugInfo* debug) { (void)debug; }
void SLinkSenderStateSync::powerOn(const SLinkDebugInfo* debug) { (void)debug; }
void SLinkSenderStateSync::powerOff(const SLinkDebugInfo* debug) { (void)debug; }
void SLinkSenderStateSync::changeDisc(const SLinkDiscInfo& disc,
                                      const SLinkDebugInfo* debug) {
  (void)disc;
  (void)debug;
}

void SLinkSenderStateSync::changeTrack(const SLinkDiscInfo& disc,
                                       const SLinkTrackInfo& track,
                                       const SLinkDebugInfo* debug) {
  (void)disc;
  (void)track;
  (void)debug;
}

void SLinkSenderStateSync::ready(const SLinkDebugInfo* debug) { (void)debug; }
void SLinkSenderStateSync::changingDisc(const SLinkDebugInfo* debug) { (void)debug; }

void SLinkSenderStateSync::discReady(const SLinkDiscInfo& disc,
                                     const SLinkDebugInfo* debug) {
  (void)debug;
  updateDisc(disc);
}

void SLinkSenderStateSync::discLoaded(const SLinkDiscInfo& disc,
                                      const SLinkDebugInfo* debug) {
  (void)disc;
  (void)debug;
}

void SLinkSenderStateSync::loadingDisc(const SLinkDiscInfo& disc,
                                       const SLinkDebugInfo* debug) {
  (void)disc;
  (void)debug;
}

void SLinkSenderStateSync::changingTrack(const SLinkDiscInfo& disc,
                                         const SLinkTrackInfo& track,
                                         const SLinkDebugInfo* debug) {
  (void)debug;
  updateDisc(disc);
  (void)track;
}

void SLinkSenderStateSync::unknown(const SLinkDebugInfo* debug) { (void)debug; }
