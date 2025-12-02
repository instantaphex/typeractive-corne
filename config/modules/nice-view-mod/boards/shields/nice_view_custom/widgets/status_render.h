/*
 * Rendering abstraction for the nice!view status widget.
 *
 * These helpers consume the lightweight status_render_state_t structure so they
 * can be reused outside of ZMK (e.g. in a desktop LVGL harness or another
 * backend altogether).
 */

#pragma once

#include <lvgl.h>
#include <stdbool.h>
#include <stdint.h>

typedef struct {
    uint8_t battery;
    bool charging;

    bool is_usb;
    uint8_t bt_profile;
    bool bt_bonded;
    bool bt_connected;

    uint8_t wpm_hist[10];

    uint8_t layer;
    const char *layer_label;
} status_render_state_t;

void draw_top_render(lv_obj_t *canvas, const status_render_state_t *state);
void draw_middle_render(lv_obj_t *canvas, const status_render_state_t *state);
void draw_bottom_render(lv_obj_t *canvas, const status_render_state_t *state);
