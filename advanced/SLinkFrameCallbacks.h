#pragma once
#include <Arduino.h>
#include "SLinkDebugPrinter.h"
#include "SLinkUnitEventHandler.h"

class SLinkFrameCallbacks {
public:
  SLinkFrameCallbacks(Stream& io,
                      SLinkTranslator& translator,
                      SLinkUnitEventHandler& stateObserver,
                      SLinkDebugPrinter& debugPrinter,
                      SLinkUnitEventHandler* outputHandler,
                      bool debugToSerial);

  static void onTxFrame(const uint8_t* data, uint16_t len, void* context);
  static void onRxFrame(const uint8_t* data, uint16_t len, bool error, void* context);
  void setOutputHandler(SLinkUnitEventHandler* outputHandler);

private:
  void handleTx(const uint8_t* data, uint16_t len);
  void handleRx(const uint8_t* data, uint16_t len, bool error);

  Stream& _io;
  SLinkTranslator& _translator;
  SLinkUnitEventHandler& _stateObserver;
  SLinkDebugPrinter& _debugPrinter;
  SLinkUnitEventHandler* _outputHandler;
  bool _debugToSerial;
  SLinkMessage _rxMessage;
  SLinkMessage _txMessage;
};
