#include "intent/SLinkIntentProcessor.h"

SLinkIntentProcessor::SLinkIntentProcessor(SLinkIntentQueue& queue,
                                           SLinkIntentArbiter& arbiter,
                                           SLinkUnitCommandSender& sender)
    : _queue(queue), _arbiter(arbiter), _sender(sender) {}

void SLinkIntentProcessor::poll() {
  const uint32_t nowUs = micros();
  if (_retryAtUs != 0 && (int32_t)(nowUs - _retryAtUs) < 0) {
    return;
  }
  _retryAtUs = 0;

  while (true) {
    SLinkCommandIntent intent;
    uint8_t offset = 0;
    if (!_arbiter.selectNext(_queue, intent, offset)) return;

    const uint32_t attemptUs = micros();
    if (!_sender.canSendNow(attemptUs)) {
      _retryAtUs = _sender.nextSendUs(attemptUs);
      return;
    }

    if (!dispatch(intent)) {
      const uint32_t failedAtUs = micros();
      uint32_t nextUs = _sender.nextSendUs(failedAtUs);
      if (nextUs < (failedAtUs + 1000u)) nextUs = failedAtUs + 1000u;
      _retryAtUs = nextUs;
      return;
    }

    SLinkCommandIntent removed;
    uint32_t enqueuedAt = 0;
    if (_queue.removeAt(offset, removed, enqueuedAt)) {
      _arbiter.noteDispatched(removed.type);
    } else {
      return;
    }
  }
}

bool SLinkIntentProcessor::dispatch(const SLinkCommandIntent& intent) {
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
    case SLinkIntentType::GetCurrentDisc:
      cmd.type = SLinkUnitCommandType::GetCurrentDisc;
      break;
    case SLinkIntentType::GetStatus:
      cmd.type = SLinkUnitCommandType::GetStatus;
      break;
  }
  return _sender.send(cmd);
}
