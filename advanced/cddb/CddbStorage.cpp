#include "cddb/CddbStorage.h"

bool CddbStorage::begin() {
  mounted_ = SPIFFS.begin(true);
  return mounted_;
}

String CddbStorage::path_(uint16_t disc) const {
  String p = "/cddb_";
  p += disc;
  p += ".json";
  return p;
}

bool CddbStorage::has(uint16_t disc) const {
  if (!mounted_) return false;
  String p = path_(disc);
  return SPIFFS.exists(p);
}

bool CddbStorage::save(uint16_t disc, const CddbMetadata& meta, const CddbLookup::Result& lookup) {
  if (!mounted_) return false;
  String p = path_(disc);
  File f = SPIFFS.open(p, FILE_WRITE);
  if (!f) return false;
  f.print("{\"disc\":");
  f.print(disc);
  f.print(",\"discid\":\"");
  f.print(meta.discId.length() ? meta.discId : String(lookup.discId, HEX));
  f.print("\",\"artist\":\"");
  f.print(meta.artist);
  f.print("\",\"title\":\"");
  f.print(meta.title);
  f.print("\",\"tracks\":[");
  for (size_t i = 0; i < meta.trackCount; ++i) {
    if (i) f.print(',');
    f.print("\"");
    f.print(meta.tracks[i]);
    f.print("\"");
  }
  f.print("],\"lengths_sec\":[");
  for (uint8_t i = 0; i < lookup.trackCount; ++i) {
    if (i) f.print(',');
    f.print(lookup.trackSeconds[i]);
  }
  f.print("]}");
  f.close();
  return true;
}

bool CddbStorage::remove(uint16_t disc) {
  if (!mounted_) return false;
  String p = path_(disc);
  return SPIFFS.remove(p);
}

bool CddbStorage::trackCount(uint16_t disc, uint8_t& outCount) const {
  outCount = 0;
  if (!mounted_) return false;
  File f = SPIFFS.open(path_(disc), FILE_READ);
  if (!f) return false;
  const String needle = "\"lengths_sec\":[";
  String window;
  bool found = false;
  while (f.available()) {
    char c = (char)f.read();
    window += c;
    if (window.length() > needle.length()) {
      window.remove(0, window.length() - needle.length());
    }
    if (window.length() >= needle.length() &&
        window.substring(window.length() - needle.length()) == needle) {
      found = true;
      break;
    }
  }
  if (!found) {
    f.close();
    return false;
  }
  bool inArray = true;
  bool inNumber = false;
  uint16_t count = 0;
  while (f.available()) {
    char c = (char)f.read();
    if (!inArray) {
      if (c == '[') inArray = true;
      continue;
    }
    if (c == ']') {
      break;
    }
    if (c >= '0' && c <= '9') {
      if (!inNumber) {
        ++count;
        inNumber = true;
      }
    } else {
      inNumber = false;
    }
  }
  f.close();
  if (count == 0 || count > 255) return false;
  outCount = (uint8_t)count;
  return true;
}
