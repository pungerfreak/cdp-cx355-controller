#include "cddb/CddbClient.h"
#include <ctype.h>

CddbClient::CddbClient(const CddbClientConfig& cfg) : cfg_(cfg) {}

bool CddbClient::queryAndRead(const String& queryCmd, CddbMetadata& out) {
  if (!ensureWifi_()) return false;

  const String queryUrl = buildQueryUrl_(queryCmd);
  String queryBody = httpGet_(queryUrl);
  if (queryBody.length() == 0) {
    disconnectWifi_();
    return false;
  }

  String discId;
  if (!parseQuery_(queryBody, discId)) {
    disconnectWifi_();
    return false;
  }

  const String readUrl = buildReadUrl_(discId);
  String readBody = httpGet_(readUrl);
  if (readBody.length() == 0) {
    disconnectWifi_();
    return false;
  }

  const bool ok = parseRead_(readBody, out);
  disconnectWifi_();
  return ok;
}

bool CddbClient::ensureWifi_() {
  if (WiFi.status() == WL_CONNECTED) return true;
  if (!cfg_.wifiSsid || !cfg_.wifiSsid[0]) return false;

  WiFi.mode(WIFI_STA);
  WiFi.begin(cfg_.wifiSsid, cfg_.wifiPass ? cfg_.wifiPass : "");

  const uint32_t start = millis();
  while (WiFi.status() != WL_CONNECTED && (millis() - start) < 10000) {
    delay(50);
  }
  return WiFi.status() == WL_CONNECTED;
}

void CddbClient::disconnectWifi_() {
  WiFi.disconnect(true, true);
  WiFi.mode(WIFI_OFF);
}

String CddbClient::buildQueryUrl_(const String& cmd) const {
  String url = "http://";
  url += (cfg_.apiSubdomain && cfg_.apiSubdomain[0]) ? cfg_.apiSubdomain : "freedb";
  url += ".gnudb.org/~cddb/cddb.cgi?cmd=";
  url += cmd;
  url += "&hello=";
  url += (cfg_.helloEmail && cfg_.helloEmail[0]) ? cfg_.helloEmail : "email@example.com";
  url += "+";
  url += (cfg_.helloApp && cfg_.helloApp[0]) ? cfg_.helloApp : "s-link";
  url += "+";
  url += (cfg_.helloVersion && cfg_.helloVersion[0]) ? cfg_.helloVersion : "0.1";
  url += "&proto=6";
  return url;
}

String CddbClient::buildReadUrl_(const String& discId) const {
  String cmd = "cddb+read+data+";
  cmd += discId;
  return buildQueryUrl_(cmd);
}

String CddbClient::httpGet_(const String& url) {
  WiFiClient client;
  HTTPClient http;
  if (!http.begin(client, url)) {
    return "";
  }
  const int code = http.GET();
  String body;
  if (code > 0) {
    body = http.getString();
  }
  http.end();
  return body;
}

bool CddbClient::parseQuery_(const String& body, String& discIdOut) {
  int start = 0;
  int end = body.indexOf('\n', start);
  if (end < 0) end = body.length();
  String statusLine = body.substring(start, end);
  statusLine.trim();
  const int status = parseStatus_(statusLine);
  if (status == 0 || status == 202 || status == 403) return false;

  start = end + 1;
  while (start < body.length()) {
    end = body.indexOf('\n', start);
    if (end < 0) end = body.length();
    String line = body.substring(start, end);
    line.trim();
    if (line == ".") break;
    if (parseCandidateLine_(line, discIdOut)) {
      return true;
    }
    start = end + 1;
  }

  // fallback: try status line for 200 responses that embed the data
  if (discIdOut.isEmpty()) {
    parseCandidateLine_(statusLine, discIdOut);
  }
  return !discIdOut.isEmpty();
}

bool CddbClient::parseRead_(const String& body, CddbMetadata& out) {
  out = {};

  int start = 0;
  int end = body.indexOf('\n', start);
  if (end < 0) end = body.length();
  String statusLine = body.substring(start, end);
  statusLine.trim();
  const int status = parseStatus_(statusLine);
  if (status != 210) return false;

  start = end + 1;
  while (start < body.length()) {
    end = body.indexOf('\n', start);
    if (end < 0) end = body.length();
    String line = body.substring(start, end);
    line.trim();
    if (line == ".") break;
    if (line.startsWith("DTITLE=")) {
      String dtitle = trimPrefix_(line, "DTITLE=");
      int slash = dtitle.indexOf('/');
      if (slash >= 0) {
        out.artist = dtitle.substring(0, slash);
        out.artist.trim();
        out.title = dtitle.substring(slash + 1);
        out.title.trim();
      } else {
        out.title = dtitle;
      }
    } else if (line.startsWith("DISCID=")) {
      out.discId = trimPrefix_(line, "DISCID=");
    } else if (line.startsWith("TTITLE")) {
      int eq = line.indexOf('=');
      if (eq > 6 && eq < line.length()) {
        String idxStr = line.substring(6, eq);
        int idx = idxStr.toInt();
        if (idx >= 0 && idx < (int)CddbMetadata::kMaxTracks) {
          String val = line.substring(eq + 1);
          out.tracks[idx] = val;
          if ((size_t)(idx + 1) > out.trackCount) {
            out.trackCount = idx + 1;
          }
        }
      }
    }
    start = end + 1;
  }
  return true;
}

int CddbClient::parseStatus_(const String& line) const {
  int code = 0;
  uint8_t i = 0;
  while (i < line.length() && isspace(line[i])) i++;
  while (i < line.length() && isdigit(line[i])) {
    code = code * 10 + (line[i] - '0');
    i++;
  }
  return code;
}

bool CddbClient::parseCandidateLine_(const String& line, String& discIdOut) const {
  String working = line;
  working.trim();
  if (working.startsWith("data ")) {
    int firstSpace = working.indexOf(' ', 5);
    if (firstSpace < 0) firstSpace = working.length();
    int secondSpace = working.indexOf(' ', firstSpace + 1);
    if (secondSpace < 0) secondSpace = working.length();
    discIdOut = working.substring(firstSpace + 1, secondSpace);
    return true;
  }

  // generic: second token is disc id
  int first = working.indexOf(' ');
  if (first <= 0) return false;
  int second = working.indexOf(' ', first + 1);
  if (second <= 0) second = working.length();
  discIdOut = working.substring(first + 1, second);
  return !discIdOut.isEmpty();
}

String CddbClient::trimPrefix_(const String& s, const char* prefix) const {
  const size_t plen = strlen(prefix);
  if (s.startsWith(prefix)) {
    return s.substring(plen);
  }
  return "";
}
