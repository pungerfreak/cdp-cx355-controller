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

void setup_root_obj(lv_obj_t* root)
{
    lv_obj_set_size(root, LV_HOR_RES, LV_VER_RES);
    lv_obj_set_pos(root, 0, 0);
    lv_obj_set_layout(root, LV_LAYOUT_NONE);
    lv_obj_set_style_pad_all(root, 0, 0);
    lv_obj_set_style_pad_row(root, 0, 0);
    lv_obj_set_style_pad_column(root, 0, 0);
    lv_obj_clear_flag(root, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_opa(root, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_opa(root, LV_OPA_TRANSP, 0);
}
}  // namespace

void UiApp::init()
{
    lv_obj_t* screen = lv_screen_active();

    nowPlayingRoot_ = lv_obj_create(screen);
    setup_root_obj(nowPlayingRoot_);

    keypadRoot_ = lv_obj_create(screen);
    setup_root_obj(keypadRoot_);
    lv_obj_add_flag(keypadRoot_, LV_OBJ_FLAG_HIDDEN);

    const lv_coord_t screen_w = LV_HOR_RES;
    const lv_coord_t header_w = 140;
    const lv_coord_t time_w = 170;
    const lv_coord_t meta_w = 160;
    const lv_coord_t state_w = 200;
    const lv_coord_t controls_w = 200;

    labelHeader_ = lv_label_create(nowPlayingRoot_);
    lv_obj_set_size(labelHeader_, header_w, 20);
    lv_obj_set_pos(labelHeader_, (screen_w - header_w) / 2, 24);
    lv_obj_set_style_text_align(labelHeader_, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text(labelHeader_, "Disc - | Track -");

    labelTime_ = lv_label_create(nowPlayingRoot_);
    lv_obj_set_size(labelTime_, time_w, 18);
    lv_obj_set_pos(labelTime_, (screen_w - time_w) / 2, 46);
    lv_obj_set_style_text_align(labelTime_, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text(labelTime_, "00:00");

    labelMeta_ = lv_label_create(nowPlayingRoot_);
    lv_obj_set_size(labelMeta_, meta_w, 56);
    lv_obj_set_pos(labelMeta_, 20, 66);
    lv_obj_set_style_text_align(labelMeta_, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_long_mode(labelMeta_, LV_LABEL_LONG_DOT);
    lv_label_set_text(labelMeta_, "- -\n-");

    labelState_ = lv_label_create(nowPlayingRoot_);
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
    bindings_[6] = {UiAction::OpenDiscKeypad, this};
    bindings_[7] = {UiAction::KeypadDigit0, this};
    bindings_[8] = {UiAction::KeypadDigit1, this};
    bindings_[9] = {UiAction::KeypadDigit2, this};
    bindings_[10] = {UiAction::KeypadDigit3, this};
    bindings_[11] = {UiAction::KeypadDigit4, this};
    bindings_[12] = {UiAction::KeypadDigit5, this};
    bindings_[13] = {UiAction::KeypadDigit6, this};
    bindings_[14] = {UiAction::KeypadDigit7, this};
    bindings_[15] = {UiAction::KeypadDigit8, this};
    bindings_[16] = {UiAction::KeypadDigit9, this};
    bindings_[17] = {UiAction::KeypadBackspace, this};
    bindings_[18] = {UiAction::KeypadClear, this};
    bindings_[19] = {UiAction::KeypadGo, this};
    bindings_[20] = {UiAction::KeypadCancel, this};

    auto wire_press_release = [&](lv_obj_t* obj, ActionBinding* binding) {
        lv_obj_add_event_cb(obj, UiApp::onButtonEvent_, LV_EVENT_PRESSED, binding);
        lv_obj_add_event_cb(obj, UiApp::onButtonEvent_, LV_EVENT_RELEASED, binding);
    };

    lv_obj_t* controls = lv_obj_create(nowPlayingRoot_);
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
        wire_press_release(btn, binding);
        lv_obj_t* label = lv_label_create(btn);
        lv_label_set_text(label, text);
        lv_obj_center(label);
    };

    add_button("Prev", &bindings_[0], 0);
    add_button("Play", &bindings_[1], 1);
    add_button("Pause", &bindings_[2], 2);
    add_button("Stop", &bindings_[3], 3);
    add_button("Next", &bindings_[4], 4);

    lv_obj_t* powerBtn = lv_btn_create(nowPlayingRoot_);
    lv_obj_set_size(powerBtn, 42, 32);
    lv_obj_set_pos(powerBtn, 188, 80);
    wire_press_release(powerBtn, &bindings_[5]);
    lv_obj_t* powerLabel = lv_label_create(powerBtn);
    lv_label_set_text(powerLabel, "Power");
    lv_obj_center(powerLabel);

    lv_obj_t* discBtn = lv_btn_create(nowPlayingRoot_);
    lv_obj_set_size(discBtn, 56, 32);
    lv_obj_set_pos(discBtn, (screen_w - 56) / 2, 188);
    wire_press_release(discBtn, &bindings_[6]);
    lv_obj_t* discLabel = lv_label_create(discBtn);
    lv_label_set_text(discLabel, "Disc");
    lv_obj_center(discLabel);

    const lv_coord_t inset = 20;
    const lv_coord_t label_w = screen_w - (inset * 2);

    lv_obj_t* keypadTitle = lv_label_create(keypadRoot_);
    lv_obj_set_size(keypadTitle, label_w, 18);
    lv_obj_set_pos(keypadTitle, inset, 20);
    lv_obj_set_style_text_align(keypadTitle, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text(keypadTitle, "Select Disc");

    keypadEntry_ = lv_label_create(keypadRoot_);
    lv_obj_set_size(keypadEntry_, 140, 28);
    lv_obj_set_pos(keypadEntry_, (screen_w - 140) / 2, 42);
    lv_obj_set_style_text_align(keypadEntry_, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_long_mode(keypadEntry_, LV_LABEL_LONG_CLIP);
    lv_label_set_text(keypadEntry_, "___");

    keypadError_ = lv_label_create(keypadRoot_);
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

    auto add_key = [&](const char* text, ActionBinding* binding, lv_coord_t col, lv_coord_t row) {
        lv_obj_t* btn = lv_btn_create(keypadRoot_);
        lv_obj_set_size(btn, key_w, key_h);
        lv_obj_set_pos(btn,
                       key_grid_x + col * (key_w + key_gap),
                       key_grid_y + row * (key_h + key_gap));
        wire_press_release(btn, binding);
        lv_obj_t* label = lv_label_create(btn);
        lv_label_set_text(label, text);
        lv_obj_center(label);
    };

    add_key("1", &bindings_[8], 0, 0);
    add_key("2", &bindings_[9], 1, 0);
    add_key("3", &bindings_[10], 2, 0);
    add_key("Bksp", &bindings_[17], 3, 0);
    add_key("Go", &bindings_[19], 4, 0);

    add_key("4", &bindings_[11], 0, 1);
    add_key("5", &bindings_[12], 1, 1);
    add_key("6", &bindings_[13], 2, 1);
    add_key("Clear", &bindings_[18], 3, 1);
    add_key("Cncl", &bindings_[20], 4, 1);

    add_key("7", &bindings_[14], 0, 2);
    add_key("8", &bindings_[15], 1, 2);
    add_key("9", &bindings_[16], 2, 2);
    add_key("0", &bindings_[7], 3, 2);
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

void UiApp::setKeypadError(bool on)
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

void UiApp::showNowPlaying()
{
    showNowPlaying_();
}

void UiApp::onButtonEvent_(lv_event_t* e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if (code != LV_EVENT_PRESSED && code != LV_EVENT_RELEASED) {
        return;
    }

    ActionBinding* binding = static_cast<ActionBinding*>(lv_event_get_user_data(e));
    if (binding == nullptr || binding->app == nullptr) {
        return;
    }
    if (code == LV_EVENT_RELEASED) {
        binding->app->emitAction_(binding->action);
    }
}

void UiApp::emitAction_(UiAction action)
{
    switch (action) {
        case UiAction::OpenDiscKeypad:
            showKeypad_();
            break;
        case UiAction::KeypadCancel:
            showNowPlaying_();
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
        case UiAction::KeypadBackspace:
        case UiAction::KeypadClear:
            handleKeypadInput_(action);
            break;
        default:
            break;
    }
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

void UiApp::showNowPlaying_()
{
    if (nowPlayingRoot_ == nullptr || keypadRoot_ == nullptr) {
        return;
    }
    keypadLen_ = 0;
    keypadBuf_[0] = '\0';
    updateKeypadDisplay_();
    lv_obj_clear_flag(nowPlayingRoot_, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(keypadRoot_, LV_OBJ_FLAG_HIDDEN);
    setKeypadError(false);
}

void UiApp::showKeypad_()
{
    if (nowPlayingRoot_ == nullptr || keypadRoot_ == nullptr) {
        return;
    }
    keypadLen_ = 0;
    keypadBuf_[0] = '\0';
    updateKeypadDisplay_();
    setKeypadError(false);
    lv_obj_clear_flag(keypadRoot_, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(nowPlayingRoot_, LV_OBJ_FLAG_HIDDEN);
}

void UiApp::updateKeypadDisplay_()
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

void UiApp::handleKeypadInput_(UiAction action)
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
        setKeypadError(false);
        updateKeypadDisplay_();
    }
}
