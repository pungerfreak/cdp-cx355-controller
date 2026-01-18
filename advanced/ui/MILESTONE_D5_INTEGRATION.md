# Milestone D.5 Integration

- UiApp and UiAdapter moved from `advanced/ui_scratch/round_lvgl_v9_smoke/` into `advanced/ui/`.
- `advanced/advanced.ino` is now the canonical sketch and wires LVGL init, display/touch init, and UiAdapter/SLinkSystem.
- `ui_scratch` is no longer required for build/run; symlink folders and `SLinkSources.cpp` were removed.
- Power state management remains pinned for a later milestone.
