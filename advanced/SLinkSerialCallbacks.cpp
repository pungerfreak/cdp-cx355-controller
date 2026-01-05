#include "SLinkSerialCallbacks.h"

SLinkSerialCallbacks::SLinkSerialCallbacks(Stream& io,
                                           SLinkTranslator& translator,
                                           SLinkStateTracker& stateTracker,
                                           SLinkDebugPrinter& debugPrinter,
                                           SLinkPrettyPrinter& prettyPrinter,
                                           bool debugToSerial)
    : _io(io),
      _translator(translator),
      _stateTracker(stateTracker),
      _debugPrinter(debugPrinter),
      _prettyPrinter(prettyPrinter),
      _debugToSerial(debugToSerial) {}

void SLinkSerialCallbacks::onTxFrame(const uint8_t* data, uint16_t len, void* context) {
  if (!context) return;
  static_cast<SLinkSerialCallbacks*>(context)->handleTx(data, len);
}

void SLinkSerialCallbacks::onRxFrame(const uint8_t* data, uint16_t len,
                                     bool error, void* context) {
  if (!context) return;
  static_cast<SLinkSerialCallbacks*>(context)->handleRx(data, len, error);
}

void SLinkSerialCallbacks::handleTx(const uint8_t* data, uint16_t len) {
  if (!_debugToSerial) return;
  if (!data || len == 0) return;
  if (_translator.decode(data, len, _txMessage, true)) {
    SLinkDebugInfo txDebug = SLinkDispatcher::buildDebugInfo(_txMessage);
    _io.print("tx ");
    SLinkDispatcher::dispatch(_txMessage, _debugPrinter, &txDebug);
    // _io.flush();
  }
}

void SLinkSerialCallbacks::handleRx(const uint8_t* data, uint16_t len, bool error) {
  if (error) {
    if (_debugToSerial) _io.println("frame error");
    return;
  }
  if (!data || len == 0) return;
  if (_translator.decode(data, len, _rxMessage)) {
    SLinkDebugInfo debugInfo = SLinkDispatcher::buildDebugInfo(_rxMessage);
    SLinkDispatcher::dispatch(_rxMessage, _stateTracker, &debugInfo);
    if (_debugToSerial) {
      _io.print("rx ");
      SLinkDispatcher::dispatch(_rxMessage, _debugPrinter, &debugInfo);
    } else {
      SLinkDispatcher::dispatch(_rxMessage, _prettyPrinter, &debugInfo);
    }
  }
}
