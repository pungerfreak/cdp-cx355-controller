#include "SLinkCommandConsole.h"

SLinkCommandConsole::SLinkCommandConsole(Stream& io, SLinkCommandSink& sink)
    : _io(io), _sink(sink) {}

void SLinkCommandConsole::printHelp() {
  _io.println("commands: PLAY, STOP, PAUSE, POWER_ON, POWER_OFF");
}

bool SLinkCommandConsole::normalizeCommand(const char* in, char* out, uint8_t outSize) const {
  if (!in || !out || outSize < 2) return false;
  uint8_t n = 0;
  bool lastUnderscore = false;
  for (uint8_t i = 0; in[i] != '\0'; i++) {
    char c = in[i];
    if (c <= ' ') {
      if (!lastUnderscore && n < (outSize - 1)) {
        out[n++] = '_';
        lastUnderscore = true;
      }
      continue;
    }
    if (c >= 'a' && c <= 'z') c = (char)(c - 'a' + 'A');
    if (n < (outSize - 1)) {
      out[n++] = c;
      lastUnderscore = false;
    }
  }
  if (n && out[n - 1] == '_') n--;
  out[n] = '\0';
  return n > 0;
}

bool SLinkCommandConsole::isHelpCommand(const char* cmd) const {
  if (!cmd) return false;
  return (strcmp(cmd, "?") == 0 || strcmp(cmd, "HELP") == 0);
}

bool SLinkCommandConsole::parseCommand(const char* cmd, SLinkCommandId& id) const {
  if (!cmd) return false;
  if (strcmp(cmd, "PLAY") == 0) {
    id = PLAY;
    return true;
  }
  if (strcmp(cmd, "STOP") == 0) {
    id = STOP;
    return true;
  }
  if (strcmp(cmd, "PAUSE") == 0) {
    id = PAUSE;
    return true;
  }
  if (strcmp(cmd, "POWER_ON") == 0) {
    id = POWER_ON;
    return true;
  }
  if (strcmp(cmd, "POWER_OFF") == 0) {
    id = POWER_OFF;
    return true;
  }
  return false;
}

void SLinkCommandConsole::handleLine(const char* line) {
  char normalized[kBufferSize];
  if (!normalizeCommand(line, normalized, sizeof(normalized))) return;

  if (isHelpCommand(normalized)) {
    printHelp();
    return;
  }

  SLinkCommandId id;
  if (parseCommand(normalized, id)) {
    _io.print("tx: ");
    _io.println(normalized);
    _sink.sendCommand(id);
    return;
  }

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
