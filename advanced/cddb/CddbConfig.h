#pragma once
#include "Secrets.h"

// Override via Secrets.h; fall back to reasonable defaults if blank.
constexpr const char* kCddbApiSubdomain = kSecretGnudbApiSubdomain;
constexpr const char* kCddbHelloEmail = kSecretGnudbHelloEmail;
constexpr const char* kCddbHelloAppName = "app-name";
constexpr const char* kCddbHelloVersion = "0.1";

constexpr const char* kCddbWifiSsid = kSecretWifiSsid;
constexpr const char* kCddbWifiPass = kSecretWifiPass;
