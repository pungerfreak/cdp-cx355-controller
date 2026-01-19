Milestone D (no scrubbing yet): Add touch controls that emit intents; UI stays snapshot-driven

Status
- Milestones A–C complete.
- Working sketch folder:
  advanced/ui_scratch/round_lvgl_v9_smoke/
- UI renders disc/track + transport state from SLink snapshot via UiAdapter.
- Touch is working and logged.
- Transport state now exists in snapshots.

Scope (Milestone D)
Add basic on-screen controls:
- Play
- Pause
- Stop
- Next Track
- Previous Track
- Power (toggle or discrete on/off if available)

Controls must emit intents through SLinkCommandIntentSource (via SLinkSystem::intentSource()).
UI must NOT be optimistic: it only changes displayed transport state based on events/snapshot.

Out of scope
- Disc selection scrubber
- Numeric disc entry keypad
- Elapsed-time ticking/polling

Rules / Architecture
- UiApp: LVGL view only. No SLink includes.
- UiAdapter: owns SLink includes and intent source usage.
- round_lvgl_v9_smoke.ino: wiring only.
- Keep changes localized to the scratch folder unless absolutely required (do not refactor SLink core).

Files to modify
- advanced/ui_scratch/round_lvgl_v9_smoke/UiApp.h
- advanced/ui_scratch/round_lvgl_v9_smoke/UiApp.cpp
- advanced/ui_scratch/round_lvgl_v9_smoke/UiAdapter.h
- advanced/ui_scratch/round_lvgl_v9_smoke/UiAdapter.cpp
- advanced/ui_scratch/round_lvgl_v9_smoke/round_lvgl_v9_smoke.ino
- Add: advanced/ui_scratch/round_lvgl_v9_smoke/MILESTONE_D.md

UI requirements (layout + behavior)
1) Keep the existing “Now Playing” labels (disc/track, metadata block, state line).
2) Add a bottom control row (or two small rows) of LVGL buttons:
   - Prev | Play | Pause | Stop | Next
   - Power (small button, e.g. top-right or below row)
3) Buttons should be comfortably tappable on 240x240.
4) On button press:
   - call UiApp callback -> UiAdapter emits corresponding intent.
   - Do not directly update the “PLAYING/PAUSED/STOPPED” label in response to tap.
   - The label updates only when snapshot changes.

UiApp <-> UiAdapter contract
- UiApp should expose a way to register callbacks for UI actions without including SLink.
Implement in UiApp.h:

  enum class UiAction {
    PrevTrack,
    NextTrack,
    Play,
    Pause,
    Stop,
    Power
  };

  using UiActionCb = void(*)(UiAction action, void* user);

  class UiApp {
  public:
    ...
    void setActionCallback(UiActionCb cb, void* user);
    ...
  };

- UiApp internal LVGL button event handlers should call:
    if (cb_) cb_(UiAction::Play, user_);
  Use LV_EVENT_CLICKED for taps (not PRESSED) to avoid accidental triggers.

UiAdapter responsibilities
1) Hold reference to SLinkSystem and UiApp (already).
2) On start():
   - subscribe to unit events as already done
   - prime snapshot render
   - register UiApp action callback:
       app_.setActionCallback(&UiAdapter::onUiActionThunk_, this);

3) Implement a static thunk + instance method:
   - static void onUiActionThunk_(UiAction action, void* user);
   - void onUiAction_(UiAction action);

4) In onUiAction_:
   - translate UiAction -> intentSource() call.
   Use the existing SLinkCommandIntentSource interface. Map as:
   - Play  -> intents.play()
   - Pause -> intents.pause()  (or pauseToggle if that’s the API; prefer explicit)
   - Stop  -> intents.stop()
   - Next  -> intents.nextTrack()
   - Prev  -> intents.prevTrack()
   - Power -> intents.powerToggle() OR intents.powerOn()/powerOff() if toggle not present
     (If only discrete calls exist and state is known, choose based on current snapshot transport/power state if available;
      otherwise implement as PowerOn for now and add TODO.)

5) Rate limiting / debounce (minimal, but important)
   Add a simple per-action debounce in UiAdapter:
   - lastActionMs_[UiAction] and ignore repeats within 200ms.
   This prevents accidental double-taps causing duplicate intents.

6) Optional: publish a UI “toast” line (non-authoritative)
   If you want feedback on tap without optimistic state, you may add a small “last command” line:
   - “Sent: Play”
   This must be purely informational and not treated as state.

Acceptance criteria
- Compiles and runs on XIAO ESP32-C3.
- Buttons are visible and tappable.
- Clicking buttons results in Serial log lines indicating the intent was emitted (add logs in UiAdapter).
- When the real unit reports state changes, the UI state label updates accordingly.
- UI does not change state label just because a button was pressed.
- No SLink includes in UiApp.
- No scrubbing UI.

Notes / Guidance
- Prefer LVGL v9 active screen API (lv_screen_active()).
- Use lv_obj_set_size / lv_obj_align for layout. Keep it simple.
- Avoid creating/destroying objects in loop; build once in UiApp::init().

Output required from Codex
- Full contents of modified files (UiApp.h/.cpp, UiAdapter.h/.cpp, updated .ino)
- New MILESTONE_D.md
- A short note listing the exact intent method names used (play/pause/stop/next/prev/power).

If intent method names differ
- Do not guess wildly. Search within the repo for SLinkCommandIntentSource methods and use those exact names.
- If there’s no “power” intent method yet, stub Power button to log “TODO: power intent not implemented” and do not call anything.

Implementation notes
- UiApp now builds a control row plus a small Power button and emits UiAction callbacks on LV_EVENT_CLICKED.
- UiAdapter debounces actions (200ms) and logs each intent to Serial.
- Next/Prev map to changeTrack using the current snapshot track; ignored if track is unknown/out of range.
- Power toggles between powerOff/powerOn using a local assumption until real power state exists.
