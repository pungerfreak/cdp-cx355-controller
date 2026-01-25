#include "cddb/CddbStorage.h"

bool CddbStorage::begin() {
  mounted_ = SPIFFS.begin(true);
  if (mounted_) {
    Serial.println("cddb storage: mounted");
  } else {
    Serial.println("cddb storage: mount failed");
  }
  return mounted_;
}

String CddbStorage::path_(uint16_t disc) const {
  String p = "/cddb_";
  p += disc;
  p += ".json";
  return p;
}

bool CddbStorage::has(uint16_t disc) const {
  if (!mounted_) {
    Serial.println("cddb storage: has() while not mounted");
    return false;
  }
  String p = path_(disc);
  bool exists = SPIFFS.exists(p);
  Serial.print("cddb storage: has ");
  Serial.print(p);
  Serial.print(" -> ");
  Serial.println(exists ? "true" : "false");
  return exists;
}

bool CddbStorage::save(uint16_t disc, const CddbMetadata& meta, const CddbLookup::Result& lookup) {
  if (!mounted_) {
    Serial.println("cddb storage: not mounted");
    return false;
  }
  String p = path_(disc);
  Serial.print("cddb storage: writing ");
  Serial.println(p);
  File f = SPIFFS.open(p, FILE_WRITE);
  if (!f) {
    Serial.println("cddb storage: open failed");
    return false;
  }
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
