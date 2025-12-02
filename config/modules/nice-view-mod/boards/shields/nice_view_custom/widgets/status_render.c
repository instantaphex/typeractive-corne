#include <lvgl.h>

#include "status_render.h"

#include <stdio.h>
#include <string.h>

LV_IMG_DECLARE(bolt);

static inline lv_color_t fg_color(void) {
#if CONFIG_NICE_VIEW_WIDGET_INVERTED
    return lv_color_white();
#else
    return lv_color_black();
#endif
}

static inline lv_color_t bg_color(void) {
#if CONFIG_NICE_VIEW_WIDGET_INVERTED
    return lv_color_black();
#else
    return lv_color_white();
#endif
}

// init label
static void init_label(lv_draw_label_dsc_t *dsc, const lv_font_t *font, lv_text_align_t align,
                       lv_color_t color) {
    lv_draw_label_dsc_init(dsc);
    dsc->color = color;
    dsc->font = font;
    dsc->align = align;
}

static void init_rect(lv_draw_rect_dsc_t *dsc, lv_color_t bg) {
    lv_draw_rect_dsc_init(dsc);
    dsc->bg_color = bg;
    dsc->border_color = bg;
    dsc->bg_opa = LV_OPA_COVER;
    dsc->border_opa = LV_OPA_COVER;
}

static void init_line(lv_draw_line_dsc_t *dsc, lv_color_t color, uint8_t width) {
    lv_draw_line_dsc_init(dsc);
    dsc->color = color;
    dsc->width = width;
}

static void init_arc(lv_draw_arc_dsc_t *dsc, lv_color_t color, uint8_t width) {
    lv_draw_arc_dsc_init(dsc);
    dsc->color = color;
    dsc->width = width;
}

static void draw_battery_icon(lv_obj_t *canvas, const status_render_state_t *state) {
    lv_draw_rect_dsc_t rect_bg, rect_fg;
    init_rect(&rect_bg, bg_color());
    init_rect(&rect_fg, fg_color());

    lv_canvas_draw_rect(canvas, 0, 2, 29, 12, &rect_fg);
    lv_canvas_draw_rect(canvas, 1, 3, 27, 10, &rect_bg);

    int width = (state->battery + 2) / 4;
    if (width < 0) {
        width = 0;
    } else if (width > 27) {
        width = 27;
    }
    lv_canvas_draw_rect(canvas, 2, 4, width, 8, &rect_fg);

    lv_canvas_draw_rect(canvas, 30, 5, 3, 6, &rect_fg);
    lv_canvas_draw_rect(canvas, 31, 6, 1, 4, &rect_bg);

    if (state->charging) {
        lv_draw_img_dsc_t img_dsc;
        lv_draw_img_dsc_init(&img_dsc);
        lv_canvas_draw_img(canvas, 9, -1, &bolt, &img_dsc);
    }
}

