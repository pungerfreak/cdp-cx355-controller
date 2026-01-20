#include "unit/SLinkUnitEventPublisher.h"

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
                                      SLinkTransportState transport,
                                      const SLinkDebugInfo* debug) {
  SLinkUnitEvent event{type, disc, track, transport, debug};
  _bus.publish(event);
}

void SLinkUnitEventPublisher::play(const SLinkDebugInfo* debug) {
  publish(SLinkUnitEventType::TransportStateChanged,
          emptyDisc(),
          emptyTrack(),
          SLinkTransportState::Playing,
          debug);
}

void SLinkUnitEventPublisher::stop(const SLinkDebugInfo* debug) {
  publish(SLinkUnitEventType::TransportStateChanged,
          emptyDisc(),
          emptyTrack(),
          SLinkTransportState::Stopped,
          debug);
}

void SLinkUnitEventPublisher::pause(const SLinkDebugInfo* debug) {
  publish(SLinkUnitEventType::TransportStateChanged,
          emptyDisc(),
          emptyTrack(),
          SLinkTransportState::Paused,
          debug);
}

void SLinkUnitEventPublisher::powerOn(const SLinkDebugInfo* debug) {
  publish(SLinkUnitEventType::TransportStateChanged,
          emptyDisc(),
          emptyTrack(),
          SLinkTransportState::Stopped,
          debug);
}

void SLinkUnitEventPublisher::powerOff(const SLinkDebugInfo* debug) {
  publish(SLinkUnitEventType::TransportStateChanged,
          emptyDisc(),
          emptyTrack(),
          SLinkTransportState::PowerOff,
          debug);
}

void SLinkUnitEventPublisher::changeDisc(const SLinkDiscInfo& disc,
                                         const SLinkDebugInfo* debug) {
  publish(SLinkUnitEventType::DiscChanged,
          disc,
          emptyTrack(),
          SLinkTransportState::Unchanged,
          debug);
}

void SLinkUnitEventPublisher::changeTrack(const SLinkDiscInfo& disc,
                                          const SLinkTrackInfo& track,
                                          const SLinkDebugInfo* debug) {
  publish(SLinkUnitEventType::TrackChanged,
          disc,
          track,
          SLinkTransportState::Unchanged,
          debug);
}

void SLinkUnitEventPublisher::ready(const SLinkDebugInfo* debug) {
  publish(SLinkUnitEventType::DiscChanged,
          emptyDisc(),
          emptyTrack(),
          SLinkTransportState::Unchanged,
          debug);
}

void SLinkUnitEventPublisher::changingDisc(const SLinkDebugInfo* debug) {
  publish(SLinkUnitEventType::DiscChanged,
          emptyDisc(),
          emptyTrack(),
          SLinkTransportState::Unchanged,
          debug);
}

void SLinkUnitEventPublisher::discReady(const SLinkDiscInfo& disc,
                                        const SLinkDebugInfo* debug) {
  publish(SLinkUnitEventType::DiscChanged,
          disc,
          emptyTrack(),
          SLinkTransportState::Unchanged,
          debug);
}

void SLinkUnitEventPublisher::discLoaded(const SLinkDiscInfo& disc,
                                         const SLinkDebugInfo* debug) {
  publish(SLinkUnitEventType::DiscChanged,
          disc,
          emptyTrack(),
          SLinkTransportState::Unchanged,
          debug);
}

void SLinkUnitEventPublisher::loadingDisc(const SLinkDiscInfo& disc,
                                          const SLinkDebugInfo* debug) {
  publish(SLinkUnitEventType::DiscChanged,
          disc,
          emptyTrack(),
          SLinkTransportState::Unchanged,
          debug);
}

void SLinkUnitEventPublisher::changingTrack(const SLinkDiscInfo& disc,
                                            const SLinkTrackInfo& track,
                                            const SLinkDebugInfo* debug) {
  publish(SLinkUnitEventType::TrackChanged,
          disc,
          track,
          SLinkTransportState::Unchanged,
          debug);
}

void SLinkUnitEventPublisher::currentDiscInfo(const SLinkDiscInfo& disc,
                                              const SLinkTrackInfo& track,
                                              const SLinkDebugInfo* debug) {
  publish(SLinkUnitEventType::CurrentDiscInfo,
          disc,
          track,
          SLinkTransportState::Unchanged,
          debug);
}

void SLinkUnitEventPublisher::currentDiscBankSwitchNeeded(const SLinkDebugInfo* debug) {
  publish(SLinkUnitEventType::CurrentDiscBankSwitchNeeded,
          emptyDisc(),
          emptyTrack(),
          SLinkTransportState::Unchanged,
          debug);
}

void SLinkUnitEventPublisher::status(const SLinkDiscInfo& disc,
                                     const SLinkTrackInfo& track,
                                     SLinkTransportState transport,
                                     const SLinkDebugInfo* debug) {
  publish(SLinkUnitEventType::Status,
          disc,
          track,
          transport,
          debug);
}

void SLinkUnitEventPublisher::unknown(const SLinkDebugInfo* debug) {
  publish(SLinkUnitEventType::Unknown,
          emptyDisc(),
          emptyTrack(),
          SLinkTransportState::Unchanged,
          debug);
}
