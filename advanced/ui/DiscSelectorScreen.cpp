#include "DiscSelectorScreen.h"

#include <stdio.h>

void DiscSelectorScreen::init(lv_obj_t* parent, UiActionCb cb, void* user)
{
    cb_ = cb;
    user_ = user;

    root_ = lv_obj_create(parent);
    setup_root_obj(root_);
    lv_obj_add_flag(root_, LV_OBJ_FLAG_HIDDEN);

    const lv_coord_t screen_w = LV_HOR_RES;
    const lv_coord_t inset = 20;
    const lv_coord_t label_w = screen_w - (inset * 2);

    lv_obj_t* keypadTitle = lv_label_create(root_);
    lv_obj_set_size(keypadTitle, label_w, 18);
    lv_obj_set_pos(keypadTitle, inset, 20);
    lv_obj_set_style_text_align(keypadTitle, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text(keypadTitle, "Select Disc");

    keypadEntry_ = lv_label_create(root_);
    lv_obj_set_size(keypadEntry_, 140, 28);
    lv_obj_set_pos(keypadEntry_, (screen_w - 140) / 2, 42);
    lv_obj_set_style_text_align(keypadEntry_, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_long_mode(keypadEntry_, LV_LABEL_LONG_CLIP);
    lv_label_set_text(keypadEntry_, "___");

    keypadError_ = lv_label_create(root_);
    lv_obj_set_size(keypadError_, label_w, 16);
    lv_obj_set_pos(keypadError_, inset, 72);
    lv_obj_set_style_text_align(keypadError_, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text(keypadError_, "Enter 1-300");
    lv_obj_add_flag(keypadError_, LV_OBJ_FLAG_HIDDEN);

    const lv_coord_t key_w = 36;
    const lv_coord_t key_h = 36;
    const lv_coord_t key_gap = 4;
    const lv_coord_t key_cols = 5;
    const lv_coord_t key_grid_w = key_cols * key_w + (key_cols - 1) * key_gap;
    const lv_coord_t key_grid_x = (screen_w - key_grid_w) / 2;
    const lv_coord_t key_grid_y = 92;

    bindings_[0] = {UiAction::KeypadDigit1, this};
    bindings_[1] = {UiAction::KeypadDigit2, this};
    bindings_[2] = {UiAction::KeypadDigit3, this};
    bindings_[3] = {UiAction::KeypadBackspace, this};
    bindings_[4] = {UiAction::KeypadGo, this};
    bindings_[5] = {UiAction::KeypadDigit4, this};
    bindings_[6] = {UiAction::KeypadDigit5, this};
    bindings_[7] = {UiAction::KeypadDigit6, this};
    bindings_[8] = {UiAction::KeypadClear, this};
    bindings_[9] = {UiAction::KeypadCancel, this};
    bindings_[10] = {UiAction::KeypadDigit7, this};
    bindings_[11] = {UiAction::KeypadDigit8, this};
    bindings_[12] = {UiAction::KeypadDigit9, this};
    bindings_[13] = {UiAction::KeypadDigit0, this};

    auto wire_button_events = [&](lv_obj_t* obj, ActionBinding* binding) {
        lv_obj_add_event_cb(obj, DiscSelectorScreen::onButtonEvent_, LV_EVENT_RELEASED, binding);
    };

    auto add_key = [&](const char* text, ActionBinding* binding, lv_coord_t col, lv_coord_t row) {
        lv_obj_t* btn = lv_btn_create(root_);
        lv_obj_set_size(btn, key_w, key_h);
        lv_obj_set_pos(btn,
                       key_grid_x + col * (key_w + key_gap),
                       key_grid_y + row * (key_h + key_gap));
        wire_button_events(btn, binding);
        lv_obj_t* label = lv_label_create(btn);
        lv_label_set_text(label, text);
        lv_obj_center(label);
    };

    add_key("1", &bindings_[0], 0, 0);
    add_key("2", &bindings_[1], 1, 0);
    add_key("3", &bindings_[2], 2, 0);
    add_key("Bksp", &bindings_[3], 3, 0);
    add_key("Go", &bindings_[4], 4, 0);

    add_key("4", &bindings_[5], 0, 1);
    add_key("5", &bindings_[6], 1, 1);
    add_key("6", &bindings_[7], 2, 1);
    add_key("Clear", &bindings_[8], 3, 1);
    add_key("Cncl", &bindings_[9], 4, 1);

    add_key("7", &bindings_[10], 0, 2);
    add_key("8", &bindings_[11], 1, 2);
    add_key("9", &bindings_[12], 2, 2);
    add_key("0", &bindings_[13], 3, 2);
}

void DiscSelectorScreen::show()
{
    reset();
    if (root_ != nullptr) {
        lv_obj_clear_flag(root_, LV_OBJ_FLAG_HIDDEN);
    }
}

void DiscSelectorScreen::hide()
{
    if (root_ != nullptr) {
        lv_obj_add_flag(root_, LV_OBJ_FLAG_HIDDEN);
    }
}

void DiscSelectorScreen::reset()
{
    keypadLen_ = 0;
    keypadBuf_[0] = '\0';
    setError(false);
    updateKeypadDisplay_();
}

void DiscSelectorScreen::setError(bool on)
{
    if (keypadError_ == nullptr) {
        return;
    }
    if (on) {
        lv_obj_clear_flag(keypadError_, LV_OBJ_FLAG_HIDDEN);
    } else {
        lv_obj_add_flag(keypadError_, LV_OBJ_FLAG_HIDDEN);
    }
}

void DiscSelectorScreen::setActionCallback(UiActionCb cb, void* user)
{
    cb_ = cb;
    user_ = user;
}

void DiscSelectorScreen::onButtonEvent_(lv_event_t* e)
{
    if (lv_event_get_code(e) != LV_EVENT_RELEASED) {
        return;
    }

    ActionBinding* binding = static_cast<ActionBinding*>(lv_event_get_user_data(e));
    if (binding == nullptr || binding->screen == nullptr) {
        return;
    }

    UiAction action = binding->action;
    binding->screen->handleKeypadInput_(action);
    binding->screen->emitAction_(action);
}

void DiscSelectorScreen::emitAction_(UiAction action)
{
    if (cb_ != nullptr) {
        cb_(action, user_);
    }
}

void DiscSelectorScreen::handleKeypadInput_(UiAction action)
{
    bool updated = false;
    switch (action) {
        case UiAction::KeypadBackspace:
            if (keypadLen_ > 0) {
                keypadLen_--;
                keypadBuf_[keypadLen_] = '\0';
                updated = true;
            }
            break;
        case UiAction::KeypadClear:
            if (keypadLen_ > 0) {
                keypadLen_ = 0;
                keypadBuf_[0] = '\0';
                updated = true;
            }
            break;
        case UiAction::KeypadDigit0:
        case UiAction::KeypadDigit1:
        case UiAction::KeypadDigit2:
        case UiAction::KeypadDigit3:
        case UiAction::KeypadDigit4:
        case UiAction::KeypadDigit5:
        case UiAction::KeypadDigit6:
        case UiAction::KeypadDigit7:
        case UiAction::KeypadDigit8:
        case UiAction::KeypadDigit9:
            if (keypadLen_ < 3) {
                char digit = '0';
                switch (action) {
                    case UiAction::KeypadDigit1:
                        digit = '1';
                        break;
                    case UiAction::KeypadDigit2:
                        digit = '2';
                        break;
                    case UiAction::KeypadDigit3:
                        digit = '3';
                        break;
                    case UiAction::KeypadDigit4:
                        digit = '4';
                        break;
                    case UiAction::KeypadDigit5:
                        digit = '5';
                        break;
                    case UiAction::KeypadDigit6:
                        digit = '6';
                        break;
                    case UiAction::KeypadDigit7:
                        digit = '7';
                        break;
                    case UiAction::KeypadDigit8:
                        digit = '8';
                        break;
                    case UiAction::KeypadDigit9:
                        digit = '9';
                        break;
                    case UiAction::KeypadDigit0:
                    default:
                        digit = '0';
                        break;
                }
                keypadBuf_[keypadLen_] = digit;
                keypadLen_++;
                keypadBuf_[keypadLen_] = '\0';
                updated = true;
            }
            break;
        default:
            break;
    }

    if (updated) {
        setError(false);
        updateKeypadDisplay_();
    }
}

void DiscSelectorScreen::updateKeypadDisplay_()
{
    if (keypadEntry_ == nullptr) {
        return;
    }
    if (keypadLen_ == 0) {
        lv_label_set_text(keypadEntry_, "___");
    } else {
        lv_label_set_text(keypadEntry_, keypadBuf_);
    }
}
