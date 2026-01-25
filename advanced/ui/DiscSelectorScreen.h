#pragma once

#include "UiTypes.h"

class DiscSelectorScreen {
  public:
    void init(lv_obj_t* parent, UiActionCb cb, void* user);
    void show();
    void hide();
    void reset();
    void setError(bool on);
    void setActionCallback(UiActionCb cb, void* user);

  private:
    struct ActionBinding {
        UiAction action;
        DiscSelectorScreen* screen;
    };

    lv_obj_t* root_ = nullptr;
    lv_obj_t* keypadEntry_ = nullptr;
    lv_obj_t* keypadError_ = nullptr;
    ActionBinding bindings_[14]{};

    UiActionCb cb_ = nullptr;
    void* user_ = nullptr;

    char keypadBuf_[4]{};
    uint8_t keypadLen_ = 0;

    static void onButtonEvent_(lv_event_t* e);
    void emitAction_(UiAction action);
    void handleKeypadInput_(UiAction action);
    void updateKeypadDisplay_();
};
