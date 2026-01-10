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
      _commandConsole(_serial, _intentAdapter, true),
      _translator(),
      _debugPrinter(_serial),
      _prettyPrinter(_serial),
      _unitStateStore(),
      _unitEventBus(),
      _unitEventPublisher(_unitEventBus),
      _senderStateSync(_commandSender),
      _serialCallbacks(_serial,
                       _translator,
                       _unitEventPublisher,
                       _debugPrinter,
                       _prettyPrinter,
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
      _commandConsole(_serial, _intentAdapter, true),
      _translator(),
      _debugPrinter(_serial),
      _prettyPrinter(_serial),
      _unitStateStore(),
      _unitEventBus(),
      _unitEventPublisher(_unitEventBus),
      _senderStateSync(_commandSender),
      _serialCallbacks(_serial,
                       _translator,
                       _unitEventPublisher,
                       _debugPrinter,
                       _prettyPrinter,
                       kDebugToSerial) {}

void SLinkSystem::begin() {
  if (_hardwareSerial != nullptr) {
    _hardwareSerial->begin(230400);
  }
  _unitEventBus.addObserver(_unitStateStore);
  _unitEventBus.addObserver(_senderStateSync);
  _commandSender.setTxCallback(SLinkSerialCallbacks::onTxFrame, &_serialCallbacks);
  _slinkRx.setRxCallback(SLinkSerialCallbacks::onRxFrame, &_serialCallbacks);
  _slinkRx.begin();
  _slinkTx.begin();
}

void SLinkSystem::poll() {
  _commandConsole.poll();
  _intentProcessor.poll();
  _slinkRx.poll(5000);
}
