Milestone S (redo): Rework UI layout to fixed positioning (eliminate flex-driven drift/disappearance)

Context
- Hardware: XIAO ESP32-C3 + Seeed Round Display, LVGL v9.3.0
- We have observed UI glitches: control row sliding off-screen, disappearing while the rest of UI remains.
- Root cause identified: flex/auto layout reacting to dynamic label size changes (wrapping/content changes),
  which reflows and moves the button container.
- Fix strategy: move UI layout to explicit, fixed positioning so controls are not impacted by other elements.

Goal
Make the UI geometry stable by:
- disabling flex (and other auto layouts) at the root
- using explicit sizes and positions for labels and the controls container
- preventing any layout recalculation from moving the control row

Non-goals
- Do NOT change intent wiring, debounce, or snapshot mapping semantics.
- Do NOT add disc scrubbing, keypad, or elapsed time.
- Do NOT change transport/state store logic.
- Do NOT add more instrumentation (keep existing logs if present, but do not expand).

Output constraints
- Do NOT print entire file contents in the final response.
- Provide diff-style summaries only: list changed files + key edits + a few small snippets.

Scope / Files allowed to change
- advanced/ui/UiApp.h (only if needed)
- advanced/ui/UiApp.cpp (primary)
Optional, only if required to support stable layout:
- advanced/advanced.ino (avoid unless absolutely necessary)
No other files should be touched.

Implementation requirements (layout)

1) Root container
In UiApp::init():
- Create or obtain a dedicated root container (preferred) instead of placing objects directly on the screen.
- Set root:
  - size = full screen (240x240 or LV_HOR_RES / LV_VER_RES)
  - layout = LV_LAYOUT_NONE (no flex/grid)
  - clear padding to 0 on all sides
  - clear scroll flags:
      lv_obj_clear_flag(root, LV_OBJ_FLAG_SCROLLABLE);

2) Explicit object positioning (fixed geometry)
- Every label and every control container must have explicit:
  - lv_obj_set_size(obj, w, h)
  - lv_obj_set_pos(obj, x, y)
- Do NOT use:
  - lv_obj_set_flex_flow
  - lv_obj_set_flex_align
  - lv_obj_align_to (unless aligning to root with fixed offsets and it cannot be impacted by dynamic size)
  - LV_SIZE_CONTENT on any container that could affect controls

3) Recommended geometry (adjust if needed, but keep fixed)
Assume 240x240.
- Header label (disc/track): at y=8, height ~22, width 240-16, x=8
- Time label: at y=32, height ~18, width 240-16, x=8
- Metadata label: at y=54, height ~100 (fixed), width 240-16, x=8
  - Set label long mode to clip/dots to avoid height changes:
      lv_label_set_long_mode(meta, LV_LABEL_LONG_DOT);
  - Ensure line breaks don’t change object height (fixed height wins).
- State label (“PLAYING/PAUSED/STOPPED”): at y=158, height ~18, width 240-16, x=8
- Controls container: fixed at bottom:
  - x=8, y=182, w=224, h=50 (or similar)
  - Inside this container, button positions are also explicit OR use a local, stable row layout that cannot
    be affected by external elements. Preferred: explicit positions for each button.
- Power button:
  - position explicitly (e.g., top-right corner x=190,y=8,w=42,h=32) OR near controls area
  - must not affect the control row geometry

4) No geometry changes during render
In UiApp::render():
- Update only label text / state.
- Do NOT call set_pos/set_size/align/layout functions.
- Do NOT recreate controls or containers.

5) Visual safety
- Ensure all controls remain within 0..239 bounds (no negative positions).
- Ensure touchable sizes are reasonable (>= 40px in at least one dimension for main controls if possible).

Validation checklist (acceptance criteria)
- Build succeeds.
- Run for 5+ minutes with frequent snapshot updates (disc/track/transport changes).
- The button row never slides, jitters, or disappears.
- The rest of the screen remains functional.
- Intent wiring still works (buttons emit intents) without any changes required in UiAdapter.
- No new layout code is added outside UiApp::init().

What to deliver (no full files)
- Diff-style summary of changes to UiApp.cpp (and UiApp.h only if touched).
- Include small snippets showing:
  - root container setup (LV_LAYOUT_NONE, padding/scroll cleared)
  - explicit sizing/positioning calls for labels + controls container
  - meta label long mode set to DOT/CLIP

Notes
- The goal is stability, not aesthetics. Keep styling minimal.
- If existing code already partially moved to fixed positioning, complete it consistently across all UI objects.
