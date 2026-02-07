# S-Link pipeline (plain-language guide)

This sketch sits between a CD changer and control surfaces (touch UI + serial console) on a **single shared line**. RX always wins; TX happens only when the bus is idle and safe. Everything the UI/console wants to do is expressed as **intents**; everything the unit tells us arrives as **events**.

## High-level picture
```
        CD changer bus (shared RX/TX line)
                     |
           RX edges captured in ISR
                     v
  Edge buffer -> Symbol decoder -> Frame assembler -> Translator
                     |                                |
                     |                        Debug info (optional)
                     v
         UnitEvent publisher -> Event bus -> Observers
                     |                |
           State store (authoritative) |--> Pretty printer (serial logs)
                     |                |
                 UI adapter renders    |
                                       ^
                                       |
 UI taps / serial commands -> Intents -> Queue -> Arbiter -> Processor
                                              v
                                     Command sender -> Frame builder
                                              v
                                    TX gate (idle/guard/backoff)
                                              v
                                         TX on bus
```

## Key roles (plain language)
- **SLinkRx + ISR helpers (`SLinkEdgeCapture`, `SLinkSymbolDecoder`, `SLinkFrameAssembler`)**: Timestamp falling edges, turn them into symbols and frames without blocking the main loop.
- **SLinkTranslator**: Names a frame (PLAY/STOP/STATUS/…) and extracts disc/track/state fields.
- **SLinkUnitEventPublisher + SLinkUnitEventBus**: Turn decoded frames into strongly typed events; deliver them to all observers.
- **SLinkUnitStateStore**: Keeps the authoritative disc/track/transport view derived only from events.
- **UiAdapter / ConsoleAdapter**: Listen to events, render UI/logs, and turn user actions into intents.
- **SLinkIntentQueueAdapter + IntentQueue/Arbiter/Processor**: Buffer intents, prioritize/throttle them, and dispatch when allowed.
- **SLinkUnitCommandSender + SLinkFrameBuilder**: Convert an intent into a specific S-Link frame (play/stop/change disc/…).
- **SLinkTxGate + SLinkTx**: Enforce bus-safety: only send after idle/guard windows; abort and back off if RX activity appears mid-send.
- **SLinkFrameCallbacks**: Bind RX/TX callbacks to the translator, event publisher, and debug printer; used by `SLinkSystem` wiring.

## Receive path (unit → UI/console)
```
[Bus edges] -> ISR (SLinkRx::onEdgeISR) -> Edge ring buffer
      main loop: SLinkRx::poll()
             -> SymbolDecoder (bit/sync classification)
             -> FrameAssembler (byte stream)
             -> SLinkTranslator (names + parses fields)
             -> SLinkUnitEventPublisher
             -> EventBus fans out to:
                  - StateStore (authoritative state)
                  - UiAdapter (renders snapshot)
                  - PrettyPrinter (serial debug)
                  - CommandSenderStateSync (keeps sender in sync with disc)
```
Notes:
- Frames are marked error and dropped if timing looks bad or byte boundaries are off.
- UI never trusts its own taps—state comes only from these events.

## Transmit path (UI/console → unit)
```
User tap / console command
    -> UiAdapter or SLinkCommandConsole
    -> SLinkIntentQueueAdapter (wraps intent queue)
    -> IntentQueue (FIFO)
    -> IntentArbiter (priority + per-type throttle + expiry)
    -> IntentProcessor (polls; waits if TX gate says "not yet")
    -> SLinkUnitCommandSender (fills command struct)
    -> SLinkFrameBuilder (bytes on the wire)
    -> SLinkTxGate (idle gap + guard time + backoff)
    -> SLinkTx (drives line low/high-Z with timing)
```
Safety:
- RX activity at any time aborts TX; the gate schedules a randomized backoff before retry.
- Throttle examples: transport intents ~250 ms, power intents ~500 ms, disc/track selection slower.
- Disc/track commands automatically resolve disc unit (bank A/B) and BCD encoding.

## Bus safety rules (why transmissions wait)
- The line is shared; we must **yield to RX**.
- `SLinkTxGate::canTransmit` checks:
  - protocol allows TX,
  - backoff window expired,
  - idle gap since last RX/TX,
  - guard time since last RX edge.
- During TX, `SLinkTxGate::shouldAbort` watches for RX edges; abort sets a backoff window.

## State & rendering loop
1) `SLinkUnitStateStore` maintains disc/track/transport based purely on events.
2) `UiAdapter::refreshFromSnapshot_` pulls the store snapshot, converts to `UiNowPlayingSnapshot`, and calls `UiApp::render`.
3) Labels and button text update only when the snapshot changes (e.g., Play/Pause toggle comes from transport state).

## Startup/boot handshake
1) `SLinkSystem::begin` arms a delayed `getStatus` intent.
2) First `Status` event may report `PowerOff`; UiAdapter seeds initial disc/track (if known) and sends `powerOn`.
3) Subsequent events bring the UI into sync without optimistic updates.

## Quick glossary
- **Intent**: “Please do X” (play, pause, change disc, change track, power on/off, get status).
- **Event**: “The unit says Y happened” (transport changed, disc/track changed, status snapshot).
- **Idle gap**: Quiet time required before we’re allowed to transmit.
- **Guard time**: Extra quiet time after any RX edge before TX may start.
- **Backoff**: Randomized wait after an aborted/collided TX.

## Minimal sequence sketch
```
[Tap Play] -> intent:play queued
Arbiter picks Play (passes throttle) -> Processor asks TxGate
TxGate sees idle -> Sender builds PLAY frame -> Tx drives bus
Unit replies (STATUS/PLAY) -> RX path translates -> EventBus
StateStore updates transport=Playing -> UiAdapter renders "PLAYING" + button text "Pause"
```
