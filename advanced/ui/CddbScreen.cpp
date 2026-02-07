#include "CddbScreen.h"

extern const lv_font_t open_sans_18;

void CddbScreen::init(lv_obj_t* parent, UiActionCb cb, void* user)
{
    cb_ = cb;
    user_ = user;

    bindings_[0] = {UiAction::StartCddbIndex, this};
    bindings_[1] = {UiAction::KeypadCancel, this};

    root_ = lv_obj_create(parent);
    setup_root_obj(root_);
    const lv_color_t color_bg = lv_color_hex(0x000000);
    const lv_color_t color_accent = lv_color_hex(0xDDCC0B);
    const lv_color_t color_text = lv_color_hex(0xFFFFFF);
    lv_obj_set_style_bg_color(root_, color_bg, 0);
    lv_obj_set_style_bg_opa(root_, LV_OPA_COVER, 0);
    lv_obj_add_flag(root_, LV_OBJ_FLAG_HIDDEN);

    lv_obj_t* title = lv_label_create(root_);
    lv_label_set_text(title, "CDDB Lookup");
    lv_obj_set_style_text_color(title, color_text, 0);
    lv_obj_set_style_text_font(title, &open_sans_18, 0);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 26);

    lv_obj_t* subtitle = lv_label_create(root_);
    lv_label_set_text(subtitle, "Build local index from discs");
    lv_obj_set_style_text_color(subtitle, color_text, 0);
    lv_obj_set_style_text_font(subtitle, &open_sans_18, 0);
    lv_obj_align(subtitle, LV_ALIGN_TOP_MID, 0, 52);

    auto wire_button_events = [&](lv_obj_t* obj, ActionBinding* binding) {
        lv_obj_add_event_cb(obj, CddbScreen::onButtonEvent_, LV_EVENT_RELEASED, binding);
    };

    lv_obj_t* startBtn = lv_btn_create(root_);
    lv_obj_set_size(startBtn, 140, 38);
    lv_obj_align(startBtn, LV_ALIGN_CENTER, 0, -6);
    lv_obj_set_style_radius(startBtn, 10, 0);
    lv_obj_set_style_bg_color(startBtn, color_accent, 0);
    lv_obj_set_style_bg_opa(startBtn, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(startBtn, 0, 0);
    wire_button_events(startBtn, &bindings_[0]);
    lv_obj_t* startLabel = lv_label_create(startBtn);
    lv_label_set_text(startLabel, "Start Index");
    lv_obj_set_style_text_color(startLabel, color_bg, 0);
    lv_obj_set_style_text_font(startLabel, &open_sans_18, 0);
    lv_obj_center(startLabel);

    lv_obj_t* backBtn = lv_btn_create(root_);
    lv_obj_set_size(backBtn, 90, 32);
    lv_obj_align(backBtn, LV_ALIGN_CENTER, 0, 46);
    lv_obj_set_style_radius(backBtn, 10, 0);
    lv_obj_set_style_bg_color(backBtn, color_bg, 0);
    lv_obj_set_style_bg_opa(backBtn, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(backBtn, 1, 0);
    lv_obj_set_style_border_color(backBtn, color_accent, 0);
    lv_obj_set_style_border_opa(backBtn, LV_OPA_COVER, 0);
    wire_button_events(backBtn, &bindings_[1]);
    lv_obj_t* backLabel = lv_label_create(backBtn);
    lv_label_set_text(backLabel, "Back");
    lv_obj_set_style_text_color(backLabel, color_accent, 0);
    lv_obj_set_style_text_font(backLabel, &open_sans_18, 0);
    lv_obj_center(backLabel);
}

void CddbScreen::show()
{
    if (root_ != nullptr) {
        lv_obj_clear_flag(root_, LV_OBJ_FLAG_HIDDEN);
    }
}

void CddbScreen::hide()
{
    if (root_ != nullptr) {
        lv_obj_add_flag(root_, LV_OBJ_FLAG_HIDDEN);
    }
}

void CddbScreen::setActionCallback(UiActionCb cb, void* user)
{
    cb_ = cb;
    user_ = user;
}

void CddbScreen::onButtonEvent_(lv_event_t* e)
{
    if (lv_event_get_code(e) != LV_EVENT_RELEASED) {
        return;
    }

    ActionBinding* binding = static_cast<ActionBinding*>(lv_event_get_user_data(e));
    if (binding == nullptr || binding->screen == nullptr) {
        return;
    }
    binding->screen->emitAction_(binding->action);
}

void CddbScreen::emitAction_(UiAction action)
{
    if (cb_ != nullptr) {
        cb_(action, user_);
    }
}
