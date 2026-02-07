#pragma once
#ifndef SLINK_SYSTEM_H_
#define SLINK_SYSTEM_H_
#include <Arduino.h>
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
  explicit SLinkSystem(HardwareSerial& serial, bool debugToSerial);
  explicit SLinkSystem(Stream& serial, bool debugToSerial);

  void begin();
  void poll();
  bool addUnitObserver(SLinkUnitEventObserver& observer);
  bool removeUnitObserver(SLinkUnitEventObserver& observer);
  void getUnitStateSnapshot(SLinkDiscInfo& disc, SLinkTrackInfo& track) const;
  void applyInitialState(uint16_t disc, uint8_t track);
  SLinkCommandIntentSource& intentSource();
  SLinkTx& tx();
  void clearIntents();
  // Push the current cached unit state to all observers (UI, printers, etc.).
  void publishSnapshot();

private:
  static constexpr uint8_t kTxPin = 2;
  static constexpr uint8_t kRxPin = 21;
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
  SLinkUnitStateStore _unitStateStore;
  SLinkUnitEventBus _unitEventBus;
  SLinkUnitEventPublisher _unitEventPublisher;
  SLinkCommandSenderStateSync _senderStateSync;
  bool _debugToSerial = true;
  SLinkFrameCallbacks _frameCallbacks;
  bool _pendingStatusRequest = false;
  uint32_t _statusRequestAtMs = 0;

  void emitInitialState();
};
#endif  // SLINK_SYSTEM_H_
