#pragma once
#include "command/SLinkUnitCommandSender.h"
#include "unit/SLinkUnitEvents.h"

class SLinkCommandSenderStateSync : public SLinkUnitEventObserver {
public:
  explicit SLinkCommandSenderStateSync(SLinkUnitCommandSender& sender);

  void onUnitEvent(const SLinkUnitEvent& event) override;

private:
  void updateDisc(const SLinkDiscInfo& disc);

  SLinkUnitCommandSender& _sender;
};
