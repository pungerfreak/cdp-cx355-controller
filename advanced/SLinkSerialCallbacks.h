#pragma once
#include <Arduino.h>
#include "SLinkDebugPrinter.h"
#include "SLinkInterface.h"
#include "SLinkPrettyPrinter.h"

class SLinkSerialCallbacks {
public:
  SLinkSerialCallbacks(Stream& io,
                       SLinkTranslator& translator,
                       SLinkInterface& stateObserver,
                       SLinkDebugPrinter& debugPrinter,
                       SLinkPrettyPrinter& prettyPrinter,
                       bool debugToSerial);

  static void onTxFrame(const uint8_t* data, uint16_t len, void* context);
  static void onRxFrame(const uint8_t* data, uint16_t len, bool error, void* context);

private:
  void handleTx(const uint8_t* data, uint16_t len);
  void handleRx(const uint8_t* data, uint16_t len, bool error);

  Stream& _io;
  SLinkTranslator& _translator;
  SLinkInterface& _stateObserver;
  SLinkDebugPrinter& _debugPrinter;
  SLinkPrettyPrinter& _prettyPrinter;
  bool _debugToSerial;
  SLinkMessage _rxMessage;
  SLinkMessage _txMessage;
};
