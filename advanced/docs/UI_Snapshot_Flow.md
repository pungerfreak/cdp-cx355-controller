# UI snapshot flow (touch UI)

The LVGL UI is **snapshot-driven**: it only updates when unit events arrive and are converted into a UI snapshot. User taps become intents; the UI never changes state optimistically.

## Cast
- **UiApp**: owns screens, switches between Main and Disc keypad, forwards actions.
- **MainControllerScreen**: now-playing labels + transport/prev/next/power buttons.
- **DiscSelectorScreen**: keypad for disc entry (1–300).
- **UiAdapter**: bridges S-Link state/events to UI snapshots; turns UI actions into intents.
- **UiNowPlayingSnapshot**: disc, track, elapsed_sec, metadata, and transport state.

## Data flow (unit → UI)
```
Unit events -> UiAdapter::refreshFromSnapshot_
    -> pulls SLinkUnitStateStore snapshot
    -> builds UiNowPlayingSnapshot
    -> UiApp::render(snapshot)
        -> MainControllerScreen updates labels/button text
```
Rules:
- Labels change only when fields differ from the last snapshot.
- Play/Pause button text follows transport state from the unit, not taps.
- Disc/track label shows "-" when unknown.

## Action flow (UI → intents)
```
Tap in MainControllerScreen/DiscSelectorScreen
    -> UiApp onScreenActionThunk_
    -> UiAdapter::onUiAction_
    -> action mapped to SLinkCommandIntentSource:
         Play/Pause/Stop/Next/Prev/Power, ChangeDisc, ChangeTrack
    -> debounced per action (200 ms) before enqueue
```
Disc keypad specifics:
- Digits buffer locally (max 3), Clear/Backspace manage input.
- Go validates 1–300; on success, sends changeDisc intent and returns to now-playing.
- Invalid Go sets an error flag in the keypad; no intent sent.

## Screen switching
- `UiAction::OpenDiscKeypad` shows DiscSelectorScreen; renders begin from a clean keypad.
- `KeypadCancel` or successful Go returns to MainControllerScreen.

## Quick sequence examples
```
[Tap Play] -> UiAdapter emits play intent (debounced)
Unit responds with STATUS/PLAY -> snapshot transport=Playing
UiApp renders: state label "PLAYING", button text "Pause"
```
```
[Enter 123, Go] -> changeDisc intent enqueued
Unit signals DISC/STATUS -> snapshot disc=123, track set/cleared by unit
UiApp renders new disc/track labels; keypad hides
```
