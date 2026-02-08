#pragma once

#include <Arduino.h>
#include "UiTypes.h"
#include "ScrollingLabel.h"

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
    ScrollingLabel midLine1_;
    ScrollingLabel midLine2_;
    ScrollingLabel midLine3_;
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
    bool discLongPressHandled_ = false;
    uint32_t lastDiscTapMs_ = 0;

    static void onButtonEvent_(lv_event_t* e);
    void emitAction_(UiAction action);
    void handleTransportButtonEvent_(lv_event_code_t code);
    void handlePowerButtonEvent_(lv_event_code_t code);
    void handleDiscButtonEvent_(lv_event_code_t code);
    UiAction transportPressAction_() const;
};
