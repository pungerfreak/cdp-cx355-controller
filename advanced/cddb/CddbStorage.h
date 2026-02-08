#ifndef CDDB_STORAGE_H_
#define CDDB_STORAGE_H_
#include <Arduino.h>
#include <FS.h>
#include "SPIFFS.h"
#include "cddb/CddbClient.h"
#include "cddb/CddbLookup.h"

class CddbStorage {
public:
  bool begin();
  bool save(uint16_t disc, const CddbMetadata& meta, const CddbLookup::Result& lookup);
  bool has(uint16_t disc) const;
  bool trackCount(uint16_t disc, uint8_t& outCount) const;
  bool remove(uint16_t disc);
  bool clearAll(uint16_t* deletedCount = nullptr);
  bool mounted() const { return mounted_; }

private:
  String path_(uint16_t disc) const;
  bool mounted_ = false;
};

#endif  // CDDB_STORAGE_H_
