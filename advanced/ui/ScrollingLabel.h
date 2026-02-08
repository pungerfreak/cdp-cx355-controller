#pragma once
#include <lvgl.h>

class ScrollingLabel {
public:
  void init(lv_obj_t* parent, const lv_font_t* font, lv_color_t color, lv_coord_t width);
  void setText(const char* text);
  lv_obj_t* obj() const { return container_; }

private:
  static void animExec_(void* var, int32_t v);
  void startAnim_();

  lv_obj_t* container_ = nullptr;
  lv_obj_t* label1_ = nullptr;
  lv_obj_t* label2_ = nullptr;
  lv_coord_t width_ = 0;
  int gap_px_ = 50;
  int speed_px_s_ = 32;
  uint32_t pause_ms_ = 5000;
  lv_coord_t textWidth_ = 0;
};
