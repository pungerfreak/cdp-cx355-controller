#pragma once
#include "unit/SLinkUnitEvents.h"

class SLinkUnitStateStore : public SLinkUnitEventObserver {
public:
  void onUnitEvent(const SLinkUnitEvent& event) override;

  bool hasDisc() const;
  uint16_t currentDisc() const;

private:
  void updateDisc(const SLinkDiscInfo& disc);

  uint16_t _currentDisc = 0;
  bool _hasDisc = false;
};
