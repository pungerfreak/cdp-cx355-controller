#pragma once
#include <Arduino.h>
#include "SLinkCommandInput.h"
#include "SLinkUnitCommandSender.h"
#include "SLinkDebugPrinter.h"
#include "SLinkBusState.h"
#include "SLinkIntentQueueAdapter.h"
#include "SLinkIntentArbiter.h"
#include "SLinkIntentProcessor.h"
#include "SLinkIntentQueue.h"
#include "SLinkRx.h"
#include "SLinkFrameCallbacks.h"
#include "SLinkCommandSenderStateSync.h"
#include "SLinkTxGate.h"
#include "SLinkTx.h"
#include "SLinkUnitEventBus.h"
#include "SLinkUnitEventPublisher.h"
#include "SLinkUnitStateStore.h"

class SLinkUnitEventHandler;

class SLinkSystem {
public:
  explicit SLinkSystem(HardwareSerial& serial);
  explicit SLinkSystem(Stream& serial);

  void begin();
  void poll();
  void attachCommandInput(SLinkCommandInput& input);
  void attachEventOutput(SLinkUnitEventHandler& output);
  SLinkCommandIntentSource& intentSource();

private:
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
  SLinkCommandInput* _commandInput = nullptr;
  SLinkUnitEventHandler* _eventOutput = nullptr;
  SLinkUnitStateStore _unitStateStore;
  SLinkUnitEventBus _unitEventBus;
  SLinkUnitEventPublisher _unitEventPublisher;
  SLinkCommandSenderStateSync _senderStateSync;
  SLinkFrameCallbacks _frameCallbacks;
};
