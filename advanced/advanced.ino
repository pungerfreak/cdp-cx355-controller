#include <lvgl.h>

// uncomment a library for display driver
#define USE_TFT_ESPI_LIBRARY
// #define USE_ARDUINO_GFX_LIBRARY

#include "lv_xiao_round_screen.h"
#include "touch_gap_filter.h"

#include "io/SLinkCommandConsole.h"
#include "io/SLinkPrettyPrinter.h"
#include "io/ConsoleAdapter.h"
#include "system/SLinkSystem.h"
#include "ui/UiApp.h"
#include "ui/UiAdapter.h"
#include "ui/LoadingScreen.h"
#include "cddb/CddbConfig.h"
#include "cddb/CddbLookup.h"
#include "cddb/CddbClient.h"
#include "cddb/CddbStorage.h"
#include "cddb/CddbIndexer.h"

constexpr bool kDebugToSerial = true;

static SLinkSystem slinkSystem(Serial, kDebugToSerial);
static CddbStorage cddbStorage;
static CddbLookup cddbLookup(slinkSystem);
static CddbClient cddbClient({kCddbApiSubdomain,
                              kCddbHelloEmail,
                              kCddbHelloAppName,
                              kCddbHelloVersion,
                              kCddbWifiSsid,
                              kCddbWifiPass});
static CddbIndexer cddbIndexer(slinkSystem, cddbLookup, cddbClient, cddbStorage, 300);
static SLinkCommandConsole slinkConsole(Serial,
                                        slinkSystem.intentSource(),
                                        true,
                                        &slinkSystem.tx(),
                                        &cddbStorage);
static SLinkPrettyPrinter slinkPrinter(Serial);
static ConsoleAdapter consoleAdapter(slinkSystem, slinkConsole, slinkPrinter);
static UiApp app;
static UiAdapter adapter(slinkSystem, app);
static LoadingScreen loadingScreen;
static bool cancelRequested = false;

static void onCancelClicked(lv_event_t* e) {
  cancelRequested = true;
}

void setup() {
  Serial.begin(230400);
  Serial.println("XIAO round screen - LVGL_Arduino");
  Serial.println("round_lvgl_v9_smoke boot");

  #if defined(LVGL_VERSION_MAJOR)
  Serial.print("LVGL version: ");
  Serial.print(LVGL_VERSION_MAJOR);
  Serial.print(".");
  Serial.print(LVGL_VERSION_MINOR);
  Serial.print(".");
  Serial.println(LVGL_VERSION_PATCH);
  #else
  Serial.println("LVGL version: unknown");
  #endif

  lv_init();
  #if LVGL_VERSION_MAJOR == 9
  lv_tick_set_cb(millis);
  #endif

  lv_xiao_disp_init();
  lv_xiao_touch_init();
  install_touch_gap_filter();

  loadingScreen.init(lv_screen_active());
  loadingScreen.hide();
  lv_obj_add_event_cb(loadingScreen.cancelButton(), onCancelClicked, LV_EVENT_CLICKED, nullptr);

  if (!cddbStorage.begin()) {
    Serial.println("cddb storage unavailable; CDDB data will not be saved");
  }

  slinkSystem.begin();
  cddbLookup.start();
  app.init();
  adapter.setIndexer(&cddbIndexer, &loadingScreen);
  adapter.setStorage(&cddbStorage);
  consoleAdapter.start();
  adapter.start();
  // Push whatever state we have cached, then ask the unit for fresh status so the UI syncs at boot.
  slinkSystem.publishSnapshot();
  adapter.requestStatus();
}

void loop() {
  consoleAdapter.poll();
  slinkSystem.poll();
  uint32_t now = millis();
  if (cancelRequested) {
    cancelRequested = false;
    cddbIndexer.abort();
    cddbLookup.cancel();
    slinkSystem.clearIntents();
    slinkSystem.intentSource().stop();
    loadingScreen.hide();
  }
  cddbIndexer.tick(now);
  CddbIndexStatus st = cddbIndexer.status();
  static bool wasActive = false;
  if (st.active) {
    loadingScreen.show();
    loadingScreen.setStatus(st.stage, st.currentDisc, st.totalDiscs, st.percent, st.unitsDone, st.unitsTotal);
  } else {
    if (wasActive) {
      slinkSystem.intentSource().stop();
    }
    loadingScreen.hide();
  }
  wasActive = st.active;

  lv_timer_handler();  // let the GUI do its work
  delay(1);
}
