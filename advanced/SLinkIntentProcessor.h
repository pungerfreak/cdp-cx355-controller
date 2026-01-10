#pragma once
#include "SLinkCommandSender.h"
#include "SLinkIntentQueue.h"

class SLinkIntentProcessor {
public:
  SLinkIntentProcessor(SLinkIntentQueue& queue, SLinkCommandSender& sender);

  void poll();

private:
  void dispatch(const SLinkCommandIntent& intent);

  SLinkIntentQueue& _queue;
  SLinkCommandSender& _sender;
};
