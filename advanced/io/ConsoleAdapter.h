#pragma once

#include "SLinkCommandConsole.h"
#include "SLinkPrettyPrinter.h"
#include "system/SLinkSystem.h"

// Bridges the serial console + pretty printer onto the newer
// observer/intent wiring used by the UI adapter.
class ConsoleAdapter {
public:
  ConsoleAdapter(SLinkSystem& system,
                 SLinkCommandConsole& console,
                 SLinkPrettyPrinter& printer);

  void start();
  void stop();
  void poll();
  void requestStatus();

private:
  SLinkSystem& _system;
  SLinkCommandConsole& _console;
  SLinkPrettyPrinter& _printer;
  bool _attached = false;
};
