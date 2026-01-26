# Serial console flow

The USB serial console uses the same intent + event wiring as the touch UI. It is a simple text parser for manual control and debugging.

## Pieces
- **SLinkCommandConsole** (`io/SLinkCommandConsole`): Reads lines, normalizes input, parses commands, emits intents via `SLinkCommandIntentSource`, optionally sends raw bytes.
- **ConsoleAdapter** (`io/ConsoleAdapter`): Attaches the pretty printer as an event observer so incoming unit events are logged; provides status polling.
- **SLinkPrettyPrinter**: Logs events (`PLAY`, `DISC`, `STATUS`, etc.) with parsed fields.

## Command path (console → unit)
```
USB serial line
  -> SLinkCommandConsole::poll() (line buffer)
  -> command decoded:
       PLAY, STOP, PAUSE, POWER_ON, POWER_OFF,
       CHANGE_DISC <1-300>, CHANGE_TRACK <1-99>,
       GET_CURRENT_DISC, GET_STATUS,
       SEND <hex> (raw bytes, if raw TX provided)
  -> mapped to SLinkCommandIntentSource (queue/arbiter/tx gate)
  -> S-Link TX on the shared bus
```
Notes:
- `SEND` bypasses intents and writes bytes via `SLinkTx` when allowed; use for low-level tests.
- Unknown or out-of-range values are rejected with a console message.

### Commands & expected responses
- `PLAY` / `STOP` / `PAUSE` / `POWER_ON` / `POWER_OFF`  
  - Console prints `tx: <CMD>` then enqueues intent.  
  - Unit normally answers with corresponding `PLAY`/`STOP`/`PAUSE`/`STATUS` events that pretty-printer logs.
- `CHANGE_DISC <1-300>`  
  - Prints `tx: CHANGE_DISC <n>`, enqueues changeDisc intent (track defaults to 1).  
  - Unit should emit `DISC_*`/`STATUS` updates showing the new disc; logs appear via pretty-printer.
- `CHANGE_TRACK <1-99>`  
  - Prints `tx: CHANGE_TRACK <n>`, enqueues changeTrack intent (disc auto-resolved from state).  
  - Unit should emit `CHANGING_TRACK`/`STATUS` with new track; logs show track info.
- `GET_CURRENT_DISC`  
  - Prints `tx: GET_CURRENT_DISC`, enqueues status query across both banks.  
  - Expect `CURRENT_DISC_INFO` (possibly bank A then bank B) pretty-printed when unit replies.
- `GET_STATUS`  
  - Prints `tx: GET_STATUS`, enqueues a status poll.  
  - Expect a `STATUS` line with transport/disc/track.
- `SEND <hex bytes>` (e.g., `SEND 902E`)  
  - Prints `tx: SEND <bytes>` and writes raw bytes via `SLinkTx` (no intent queue).  
  - No automatic response; any unit reply is logged normally if it matches known patterns.

## Event path (unit → console)
```
Unit frames -> translator -> UnitEventPublisher -> EventBus
      -> SLinkPrettyPrinter (attached by ConsoleAdapter)
      -> human-readable lines on USB serial
```

## Typical session
```
> PLAY
tx: PLAY
rx PLAY (pretty-printed)
rx STATUS disc=05 track=01 transport=Playing
```
