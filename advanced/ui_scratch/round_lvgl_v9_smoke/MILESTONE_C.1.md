Milestone C.1: Add transport state (play/paused/stopped) to the system snapshot; UI reflects it.
Elapsed time is OUT OF SCOPE.

Observed:
- disc/track update correctly via getUnitStateSnapshot()
- debug shows: transport=N/A (no transport info available yet)

Goal:
- SLinkSystem snapshot must include transport state (Stopped/Playing/Paused).
- UiAdapter maps it to UI (“PLAYING/PAUSED/STOPPED”).
- Do NOT implement elapsed time updates.

Files likely involved:
- advanced/unit/SLinkUnitEvents.h (+ .cpp if any)
- advanced/unit/SLinkUnitEventPublisher.* (where handler -> bus event happens)
- advanced/unit/SLinkUnitStateStore.* (authoritative state)
- advanced/system/SLinkSystem.* (snapshot exposure)
- advanced/ui_scratch/round_lvgl_v9_smoke/UiAdapter.* (mapping only)
- advanced/ui_scratch/round_lvgl_v9_smoke/UiApp.* (display text only)

Steps:
1) In SLinkUnitEvents.h:
   - Add enum class SLinkTransportState { Unknown, Unchanged, Stopped, Playing, Paused };
   - Extend SLinkUnitEvent payload so TransportStateChanged includes a SLinkTransportState value.

2) In SLinkUnitEventPublisher:
   - When the unit-level handler detects play/pause/stop state changes, publish
     SLinkUnitEventType::TransportStateChanged with the new SLinkTransportState.
   - If the handler callbacks for play/pause/stop do not exist, locate where you already detect
     disc/track changes and add the minimal detection for transport from the same decoded info.
   - Do not touch UI to “infer” transport.

3) In SLinkUnitStateStore:
   - Add a member SLinkTransportState transport_ = Unknown;
   - On receiving TransportStateChanged, update transport_.
   - Ensure getSnapshot/stateInfo returns transport_ (either by extending SLinkTrackInfo or adding a small transport field).

4) In SLinkSystem::getUnitStateSnapshot(...):
   - Ensure the transport state is included in the returned snapshot.

5) In UiAdapter:
   - Read transport from the snapshot and map to:
       - ui_.playing (true only when Playing)
     AND/OR a string state label if UiNowPlayingSnapshot supports it.
   - Keep elapsed time unchanged (“-” or 0). No ticking logic.

6) In UiApp:
   - Update the state label to show one of: PLAYING / PAUSED / STOPPED / UNKNOWN

Acceptance criteria:
- When the unit changes play/pause/stop, the UI updates the state label accordingly.
- Disc/track behavior remains correct.
- No elapsed time logic added.
