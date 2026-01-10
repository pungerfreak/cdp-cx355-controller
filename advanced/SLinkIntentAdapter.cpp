#include "SLinkIntentAdapter.h"

SLinkIntentAdapter::SLinkIntentAdapter(SLinkIntentQueue& queue)
    : _queue(queue) {}

bool SLinkIntentAdapter::play() {
  return enqueue({SLinkIntentType::Play});
}

bool SLinkIntentAdapter::stop() {
  return enqueue({SLinkIntentType::Stop});
}

bool SLinkIntentAdapter::pause() {
  return enqueue({SLinkIntentType::Pause});
}

bool SLinkIntentAdapter::powerOn() {
  return enqueue({SLinkIntentType::PowerOn});
}

bool SLinkIntentAdapter::powerOff() {
  return enqueue({SLinkIntentType::PowerOff});
}

bool SLinkIntentAdapter::changeDisc(uint16_t disc) {
  return enqueue({SLinkIntentType::ChangeDisc, disc, 0});
}

bool SLinkIntentAdapter::changeTrack(uint8_t track) {
  return enqueue({SLinkIntentType::ChangeTrack, 0, track});
}

bool SLinkIntentAdapter::enqueue(SLinkCommandIntent intent) {
  return _queue.push(intent);
}
