#pragma once
#include <Arduino.h>
#include "command/SLinkCommandInput.h"
#include "command/SLinkUnitCommandSender.h"
#include "debug/SLinkDebugPrinter.h"
#include "transport/SLinkBusState.h"
#include "intent/SLinkIntentQueueAdapter.h"
#include "intent/SLinkIntentArbiter.h"
#include "intent/SLinkIntentProcessor.h"
#include "intent/SLinkIntentQueue.h"
#include "transport/SLinkRx.h"
#include "frame/SLinkFrameCallbacks.h"
#include "command/SLinkCommandSenderStateSync.h"
#include "transport/SLinkTxGate.h"
#include "transport/SLinkTx.h"
#include "unit/SLinkUnitEventBus.h"
#include "unit/SLinkUnitEventPublisher.h"
#include "unit/SLinkUnitStateStore.h"

class SLinkUnitEventHandler;

class SLinkSystem {
public:
  explicit SLinkSystem(HardwareSerial& serial);
  explicit SLinkSystem(Stream& serial);

  void begin();
  void poll();
  bool addCommandInput(SLinkCommandInput& input);
  bool addEventOutput(SLinkUnitEventHandler& output);
  void attachCommandInput(SLinkCommandInput& input);
  void attachEventOutput(SLinkUnitEventHandler& output);
  SLinkCommandIntentSource& intentSource();

private:
  static constexpr uint8_t kMaxCommandInputs = 4;
  static constexpr uint8_t kMaxEventOutputs = 4;
  static constexpr uint8_t kTxPin = 2;
  static constexpr uint8_t kRxPin = 21;
  static constexpr bool kDebugToSerial = true;

  Stream& _serial;
  HardwareSerial* _hardwareSerial = nullptr;
  SLinkBusState _busState;
  SLinkTxGate _txGate;
  SLinkRx _slinkRx;
  SLinkTx _slinkTx;
  SLinkUnitCommandSender _commandSender;
  SLinkIntentQueue _intentQueue;
  SLinkIntentArbiter _intentArbiter;
  SLinkIntentQueueAdapter _intentAdapter;
  SLinkIntentProcessor _intentProcessor;
  SLinkTranslator _translator;
  SLinkDebugPrinter _debugPrinter;
  SLinkCommandInput* _commandInputs[kMaxCommandInputs] = {};
  uint8_t _commandInputCount = 0;
  SLinkUnitEventHandler* _eventOutputs[kMaxEventOutputs] = {};
  uint8_t _eventOutputCount = 0;
  SLinkUnitStateStore _unitStateStore;
  SLinkUnitEventBus _unitEventBus;
  SLinkUnitEventPublisher _unitEventPublisher;
  SLinkCommandSenderStateSync _senderStateSync;
  SLinkFrameCallbacks _frameCallbacks;
};
