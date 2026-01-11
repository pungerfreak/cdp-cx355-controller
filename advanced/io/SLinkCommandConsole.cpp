#include "io/SLinkCommandConsole.h"
#include <string.h>

SLinkCommandConsole::SLinkCommandConsole(Stream& io,
                                         SLinkCommandIntentSource& input,
                                         bool printTx)
    : _io(io), _input(input), _printTx(printTx) {}

void SLinkCommandConsole::printHelp() {
  _io.println("commands: PLAY, STOP, PAUSE, POWER_ON, POWER_OFF, CHANGE_DISC <1-300>, CHANGE_TRACK <1-99>");
}

bool SLinkCommandConsole::normalizeCommand(const char* in, char* out, uint8_t outSize) const {
  if (!in || !out || outSize < 2) return false;
  uint8_t n = 0;
  bool lastSpace = false;
  for (uint8_t i = 0; in[i] != '\0'; i++) {
    char c = in[i];
    if (c <= ' ') {
      if (!lastSpace && n < (outSize - 1)) {
        out[n++] = ' ';
        lastSpace = true;
      }
      continue;
    }
    if (c >= 'a' && c <= 'z') c = (char)(c - 'a' + 'A');
    if (n < (outSize - 1)) {
      out[n++] = c;
      lastSpace = false;
    }
  }
  if (n && out[n - 1] == ' ') n--;
  out[n] = '\0';
  return n > 0;
}

bool SLinkCommandConsole::isHelpCommand(const char* cmd) const {
  if (!cmd) return false;
  return (strcmp(cmd, "?") == 0 || strcmp(cmd, "HELP") == 0);
}

bool SLinkCommandConsole::parseNumber(const char* in, uint16_t& value) const {
  if (!in || !*in) return false;
  uint32_t v = 0;
  for (uint8_t i = 0; in[i] != '\0'; i++) {
    char c = in[i];
    if (c < '0' || c > '9') return false;
    v = v * 10u + (uint32_t)(c - '0');
    if (v > 65535u) return false;
  }
  value = (uint16_t)v;
  return true;
}

bool SLinkCommandConsole::dispatchSimple(const char* cmd) {
  if (!cmd) return false;
  if (strcmp(cmd, "PLAY") == 0) {
    printTx("PLAY");
    if (!_input.play()) _io.println("unsupported: PLAY");
    return true;
  }
  if (strcmp(cmd, "STOP") == 0) {
    printTx("STOP");
    if (!_input.stop()) _io.println("unsupported: STOP");
    return true;
  }
  if (strcmp(cmd, "PAUSE") == 0) {
    printTx("PAUSE");
    if (!_input.pause()) _io.println("unsupported: PAUSE");
    return true;
  }
  if (strcmp(cmd, "POWER_ON") == 0) {
    printTx("POWER_ON");
    if (!_input.powerOn()) _io.println("unsupported: POWER_ON");
    return true;
  }
  if (strcmp(cmd, "POWER_OFF") == 0) {
    printTx("POWER_OFF");
    if (!_input.powerOff()) _io.println("unsupported: POWER_OFF");
    return true;
  }
  return false;
}

void SLinkCommandConsole::printTx(const char* label) {
  if (!_printTx) return;
  _io.print("tx: ");
  _io.println(label);
}

void SLinkCommandConsole::printTx(const char* label, uint16_t value) {
  if (!_printTx) return;
  _io.print("tx: ");
  _io.print(label);
  _io.print(' ');
  _io.println(value);
}

bool SLinkCommandConsole::dispatchDisc(const char* cmd) {
  uint16_t disc = 0;
  const char* prefix = "CHANGE_DISC ";
  const size_t len = strlen(prefix);
  if (!cmd || strncmp(cmd, prefix, len) != 0) {
    return false;
  }
  if (!parseNumber(cmd + len, disc)) {
    return false;
  }
  if (disc == 0 || disc > 300u) {
    _io.println("invalid: DISC");
    return true;
  }
  printTx("DISC", disc);
  if (_input.changeDisc(disc)) {
  } else {
    _io.println("unsupported: DISC");
  }
  return true;
}

bool SLinkCommandConsole::dispatchTrack(const char* cmd) {
  uint16_t track = 0;
  const char* prefix = "CHANGE_TRACK ";
  const size_t len = strlen(prefix);
  if (!cmd || strncmp(cmd, prefix, len) != 0) {
    return false;
  }
  if (!parseNumber(cmd + len, track)) {
    return false;
  }
  if (track == 0 || track > 99u) {
    _io.println("invalid: TRACK");
    return true;
  }
  printTx("TRACK", track);
  if (_input.changeTrack((uint8_t)track)) {
  } else {
    _io.println("unsupported: TRACK");
  }
  return true;
}

void SLinkCommandConsole::handleLine(const char* line) {
  char normalized[kBufferSize];
  if (!normalizeCommand(line, normalized, sizeof(normalized))) return;

  if (isHelpCommand(normalized)) {
    printHelp();
    return;
  }

  if (dispatchSimple(normalized)) return;
  if (dispatchDisc(normalized)) return;
  if (dispatchTrack(normalized)) return;

  _io.print("unknown: ");
  _io.println(normalized);
}

void SLinkCommandConsole::poll() {
  while (_io.available() > 0) {
    char c = (char)_io.read();
    if (c == '\n' || c == '\r') {
      if (_len == 0) continue;
      _buf[_len] = '\0';
      _len = 0;
      handleLine(_buf);
    } else if (_len < (kBufferSize - 1)) {
      _buf[_len++] = c;
    }
  }
}
