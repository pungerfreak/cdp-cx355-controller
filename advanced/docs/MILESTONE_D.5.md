Milestone D.5: Integrate UI from ui_scratch into main advanced/ tree; update existing advanced/advanced.ino; remove symlinks + scratch compile hacks

Status
- Main Arduino entrypoint already exists: advanced/advanced.ino (this is canonical).
- Working UI implementation currently lives in:
  advanced/ui_scratch/round_lvgl_v9_smoke/
  and includes UiApp + UiAdapter + sketch wiring and uses symlink folders + SLinkSources.cpp to compile.

Goal
- Move UiApp + UiAdapter into the real codebase: advanced/ui/
- Update the existing main sketch: advanced/advanced.ino
  so it uses UiApp/UiAdapter (touch buttons emit intents; UI snapshot-driven).
- Remove the ui_scratch symlink approach and remove reliance on SLinkSources.cpp.
- Keep behavior the same as Milestone D (scrubbing out of scope; power state management pinned).

Rules
- Do not create a new main .ino.
- Only update advanced/advanced.ino.
- UiApp must remain free of SLink includes.
- UiAdapter may include SLink headers and use intent source + unit observer seam.
- Avoid refactors in transport/frame/etc. Focus on file placement + include path hygiene.

Files to add (copy from ui_scratch)
1) advanced/ui/UiApp.h
2) advanced/ui/UiApp.cpp
3) advanced/ui/UiAdapter.h
4) advanced/ui/UiAdapter.cpp

Files to modify
- advanced/advanced.ino (update wiring to use UiApp/UiAdapter)
- (Optional) advanced/ui/MILESTONE_D5_INTEGRATION.md (doc)
- Remove/stop using:
  - advanced/ui_scratch/round_lvgl_v9_smoke/SLinkSources.cpp
  - symlink directories under advanced/ui_scratch/round_lvgl_v9_smoke/

Step-by-step

Step 1: Copy UI modules into advanced/ui
- Copy these files verbatim as a starting point:
  from: advanced/ui_scratch/round_lvgl_v9_smoke/UiApp.h
        advanced/ui_scratch/round_lvgl_v9_smoke/UiApp.cpp
        advanced/ui_scratch/round_lvgl_v9_smoke/UiAdapter.h
        advanced/ui_scratch/round_lvgl_v9_smoke/UiAdapter.cpp
  to:   advanced/ui/UiApp.h
        advanced/ui/UiApp.cpp
        advanced/ui/UiAdapter.h
        advanced/ui/UiAdapter.cpp

- Then fix includes inside the copied files so they compile from their new location.
  UiApp: LVGL-only includes.
  UiAdapter: SLink includes using correct relative paths from advanced/ui/.

Step 2: Update advanced/advanced.ino to wire the UI
- In advanced/advanced.ino:
  - Include the display/touch headers you already use today (do not re-invent init).
  - Include the new UI headers:
      #include "ui/UiApp.h"
      #include "ui/UiAdapter.h"
  - Ensure SLinkSystem is constructed as it already is today in advanced.ino.
  - Add globals (or statics) for:
      UiApp app;
      UiAdapter* adapter (or static UiAdapter adapter(system, app); if construction order is safe)

  In setup():
  - keep existing Serial + LVGL init + display init as-is (unless your current advanced.ino does it differently).
  - call app.init()
  - construct adapter with (system, app)
  - call adapter.start()

  In loop():
  - keep existing system servicing (if advanced.ino already calls something to drive RX/TX / decode, keep it).
  - call lv_timer_handler() as appropriate for LVGL.
  - do not keep any ui_scratch touch logging code unless you want it (optional).

Important: advanced/advanced.ino should now be the running UI, not the scratch sketch.

Step 3: Remove scratch compile hacks
- Delete or at least ensure unused (not included by any build):
  - advanced/ui_scratch/round_lvgl_v9_smoke/SLinkSources.cpp
- Remove the symlink directories under:
  advanced/ui_scratch/round_lvgl_v9_smoke/
  (command, debug, frame, intent, io, system, transport, unit)
- The main build should not depend on ui_scratch at all.

Step 4: Verify build boundaries
- Confirm UiApp has no SLink includes.
- Confirm UiAdapter is the only UI module touching SLink.
- Confirm advanced/advanced.ino builds and links without referencing ui_scratch.

Step 5: Documentation
- Add advanced/ui/MILESTONE_D5_INTEGRATION.md describing:
  - what moved
  - that advanced/advanced.ino is the canonical sketch
  - ui_scratch is no longer required for build/run
  - power state management remains “pinned” for later

Acceptance criteria
- Build + run using advanced/advanced.ino with no ui_scratch dependencies.
- UI shows Now Playing + transport state.
- Buttons emit intents via SLinkCommandIntentSource with debounce.
- No scrubbing UI.
- ui_scratch symlinks removed and SLinkSources.cpp not required.

Output requested from Codex
- Full contents of modified/added files:
  - advanced/advanced.ino (updated)
  - advanced/ui/UiApp.h/.cpp
  - advanced/ui/UiAdapter.h/.cpp
  - advanced/ui/MILESTONE_D5_INTEGRATION.md (if added)
- Confirm ui_scratch hacks were removed or are unused.
