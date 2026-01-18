#pragma once

#include <lvgl.h>
#include <stddef.h>

enum class UiAction : uint8_t {
    PrevTrack = 0,
    NextTrack,
    Play,
    Pause,
    Stop,
    Power
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

class UiApp {
  public:
    UiApp() = default;

    // Build all LVGL objects. Call once after LVGL init + display init.
    void init();

    // Render/update widgets from the snapshot. Must be safe to call repeatedly.
    void render(const UiNowPlayingSnapshot& s);

    void setActionCallback(UiActionCb cb, void* user);

  private:
    struct ActionBinding {
        UiAction action;
        UiApp* app;
    };

    // Store LVGL object pointers for labels/buttons needed in milestone B
    lv_obj_t* labelHeader_ = nullptr;   // e.g., "Disc 12 | Track 3"
    lv_obj_t* labelTime_   = nullptr;   // e.g., "01:23"
    lv_obj_t* labelMeta_   = nullptr;   // e.g., "Artist - Album\nTitle"
    lv_obj_t* labelState_  = nullptr;   // e.g., "PLAYING"/"PAUSED"

    // Keep minimal UI-local state if needed (optional)
    UiNowPlayingSnapshot last_{};
    bool hasLast_ = false;

    UiActionCb cb_ = nullptr;
    void* user_ = nullptr;
    static constexpr size_t kActionCount_ = 6;
    ActionBinding bindings_[kActionCount_]{};

    // Helpers
    static void onButtonEvent_(lv_event_t* e);
    void emitAction_(UiAction action);
    static void formatTime_(char* out, size_t outLen, uint32_t elapsed_sec);
};
