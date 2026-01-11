#include "io/SLinkCommandConsole.h"
#include <string.h>
#include "transport/SLinkTx.h"

SLinkCommandConsole::SLinkCommandConsole(Stream& io,
                                         SLinkCommandIntentSource& input,
                                         bool printTx,
                                         SLinkTx* rawTx)
    : _io(io), _input(input), _rawTx(rawTx), _printTx(printTx) {}

void SLinkCommandConsole::printHelp() {
  _io.println("commands: PLAY, STOP, PAUSE, POWER_ON, POWER_OFF, CHANGE_DISC <1-300>, CHANGE_TRACK <1-99>, SEND <HEX>");
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

void SLinkCommandConsole::printTxBytes(const char* label, const uint8_t* data, uint8_t len) {
  if (!_printTx) return;
  _io.print("tx: ");
  _io.print(label);
  _io.print(' ');
  for (uint8_t i = 0; i < len; ++i) {
    if (data[i] < 0x10) {
      _io.print('0');
    }
    _io.print(data[i], HEX);
  }
  _io.println();
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
  printTx("CHANGE_DISC", disc);
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
  printTx("CHANGE_TRACK", track);
  if (_input.changeTrack((uint8_t)track)) {
  } else {
    _io.println("unsupported: TRACK");
  }
  return true;
}

bool SLinkCommandConsole::dispatchSend(const char* cmd) {
  const char* prefix = "SEND ";
  const size_t len = strlen(prefix);
  if (!cmd || strncmp(cmd, prefix, len) != 0) {
    return false;
  }
  if (_rawTx == nullptr) {
    _io.println("unsupported: SEND");
    return true;
  }

  const char* hex = cmd + len;
  if (!hex || !*hex) {
    _io.println("invalid: SEND");
    return true;
  }

  static constexpr uint8_t kMaxSendBytes = 16;
  uint8_t bytes[kMaxSendBytes];
  uint8_t count = 0;
  bool haveNibble = false;
  uint8_t nibble = 0;

  for (uint8_t i = 0; hex[i] != '\0'; ++i) {
    char c = hex[i];
    if (c == ' ') continue;
    uint8_t value = 0;
    if (c >= '0' && c <= '9') {
      value = (uint8_t)(c - '0');
    } else if (c >= 'A' && c <= 'F') {
      value = (uint8_t)(c - 'A' + 10);
    } else {
      _io.println("invalid: SEND");
      return true;
    }
    if (!haveNibble) {
      nibble = value;
      haveNibble = true;
    } else {
      if (count >= kMaxSendBytes) {
        _io.println("invalid: SEND");
        return true;
      }
      bytes[count++] = (uint8_t)((nibble << 4) | value);
      haveNibble = false;
    }
  }

  if (haveNibble || count == 0) {
    _io.println("invalid: SEND");
    return true;
  }

  printTxBytes("SEND", bytes, count);
  if (!_rawTx->sendBytes(bytes, count)) {
    _io.println("failed: SEND");
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
  if (dispatchSend(normalized)) return;

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
