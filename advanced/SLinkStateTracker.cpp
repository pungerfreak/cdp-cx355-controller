#include "SLinkStateTracker.h"

SLinkStateTracker::SLinkStateTracker(SLinkCommandSender& sender)
    : _sender(sender) {}

void SLinkStateTracker::updateDisc(const SLinkDiscInfo& disc) {
  if (!disc.present || !disc.valid) return;
  if (_hasDisc && _currentDisc == disc.disc) return;
  _currentDisc = disc.disc;
  _hasDisc = true;
  _hasTrack = false;
  _sender.setCurrentDisc(disc.disc);
}

void SLinkStateTracker::updateTrack(const SLinkTrackInfo& track) {
  if (!track.present || !track.valid) return;
  _currentTrack = track.track;
  _hasTrack = true;
}

void SLinkStateTracker::play(const SLinkDebugInfo* debug) { (void)debug; }
void SLinkStateTracker::stop(const SLinkDebugInfo* debug) { (void)debug; }
void SLinkStateTracker::pause(const SLinkDebugInfo* debug) { (void)debug; }
void SLinkStateTracker::powerOn(const SLinkDebugInfo* debug) { (void)debug; }
void SLinkStateTracker::powerOff(const SLinkDebugInfo* debug) { (void)debug; }
void SLinkStateTracker::changeDisc(const SLinkDiscInfo& disc,
                                   const SLinkDebugInfo* debug) {
  (void)disc;
  (void)debug;
}

void SLinkStateTracker::changeTrack(const SLinkDiscInfo& disc,
                                    const SLinkTrackInfo& track,
                                    const SLinkDebugInfo* debug) {
  (void)disc;
  (void)track;
  (void)debug;
}
void SLinkStateTracker::ready(const SLinkDebugInfo* debug) { (void)debug; }
void SLinkStateTracker::changingDisc(const SLinkDebugInfo* debug) { (void)debug; }

void SLinkStateTracker::discReady(const SLinkDiscInfo& disc,
                                  const SLinkDebugInfo* debug) {
  (void)debug;
  updateDisc(disc);
}

void SLinkStateTracker::discLoaded(const SLinkDiscInfo& disc,
                                   const SLinkDebugInfo* debug) {
  (void)disc;
  (void)debug;
}

void SLinkStateTracker::loadingDisc(const SLinkDiscInfo& disc,
                                    const SLinkDebugInfo* debug) {
  (void)disc;
  (void)debug;
}

void SLinkStateTracker::changingTrack(const SLinkDiscInfo& disc,
                                      const SLinkTrackInfo& track,
                                      const SLinkDebugInfo* debug) {
  (void)debug;
  updateDisc(disc);
  updateTrack(track);
}

void SLinkStateTracker::unknown(const SLinkDebugInfo* debug) { (void)debug; }
