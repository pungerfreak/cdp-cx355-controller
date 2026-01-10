#include "SLinkUnitEventPublisher.h"

namespace {
SLinkDiscInfo emptyDisc() {
  return {};
}

SLinkTrackInfo emptyTrack() {
  return {};
}
}  // namespace

SLinkUnitEventPublisher::SLinkUnitEventPublisher(SLinkUnitEventBus& bus)
    : _bus(bus) {}

void SLinkUnitEventPublisher::publish(SLinkUnitEventType type,
                                      const SLinkDiscInfo& disc,
                                      const SLinkTrackInfo& track,
                                      const SLinkDebugInfo* debug) {
  SLinkUnitEvent event{type, disc, track, debug};
  _bus.publish(event);
}

void SLinkUnitEventPublisher::play(const SLinkDebugInfo* debug) {
  publish(SLinkUnitEventType::Play, emptyDisc(), emptyTrack(), debug);
}

void SLinkUnitEventPublisher::stop(const SLinkDebugInfo* debug) {
  publish(SLinkUnitEventType::Stop, emptyDisc(), emptyTrack(), debug);
}

void SLinkUnitEventPublisher::pause(const SLinkDebugInfo* debug) {
  publish(SLinkUnitEventType::Pause, emptyDisc(), emptyTrack(), debug);
}

void SLinkUnitEventPublisher::powerOn(const SLinkDebugInfo* debug) {
  publish(SLinkUnitEventType::PowerOn, emptyDisc(), emptyTrack(), debug);
}

void SLinkUnitEventPublisher::powerOff(const SLinkDebugInfo* debug) {
  publish(SLinkUnitEventType::PowerOff, emptyDisc(), emptyTrack(), debug);
}

void SLinkUnitEventPublisher::changeDisc(const SLinkDiscInfo& disc,
                                         const SLinkDebugInfo* debug) {
  publish(SLinkUnitEventType::ChangeDisc, disc, emptyTrack(), debug);
}

void SLinkUnitEventPublisher::changeTrack(const SLinkDiscInfo& disc,
                                          const SLinkTrackInfo& track,
                                          const SLinkDebugInfo* debug) {
  publish(SLinkUnitEventType::ChangeTrack, disc, track, debug);
}

void SLinkUnitEventPublisher::ready(const SLinkDebugInfo* debug) {
  publish(SLinkUnitEventType::Ready, emptyDisc(), emptyTrack(), debug);
}

void SLinkUnitEventPublisher::changingDisc(const SLinkDebugInfo* debug) {
  publish(SLinkUnitEventType::ChangingDisc, emptyDisc(), emptyTrack(), debug);
}

void SLinkUnitEventPublisher::discReady(const SLinkDiscInfo& disc,
                                        const SLinkDebugInfo* debug) {
  publish(SLinkUnitEventType::DiscReady, disc, emptyTrack(), debug);
}

void SLinkUnitEventPublisher::discLoaded(const SLinkDiscInfo& disc,
                                         const SLinkDebugInfo* debug) {
  publish(SLinkUnitEventType::DiscLoaded, disc, emptyTrack(), debug);
}

void SLinkUnitEventPublisher::loadingDisc(const SLinkDiscInfo& disc,
                                          const SLinkDebugInfo* debug) {
  publish(SLinkUnitEventType::LoadingDisc, disc, emptyTrack(), debug);
}

void SLinkUnitEventPublisher::changingTrack(const SLinkDiscInfo& disc,
                                            const SLinkTrackInfo& track,
                                            const SLinkDebugInfo* debug) {
  publish(SLinkUnitEventType::ChangingTrack, disc, track, debug);
}

void SLinkUnitEventPublisher::unknown(const SLinkDebugInfo* debug) {
  publish(SLinkUnitEventType::Unknown, emptyDisc(), emptyTrack(), debug);
}
