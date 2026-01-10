#pragma once
#include "SLinkInputInterface.h"
#include "SLinkIntentQueue.h"

class SLinkIntentAdapter : public SLinkInputInterface {
public:
  explicit SLinkIntentAdapter(SLinkIntentQueue& queue);

  bool play() override;
  bool stop() override;
  bool pause() override;
  bool powerOn() override;
  bool powerOff() override;
  bool changeDisc(uint16_t disc) override;
  bool changeTrack(uint8_t track) override;

private:
  bool enqueue(SLinkCommandIntent intent);

  SLinkIntentQueue& _queue;
};
