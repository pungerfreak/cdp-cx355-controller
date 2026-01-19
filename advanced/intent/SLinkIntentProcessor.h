#pragma once
#include "command/SLinkUnitCommandSender.h"
#include "intent/SLinkIntentArbiter.h"
#include "intent/SLinkIntentQueue.h"
#include "command/SLinkUnitCommand.h"

class SLinkIntentProcessor {
public:
  SLinkIntentProcessor(SLinkIntentQueue& queue,
                       SLinkIntentArbiter& arbiter,
                       SLinkUnitCommandSender& sender);

  void poll();

private:
  bool dispatch(const SLinkCommandIntent& intent);

  SLinkIntentQueue& _queue;
  SLinkIntentArbiter& _arbiter;
  SLinkUnitCommandSender& _sender;
  uint32_t _retryAtUs = 0;
};
