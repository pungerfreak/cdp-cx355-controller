#include "SLinkIntentProcessor.h"

SLinkIntentProcessor::SLinkIntentProcessor(SLinkIntentQueue& queue,
                                           SLinkCommandSender& sender)
    : _queue(queue), _sender(sender) {}

void SLinkIntentProcessor::poll() {
  SLinkCommandIntent intent;
  while (_queue.pop(intent)) {
    dispatch(intent);
  }
}

void SLinkIntentProcessor::dispatch(const SLinkCommandIntent& intent) {
  switch (intent.type) {
    case SLinkIntentType::Play:
      _sender.play();
      break;
    case SLinkIntentType::Stop:
      _sender.stop();
      break;
    case SLinkIntentType::Pause:
      _sender.pause();
      break;
    case SLinkIntentType::PowerOn:
      _sender.powerOn();
      break;
    case SLinkIntentType::PowerOff:
      _sender.powerOff();
      break;
    case SLinkIntentType::ChangeDisc:
      _sender.changeDisc(intent.disc);
      break;
    case SLinkIntentType::ChangeTrack:
      _sender.changeTrack(intent.track);
      break;
  }
}
