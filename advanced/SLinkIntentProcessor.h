#pragma once
#include "SLinkCommandSender.h"
#include "SLinkIntentArbiter.h"
#include "SLinkIntentQueue.h"
#include "SLinkUnitCommand.h"

class SLinkIntentProcessor {
public:
  SLinkIntentProcessor(SLinkIntentQueue& queue,
                       SLinkIntentArbiter& arbiter,
                       SLinkCommandSender& sender);

  void poll();

private:
  void dispatch(const SLinkCommandIntent& intent);

  SLinkIntentQueue& _queue;
  SLinkIntentArbiter& _arbiter;
  SLinkCommandSender& _sender;
};
