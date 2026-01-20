#include "intent/SLinkIntentQueueAdapter.h"

SLinkIntentQueueAdapter::SLinkIntentQueueAdapter(SLinkIntentQueue& queue)
    : _queue(queue) {}

bool SLinkIntentQueueAdapter::play() {
  return enqueue({SLinkIntentType::Play});
}

bool SLinkIntentQueueAdapter::stop() {
  return enqueue({SLinkIntentType::Stop});
}

bool SLinkIntentQueueAdapter::pause() {
  return enqueue({SLinkIntentType::Pause});
}

bool SLinkIntentQueueAdapter::powerOn() {
  return enqueue({SLinkIntentType::PowerOn});
}

bool SLinkIntentQueueAdapter::powerOff() {
  return enqueue({SLinkIntentType::PowerOff});
}

bool SLinkIntentQueueAdapter::changeDisc(uint16_t disc) {
  return enqueue({SLinkIntentType::ChangeDisc, disc, 0});
}

bool SLinkIntentQueueAdapter::changeTrack(uint8_t track) {
  return enqueue({SLinkIntentType::ChangeTrack, 0, track});
}

bool SLinkIntentQueueAdapter::getCurrentDisc() {
  return enqueue({SLinkIntentType::GetCurrentDisc});
}

bool SLinkIntentQueueAdapter::getStatus() {
  return enqueue({SLinkIntentType::GetStatus});
}

bool SLinkIntentQueueAdapter::enqueue(SLinkCommandIntent intent) {
  return _queue.push(intent);
}
