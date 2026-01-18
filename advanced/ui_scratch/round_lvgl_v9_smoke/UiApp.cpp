#include "UiApp.h"

#include <stdio.h>
#include <string.h>

namespace {
bool str_equal(const char* a, const char* b)
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
    lv_obj_set_size(root, 220, 220);
    lv_obj_center(root);
    lv_obj_set_flex_flow(root, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(root, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_bg_opa(root, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_opa(root, LV_OPA_TRANSP, 0);
    lv_obj_set_style_pad_all(root, 4, 0);
    lv_obj_set_style_pad_row(root, 4, 0);

    labelHeader_ = lv_label_create(root);
    lv_obj_set_width(labelHeader_, 220);
    lv_obj_set_style_text_align(labelHeader_, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text(labelHeader_, "Disc - | Track -");

    labelTime_ = lv_label_create(root);
    lv_obj_set_width(labelTime_, 220);
    lv_obj_set_style_text_align(labelTime_, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text(labelTime_, "00:00");

    labelMeta_ = lv_label_create(root);
    lv_obj_set_width(labelMeta_, 220);
    lv_obj_set_style_text_align(labelMeta_, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_long_mode(labelMeta_, LV_LABEL_LONG_WRAP);
    lv_label_set_text(labelMeta_, "- -\n-");

    labelState_ = lv_label_create(root);
    lv_obj_set_width(labelState_, 220);
    lv_obj_set_style_text_align(labelState_, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text(labelState_, "PAUSED");

    bindings_[0] = {UiAction::PrevTrack, this};
    bindings_[1] = {UiAction::Play, this};
    bindings_[2] = {UiAction::Pause, this};
    bindings_[3] = {UiAction::Stop, this};
    bindings_[4] = {UiAction::NextTrack, this};
    bindings_[5] = {UiAction::Power, this};

    lv_obj_t* controls = lv_obj_create(root);
    lv_obj_set_size(controls, 220, 36);
    lv_obj_set_flex_flow(controls, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(controls, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_bg_opa(controls, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_opa(controls, LV_OPA_TRANSP, 0);
    lv_obj_set_style_pad_all(controls, 0, 0);

    auto add_button = [&](const char* text, ActionBinding* binding) {
        lv_obj_t* btn = lv_btn_create(controls);
        lv_obj_set_size(btn, 40, 32);
        lv_obj_add_event_cb(btn, UiApp::onButtonEvent_, LV_EVENT_CLICKED, binding);
        lv_obj_t* label = lv_label_create(btn);
        lv_label_set_text(label, text);
        lv_obj_center(label);
    };

    add_button("Prev", &bindings_[0]);
    add_button("Play", &bindings_[1]);
    add_button("Pause", &bindings_[2]);
    add_button("Stop", &bindings_[3]);
    add_button("Next", &bindings_[4]);

    lv_obj_t* powerBtn = lv_btn_create(root);
    lv_obj_set_size(powerBtn, 68, 28);
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

    if (!hasLast_ || !str_equal(s.artist, last_.artist) || !str_equal(s.album, last_.album)
        || !str_equal(s.title, last_.title)) {
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
