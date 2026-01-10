#include "SLinkFrameCallbacks.h"

SLinkFrameCallbacks::SLinkFrameCallbacks(Stream& io,
                                         SLinkTranslator& translator,
                                         SLinkUnitEventHandler& stateObserver,
                                         SLinkDebugPrinter& debugPrinter,
                                         SLinkPrettyPrinter& prettyPrinter,
                                         bool debugToSerial)
    : _io(io),
      _translator(translator),
      _stateObserver(stateObserver),
      _debugPrinter(debugPrinter),
      _prettyPrinter(prettyPrinter),
      _debugToSerial(debugToSerial) {}

void SLinkFrameCallbacks::onTxFrame(const uint8_t* data, uint16_t len, void* context) {
  if (!context) return;
  static_cast<SLinkFrameCallbacks*>(context)->handleTx(data, len);
}

void SLinkFrameCallbacks::onRxFrame(const uint8_t* data, uint16_t len,
                                     bool error, void* context) {
  if (!context) return;
  static_cast<SLinkFrameCallbacks*>(context)->handleRx(data, len, error);
}

void SLinkFrameCallbacks::handleTx(const uint8_t* data, uint16_t len) {
  if (!_debugToSerial) return;
  if (!data || len == 0) return;
  if (_translator.decode(data, len, _txMessage, true)) {
    SLinkDebugInfo txDebug = SLinkDispatcher::buildDebugInfo(_txMessage);
    _io.print("tx ");
    SLinkDispatcher::dispatch(_txMessage, _debugPrinter, &txDebug);
    // _io.flush();
  }
}

void SLinkFrameCallbacks::handleRx(const uint8_t* data, uint16_t len, bool error) {
  if (error) {
    if (_debugToSerial) _io.println("frame error");
    return;
  }
  if (!data || len == 0) return;
  if (_translator.decode(data, len, _rxMessage)) {
    SLinkDebugInfo debugInfo = SLinkDispatcher::buildDebugInfo(_rxMessage);
    SLinkDispatcher::dispatch(_rxMessage, _stateObserver, &debugInfo);
    if (_debugToSerial) {
      _io.print("rx ");
      SLinkDispatcher::dispatch(_rxMessage, _debugPrinter, &debugInfo);
    } else {
      SLinkDispatcher::dispatch(_rxMessage, _prettyPrinter, &debugInfo);
    }
  }
}
