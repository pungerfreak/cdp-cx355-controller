Milestone A (in-repo, deterministic): LVGL v9 smoke test using the existing seed sketch

Context
- Hardware: Seeed Round Display + XIAO ESP32-C3
- LVGL: v9
- We already have a compiling seed sketch at:
  advanced/ui_scratch/milestone_a_seed/HardwearTest/HardwearTest.ino
  (NOTE: directory name is spelled “HardwearTest”)

Goal
Create a new smoke-test sketch folder that:
- boots reliably
- shows a centered label with “LVGL v9 OK” and a 1Hz counter
- prints LVGL version and touch coordinates to Serial
- does NOT rely on lv_hardware_test()

Rules
- Do NOT modify any production S-Link code.
- Do NOT change the seed sketch in place.
- Only add a new folder and new sketch derived from the seed.

Steps

1) Create new folder:
   advanced/ui_scratch/round_lvgl_v9_smoke/

2) Add new sketch file:
   advanced/ui_scratch/round_lvgl_v9_smoke/round_lvgl_v9_smoke.ino

3) Start by copying the seed sketch contents exactly, then apply these edits:

   A) Remove the hardware test include + call
      - Remove: #include "lv_hardware_test.h"
      - Remove: lv_hardware_test();

   B) Keep the display + touch init exactly as in seed:
      - lv_init();
      - lv_tick_set_cb(millis);  (guarded for v9 as in seed)
      - lv_xiao_disp_init();
      - lv_xiao_touch_init();

   C) Add a single label on the active screen:
      - Create lv_obj_t* label;
      - Set label text to "LVGL v9 OK\n t=0"
      - Center it.

   D) Add a counter that increments once per second:
      - Use millis() scheduling (no long delay).
      - Maintain uint32_t lastUpdateMs and uint32_t secondsCounter.
      - Every 1000ms:
        - secondsCounter++
        - update label text to: "LVGL v9 OK\n t=<secondsCounter>"

   E) Add touch logging using LVGL input device data:
      - Use lv_indev_get_act() and lv_indev_get_point() to get current touch point when pressed
        (If those exact APIs differ, use the closest LVGL v9 equivalents, but do not add new external libs.)
      - Print to Serial at most every ~100ms while pressed:
        "touch: x=<x> y=<y>"
      - When not pressed, optionally print "touch: none" at a slower rate (e.g., every 500ms) to avoid spam.
      - If LVGL indev APIs are awkward, fallback approach:
        - Attach an LVGL event callback to lv_screen_active()
        - On LV_EVENT_PRESSED / LV_EVENT_PRESSING / LV_EVENT_RELEASED, log coords using lv_indev_get_point()

   F) Loop timing:
      - Replace delay(5) with delay(1) OR no delay (if stable), but keep lv_timer_handler() called frequently.
      - Keep the superloop simple:
        - lv_timer_handler();
        - tiny delay(1) optional

   G) Serial startup info:
      - Print LVGL version major/minor/patch at boot if available via macros (LVGL_VERSION_MAJOR etc.).
      - Also print “round_lvgl_v9_smoke boot” so we can distinguish from seed.

4) Add a short README (optional but helpful):
   advanced/ui_scratch/round_lvgl_v9_smoke/README.md
   Include:
   - Uses same init sequence as milestone_a_seed/HardwearTest
   - Board: XIAO ESP32-C3
   - Libraries: same as seed
   - Expected serial output examples

Acceptance Criteria
- Sketch compiles and runs on XIAO ESP32-C3.
- Screen shows centered text “LVGL v9 OK” and a counter that increments every second.
- Touching screen produces serial logs with coordinates.
- Runs for 60+ seconds without hang/reset.
- No dependency on lv_hardware_test().

Output to provide
- Full contents of:
  advanced/ui_scratch/round_lvgl_v9_smoke/round_lvgl_v9_smoke.ino
- (If created) README.md content
- Mention any LVGL API substitutions you made for touch coordinate retrieval.
