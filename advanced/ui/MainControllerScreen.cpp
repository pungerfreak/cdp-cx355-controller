#include "MainControllerScreen.h"

#include <stdio.h>
#include "../fonts/open_sans_18_bold.c"
#include "../fonts/open_sans_18.c"
#include "../images/power.c"
#include "../images/disc.c"
#include "../images/track.c"
#include "../images/play.c"
#include "../images/pause.c"
#include "../images/prev-next.c"

void MainControllerScreen::init(lv_obj_t* parent, UiActionCb cb, void* user)
{
    cb_ = cb;
    user_ = user;

    bindings_[0] = {UiAction::PrevTrack, this};
    bindings_[1] = {UiAction::Play, this};
    bindings_[2] = {UiAction::NextTrack, this};
    bindings_[3] = {UiAction::Power, this};
    bindings_[4] = {UiAction::OpenDiscKeypad, this};

    auto wire_button_events = [&](lv_obj_t* obj, ActionBinding* binding) {
        lv_obj_add_event_cb(obj, MainControllerScreen::onButtonEvent_, LV_EVENT_PRESSED, binding);
        lv_obj_add_event_cb(obj, MainControllerScreen::onButtonEvent_, LV_EVENT_LONG_PRESSED, binding);
        lv_obj_add_event_cb(obj, MainControllerScreen::onButtonEvent_, LV_EVENT_RELEASED, binding);
    };

    const lv_coord_t screen_w = LV_HOR_RES;
    const lv_coord_t screen_h = LV_VER_RES;

    const lv_color_t color_bg = lv_color_hex(0x000000);
    const lv_color_t color_accent = lv_color_hex(0xDDCC0B);
    const lv_color_t color_txt = lv_color_hex(0xFFFFFF);

    static lv_style_t style_font_normal;
    lv_style_init(&style_font_normal);
    lv_style_set_text_font(&style_font_normal, &open_sans_18);

    static lv_style_t style_font_bold;
    lv_style_init(&style_font_bold);
    lv_style_set_text_font(&style_font_bold, &open_sans_18_bold);

    root_ = lv_obj_create(parent);
    setup_root_obj(root_);
    lv_obj_set_size(root_, screen_w, screen_h);
    lv_obj_set_layout(root_, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(root_, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(root_,
        LV_FLEX_ALIGN_START,   // justify-content
        LV_FLEX_ALIGN_CENTER,  // align-content
        LV_FLEX_ALIGN_CENTER   // align-items
    );

    lv_obj_set_style_bg_color(root_, color_bg, 0);
    lv_obj_set_style_bg_opa(root_, LV_OPA_COVER, 0);

    // Top row: Power button as icon
    lv_obj_t* powerBtn = lv_imgbtn_create(root_);
    lv_imgbtn_set_src(powerBtn, LV_IMGBTN_STATE_RELEASED, NULL, &power, NULL);
    lv_imgbtn_set_src(powerBtn, LV_IMGBTN_STATE_PRESSED, NULL, &power, NULL);
    lv_imgbtn_set_src(powerBtn, LV_IMGBTN_STATE_DISABLED, NULL, &power, NULL);
    lv_obj_set_size(powerBtn, power.header.w, power.header.h);
    wire_button_events(powerBtn, &bindings_[3]);
    topRowPower_ = powerBtn;

    // Top row: Disc / Track numbers
    const lv_coord_t disc_track_row_height = 28;

    // Disc & Track row container
    discTrackRow_ = lv_obj_create(root_);
    lv_obj_set_layout(discTrackRow_, LV_LAYOUT_FLEX);
    lv_obj_set_flex_align(discTrackRow_,
        LV_FLEX_ALIGN_CENTER,   // justify-content
        LV_FLEX_ALIGN_CENTER,   // align-content
        LV_FLEX_ALIGN_CENTER    // align-items
    );
    lv_obj_set_style_pad_all(discTrackRow_, 0, 0);
    lv_obj_set_style_pad_column(discTrackRow_, 10, 0);
    lv_obj_set_style_margin_top(discTrackRow_, 10, 0);
    lv_obj_set_style_border_width(discTrackRow_, 0, 0);
    lv_obj_set_style_bg_opa(discTrackRow_, LV_OPA_TRANSP, 0);
    lv_obj_set_size(discTrackRow_, LV_PCT(100), LV_SIZE_CONTENT);

    // Disc icon
    lv_obj_t* discBtn = lv_imgbtn_create(discTrackRow_);
    lv_imgbtn_set_src(discBtn, LV_IMGBTN_STATE_RELEASED, NULL, &disc, NULL);
    lv_obj_set_size(discBtn, disc.header.w, disc.header.h);

    // Disc label
    topRowDiscLabel_ = lv_label_create(discTrackRow_);
    lv_label_set_text(topRowDiscLabel_, "3");
    lv_obj_add_style(topRowDiscLabel_, &style_font_bold, 0);
    lv_obj_set_style_pad_right(topRowDiscLabel_, 10, 0);
    lv_obj_set_style_text_color(topRowDiscLabel_, color_txt, 0);

    // Track icon
    lv_obj_t* trackIcon = lv_img_create(discTrackRow_);
    lv_img_set_src(trackIcon, &track);
    lv_obj_set_size(trackIcon, track.header.w, track.header.h);

    // Track label
    topRowTrackLabel_ = lv_label_create(discTrackRow_);
    lv_label_set_text(topRowTrackLabel_, "3");
    lv_obj_add_style(topRowDiscLabel_, &style_font_bold, 0);
    lv_obj_set_width(topRowTrackLabel_, 25);
    lv_obj_set_style_text_color(topRowTrackLabel_, color_txt, 0);

    // Middle: three-line segment
    lv_obj_t* midContainer = lv_obj_create(root_);
    lv_obj_set_style_bg_opa(midContainer, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(midContainer, 0, 0);
    lv_obj_set_style_pad_all(midContainer, 0, 0);
    lv_obj_set_style_margin_ver(midContainer, 10, 0);
    lv_obj_set_size(midContainer, LV_PCT(100), LV_SIZE_CONTENT);
    lv_obj_clear_flag(midContainer, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_layout(midContainer, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(midContainer, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(midContainer,
        LV_FLEX_ALIGN_START,   // justify-content
        LV_FLEX_ALIGN_CENTER,  // align-content
        LV_FLEX_ALIGN_CENTER   // align-items
    );

    static lv_style_t mid_label_style_base;
    lv_style_init(&mid_label_style_base);
    lv_style_set_text_align(&mid_label_style_base, LV_TEXT_ALIGN_CENTER);
    lv_style_set_text_color(&mid_label_style_base, color_txt);

    midLine1_ = lv_label_create(midContainer);
    lv_label_set_text(midLine1_, "ARTIST");
    lv_obj_add_style(midLine1_, &mid_label_style_base, 0);
    lv_obj_add_style(midLine1_, &style_font_bold, 0);

    midLine2_ = lv_label_create(midContainer);
    lv_label_set_text(midLine2_, "Album");
    lv_obj_add_style(midLine2_, &mid_label_style_base, 0);
    lv_obj_add_style(midLine2_, &style_font_normal, 0);

    midLine3_ = lv_label_create(midContainer);
    lv_label_set_text(midLine3_, "Title");
    lv_obj_add_style(midLine3_, &mid_label_style_base, 0);
    lv_obj_add_style(midLine3_, &style_font_normal, 0);

    // // Bottom: transport buttons
    lv_obj_t* controls = lv_obj_create(root_);
    lv_obj_set_layout(controls, LV_LAYOUT_FLEX);
    lv_obj_set_flex_align(controls,
        LV_FLEX_ALIGN_CENTER,   // justify-content
        LV_FLEX_ALIGN_CENTER,   // align-content
        LV_FLEX_ALIGN_CENTER    // align-items
    );
    lv_obj_set_style_bg_opa(controls, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(controls, 0, 0);
    lv_obj_set_style_pad_all(controls, 0, 0);
    lv_obj_set_size(controls, LV_PCT(100), LV_SIZE_CONTENT);
    lv_obj_clear_flag(controls, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t* prevBtn = lv_imgbtn_create(controls);
    lv_imgbtn_set_src(prevBtn, LV_IMGBTN_STATE_RELEASED, NULL, &prev_next, NULL);
    lv_obj_set_size(prevBtn, prev_next.header.w, prev_next.header.h);
    lv_obj_set_style_transform_pivot_x(prevBtn, lv_pct(50), 0);
    lv_obj_set_style_transform_pivot_y(prevBtn, lv_pct(50), 0);
    lv_obj_set_style_transform_angle(prevBtn, 1800, 0);

    lv_obj_t* playBtn = lv_imgbtn_create(controls);
    lv_imgbtn_set_src(playBtn, LV_IMGBTN_STATE_RELEASED, NULL, &play, NULL);
    lv_obj_set_size(playBtn, play.header.w, play.header.h);

    lv_obj_t* nextBtn = lv_imgbtn_create(controls);
    lv_imgbtn_set_src(nextBtn, LV_IMGBTN_STATE_RELEASED, NULL, &prev_next, NULL);
    lv_obj_set_size(nextBtn, prev_next.header.w, prev_next.header.h);
}

void MainControllerScreen::render(const UiNowPlayingSnapshot& s)
{
    if (topRowDisc_ == nullptr || topRowTrack_ == nullptr) {
        return;
    }

    if (!hasLast_ || s.disc != last_.disc) {
        char discBuf[16];
        if (s.disc == 0) {
            snprintf(discBuf, sizeof(discBuf), "--");
        } else {
            snprintf(discBuf, sizeof(discBuf), "%u", (unsigned)s.disc);
        }
        if (topRowDiscLabel_ != nullptr) {
            lv_label_set_text(topRowDiscLabel_, discBuf);
        }
    }

    if (!hasLast_ || s.track != last_.track) {
        char trackBuf[16];
        if (s.track == 0) {
            snprintf(trackBuf, sizeof(trackBuf), "--");
        } else {
            snprintf(trackBuf, sizeof(trackBuf), "%u", (unsigned)s.track);
        }
        if (topRowTrackLabel_ != nullptr) {
            lv_label_set_text(topRowTrackLabel_, trackBuf);
        }
    }

    if ((!hasLast_ || s.transport != last_.transport) && transportImg_ != nullptr) {
    const lv_image_dsc_t* icon = (s.transport == UiTransportState::Playing) ? &pause_icon : &play;
        lv_img_set_src(transportImg_, icon);
    }

    last_ = s;
    hasLast_ = true;
}

void MainControllerScreen::show()
{
    if (root_ != nullptr) {
        lv_obj_clear_flag(root_, LV_OBJ_FLAG_HIDDEN);
    }
}

void MainControllerScreen::hide()
{
    if (root_ != nullptr) {
        lv_obj_add_flag(root_, LV_OBJ_FLAG_HIDDEN);
    }
}

void MainControllerScreen::setActionCallback(UiActionCb cb, void* user)
{
    cb_ = cb;
    user_ = user;
}

void MainControllerScreen::onButtonEvent_(lv_event_t* e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if (code != LV_EVENT_PRESSED && code != LV_EVENT_RELEASED && code != LV_EVENT_LONG_PRESSED) {
        return;
    }

    ActionBinding* binding = static_cast<ActionBinding*>(lv_event_get_user_data(e));
    if (binding == nullptr || binding->screen == nullptr) {
        return;
    }
    if (binding->action == UiAction::Play) {
        binding->screen->handleTransportButtonEvent_(code);
        return;
    }
    if (binding->action == UiAction::Power) {
        binding->screen->handlePowerButtonEvent_(code);
        return;
    }
    if (binding->action == UiAction::OpenDiscKeypad) {
        if (code == LV_EVENT_LONG_PRESSED) {
            binding->screen->emitAction_(binding->action);
        }
        return;
    }
    if (code == LV_EVENT_RELEASED) {
        binding->screen->emitAction_(binding->action);
    }
}

void MainControllerScreen::emitAction_(UiAction action)
{
    if (cb_ != nullptr) {
        cb_(action, user_);
    }
}

UiAction MainControllerScreen::transportPressAction_() const
{
    if (!hasLast_) {
        return UiAction::Play;
    }

    switch (last_.transport) {
        case UiTransportState::Playing:
            return UiAction::Pause;
        case UiTransportState::Paused:
        case UiTransportState::Stopped:
        case UiTransportState::Unknown:
        default:
            return UiAction::Play;
    }
}

void MainControllerScreen::handleTransportButtonEvent_(lv_event_code_t code)
{
    if (code == LV_EVENT_PRESSED) {
        transportLongPressHandled_ = false;
        return;
    }

    if (code == LV_EVENT_LONG_PRESSED) {
        emitAction_(UiAction::Stop);
        transportLongPressHandled_ = true;
        return;
    }

    if (code == LV_EVENT_RELEASED) {
        if (transportLongPressHandled_) {
            transportLongPressHandled_ = false;
            return;
        }
        emitAction_(transportPressAction_());
    }
}

void MainControllerScreen::handlePowerButtonEvent_(lv_event_code_t code)
{
    if (code == LV_EVENT_PRESSED) {
        powerLongPressHandled_ = false;
        return;
    }

    if (code == LV_EVENT_LONG_PRESSED) {
        emitAction_(UiAction::PowerOff);
        powerLongPressHandled_ = true;
        return;
    }

    if (code == LV_EVENT_RELEASED) {
        if (powerLongPressHandled_) {
            powerLongPressHandled_ = false;
            return;
        }
        emitAction_(UiAction::PowerOn);
    }
}
