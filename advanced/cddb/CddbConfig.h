#ifndef CDDB_CONFIG_H_
#define CDDB_CONFIG_H_

#include "Secrets.h"

// Override via Secrets.h; fall back to reasonable defaults if blank.
static const char* const kCddbApiSubdomain = (kSecretGnudbApiSubdomain[0] != '\0')
                                                 ? kSecretGnudbApiSubdomain
                                                 : "freedb";
static const char* const kCddbHelloEmail = kSecretGnudbHelloEmail;
static const char* const kCddbHelloAppName = "app-name";
static const char* const kCddbHelloVersion = "0.1";

static const char* const kCddbWifiSsid = kSecretWifiSsid;
static const char* const kCddbWifiPass = kSecretWifiPass;

#endif  // CDDB_CONFIG_H_
