#include "UiApp.h"

#include <stdio.h>
#include <string.h>

namespace {
bool str_equal_ui_app(const char* a, const char* b)
{
    if (a == b) {
        return true;
    }
    if (a == nullptr || b == nullptr) {
        return false;
    }
    return strcmp(a, b) == 0;
}
}  // namespace

void UiApp::init()
{
    lv_obj_t* screen = lv_screen_active();

    lv_obj_t* root = lv_obj_create(screen);
    lv_obj_set_size(root, LV_HOR_RES, LV_VER_RES);
    lv_obj_set_pos(root, 0, 0);
    lv_obj_set_layout(root, LV_LAYOUT_NONE);
    lv_obj_set_style_pad_all(root, 0, 0);
    lv_obj_set_style_pad_row(root, 0, 0);
    lv_obj_set_style_pad_column(root, 0, 0);
    lv_obj_clear_flag(root, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_opa(root, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_opa(root, LV_OPA_TRANSP, 0);

    const lv_coord_t screen_w = LV_HOR_RES;
    const lv_coord_t header_w = 140;
    const lv_coord_t time_w = 170;
    const lv_coord_t meta_w = 160;
    const lv_coord_t state_w = 200;
    const lv_coord_t controls_w = 200;

    labelHeader_ = lv_label_create(root);
    lv_obj_set_size(labelHeader_, header_w, 20);
    lv_obj_set_pos(labelHeader_, (screen_w - header_w) / 2, 24);
    lv_obj_set_style_text_align(labelHeader_, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text(labelHeader_, "Disc - | Track -");

    labelTime_ = lv_label_create(root);
    lv_obj_set_size(labelTime_, time_w, 18);
    lv_obj_set_pos(labelTime_, (screen_w - time_w) / 2, 46);
    lv_obj_set_style_text_align(labelTime_, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text(labelTime_, "00:00");

    labelMeta_ = lv_label_create(root);
    lv_obj_set_size(labelMeta_, meta_w, 56);
    lv_obj_set_pos(labelMeta_, 20, 66);
    lv_obj_set_style_text_align(labelMeta_, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_long_mode(labelMeta_, LV_LABEL_LONG_DOT);
    lv_label_set_text(labelMeta_, "- -\n-");

    labelState_ = lv_label_create(root);
    lv_obj_set_size(labelState_, state_w, 18);
    lv_obj_set_pos(labelState_, (screen_w - state_w) / 2, 124);
    lv_obj_set_style_text_align(labelState_, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text(labelState_, "PAUSED");

    bindings_[0] = {UiAction::PrevTrack, this};
    bindings_[1] = {UiAction::Play, this};
    bindings_[2] = {UiAction::Pause, this};
    bindings_[3] = {UiAction::Stop, this};
    bindings_[4] = {UiAction::NextTrack, this};
    bindings_[5] = {UiAction::Power, this};

    lv_obj_t* controls = lv_obj_create(root);
    lv_obj_set_size(controls, controls_w, 40);
    lv_obj_set_pos(controls, (screen_w - controls_w) / 2, 146);
    lv_obj_set_layout(controls, LV_LAYOUT_NONE);
    lv_obj_set_style_bg_opa(controls, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_opa(controls, LV_OPA_TRANSP, 0);
    lv_obj_set_style_pad_all(controls, 0, 0);
    lv_obj_clear_flag(controls, LV_OBJ_FLAG_SCROLLABLE);

    const lv_coord_t btn_w = 40;
    const lv_coord_t btn_h = 40;
    const lv_coord_t btn_gap = 0;
    const lv_coord_t btn_y = 0;

    auto add_button = [&](const char* text, ActionBinding* binding, int index) {
        lv_obj_t* btn = lv_btn_create(controls);
        lv_obj_set_size(btn, btn_w, btn_h);
        lv_obj_set_pos(btn, index * (btn_w + btn_gap), btn_y);
        lv_obj_add_event_cb(btn, UiApp::onButtonEvent_, LV_EVENT_CLICKED, binding);
        lv_obj_t* label = lv_label_create(btn);
        lv_label_set_text(label, text);
        lv_obj_center(label);
    };

    add_button("Prev", &bindings_[0], 0);
    add_button("Play", &bindings_[1], 1);
    add_button("Pause", &bindings_[2], 2);
    add_button("Stop", &bindings_[3], 3);
    add_button("Next", &bindings_[4], 4);

    lv_obj_t* powerBtn = lv_btn_create(root);
    lv_obj_set_size(powerBtn, 42, 32);
    lv_obj_set_pos(powerBtn, 188, 80);
    lv_obj_add_event_cb(powerBtn, UiApp::onButtonEvent_, LV_EVENT_CLICKED, &bindings_[5]);
    lv_obj_t* powerLabel = lv_label_create(powerBtn);
    lv_label_set_text(powerLabel, "Power");
    lv_obj_center(powerLabel);
}

void UiApp::render(const UiNowPlayingSnapshot& s)
{
    if (labelHeader_ == nullptr || labelTime_ == nullptr || labelMeta_ == nullptr || labelState_ == nullptr) {
        return;
    }

    if (!hasLast_ || s.disc != last_.disc || s.track != last_.track) {
        char header[40];
        char discBuf[8];
        char trackBuf[8];
        if (s.disc == 0) {
            snprintf(discBuf, sizeof(discBuf), "-");
        } else {
            snprintf(discBuf, sizeof(discBuf), "%u", (unsigned)s.disc);
        }
        if (s.track == 0) {
            snprintf(trackBuf, sizeof(trackBuf), "-");
        } else {
            snprintf(trackBuf, sizeof(trackBuf), "%u", (unsigned)s.track);
        }
        snprintf(header, sizeof(header), "Disc %s | Track %s", discBuf, trackBuf);
        lv_label_set_text(labelHeader_, header);
    }

    if (!hasLast_ || s.elapsed_sec != last_.elapsed_sec) {
        char timeBuf[8];
        formatTime_(timeBuf, sizeof(timeBuf), s.elapsed_sec);
        lv_label_set_text(labelTime_, timeBuf);
    }

    if (!hasLast_ || !str_equal_ui_app(s.artist, last_.artist) || !str_equal_ui_app(s.album, last_.album)
        || !str_equal_ui_app(s.title, last_.title)) {
        const char* artist = s.artist != nullptr ? s.artist : "-";
        const char* album = s.album != nullptr ? s.album : "-";
        const char* title = s.title != nullptr ? s.title : "-";
        char meta[128];
        snprintf(meta, sizeof(meta), "%s - %s\n%s", artist, album, title);
        lv_label_set_text(labelMeta_, meta);
    }

    if (!hasLast_ || s.transport != last_.transport) {
        const char* stateText = "UNKNOWN";
        switch (s.transport) {
            case UiTransportState::Playing:
                stateText = "PLAYING";
                break;
            case UiTransportState::Paused:
                stateText = "PAUSED";
                break;
            case UiTransportState::Stopped:
                stateText = "STOPPED";
                break;
            case UiTransportState::Unknown:
            default:
                stateText = "UNKNOWN";
                break;
        }
        lv_label_set_text(labelState_, stateText);
    }

    last_ = s;
    hasLast_ = true;
}

void UiApp::setActionCallback(UiActionCb cb, void* user)
{
    cb_ = cb;
    user_ = user;
}

void UiApp::onButtonEvent_(lv_event_t* e)
{
    if (lv_event_get_code(e) != LV_EVENT_CLICKED) {
        return;
    }

    ActionBinding* binding = static_cast<ActionBinding*>(lv_event_get_user_data(e));
    if (binding == nullptr || binding->app == nullptr) {
        return;
    }
    binding->app->emitAction_(binding->action);
}

void UiApp::emitAction_(UiAction action)
{
    if (cb_ != nullptr) {
        cb_(action, user_);
    }
}

void UiApp::formatTime_(char* out, size_t outLen, uint32_t elapsed_sec)
{
    uint32_t minutes = elapsed_sec / 60;
    uint32_t seconds = elapsed_sec % 60;
    snprintf(out, outLen, "%02lu:%02lu", (unsigned long)minutes, (unsigned long)seconds);
}
