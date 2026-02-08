#ifndef CDDB_CLIENT_H_
#define CDDB_CLIENT_H_
#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>

struct CddbMetadata {
  static constexpr size_t kMaxTracks = 100;
  static constexpr size_t kMaxCandidates = 10;
  String discId;
  String artist;
  String title;
  String tracks[kMaxTracks];
  size_t trackCount = 0;
  size_t candidateCount = 0;
  String candidateCategory[kMaxCandidates];
  String candidateDiscId[kMaxCandidates];
  String candidateTitle[kMaxCandidates];
};

struct CddbClientConfig {
  const char* apiSubdomain = nullptr;
  const char* helloEmail = nullptr;
  const char* helloApp = nullptr;
  const char* helloVersion = nullptr;
  const char* wifiSsid = nullptr;
  const char* wifiPass = nullptr;
};

class CddbClient {
public:
  explicit CddbClient(const CddbClientConfig& cfg);

  // queryCmd is the "cddb+query+..." command (no URL encoding needed).
  // Returns true on successful query+read and fills metadata.
  bool queryAndRead(const String& queryCmd, CddbMetadata& out);

private:
  bool ensureWifi_();
  void disconnectWifi_();
  String buildQueryUrl_(const String& cmd, const char* overrideSubdomain = nullptr) const;
  String buildReadUrl_(const String& discId, const char* overrideSubdomain = nullptr) const;
  String httpGet_(const String& url);
  bool parseQuery_(const String& body, String& discIdOut, CddbMetadata* meta);
  bool parseRead_(const String& body, CddbMetadata& out);
  int parseStatus_(const String& line) const;
  bool parseCandidateLine_(const String& line, String& discIdOut, CddbMetadata* meta) const;
  String trimPrefix_(const String& s, const char* prefix) const;

  CddbClientConfig cfg_;
};

#endif  // CDDB_CLIENT_H_
