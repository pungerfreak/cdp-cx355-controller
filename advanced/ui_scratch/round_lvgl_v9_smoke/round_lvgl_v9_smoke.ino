#include <lvgl.h>

// uncomment a library for display driver
#define USE_TFT_ESPI_LIBRARY
// #define USE_ARDUINO_GFX_LIBRARY

#include "lv_xiao_round_screen.h"
#include "UiApp.h"
#include "UiAdapter.h"

static uint32_t lastTouchLogMs;
static bool lastTouching;

constexpr bool kDebugToSerial = true;
constexpr uint32_t kTouchLogIntervalMs = 100;

static SLinkSystem slinkSystem(Serial, kDebugToSerial);
static UiApp app;
static UiAdapter adapter(slinkSystem, app);

static void log_touch(uint32_t now_ms)
{
    lv_indev_t *indev = lv_indev_get_next(NULL);
    if (indev == NULL) {
        if (lastTouching) {
            Serial.println("touch: none");
            lastTouching = false;
        }
        return;
    }

    lv_indev_state_t state = lv_indev_get_state(indev);
    if (state == LV_INDEV_STATE_PRESSED) {
        if (!lastTouching || now_ms - lastTouchLogMs >= kTouchLogIntervalMs) {
            lv_point_t point;
            lv_indev_get_point(indev, &point);
            Serial.print("touch: x=");
            Serial.print(point.x);
            Serial.print(" y=");
            Serial.println(point.y);
            lastTouchLogMs = now_ms;
        }
        lastTouching = true;
    } else {
        if (lastTouching) {
            Serial.println("touch: none");
        }
        lastTouching = false;
    }
}

void setup()
{
    Serial.begin( 230400 );  //prepare for possible serial debug
    Serial.println( "XIAO round screen - LVGL_Arduino" );
    Serial.println( "round_lvgl_v9_smoke boot" );

    #if defined(LVGL_VERSION_MAJOR)
    Serial.print( "LVGL version: " );
    Serial.print( LVGL_VERSION_MAJOR );
    Serial.print( "." );
    Serial.print( LVGL_VERSION_MINOR );
    Serial.print( "." );
    Serial.println( LVGL_VERSION_PATCH );
    #else
    Serial.println( "LVGL version: unknown" );
    #endif

    lv_init();
    #if LVGL_VERSION_MAJOR == 9
    lv_tick_set_cb(millis);
    #endif

    lv_xiao_disp_init();
    lv_xiao_touch_init();

    app.init();
    slinkSystem.begin();
    adapter.start();
}

void loop()
{
    uint32_t now_ms = millis();

    slinkSystem.poll();
    lv_timer_handler();  //let the GUI do its work
    log_touch(now_ms);
    delay( 1 );
}
