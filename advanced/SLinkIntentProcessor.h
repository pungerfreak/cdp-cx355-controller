#pragma once
#include "SLinkUnitCommandSender.h"
#include "SLinkIntentArbiter.h"
#include "SLinkIntentQueue.h"
#include "SLinkUnitCommand.h"

class SLinkIntentProcessor {
public:
  SLinkIntentProcessor(SLinkIntentQueue& queue,
                       SLinkIntentArbiter& arbiter,
                       SLinkUnitCommandSender& sender);

  void poll();

private:
  void dispatch(const SLinkCommandIntent& intent);

  SLinkIntentQueue& _queue;
  SLinkIntentArbiter& _arbiter;
  SLinkUnitCommandSender& _sender;
};
