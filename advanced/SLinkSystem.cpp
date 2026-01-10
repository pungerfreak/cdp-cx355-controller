#include "SLinkSystem.h"

SLinkSystem::SLinkSystem(HardwareSerial& serial)
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
      _frameCallbacks(_serial,
                       _translator,
                       _unitEventPublisher,
                       _debugPrinter,
                       kDebugToSerial) {}

SLinkSystem::SLinkSystem(Stream& serial)
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
      _frameCallbacks(_serial,
                       _translator,
                       _unitEventPublisher,
                       _debugPrinter,
                       kDebugToSerial) {}

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

void SLinkSystem::attachCommandInput(SLinkCommandInput& input) {
  addCommandInput(input);
}

void SLinkSystem::attachEventOutput(SLinkUnitEventHandler& output) {
  addEventOutput(output);
}

SLinkCommandIntentSource& SLinkSystem::intentSource() {
  return _intentAdapter;
}
