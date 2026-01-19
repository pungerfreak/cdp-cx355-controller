Milestone C: Bind UI to SLinkSystem via the new "bus + snapshot" seam (UiAdapter)

Status
- Milestone A + B complete.
- Files are under: advanced/ui_scratch/round_lvgl_v9_smoke/
- Milestone docs renamed to include milestone letter (e.g., MILESTONE_A.md, MILESTONE_B.md).
- The design tweak is implemented in the main codebase:
  SLinkSystem exposes:
    - addUnitObserver(SLinkUnitEventObserver&)
    - (optional) removeUnitObserver(SLinkUnitEventObserver&)
    - getUnitStateSnapshot(SLinkDiscInfo&, SLinkTrackInfo&) const
  and UI input port remains:
    - SLinkSystem::intentSource() -> SLinkCommandIntentSource

Goal
Introduce a UiAdapter module that:
- Subscribes to SLinkSystem unit events using addUnitObserver()
- Pulls snapshots using getUnitStateSnapshot()
- Converts snapshot to UiNowPlayingSnapshot
- Calls UiApp.render(...) when state changes
- (Optional for this milestone) wires 1-2 basic UI actions to intents (can be deferred to Milestone D)

Critical Rule: Clean dependency direction
- UiApp remains UI-only and MUST NOT include SLink headers.
- UiAdapter is the ONLY code in this folder allowed to include SLink headers.
- round_lvgl_v9_smoke.ino wires SLinkSystem + UiAdapter + UiApp together.

Allowed edits
- You may modify/add files ONLY under:
  advanced/ui_scratch/round_lvgl_v9_smoke/
- You may include headers from the existing advanced/* system (SLink) codebase, but do not modify those files in this milestone.

Files to add
1) advanced/ui_scratch/round_lvgl_v9_smoke/UiAdapter.h
2) advanced/ui_scratch/round_lvgl_v9_smoke/UiAdapter.cpp

Required API (implement exactly)

UiAdapter.h:
- Forward declare UiApp and UiNowPlayingSnapshot (from UiApp.h).
- Include only what you need from SLink headers.

Class UiAdapter final : public SLinkUnitEventObserver
public:
  UiAdapter(SLinkSystem& system, UiApp& app);

  // Call after LVGL + display init and after SLinkSystem is constructed/started (if applicable)
  void start();

  // Optional but recommended if removeUnitObserver exists
  void stop();

  // SLinkUnitEventObserver
  void onUnitEvent(const SLinkUnitEvent& e) override;

private:
  void refreshFromSnapshot_();   // pulls snapshot, converts, calls app.render()

  SLinkSystem& system_;
  UiApp&       app_;

  // Cached SLink snapshot types
  SLinkDiscInfo disc_{};
  SLinkTrackInfo track_{};

  // Cached UI snapshot
  UiNowPlayingSnapshot ui_{};
  bool hasUi_ = false;

  // Throttle refresh to avoid spamming LVGL if events burst
  uint32_t lastRefreshMs_ = 0;
  static constexpr uint32_t MIN_REFRESH_MS = 50;  // keep modest

Conversion rules (SLink -> UiNowPlayingSnapshot)
- disc number -> ui_.disc
- track number -> ui_.track
- elapsed time:
  - If SLinkTrackInfo exposes elapsed seconds directly, use it.
  - If only mm:ss or frames exist, convert to seconds best-effort.
  - If not available, keep existing simulated counter removed; set 0.
- title/artist/album:
  - If available in your current SLink state structs, map them.
  - If not available yet, set to nullptr or "-" placeholders.
- playing:
  - Map from unit transport state if available; otherwise false.

Important: Do NOT add new parsing logic in UiAdapter.
UiAdapter must only use existing "state store snapshot" info from SLinkSystem.
If some fields are not available, leave them blank.

Update round_lvgl_v9_smoke.ino
- Remove the milestone B "simulated snapshot" loop updates.
- Keep LVGL init and touch logging.
- Construct and start the SLink system (minimal required wiring):
  - Include "advanced/system/SLinkSystem.h" (or appropriate relative include) and create SLinkSystem instance.
  - Call any required begin/init methods if the system expects it (use existing patterns from the repo).
  - Obtain the intent source only if needed later; for now, event-driven updates are enough.
- Construct UiApp app; app.init();
- Construct UiAdapter adapter(system, app); adapter.start();
- In loop:
  - lv_timer_handler() frequently
  - keep touch logging
  - no simulated snapshot updates
  - optional tiny delay(1)

Event handling behavior
- start():
  - system_.addUnitObserver(*this);
  - system_.getUnitStateSnapshot(disc_, track_);
  - refreshFromSnapshot_() once to prime the UI
- onUnitEvent(...):
  - If the event type indicates a state change (DiscChanged, TrackChanged, TransportStateChanged, etc.),
    call refreshFromSnapshot_()
  - If event type is Unknown, ignore OR refresh at most once per MIN_REFRESH_MS (choose conservative).
- refreshFromSnapshot_():
  - pull snapshot via system_.getUnitStateSnapshot(disc_, track_)
  - convert to UiNowPlayingSnapshot ui_
  - app_.render(ui_)

Acceptance Criteria
- Compiles for XIAO ESP32-C3.
- UiApp still has zero SLink includes.
- UiAdapter is the only place with SLink includes in this folder.
- UI updates occur from real SLink snapshot + unit events (no simulation).
- Touch logging still works.
- If the unit is connected and producing events, you see disc/track updates reflected.
- Runs stable 60+ seconds.

Output requested from Codex
- Full contents of:
  - UiAdapter.h
  - UiAdapter.cpp
  - Updated round_lvgl_v9_smoke.ino
- Note any SLink include paths used and any fields that were not available for mapping.

If compilation fails due to include paths
- Prefer using relative includes consistent with how the advanced/ code is built in the repo.
- Do not change SLink code; adjust includes in the scratch sketch folder.
