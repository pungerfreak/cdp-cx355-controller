# Shared-Line CD Changer Interface – System Design Reference

## Overview
This document describes the system architecture, collaboration model, and domain language for a microcontroller-based interface that sits on a **shared TX/RX line** between a CD changer unit and one or more control interfaces.  
The unit may transmit asynchronously at any time; control interfaces propose actions that are mediated and transmitted opportunistically.

The first target control interface is a **touchscreen with a custom UI**.

---

## Design Principles

1. **RX always has priority over TX**
2. **The unit is authoritative** – it may speak at any time
3. **Only the protocol engine drives the bus**
4. **Interfaces never deal with timing, symbols, or frames**
5. **TX is opportunistic and gated by bus state**
6. **All UI and automation input is expressed as intents**
7. **State is derived from events, never inferred from UI actions**

---

## Bus Model

### Characteristics
- Single shared signal line for TX and RX
- Open-collector / open-drain–style behavior assumed
- The interface must:
  - Always listen
  - Transmit only during safe idle windows
  - Abort immediately if RX activity is detected

### Bus States
- `Idle` – no edges for a defined idle gap
- `RxActive` – receiving symbols/frames
- `TxActive` – transmitting a scheduled waveform
- `Recovering` – collision/backoff period

### Timing Concepts
- **Idle Gap**: minimum quiet time to consider the bus idle
- **Guard Time**: quiet time required before TX is allowed
- **Backoff**: randomized delay after abort or collision

---

## Layered Architecture

### 1. Hardware / ISR Layer
**Purpose:** Capture edges and drive TX with deterministic timing

- `RxCapture` (ISR)
  - Timestamp edges
  - Push `EdgeEvent` into a lock-free ring buffer
- `TxScheduler`
  - Executes a precomputed waveform or symbol schedule
- `Clock`
  - Monotonic microsecond-resolution timebase

> ISR rule: record → enqueue → exit

---

### 2. Protocol Engine
**Purpose:** Convert electrical signaling into semantic meaning and back

RX path:
```
EdgeEvent → Symbol → FrameRx → UnitEvent
```

TX path:
```
UnitCommand → FrameTx → SymbolSchedule → TxScheduler
```

Components:
- `SymbolDecoder`
- `FrameAssembler`
- `FrameParser`
- `FrameBuilder`
- `ProtocolStateMachine`
- `TxGate` (bus state + timing authority)

The protocol engine alone decides *when* TX is permitted.

---

### 3. Mediation / Routing Layer
**Purpose:** Coordinate multiple control interfaces safely

- `EventBus` – publishes `UnitEvent`
- `IntentQueue` – receives `CommandIntent`
- `Arbiter` – selects the next command candidate
- `TransactionManager` (optional)
  - Correlates command/response pairs

The arbiter decides *what* should be sent; the protocol engine decides *when*.

---

### 4. Control Interfaces
**Purpose:** Observe unit state and propose user intent

Each interface:
- Subscribes to `UnitEvent`
- Emits `CommandIntent`
- Never touches protocol details

Initial interface:
- **Touchscreen UI**

---

## Touchscreen Interface Model

### Internal Structure
- `UnitStateStore` – authoritative state derived from events
- `UiViewModel` – formatted/derived UI data
- `TouchscreenInterface`
  - Updates state on `UnitEvent`
  - Emits intents based on gestures

### UX Rules
- UI is responsive even if TX is busy
- Intents may be queued, deduped, or expired
- UI never assumes a command succeeded until confirmed by events

---

## Domain Language

### Core Nouns
- **Bus**
- **EdgeEvent**
- **Symbol**
- **FrameRx / FrameTx**
- **UnitEvent**
- **CommandIntent**
- **UnitCommand**
- **TxWindow**
- **Focus**
- **Backoff**

### Core Verbs
- observe
- propose
- accept / reject
- gate
- abort
- retry
- dedupe
- throttle

### Invariants
1. Unit events are immutable history
2. State is always derived
3. Interfaces only deal in intents and events
4. RX preempts TX under all conditions

---

## Intent Taxonomy (UI → System)

- `Intent.Transport.Play`
- `Intent.Transport.Pause`
- `Intent.Transport.Stop`
- `Intent.Transport.NextTrack`
- `Intent.Transport.PrevTrack`
- `Intent.Disc.Select(disc)`
- `Intent.Track.Select(track)`
- `Intent.Seek.Fwd`
- `Intent.Seek.Rev`
- `Intent.Query.Refresh`

Intent metadata:
- `source`
- `urgency`
- `dedupe_key`
- `expires_at`

---

## UnitEvent Taxonomy (Unit → UI)

- `UnitEvent.TransportStateChanged`
- `UnitEvent.DiscChanged`
- `UnitEvent.TrackChanged`
- `UnitEvent.TimeUpdated`
- `UnitEvent.Error`
- `UnitEvent.FrameRx` (debug)

---

## Arbitration Policies (Touchscreen-Focused)

- Deduplicate transport intents within 250 ms
- Throttle seek intents to a fixed rate
- Expire stale intents from press-and-hold gestures
- Priority example:
  1. Stop
  2. Transport toggles
  3. Disc/track selection
  4. Queries

---

## Shared-Line TX Rules

- TX allowed only when:
  - Bus is idle for `idle_gap`
  - Protocol state allows transmission
- Abort immediately on unexpected RX edge
- Enter backoff after abort
- Always return the line to Hi-Z when idle

---

## Example DSL (Text)

```
bus UnitBus shared_line

unit CDChanger on UnitBus emits UnitEvent.*

interface Touchscreen
  observes UnitEvent.*
  proposes Intent.Transport.*, Intent.Disc.Select, Intent.Seek.*

policy:
  focus = Touchscreen
  dedupe Intent.Transport.* within 250ms
  throttle Intent.Seek.* to 8/s
  expire Intent.Seek.* after 300ms

tx:
  allowed_when BusIdle for 2.5ms
  abort_on_rx_edge = true
  backoff = random(4..12ms)
```

---

## Minimal Viable System Flow

1. ISR captures edges
2. Main loop decodes symbols and frames
3. Unit events published
4. Touchscreen proposes intents
5. Arbiter selects next intent
6. Protocol engine gates and transmits
7. RX always preempts TX

---

## Final Notes
This design deliberately separates:
- **Timing-critical logic**
- **Protocol semantics**
- **User interaction**
- **Policy and arbitration**

That separation is what allows the system to scale from a single touchscreen to multiple independent control interfaces without destabilizing the shared bus.
