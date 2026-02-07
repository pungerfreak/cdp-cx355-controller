# Making LVGL image C files renderable (LVGL v9, C++)

Steps we applied to `power.c`, `disc.c`, `track.c`, `play.c`, `pause.c`, `stop.c`, and `prev_next.c` so they compile and render:

- Use positional struct initializers (not designated) so the files compile as C++.
- Set the `lv_image_header_t` fields in LVGL v9 order:
  - `{LV_IMAGE_HEADER_MAGIC, LV_COLOR_FORMAT_ARGB8888, 0, width_px, height_px, width_px * 4, 0}`
    - `0` after color format is the flags/reserved field.
    - `width_px * 4` is the stride in bytes for ARGB8888.
    - The final `0` is the reserved field.
- Full `lv_image_dsc_t` initializer pattern:
  - `{ header, data_size_bytes, pixel_map_pointer, NULL, NULL }`
  - `data_size_bytes` equals `width_px * height_px * 4` for ARGB8888.
  - Keep palette pointers `NULL` for true-color images.
- Ensure the symbol names are valid C identifiers (e.g., `prev_next`, not `prev-next`).
- Keep the pixel map (`*_map[]`) unchanged; only adjust the descriptor.

Follow this template for any new ARGB8888 image:

```c
const lv_image_dsc_t my_image = {
    {LV_IMAGE_HEADER_MAGIC, LV_COLOR_FORMAT_ARGB8888, 0, WIDTH, HEIGHT, WIDTH * 4, 0},
    WIDTH * HEIGHT * 4,
    my_image_map,
    NULL,
    NULL
};
```

Including `lvgl.h` (via the existing guard) is sufficient; no other code changes are required once the descriptor matches this structure.
