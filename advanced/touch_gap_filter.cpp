#include "touch_gap_filter.h"

static const uint32_t RELEASE_DEBOUNCE_MS = 40;

static lv_indev_read_cb_t orig_read_cb = nullptr;
static bool latched_pressed = false;
static uint32_t last_pressed_ms = 0;

static void filtered_touch_read(lv_indev_t* indev, lv_indev_data_t* data)
{
    if (orig_read_cb != nullptr) {
        orig_read_cb(indev, data);
    }

    if (data->state == LV_INDEV_STATE_PRESSED) {
        latched_pressed = true;
        last_pressed_ms = lv_tick_get();
        return;
    }

    if (latched_pressed && data->state == LV_INDEV_STATE_RELEASED) {
        uint32_t now = lv_tick_get();
        if (now - last_pressed_ms < RELEASE_DEBOUNCE_MS) {
            data->state = LV_INDEV_STATE_PRESSED;
        } else {
            latched_pressed = false;
        }
    }
}

void install_touch_gap_filter()
{
    if (orig_read_cb != nullptr) {
        return;
    }

    lv_indev_t* indev = lv_indev_get_next(nullptr);
    while (indev != nullptr) {
        if (lv_indev_get_type(indev) == LV_INDEV_TYPE_POINTER) {
            orig_read_cb = lv_indev_get_read_cb(indev);
            if (orig_read_cb != nullptr) {
                lv_indev_set_read_cb(indev, filtered_touch_read);
            }
            break;
        }
        indev = lv_indev_get_next(indev);
    }
}
