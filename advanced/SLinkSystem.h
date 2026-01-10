#pragma once
#include <Arduino.h>
#include "SLinkCommandConsole.h"
#include "SLinkCommandSender.h"
#include "SLinkDebugPrinter.h"
#include "SLinkIntentAdapter.h"
#include "SLinkIntentArbiter.h"
#include "SLinkIntentProcessor.h"
#include "SLinkIntentQueue.h"
#include "SLinkPrettyPrinter.h"
#include "SLinkRx.h"
#include "SLinkSerialCallbacks.h"
#include "SLinkSenderStateSync.h"
#include "SLinkTx.h"
#include "SLinkUnitEventBus.h"
#include "SLinkUnitEventPublisher.h"
#include "SLinkUnitStateStore.h"

class SLinkSystem {
public:
  explicit SLinkSystem(HardwareSerial& serial);

  void begin();
  void poll();

private:
  static constexpr uint8_t kTxPin = 2;
  static constexpr uint8_t kRxPin = 21;
  static constexpr bool kDebugToSerial = true;

  HardwareSerial& _serial;
  SLinkRx _slinkRx;
  SLinkTx _slinkTx;
  SLinkCommandSender _commandSender;
  SLinkIntentQueue _intentQueue;
  SLinkIntentArbiter _intentArbiter;
  SLinkIntentAdapter _intentAdapter;
  SLinkIntentProcessor _intentProcessor;
  SLinkCommandConsole _commandConsole;
  SLinkTranslator _translator;
  SLinkDebugPrinter _debugPrinter;
  SLinkPrettyPrinter _prettyPrinter;
  SLinkUnitStateStore _unitStateStore;
  SLinkUnitEventBus _unitEventBus;
  SLinkUnitEventPublisher _unitEventPublisher;
  SLinkSenderStateSync _senderStateSync;
  SLinkSerialCallbacks _serialCallbacks;
};
