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
      _commandInput(nullptr),
      _eventOutput(nullptr),
      _unitStateStore(),
      _unitEventBus(),
      _unitEventPublisher(_unitEventBus),
      _senderStateSync(_commandSender),
      _frameCallbacks(_serial,
                       _translator,
                       _unitEventPublisher,
                       _debugPrinter,
                       _eventOutput,
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
      _commandInput(nullptr),
      _eventOutput(nullptr),
      _unitStateStore(),
      _unitEventBus(),
      _unitEventPublisher(_unitEventBus),
      _senderStateSync(_commandSender),
      _frameCallbacks(_serial,
                       _translator,
                       _unitEventPublisher,
                       _debugPrinter,
                       _eventOutput,
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
  if (_commandInput != nullptr) {
    _commandInput->poll();
  }
  _intentProcessor.poll();
  _slinkRx.poll(5000);
}

void SLinkSystem::attachCommandInput(SLinkCommandInput& input) {
  _commandInput = &input;
}

void SLinkSystem::attachEventOutput(SLinkUnitEventHandler& output) {
  _eventOutput = &output;
  _frameCallbacks.setOutputHandler(_eventOutput);
}

SLinkCommandIntentSource& SLinkSystem::intentSource() {
  return _intentAdapter;
}
