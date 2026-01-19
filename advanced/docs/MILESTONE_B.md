Milestone B: Introduce UiApp skeleton (LVGL v9) with clean boundaries (no SLink integration yet)

Status
- Milestone A is complete and working in:
  advanced/ui_scratch/round_lvgl_v9_smoke/
- Acceptance criteria for A met.
- We are moving to Milestone B.

Goal
Create a minimal “UiApp” module that owns LVGL objects and UI-local state, and can render a snapshot struct.
This milestone is still UI-only: NO SLink includes, NO SLinkSystem usage, NO event bus subscription.
We will simulate the snapshot with a tiny local struct and update it periodically.

Key Architectural Rules
- UiApp is the view layer: it builds screens/widgets and updates them.
- UiApp must not include or reference:
  - advanced/transport/*
  - advanced/frame/*
  - advanced/command/*
  - advanced/intent/*
  - advanced/system/*
  - advanced/unit/*
- For now, define a local “snapshot” struct inside the UiApp module (we’ll swap it to real SLink types in Milestone C).

Allowed edits
- You may modify:
  advanced/ui_scratch/round_lvgl_v9_smoke/round_lvgl_v9_smoke.ino
- You may add new files under:
  advanced/ui_scratch/round_lvgl_v9_smoke/
- Do not touch other folders.

Deliverables / Files to add
1) advanced/ui_scratch/round_lvgl_v9_smoke/UiApp.h
2) advanced/ui_scratch/round_lvgl_v9_smoke/UiApp.cpp

UiApp API (implement exactly)
- In UiApp.h define:

  struct UiNowPlayingSnapshot {
    uint16_t disc;
    uint16_t track;
    uint32_t elapsed_sec;
    const char* title;
    const char* artist;
    const char* album;
    bool playing;
  };

  class UiApp {
  public:
    UiApp() = default;

    // Build all LVGL objects. Call once after LVGL init + display init.
    void init();

    // Render/update widgets from the snapshot. Must be safe to call repeatedly.
    void render(const UiNowPlayingSnapshot& s);

  private:
    // Store LVGL object pointers for labels/buttons needed in milestone B
    lv_obj_t* labelHeader_ = nullptr;   // e.g., "Disc 12 • Track 3"
    lv_obj_t* labelTime_   = nullptr;   // e.g., "01:23"
    lv_obj_t* labelMeta_   = nullptr;   // e.g., "Artist — Album\nTitle"
    lv_obj_t* labelState_  = nullptr;   // e.g., "PLAYING"/"PAUSED"

    // Keep minimal UI-local state if needed (optional)
    UiNowPlayingSnapshot last_{};
    bool hasLast_ = false;

    // Helpers
    static void formatTime_(char* out, size_t outLen, uint32_t elapsed_sec);
  };

Notes:
- Use LVGL v9 APIs (lv_screen_active()) for active screen retrieval if needed.
- Keep formatting lightweight; do not allocate in loop.
- Use fixed buffers on the stack when formatting strings.

Implementation requirements
- UiApp::init():
  - Create a “Now Playing” layout with 4 labels:
    1) Header (disc/track)
    2) Time
    3) Metadata block (artist/album/title)
    4) State line (PLAYING/PAUSED)
  - Centered and readable on 240x240. Keep it simple (no fancy styling).
  - No buttons yet (Milestone D later); only text widgets.

- UiApp::render(snapshot):
  - Update label text only if values changed (avoid constant lv_label_set_text calls).
  - Format time as mm:ss with leading zeros.
  - Handle null strings: if title/artist/album is null, show "-" for that field.
  - Keep “playing” to show PLAYING vs PAUSED.

Update the existing sketch (round_lvgl_v9_smoke.ino)
- Replace direct label creation/update from Milestone A with UiApp usage:
  - In setup():
    - Do the same LVGL init and lv_xiao_* init as Milestone A.
    - Create a global/static UiApp app; call app.init().
  - In loop():
    - Keep lv_timer_handler() call.
    - Create a periodic simulation of UiNowPlayingSnapshot:
      - Every 1 second:
        - increment elapsed_sec
        - toggle playing every ~10 seconds (optional)
        - change track every ~15 seconds (optional)
        - keep disc fixed or change slowly
      - Call app.render(snapshot) each second.
    - Keep touch logging from Milestone A intact (do not remove).
      (Touch logging can remain in the ino; UiApp does not need touch yet.)

Acceptance Criteria
- Compiles and runs on XIAO ESP32-C3.
- Screen shows the 4-line Now Playing view built by UiApp (not built in the ino).
- Text updates once per second via app.render(snapshot).
- Touch logging still works (serial logs on touch).
- No dynamic allocations in loop; no LVGL object recreation in loop.
- Runs 60+ seconds without hang/reset.

Output
- Provide full contents of:
  - UiApp.h
  - UiApp.cpp
  - Updated round_lvgl_v9_smoke.ino
- Briefly describe how the snapshot simulation updates disc/track/time.

Do NOT do yet
- No SLinkSystem, no UnitEventBus subscription, no intents.
- No button handlers.
- No disc scrubber / keypad.
