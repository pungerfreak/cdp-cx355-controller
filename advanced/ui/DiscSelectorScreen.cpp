#include "DiscSelectorScreen.h"

#include <stdio.h>

#include "../images/backspace.c"
#include "../fonts/open_sans_22.c"

void DiscSelectorScreen::init(lv_obj_t* parent, UiActionCb cb, void* user)
{
    cb_ = cb;
    user_ = user;

    root_ = lv_obj_create(parent);
    setup_root_obj(root_);
    const lv_color_t color_bg = lv_color_hex(0x000000);
    const lv_color_t color_accent = lv_color_hex(0xDDCC0B);
    lv_obj_set_style_bg_color(root_, color_bg, 0);
    lv_obj_set_style_bg_opa(root_, LV_OPA_COVER, 0);
    lv_obj_add_flag(root_, LV_OBJ_FLAG_HIDDEN);

    const lv_coord_t screen_w = LV_HOR_RES;
    const lv_coord_t screen_h = LV_VER_RES;
    const lv_coord_t key_diam = 36;
    const lv_coord_t key_gap_x = 16;
    const lv_coord_t key_gap_y = 12;
    const lv_coord_t key_rows = 3;
    const lv_coord_t key_grid_h = key_rows * key_diam + (key_rows - 1) * key_gap_y;
    const lv_coord_t available_h = screen_h - key_grid_h;
    lv_coord_t top_bar_h = available_h / 2;
    if (top_bar_h > 4) {
        top_bar_h -= 4;
    } else {
        top_bar_h = 0;
    }
    const lv_coord_t bottom_bar_h = available_h - top_bar_h;

    topBar_ = lv_obj_create(root_);
    lv_obj_set_size(topBar_, screen_w, top_bar_h);
    lv_obj_set_pos(topBar_, 0, 0);
    lv_obj_clear_flag(topBar_, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_layout(topBar_, LV_LAYOUT_NONE);
    lv_obj_set_style_bg_opa(topBar_, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_opa(topBar_, LV_OPA_TRANSP, 0);
    lv_obj_set_style_pad_all(topBar_, 0, 0);

    lv_obj_t* bottomBar = lv_obj_create(root_);
    lv_obj_set_size(bottomBar, screen_w, bottom_bar_h);
    lv_obj_set_pos(bottomBar, 0, screen_h - bottom_bar_h);
    lv_obj_clear_flag(bottomBar, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_layout(bottomBar, LV_LAYOUT_NONE);
    lv_obj_set_style_bg_opa(bottomBar, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_opa(bottomBar, LV_OPA_TRANSP, 0);

    auto wire_button_events = [&](lv_obj_t* obj, ActionBinding* binding) {
        lv_obj_add_event_cb(obj, DiscSelectorScreen::onButtonEvent_, LV_EVENT_RELEASED, binding);
    };

    const lv_coord_t entry_w = screen_w;
    const lv_font_t* entry_font = &open_sans_22;
    lv_coord_t font_h = lv_font_get_line_height(entry_font);
    const lv_coord_t entry_h = font_h + 10;
    keypadEntry_ = lv_label_create(topBar_);
    lv_obj_set_size(keypadEntry_, entry_w, entry_h);
    lv_obj_center(keypadEntry_);
    lv_obj_set_style_text_align(keypadEntry_, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_text_color(keypadEntry_, color_accent, 0);
    lv_obj_set_style_text_opa(keypadEntry_, LV_OPA_COVER, 0);
    lv_obj_set_style_pad_all(keypadEntry_, 0, 0);
    lv_obj_set_style_pad_top(keypadEntry_, (entry_h - font_h) / 2, 0);
    lv_obj_set_style_pad_bottom(keypadEntry_, (entry_h - font_h) / 2, 0);
    lv_obj_set_style_text_font(keypadEntry_, entry_font, 0);
    lv_obj_set_style_transform_pivot_x(keypadEntry_, entry_w / 2, 0);
    lv_obj_set_style_transform_pivot_y(keypadEntry_, entry_h / 2, 0);
    entryZoom_ = 256;  // no scaling with custom font
    lv_obj_set_style_transform_zoom(keypadEntry_, entryZoom_, 0);
    lv_obj_set_style_text_letter_space(keypadEntry_, 1, 0);
    lv_label_set_long_mode(keypadEntry_, LV_LABEL_LONG_CLIP);
    lv_obj_add_flag(keypadEntry_, LV_OBJ_FLAG_CLICKABLE);
    wire_button_events(keypadEntry_, &backspaceBinding_);

    backspaceImg_ = lv_image_create(topBar_);
    lv_image_set_src(backspaceImg_, &backspace);
    lv_obj_set_style_img_recolor(backspaceImg_, color_accent, 0);
    lv_obj_set_style_img_recolor_opa(backspaceImg_, LV_OPA_COVER, 0);
    lv_obj_set_style_img_opa(backspaceImg_, LV_OPA_COVER, 0);
    wire_button_events(backspaceImg_, &backspaceBinding_);
    lv_label_set_text(keypadEntry_, "");
    updateBackspacePosition_();

    const lv_coord_t key_grid_extra_space = screen_h - top_bar_h - bottom_bar_h - key_grid_h;
    lv_coord_t key_grid_y = top_bar_h + (key_grid_extra_space > 0 ? key_grid_extra_space / 2 : 0);

    bindings_[0] = {UiAction::KeypadDigit1, this};
    bindings_[1] = {UiAction::KeypadDigit2, this};
    bindings_[2] = {UiAction::KeypadDigit3, this};
    bindings_[3] = {UiAction::KeypadDigit4, this};
    bindings_[4] = {UiAction::KeypadDigit5, this};
    bindings_[5] = {UiAction::KeypadDigit6, this};
    bindings_[6] = {UiAction::KeypadDigit7, this};
    bindings_[7] = {UiAction::KeypadDigit8, this};
    bindings_[8] = {UiAction::KeypadDigit9, this};
    bindings_[9] = {UiAction::KeypadDigit0, this};
    bindings_[10] = {UiAction::KeypadGo, this};

    auto add_key = [&](const char* text, ActionBinding* binding, uint8_t col, uint8_t row, uint8_t cols_in_row) {
        const lv_coord_t row_w = cols_in_row * key_diam + (cols_in_row - 1) * key_gap_x;
        const lv_coord_t row_x = (screen_w - row_w) / 2;
        lv_obj_t* btn = lv_btn_create(root_);
        lv_obj_set_size(btn, key_diam, key_diam);
        lv_obj_set_pos(btn,
                       row_x + col * (key_diam + key_gap_x),
                       key_grid_y + row * (key_diam + key_gap_y));
        lv_obj_set_style_radius(btn, key_diam / 2, 0);
        lv_obj_set_style_pad_all(btn, 0, 0);
        lv_obj_set_style_bg_color(btn, color_accent, 0);
        lv_obj_set_style_bg_opa(btn, LV_OPA_COVER, 0);
        lv_obj_set_style_border_width(btn, 0, 0);
        wire_button_events(btn, binding);
        lv_obj_t* label = lv_label_create(btn);
        lv_label_set_text(label, text);
        lv_obj_set_style_text_color(label, color_bg, 0);
        lv_obj_set_style_text_font(label, &open_sans_22, 0);
        lv_obj_center(label);
    };

    add_key("1", &bindings_[0], 0, 0, 3);
    add_key("2", &bindings_[1], 1, 0, 3);
    add_key("3", &bindings_[2], 2, 0, 3);

    add_key("4", &bindings_[3], 0, 1, 4);
    add_key("5", &bindings_[4], 1, 1, 4);
    add_key("6", &bindings_[5], 2, 1, 4);
    add_key("7", &bindings_[6], 3, 1, 4);

    add_key("8", &bindings_[7], 0, 2, 3);
    add_key("9", &bindings_[8], 1, 2, 3);
    add_key("0", &bindings_[9], 2, 2, 3);

    lv_obj_t* okBtn = lv_btn_create(bottomBar);
    lv_obj_set_size(okBtn, 60, key_diam);
    lv_obj_set_style_radius(okBtn, key_diam / 2, 0);
    lv_obj_set_style_bg_color(okBtn, color_bg, 0);
    lv_obj_set_style_bg_opa(okBtn, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(okBtn, 1, 0);
    lv_obj_set_style_border_color(okBtn, color_accent, 0);
    lv_obj_set_style_border_opa(okBtn, LV_OPA_COVER, 0);
    lv_obj_center(okBtn);
    wire_button_events(okBtn, &bindings_[10]);
    lv_obj_t* okLabel = lv_label_create(okBtn);
    lv_label_set_text(okLabel, "OK");
    lv_obj_set_style_text_color(okLabel, color_accent, 0);
    lv_obj_set_style_text_font(okLabel, &open_sans_22, 0);
    lv_obj_center(okLabel);
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
    if (cb_ == nullptr) {
        return;
    }

    if (action == UiAction::KeypadGo) {
        cb_(UiAction::KeypadClear, user_);
        for (uint8_t i = 0; i < keypadLen_; i++) {
            UiAction digitAction = UiAction::KeypadDigit0;
            switch (keypadBuf_[i]) {
                case '1': digitAction = UiAction::KeypadDigit1; break;
                case '2': digitAction = UiAction::KeypadDigit2; break;
                case '3': digitAction = UiAction::KeypadDigit3; break;
                case '4': digitAction = UiAction::KeypadDigit4; break;
                case '5': digitAction = UiAction::KeypadDigit5; break;
                case '6': digitAction = UiAction::KeypadDigit6; break;
                case '7': digitAction = UiAction::KeypadDigit7; break;
                case '8': digitAction = UiAction::KeypadDigit8; break;
                case '9': digitAction = UiAction::KeypadDigit9; break;
                case '0':
                default:
                    digitAction = UiAction::KeypadDigit0;
                    break;
            }
            cb_(digitAction, user_);
        }
        cb_(UiAction::KeypadGo, user_);
        return;
    }
    cb_(action, user_);
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
        lv_label_set_text(keypadEntry_, "");
    } else {
        lv_label_set_text(keypadEntry_, keypadBuf_);
    }
    updateBackspacePosition_();
}

void DiscSelectorScreen::updateBackspacePosition_()
{
    if (backspaceImg_ == nullptr || keypadEntry_ == nullptr || topBar_ == nullptr) {
        return;
    }

    const char* txt = lv_label_get_text(keypadEntry_);
    if (txt == nullptr) {
        txt = "";
    }

    lv_point_t txt_size{};
    lv_txt_get_size(&txt_size, txt, LV_FONT_DEFAULT, 1, 0, LV_COORD_MAX, LV_TEXT_FLAG_NONE);

    lv_coord_t text_w = (txt_size.x * entryZoom_ + 128) / 256;

    lv_coord_t entry_x = lv_obj_get_x(keypadEntry_);
    lv_coord_t entry_w = lv_obj_get_width(keypadEntry_);
    lv_coord_t center_x = entry_x + entry_w / 2;

    const lv_coord_t icon_w = backspace.header.w;
    const lv_coord_t icon_h = backspace.header.h;
    const lv_coord_t margin = 19;

    lv_coord_t x = center_x + (text_w / 2) + margin - icon_w / 2;
    lv_coord_t y = (lv_obj_get_height(topBar_) - icon_h) / 2;

    lv_coord_t max_x = lv_obj_get_width(topBar_) - icon_w - 4;
    if (x > max_x) {
        x = max_x;
    }
    if (x < 4) {
        x = 4;
    }
    lv_obj_set_pos(backspaceImg_, x, y);
    if (text_w > 0) {
        lv_obj_clear_flag(backspaceImg_, LV_OBJ_FLAG_HIDDEN);
    } else {
        lv_obj_add_flag(backspaceImg_, LV_OBJ_FLAG_HIDDEN);
    }
}
