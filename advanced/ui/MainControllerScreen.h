#pragma once

#include "UiTypes.h"

class MainControllerScreen {
  public:
    void init(lv_obj_t* parent, UiActionCb cb, void* user);
    void render(const UiNowPlayingSnapshot& s);
    void show();
    void hide();
    void setActionCallback(UiActionCb cb, void* user);

  private:
    struct ActionBinding {
        UiAction action;
        MainControllerScreen* screen;
    };

    lv_obj_t* root_ = nullptr;
    lv_obj_t* labelHeader_ = nullptr;
    lv_obj_t* labelTime_ = nullptr;
    lv_obj_t* labelMeta_ = nullptr;
    lv_obj_t* labelState_ = nullptr;
    lv_obj_t* transportLabel_ = nullptr;
    ActionBinding bindings_[5]{};

    UiActionCb cb_ = nullptr;
    void* user_ = nullptr;

    UiNowPlayingSnapshot last_{};
    bool hasLast_ = false;
    bool transportLongPressHandled_ = false;

    static void onButtonEvent_(lv_event_t* e);
    void emitAction_(UiAction action);
    void handleTransportButtonEvent_(lv_event_code_t code);
    UiAction transportPressAction_() const;
    static bool str_equal_(const char* a, const char* b);
    static void format_time_(char* out, size_t outLen, uint32_t elapsed_sec);
};
