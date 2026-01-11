#pragma once
#include <Arduino.h>
#include "transport/SLinkDecode.h"

struct SLinkDebugInfo {
  const uint8_t* raw = nullptr;
  uint16_t len = 0;
  bool truncated = false;
  uint8_t unit = 0;
  uint8_t cmd = 0;
  bool hasChecksum = false;
  uint8_t checksumLast = 0;
  uint8_t checksumSum = 0;
  uint8_t checksumXor = 0;
  bool checksumSumMatch = false;
  bool checksumSumInvMatch = false;
  bool checksumXorMatch = false;
  bool checksumXorInvMatch = false;
};

struct SLinkDiscInfo {
  bool present = false;
  bool valid = false;
  uint16_t disc = 0;
  uint8_t raw = 0;
};

struct SLinkTrackInfo {
  bool present = false;
  bool valid = false;
  uint8_t track = 0;
  uint8_t raw = 0;
  bool lengthPresent = false;
  bool lengthValid = false;
  uint8_t minutes = 0;
  uint8_t seconds = 0;
  uint8_t minRaw = 0;
  uint8_t secRaw = 0;
};

class SLinkUnitEventHandler {
public:
  virtual ~SLinkUnitEventHandler() = default;

  virtual void play(const SLinkDebugInfo* debug) = 0;
  virtual void stop(const SLinkDebugInfo* debug) = 0;
  virtual void pause(const SLinkDebugInfo* debug) = 0;
  virtual void powerOn(const SLinkDebugInfo* debug) = 0;
  virtual void powerOff(const SLinkDebugInfo* debug) = 0;
  virtual void changeDisc(const SLinkDiscInfo& disc, const SLinkDebugInfo* debug) = 0;
  virtual void changeTrack(const SLinkDiscInfo& disc,
                           const SLinkTrackInfo& track,
                           const SLinkDebugInfo* debug) = 0;
  virtual void ready(const SLinkDebugInfo* debug) = 0;
  virtual void changingDisc(const SLinkDebugInfo* debug) = 0;
  virtual void discReady(const SLinkDiscInfo& disc, const SLinkDebugInfo* debug) = 0;
  virtual void discLoaded(const SLinkDiscInfo& disc, const SLinkDebugInfo* debug) = 0;
  virtual void loadingDisc(const SLinkDiscInfo& disc, const SLinkDebugInfo* debug) = 0;
  virtual void changingTrack(const SLinkDiscInfo& disc,
                             const SLinkTrackInfo& track,
                             const SLinkDebugInfo* debug) = 0;
  virtual void unknown(const SLinkDebugInfo* debug) = 0;
};

class SLinkDispatcher {
public:
  static SLinkDebugInfo buildDebugInfo(const SLinkMessage& msg);
  static void dispatch(const SLinkMessage& msg,
                       SLinkUnitEventHandler& iface,
                       const SLinkDebugInfo* debug);
};
