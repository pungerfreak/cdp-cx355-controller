#pragma once

#include <lvgl.h>
#include <stddef.h>
#include <stdint.h>

inline void setup_root_obj(lv_obj_t* root)
{
    lv_obj_set_size(root, LV_HOR_RES, LV_VER_RES);
    lv_obj_set_pos(root, 0, 0);
    lv_obj_set_layout(root, LV_LAYOUT_NONE);
    lv_obj_set_style_pad_all(root, 0, 0);
    lv_obj_set_style_pad_row(root, 0, 0);
    lv_obj_set_style_pad_column(root, 0, 0);
    lv_obj_clear_flag(root, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_opa(root, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_opa(root, LV_OPA_TRANSP, 0);
}

enum class UiAction : uint8_t {
    PrevTrack = 0,
    NextTrack,
    Play,
    Pause,
    Stop,
    Power,
    PowerOn,
    PowerOff,
    OpenDiscKeypad,
    OpenCddbScreen,
    KeypadDigit0,
    KeypadDigit1,
    KeypadDigit2,
    KeypadDigit3,
    KeypadDigit4,
    KeypadDigit5,
    KeypadDigit6,
    KeypadDigit7,
    KeypadDigit8,
    KeypadDigit9,
    KeypadBackspace,
    KeypadClear,
    KeypadGo,
    KeypadCancel,
    StartCddbIndex
};

using UiActionCb = void(*)(UiAction action, void* user);

enum class UiTransportState : uint8_t {
    Unknown = 0,
    Stopped,
    Playing,
    Paused
};

struct UiNowPlayingSnapshot {
    uint16_t disc;
    uint16_t track;
    uint32_t elapsed_sec;
    const char* title;
    const char* artist;
    const char* album;
    UiTransportState transport;
};
