#include "ui/LoadingScreen.h"

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
  lv_obj_set_style_bg_color(root_, lv_color_white(), 0);
  lv_obj_set_style_bg_opa(root_, LV_OPA_COVER, 0);

  labelStage_ = lv_label_create(root_);
  lv_obj_set_size(labelStage_, LV_HOR_RES - 20, 24);
  lv_obj_set_pos(labelStage_, 10, 40);
  lv_obj_set_style_text_align(labelStage_, LV_TEXT_ALIGN_CENTER, 0);
  lv_label_set_text(labelStage_, "Starting CDDB scan...");

  labelDisc_ = lv_label_create(root_);
  lv_obj_set_size(labelDisc_, LV_HOR_RES - 20, 20);
  lv_obj_set_pos(labelDisc_, 10, 70);
  lv_obj_set_style_text_align(labelDisc_, LV_TEXT_ALIGN_CENTER, 0);
  lv_label_set_text(labelDisc_, "Disc 0 / 0");

  bar_ = lv_bar_create(root_);
  lv_obj_set_size(bar_, LV_HOR_RES - 40, 16);
  lv_obj_set_pos(bar_, 20, 110);
  lv_bar_set_range(bar_, 0, 100);
  lv_obj_set_style_bg_color(bar_, lv_color_black(), LV_PART_MAIN);
  lv_obj_set_style_bg_opa(bar_, LV_OPA_20, LV_PART_MAIN);
  lv_obj_set_style_bg_color(bar_, lv_color_black(), LV_PART_INDICATOR);
  lv_obj_set_style_bg_opa(bar_, LV_OPA_COVER, LV_PART_INDICATOR);
  lv_bar_set_value(bar_, 0, LV_ANIM_OFF);
}

void LoadingScreen::show() {
  if (root_) {
    lv_obj_clear_flag(root_, LV_OBJ_FLAG_HIDDEN);
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
  lv_label_set_text(labelStage_, stage);
  char buf[64];
  if (unitsTotal > 0) {
    snprintf(buf,
             sizeof(buf),
             "Disc %u / %u (%lu/%lu tracks)",
             static_cast<unsigned>(disc),
             static_cast<unsigned>(totalDiscs),
             static_cast<unsigned long>(unitsDone),
             static_cast<unsigned long>(unitsTotal));
  } else {
    snprintf(buf,
             sizeof(buf),
             "Disc %u / %u",
             static_cast<unsigned>(disc),
             static_cast<unsigned>(totalDiscs));
  }
  lv_label_set_text(labelDisc_, buf);
  lv_bar_set_value(bar_, percent, LV_ANIM_OFF);
}
