#include "io/ConsoleAdapter.h"

ConsoleAdapter::ConsoleAdapter(SLinkSystem& system,
                               SLinkCommandConsole& console,
                               SLinkPrettyPrinter& printer)
    : _system(system), _console(console), _printer(printer) {}

void ConsoleAdapter::start() {
  if (_attached) return;
  _system.addUnitObserver(_printer);
  _attached = true;
}

void ConsoleAdapter::stop() {
  if (!_attached) return;
  _system.removeUnitObserver(_printer);
  _attached = false;
}

void ConsoleAdapter::poll() {
  _console.poll();
}

void ConsoleAdapter::requestStatus() {
  _system.intentSource().getStatus();
}
