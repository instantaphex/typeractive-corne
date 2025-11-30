#include <lvgl.h>

LV_IMG_DECLARE(akuma_img);

lv_obj_t *zmk_display_status_screen(void) {
    lv_obj_t *screen = lv_obj_create(NULL);

    lv_obj_t *img = lv_img_create(screen);
    lv_img_set_src(img, &akuma_img);
    lv_obj_center(img);

    return screen;
}

