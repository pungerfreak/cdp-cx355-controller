#include "system/SLinkSystem.h"

SLinkSystem::SLinkSystem(HardwareSerial& serial, bool debugToSerial)
    : _serial(serial),
      _hardwareSerial(&serial),
      _busState(),
      _txGate(_busState),
      _slinkRx(kRxPin, _busState),
      _slinkTx(kTxPin, _txGate),
      _commandSender(_slinkTx),
      _intentQueue(),
      _intentArbiter(),
      _intentAdapter(_intentQueue),
      _intentProcessor(_intentQueue, _intentArbiter, _commandSender),
      _translator(),
      _debugPrinter(_serial),
      _unitStateStore(),
      _unitEventBus(),
      _unitEventPublisher(_unitEventBus),
      _senderStateSync(_commandSender),
      _debugToSerial(debugToSerial),
      _frameCallbacks(_serial,
                       _translator,
                       _unitEventPublisher,
                       _debugPrinter,
                       _debugToSerial) {}

SLinkSystem::SLinkSystem(Stream& serial, bool debugToSerial)
    : _serial(serial),
      _busState(),
      _txGate(_busState),
      _slinkRx(kRxPin, _busState),
      _slinkTx(kTxPin, _txGate),
      _commandSender(_slinkTx),
      _intentQueue(),
      _intentArbiter(),
      _intentAdapter(_intentQueue),
      _intentProcessor(_intentQueue, _intentArbiter, _commandSender),
      _translator(),
      _debugPrinter(_serial),
      _unitStateStore(),
      _unitEventBus(),
      _unitEventPublisher(_unitEventBus),
      _senderStateSync(_commandSender),
      _debugToSerial(debugToSerial),
      _frameCallbacks(_serial,
                       _translator,
                       _unitEventPublisher,
                       _debugPrinter,
                       _debugToSerial) {}

void SLinkSystem::begin() {
  if (_hardwareSerial != nullptr) {
    _hardwareSerial->begin(230400);
  }
  _unitEventBus.addObserver(_unitStateStore);
  _unitEventBus.addObserver(_senderStateSync);
  _commandSender.setTxCallback(SLinkFrameCallbacks::onTxFrame, &_frameCallbacks);
  _slinkRx.setRxCallback(SLinkFrameCallbacks::onRxFrame, &_frameCallbacks);
  _slinkRx.begin();
  _slinkTx.begin();
  requestInitialState();
}

void SLinkSystem::poll() {
  for (uint8_t i = 0; i < _commandInputCount; ++i) {
    if (_commandInputs[i] != nullptr) {
      _commandInputs[i]->poll();
    }
  }
  _intentProcessor.poll();
  _slinkRx.poll(5000);
}

bool SLinkSystem::addCommandInput(SLinkCommandInput& input) {
  if (_commandInputCount >= kMaxCommandInputs) return false;
  _commandInputs[_commandInputCount++] = &input;
  return true;
}

bool SLinkSystem::addEventOutput(SLinkUnitEventHandler& output) {
  if (_eventOutputCount >= kMaxEventOutputs) return false;
  _eventOutputs[_eventOutputCount++] = &output;
  return _frameCallbacks.addOutputHandler(output);
}

void SLinkSystem::applyInitialState(uint16_t disc, uint8_t track) {
  _unitStateStore.setInitialState(disc, track);
  if (disc > 0) {
    _commandSender.setCurrentDisc(disc);
  }
  emitInitialState();
}

SLinkCommandIntentSource& SLinkSystem::intentSource() {
  return _intentAdapter;
}

void SLinkSystem::requestInitialState() {
  // TODO: send the yet-to-be-discovered status request command.
  // The response handler should call applyInitialState() with parsed disc/track.
}

void SLinkSystem::emitInitialState() {
  SLinkDiscInfo discInfo;
  SLinkTrackInfo trackInfo;
  _unitStateStore.stateInfo(discInfo, trackInfo);
  for (uint8_t i = 0; i < _eventOutputCount; ++i) {
    if (_eventOutputs[i] != nullptr) {
      if (trackInfo.present) {
        _eventOutputs[i]->changeTrack(discInfo, trackInfo, nullptr);
      } else if (discInfo.present) {
        _eventOutputs[i]->changeDisc(discInfo, nullptr);
      }
    }
  }
}