void draw_top_render(lv_obj_t *canvas, const status_render_state_t *state) {
    lv_draw_rect_dsc_t rect_bg, rect_fg;
    init_rect(&rect_bg, bg_color());
    init_rect(&rect_fg, fg_color());

    lv_canvas_draw_rect(canvas, 0, 0, 68, 68, &rect_bg);

    draw_battery_icon(canvas, state);

    lv_draw_label_dsc_t label_dsc;
    init_label(&label_dsc, &lv_font_montserrat_16, LV_TEXT_ALIGN_RIGHT, fg_color());
    char output_text[10] = {};
    if (state->is_usb) {
        strcat(output_text, LV_SYMBOL_USB);
    } else if (!state->bt_bonded) {
        strcat(output_text, LV_SYMBOL_SETTINGS);
    } else if (state->bt_connected) {
        strcat(output_text, LV_SYMBOL_WIFI);
    } else {
        strcat(output_text, LV_SYMBOL_CLOSE);
    }
    lv_canvas_draw_text(canvas, 0, 0, 68, &label_dsc, output_text);

    lv_draw_label_dsc_t wpm_label_dsc;
    init_label(&wpm_label_dsc, &lv_font_unscii_8, LV_TEXT_ALIGN_RIGHT, fg_color());
    lv_canvas_draw_rect(canvas, 0, 21, 68, 42, &rect_fg);
    lv_canvas_draw_rect(canvas, 1, 22, 66, 40, &rect_bg);

    char wpm_text[6] = {};
    snprintf(wpm_text, sizeof(wpm_text), "%u", (unsigned)state->wpm_hist[9]);
    lv_canvas_draw_text(canvas, 42, 52, 24, &wpm_label_dsc, wpm_text);

    int max = 0;
    int min = 256;
    for (int i = 0; i < 10; i++) {
        if (state->wpm_hist[i] > max) {
            max = state->wpm_hist[i];
        }
        if (state->wpm_hist[i] < min) {
            min = state->wpm_hist[i];
        }
    }
    int range = max - min;
    if (range == 0) {
        range = 1;
    }

    lv_draw_line_dsc_t line_dsc;
    init_line(&line_dsc, fg_color(), 1);

    lv_point_t points[10];
    for (int i = 0; i < 10; i++) {
        points[i].x = 2 + i * 7;
        points[i].y = 60 - (state->wpm_hist[i] - min) * 36 / range;
    }
    lv_canvas_draw_line(canvas, points, 10, &line_dsc);
}

void draw_middle_render(lv_obj_t *canvas, const status_render_state_t *state) {
    lv_draw_rect_dsc_t rect_bg;
    init_rect(&rect_bg, bg_color());
    lv_canvas_draw_rect(canvas, 0, 0, 68, 68, &rect_bg);

    lv_draw_arc_dsc_t arc_dsc;
    init_arc(&arc_dsc, fg_color(), 2);

    lv_draw_arc_dsc_t arc_dsc_filled;
    init_arc(&arc_dsc_filled, fg_color(), 9);

    lv_draw_label_dsc_t label_dsc;
    init_label(&label_dsc, &lv_font_montserrat_18, LV_TEXT_ALIGN_CENTER, fg_color());
    lv_draw_label_dsc_t label_dsc_bg;
    init_label(&label_dsc_bg, &lv_font_montserrat_18, LV_TEXT_ALIGN_CENTER, bg_color());

    const int circle_offsets[5][2] = {
        {13, 13}, {55, 13}, {34, 34}, {13, 55}, {55, 55},
    };

    int selected = state->bt_profile - 1;
    if (selected < 0) {
        selected = 0;
    } else if (selected > 4) {
        selected = 4;
    }

    for (int i = 0; i < 5; i++) {
        bool is_selected = i == selected;
        lv_canvas_draw_arc(canvas, circle_offsets[i][0], circle_offsets[i][1], 13, 0, 360,
                           &arc_dsc);

        if (is_selected) {
            lv_canvas_draw_arc(canvas, circle_offsets[i][0], circle_offsets[i][1], 9, 0, 359,
                               &arc_dsc_filled);
        }

        char label[2];
        snprintf(label, sizeof(label), "%d", i + 1);
        lv_canvas_draw_text(canvas, circle_offsets[i][0] - 8, circle_offsets[i][1] - 10, 16,
                            is_selected ? &label_dsc_bg : &label_dsc, label);
    }
}

void draw_bottom_render(lv_obj_t *canvas, const status_render_state_t *state) {
    lv_draw_rect_dsc_t rect_bg;
    init_rect(&rect_bg, bg_color());
    lv_canvas_draw_rect(canvas, 0, 0, 68, 68, &rect_bg);

    lv_draw_label_dsc_t label_dsc;
    init_label(&label_dsc, &lv_font_montserrat_14, LV_TEXT_ALIGN_CENTER, fg_color());

    if (state->layer_label == NULL) {
        char text[16] = {};
        snprintf(text, sizeof(text), "LAYER %u", (unsigned)state->layer);
        lv_canvas_draw_text(canvas, 0, 5, 68, &label_dsc, text);
    } else {
        lv_canvas_draw_text(canvas, 0, 5, 68, &label_dsc, state->layer_label);
    }
}
