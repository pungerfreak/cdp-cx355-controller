#pragma once

#include "UiTypes.h"

class CddbScreen {
  public:
    void init(lv_obj_t* parent, UiActionCb cb, void* user);
    void show();
    void hide();
    void setActionCallback(UiActionCb cb, void* user);

  private:
    struct ActionBinding {
        UiAction action;
        CddbScreen* screen;
    };

    lv_obj_t* root_ = nullptr;
    ActionBinding bindings_[2]{};

    UiActionCb cb_ = nullptr;
    void* user_ = nullptr;

    static void onButtonEvent_(lv_event_t* e);
    void emitAction_(UiAction action);
};
