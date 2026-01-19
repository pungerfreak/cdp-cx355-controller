#pragma once

#include <lvgl.h>

// Install a wrapper around the first pointer indev to suppress brief release gaps.
void install_touch_gap_filter();
