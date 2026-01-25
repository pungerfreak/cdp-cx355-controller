#ifndef LOADING_SCREEN_H_
#define LOADING_SCREEN_H_
#include <lvgl.h>
#include <Arduino.h>

class LoadingScreen {
public:
  void init(lv_obj_t* parent);
  void show();
  void hide();
  void setStatus(const char* stage,
                 uint16_t disc,
                 uint16_t totalDiscs,
                 uint8_t percent,
                 uint32_t unitsDone = 0,
                 uint32_t unitsTotal = 0);

private:
  lv_obj_t* root_ = nullptr;
  lv_obj_t* labelStage_ = nullptr;
  lv_obj_t* labelDisc_ = nullptr;
  lv_obj_t* bar_ = nullptr;
};

#endif  // LOADING_SCREEN_H_
