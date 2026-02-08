#include "ui/LoadingScreen.h"

extern const lv_font_t open_sans_18;
extern const lv_font_t open_sans_18_bold;

namespace {
void setup_root(lv_obj_t* root) {
  lv_obj_set_size(root, LV_HOR_RES, LV_VER_RES);
  lv_obj_set_pos(root, 0, 0);
  lv_obj_set_layout(root, LV_LAYOUT_NONE);
  lv_obj_set_style_pad_all(root, 0, 0);
  lv_obj_clear_flag(root, LV_OBJ_FLAG_SCROLLABLE);
}
}  // namespace

void LoadingScreen::init(lv_obj_t* parent) {
  if (!parent) parent = lv_screen_active();
  root_ = lv_obj_create(parent);
  setup_root(root_);
  const lv_color_t color_bg = lv_color_hex(0x000000);
  const lv_color_t color_accent = lv_color_hex(0xDDCC0B);
  const lv_color_t color_text = lv_color_hex(0xFFFFFF);
  lv_obj_set_style_bg_color(root_, color_bg, 0);
  lv_obj_set_style_bg_opa(root_, LV_OPA_COVER, 0);
  lv_obj_set_style_border_width(root_, 0, 0);

  labelStage_ = lv_label_create(root_);
  lv_obj_set_size(labelStage_, LV_HOR_RES - 20, 24);
  lv_obj_set_pos(labelStage_, 10, 40);
  lv_obj_set_style_text_align(labelStage_, LV_TEXT_ALIGN_CENTER, 0);
  lv_obj_set_style_text_color(labelStage_, color_text, 0);
  lv_obj_set_style_text_font(labelStage_, &open_sans_18_bold, 0);
  lv_label_set_text(labelStage_, "Starting CDDB scan...");

  labelDisc_ = lv_label_create(root_);
  lv_obj_set_size(labelDisc_, LV_HOR_RES - 20, 20);
  lv_obj_set_pos(labelDisc_, 10, 70);
  lv_obj_set_style_text_align(labelDisc_, LV_TEXT_ALIGN_CENTER, 0);
  lv_obj_set_style_text_color(labelDisc_, color_text, 0);
  lv_obj_set_style_text_font(labelDisc_, &open_sans_18, 0);
  lv_label_set_text(labelDisc_, "Disc 0 / 0");

  bar_ = lv_bar_create(root_);
  lv_obj_set_size(bar_, LV_HOR_RES - 40, 16);
  lv_obj_set_pos(bar_, 20, 110);
  lv_bar_set_range(bar_, 0, 100);
  lv_obj_set_style_bg_color(bar_, lv_color_hex(0x444444), LV_PART_MAIN);
  lv_obj_set_style_bg_opa(bar_, LV_OPA_30, LV_PART_MAIN);
  lv_obj_set_style_bg_color(bar_, color_accent, LV_PART_INDICATOR);
  lv_obj_set_style_bg_opa(bar_, LV_OPA_COVER, LV_PART_INDICATOR);
  lv_bar_set_value(bar_, 0, LV_ANIM_OFF);

  btnCancel_ = lv_btn_create(root_);
  lv_obj_set_size(btnCancel_, 140, 38);
  lv_obj_align(btnCancel_, LV_ALIGN_CENTER, 0, 60);
  lv_obj_set_style_bg_color(btnCancel_, color_bg, LV_PART_MAIN);
  lv_obj_set_style_bg_opa(btnCancel_, LV_OPA_TRANSP, LV_PART_MAIN);
  lv_obj_set_style_border_color(btnCancel_, color_accent, LV_PART_MAIN);
  lv_obj_set_style_border_opa(btnCancel_, LV_OPA_COVER, LV_PART_MAIN);
  lv_obj_set_style_border_width(btnCancel_, 2, LV_PART_MAIN);
  lv_obj_set_style_radius(btnCancel_, 12, LV_PART_MAIN);
  lv_obj_set_style_bg_color(btnCancel_, lv_color_hex(0x222222), LV_PART_MAIN | LV_STATE_PRESSED);
  lv_obj_t* labelCancel = lv_label_create(btnCancel_);
  lv_label_set_text(labelCancel, "Cancel");
  lv_obj_set_style_text_color(labelCancel, color_accent, LV_PART_MAIN);
  lv_obj_set_style_text_font(labelCancel, &open_sans_18, LV_PART_MAIN);
  lv_obj_center(labelCancel);
  lv_obj_move_foreground(btnCancel_);
}

void LoadingScreen::show() {
  if (root_) {
    lv_obj_clear_flag(root_, LV_OBJ_FLAG_HIDDEN);
    lv_obj_move_foreground(root_);
  }
}

void LoadingScreen::hide() {
  if (root_) {
    lv_obj_add_flag(root_, LV_OBJ_FLAG_HIDDEN);
  }
}

void LoadingScreen::setStatus(const char* stage,
                              uint16_t disc,
                              uint16_t totalDiscs,
                              uint8_t percent,
                              uint32_t unitsDone,
                              uint32_t unitsTotal) {
  if (!root_) return;
  if (!stage) stage = "";
  // Keep the bar from shrinking when totals change by never letting percent drop.
  if (percent == 0 && unitsDone == 0) {
    lastPercent_ = 0;
    lastDisplayPercent_ = 0.0f;
  } else if (percent < lastPercent_) {
    percent = lastPercent_;
  } else {
    lastPercent_ = percent;
  }
  float displayPercent = percent;
  if (unitsTotal > 0) {
    displayPercent = (static_cast<float>(unitsDone) * 100.0f) /
                     static_cast<float>(unitsTotal);
  }
  if (displayPercent < lastDisplayPercent_) {
    displayPercent = lastDisplayPercent_;
  } else {
    lastDisplayPercent_ = displayPercent;
  }
  if (displayPercent > 100.0f) displayPercent = 100.0f;
  lv_label_set_text(labelStage_, stage);
  char buf[64];
  snprintf(buf,
           sizeof(buf),
           "Disc %u / %u (%.2f%%)",
           static_cast<unsigned>(disc),
           static_cast<unsigned>(totalDiscs),
           static_cast<double>(displayPercent));
  lv_label_set_text(labelDisc_, buf);
  lv_bar_set_value(bar_, percent, LV_ANIM_OFF);
}
