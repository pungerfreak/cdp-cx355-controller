#include <lvgl.h>

// uncomment a library for display driver
#define USE_TFT_ESPI_LIBRARY
// #define USE_ARDUINO_GFX_LIBRARY

#include "lv_xiao_round_screen.h"
#include "touch_gap_filter.h"

#include "io/SLinkCommandConsole.h"
#include "io/SLinkPrettyPrinter.h"
#include "system/SLinkSystem.h"
#include "ui/UiApp.h"
#include "ui/UiAdapter.h"

constexpr bool kDebugToSerial = true;

static SLinkSystem slinkSystem(Serial, kDebugToSerial);
static SLinkCommandConsole slinkConsole(Serial, slinkSystem.intentSource(), true);
static SLinkPrettyPrinter slinkPrinter(Serial);
static UiApp app;
static UiAdapter adapter(slinkSystem, app);

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

  app.init();
  slinkSystem.addCommandInput(slinkConsole);
  slinkSystem.addEventOutput(slinkPrinter);
  slinkSystem.begin();
  adapter.start();
}

void loop() {
  slinkSystem.poll();
  lv_timer_handler();  // let the GUI do its work
  delay(1);
}
