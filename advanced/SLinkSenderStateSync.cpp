#include "SLinkSenderStateSync.h"

SLinkSenderStateSync::SLinkSenderStateSync(SLinkCommandSender& sender)
    : _sender(sender) {}

void SLinkSenderStateSync::updateDisc(const SLinkDiscInfo& disc) {
  if (!disc.present || !disc.valid) return;
  _sender.setCurrentDisc(disc.disc);
}

void SLinkSenderStateSync::onUnitEvent(const SLinkUnitEvent& event) {
  switch (event.type) {
    case SLinkUnitEventType::DiscReady:
    case SLinkUnitEventType::ChangingTrack:
      updateDisc(event.disc);
      break;
    case SLinkUnitEventType::ChangeDisc:
    case SLinkUnitEventType::ChangeTrack:
    case SLinkUnitEventType::DiscLoaded:
    case SLinkUnitEventType::LoadingDisc:
    case SLinkUnitEventType::Play:
    case SLinkUnitEventType::Stop:
    case SLinkUnitEventType::Pause:
    case SLinkUnitEventType::PowerOn:
    case SLinkUnitEventType::PowerOff:
    case SLinkUnitEventType::Ready:
    case SLinkUnitEventType::ChangingDisc:
    case SLinkUnitEventType::Unknown:
      break;
  }
}
