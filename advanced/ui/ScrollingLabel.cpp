#include "ScrollingLabel.h"

void ScrollingLabel::init(lv_obj_t* parent,
                          const lv_font_t* font,
                          lv_color_t color,
                          lv_coord_t width) {
  if (!parent) parent = lv_screen_active();
  width_ = width;
  container_ = lv_obj_create(parent);
  lv_obj_set_size(container_, width_, LV_SIZE_CONTENT);
  lv_obj_set_style_pad_left(container_, 5, 0);
  lv_obj_set_style_pad_right(container_, 5, 0);
  lv_obj_set_style_pad_top(container_, 0, 0);
  lv_obj_set_style_pad_bottom(container_, 0, 0);
  lv_obj_set_style_border_width(container_, 0, 0);
  lv_obj_set_style_bg_opa(container_, LV_OPA_TRANSP, 0);
  lv_obj_clear_flag(container_, LV_OBJ_FLAG_SCROLLABLE);

  label1_ = lv_label_create(container_);
  label2_ = lv_label_create(container_);
  lv_label_set_long_mode(label1_, LV_LABEL_LONG_CLIP);
  lv_label_set_long_mode(label2_, LV_LABEL_LONG_CLIP);
  lv_obj_set_style_text_align(label1_, LV_TEXT_ALIGN_CENTER, 0);
  lv_obj_set_style_text_align(label2_, LV_TEXT_ALIGN_CENTER, 0);
  if (font) {
    lv_obj_set_style_text_font(label1_, font, 0);
    lv_obj_set_style_text_font(label2_, font, 0);
  }
  lv_obj_set_style_text_color(label1_, color, 0);
  lv_obj_set_style_text_color(label2_, color, 0);
  lv_label_set_text(label1_, "");
  lv_label_set_text(label2_, "");
}

void ScrollingLabel::setText(const char* text) {
  if (!container_ || !label1_ || !label2_) return;
  const char* src = text ? text : "";
  lv_label_set_text(label1_, src);
  lv_label_set_text(label2_, src);
  lv_obj_update_layout(container_);
  lv_obj_update_layout(label1_);
  textWidth_ = lv_obj_get_width(label1_);
  if (textWidth_ == 0) textWidth_ = 1;

  // Only animate if content exceeds available width.
  lv_coord_t available = lv_obj_get_width(container_) -
                         lv_obj_get_style_pad_left(container_, 0) -
                         lv_obj_get_style_pad_right(container_, 0);
  if (available < 1) available = width_;

  if (textWidth_ > available) {
    lv_obj_clear_flag(label1_, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(label2_, LV_OBJ_FLAG_HIDDEN);
    lv_obj_set_pos(label1_, 0, 0);
    lv_obj_set_pos(label2_, textWidth_ + gap_px_, 0);
    startAnim_();
  } else {
    lv_anim_del(this, animExec_);
    lv_obj_add_flag(label2_, LV_OBJ_FLAG_HIDDEN);
    lv_coord_t offset = (available - textWidth_) / 2;
    if (offset < 0) offset = 0;
    lv_obj_set_pos(label1_, offset, 0);
  }
}

void ScrollingLabel::animExec_(void* var, int32_t v) {
  ScrollingLabel* self = static_cast<ScrollingLabel*>(var);
  if (!self || !self->label1_ || !self->label2_) return;
  int32_t dist = self->textWidth_ + self->gap_px_;
  lv_obj_set_x(self->label1_, -v);
  lv_obj_set_x(self->label2_, -v + dist);
}

void ScrollingLabel::startAnim_() {
  if (!label1_ || !label2_) return;
  int32_t dist = textWidth_ + gap_px_;
  if (dist <= 0) return;
  lv_anim_del(this, animExec_);
  lv_anim_t a;
  lv_anim_init(&a);
  lv_anim_set_var(&a, this);
  lv_anim_set_exec_cb(&a, animExec_);
  lv_anim_set_values(&a, 0, dist);
  uint32_t duration = (uint32_t)((dist * 1000) / speed_px_s_);
  if (duration < 100) duration = 100;
  lv_anim_set_time(&a, duration);
  lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
  lv_anim_set_repeat_delay(&a, pause_ms_);
  lv_anim_set_delay(&a, pause_ms_);
  lv_anim_start(&a);
}
