#pragma once
#include "SLinkCommandSender.h"
#include "SLinkUnitEvents.h"

class SLinkSenderStateSync : public SLinkUnitEventObserver {
public:
  explicit SLinkSenderStateSync(SLinkCommandSender& sender);

  void onUnitEvent(const SLinkUnitEvent& event) override;

private:
  void updateDisc(const SLinkDiscInfo& disc);

  SLinkCommandSender& _sender;
};
