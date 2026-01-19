#include "command/SLinkCommandSenderStateSync.h"

SLinkCommandSenderStateSync::SLinkCommandSenderStateSync(
    SLinkUnitCommandSender& sender)
    : _sender(sender) {}

void SLinkCommandSenderStateSync::updateDisc(const SLinkDiscInfo& disc) {
  if (!disc.present || !disc.valid) return;
  _sender.setCurrentDisc(disc.disc);
}

void SLinkCommandSenderStateSync::onUnitEvent(const SLinkUnitEvent& event) {
  switch (event.type) {
    case SLinkUnitEventType::DiscChanged:
    case SLinkUnitEventType::TrackChanged:
      updateDisc(event.disc);
      break;
    case SLinkUnitEventType::CurrentDiscInfo:
      updateDisc(event.disc);
      _sender.completeCurrentDiscRequest();
      break;
    case SLinkUnitEventType::CurrentDiscBankSwitchNeeded:
      _sender.requestCurrentDiscBankB();
      break;
    case SLinkUnitEventType::TransportStateChanged:
    case SLinkUnitEventType::Unknown:
      break;
  }
}
