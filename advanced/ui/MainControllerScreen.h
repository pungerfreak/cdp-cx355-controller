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
    lv_obj_t* mainScreen = nullptr;
    lv_obj_t* discTrackRow_ = nullptr;
    lv_obj_t* topRowPower_ = nullptr;
    lv_obj_t* topRowDisc_ = nullptr;
    lv_obj_t* topRowDiscLabel_ = nullptr;
    lv_obj_t* topRowTrack_ = nullptr;
    lv_obj_t* topRowTrackLabel_ = nullptr;
    lv_obj_t* midLine1_ = nullptr;
    lv_obj_t* midLine2_ = nullptr;
    lv_obj_t* midLine3_ = nullptr;
    lv_obj_t* prevBtn_ = nullptr;
    lv_obj_t* nextBtn_ = nullptr;
    lv_obj_t* playBtn_ = nullptr;
    lv_obj_t* transportLabel_ = nullptr;
    lv_obj_t* transportImg_ = nullptr;
    ActionBinding bindings_[6]{};

    UiActionCb cb_ = nullptr;
    void* user_ = nullptr;

    UiNowPlayingSnapshot last_{};
    bool hasLast_ = false;
    bool transportLongPressHandled_ = false;
    bool powerLongPressHandled_ = false;

    static void onButtonEvent_(lv_event_t* e);
    void emitAction_(UiAction action);
    void handleTransportButtonEvent_(lv_event_code_t code);
    void handlePowerButtonEvent_(lv_event_code_t code);
    UiAction transportPressAction_() const;
};
