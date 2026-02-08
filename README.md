# S-Link Interface (XIAO Round Screen)

Small Arduino sketch that bridges a Seeed XIAO ESP32-C3 + round display to a CD changer over the S-Link bus. The UI is snapshot-driven (no optimistic updates); all user actions become intents that are arbitrated and sent only when the shared line is idle.

- Entry point: `advanced/advanced.ino` (LVGL UI + S-Link system wiring)
- Core architecture: intents → queue/arbiter → command sender/tx gate; events → state store → UI/console
- Bus safety: RX preempts TX; tx gate enforces idle gap, guard time, and backoff on collisions

Docs:
- S-Link pipeline walkthrough with ASCII diagrams: `advanced/docs/SLink_Pipeline.md`
- UI snapshot-driven flow (UiApp/UiAdapter/screens): `advanced/docs/UI_Snapshot_Flow.md`
- Serial console flow and commands: `advanced/docs/Console_Flow.md`

## Fonts (LVGL)

How to add a new font without linker conflicts:

1) Generate the font with the LVGL font converter (https://lvgl.io/tools/fontconverter). Export as C array (`.c`), **4 bits per pixel (4 BPP)**, keep the filename stable (e.g., `open_sans_18.c`, `open_sans_18_bold.c`).

2) Compile each font exactly once:
   - Preferred: create a tiny wrapper C file in `advanced/`:
     - `advanced/open_sans_18_wrapper.c` → `#include "../fonts/open_sans_18.c"`
     - `advanced/open_sans_18_bold_wrapper.c` → `#include "../fonts/open_sans_18_bold.c"`
   - Add these wrappers to the sketch so Arduino builds them once. Do **not** `#include` the font `.c` files inside multiple C++ sources.

3) In UI code, only declare the symbols:
```cpp
extern const lv_font_t open_sans_18;
extern const lv_font_t open_sans_18_bold;
// ...
lv_obj_set_style_text_font(label, &open_sans_18, 0);
```

This avoids duplicate symbols and C++ designator-order errors while keeping fonts accessible everywhere.

## Exporting Images for LVGL

This project uses **LVGL-converted bitmap images** for UI icons and graphics.
SVG is used only as a **design-time format**, not at runtime.

---

### 1. Design icons (SVG source)

- Create icons in **SVG** using Figma, Illustrator, Inkscape, etc.
- Keep designs **simple and flat**:
  - Solid fills only
  - No gradients, shadows, filters, masks, or effects
- Prefer **monochrome or duotone** icons
- Design icons as **square** assets unless a specific shape is required

SVG files are treated as source assets for clean, scalable design.

---

### 2. Export SVG → PNG

- Export each icon to **PNG** at the **exact pixel size used in the UI**
- Export at **1× scale** (no runtime scaling)
- Use a **transparent background** if the icon overlays a button or panel

Typical sizes:
- Button icons: 64–96 px square
- Small indicators: 24–32 px

---

### 3. Convert PNG → LVGL image (.c)

Use the official LVGL Image Converter:

https://lvgl.io/tools/imageconverter

Recommended settings:
- Output format: **C array**
- Color format:
  - **ARGB1555** for icons with transparency (recommended)
  - **RGB565** for fully opaque images
- Enable alpha only if transparency is required
- Do not scale during conversion

The converter outputs a `.c` file containing an `lv_img_dsc_t` that can be compiled directly into the firmware.
