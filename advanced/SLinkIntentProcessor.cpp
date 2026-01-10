#include "SLinkIntentProcessor.h"

SLinkIntentProcessor::SLinkIntentProcessor(SLinkIntentQueue& queue,
                                           SLinkIntentArbiter& arbiter,
                                           SLinkCommandSender& sender)
    : _queue(queue), _arbiter(arbiter), _sender(sender) {}

void SLinkIntentProcessor::poll() {
  SLinkCommandIntent intent;
  while (_queue.pop(intent)) {
    if (_arbiter.shouldDispatch(intent)) {
      dispatch(intent);
    }
  }
}

void SLinkIntentProcessor::dispatch(const SLinkCommandIntent& intent) {
  SLinkUnitCommand cmd{SLinkUnitCommandType::Play, 0, 0};
  switch (intent.type) {
    case SLinkIntentType::Play:
      cmd.type = SLinkUnitCommandType::Play;
      break;
    case SLinkIntentType::Stop:
      cmd.type = SLinkUnitCommandType::Stop;
      break;
    case SLinkIntentType::Pause:
      cmd.type = SLinkUnitCommandType::Pause;
      break;
    case SLinkIntentType::PowerOn:
      cmd.type = SLinkUnitCommandType::PowerOn;
      break;
    case SLinkIntentType::PowerOff:
      cmd.type = SLinkUnitCommandType::PowerOff;
      break;
    case SLinkIntentType::ChangeDisc:
      cmd.type = SLinkUnitCommandType::ChangeDisc;
      cmd.disc = intent.disc;
      break;
    case SLinkIntentType::ChangeTrack:
      cmd.type = SLinkUnitCommandType::ChangeTrack;
      cmd.track = intent.track;
      break;
  }
  _sender.send(cmd);
}
