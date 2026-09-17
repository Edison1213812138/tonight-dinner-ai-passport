#include "dinner_ui.h"
#include "lvgl.h"
#include <assert.h>
#include <stdio.h>
static uint16_t frame[320][240], buffer[240 * 20];
static void flush(lv_display_t *display, const lv_area_t *area, uint8_t *map) {
    uint16_t *p = (uint16_t *)map;
    for (int y = area->y1; y <= area->y2; y++)
        for (int x = area->x1; x <= area->x2; x++)
            frame[y][x] = *p++;
    lv_display_flush_ready(display);
}
static void capture(const char *name, dinner_app_t *a) {
    dinner_ui_render(a);
    lv_tick_inc(20);
    lv_timer_handler();
    lv_refr_now(NULL);
    if (!name)
        return;
    char path[180];
    snprintf(path, sizeof(path), "%s.ppm", name);
    FILE *f = fopen(path, "wb");
    assert(f);
    fprintf(f, "P6\n240 320\n255\n");
    for (int y = 0; y < 320; y++)
        for (int x = 0; x < 240; x++) {
            uint16_t p = frame[y][x];
            uint8_t rgb[] = {((p >> 11) & 31) * 255 / 31, ((p >> 5) & 63) * 255 / 63,
                             (p & 31) * 255 / 31};
            fwrite(rgb, 1, 3, f);
        }
    fclose(f);
}
static unsigned objects(lv_obj_t *o) {
    unsigned n = 1;
    for (unsigned i = 0; i < lv_obj_get_child_count(o); i++)
        n += objects(lv_obj_get_child(o, i));
    return n;
}
int main(void) {
    lv_init();
    lv_display_t *display = lv_display_create(240, 320);
    lv_display_set_color_format(display, LV_COLOR_FORMAT_RGB565);
    lv_display_set_buffers(display, buffer, NULL, sizeof(buffer), LV_DISPLAY_RENDER_MODE_PARTIAL);
    lv_display_set_flush_cb(display, flush);
    dinner_ui_create();
    assert(dinner_ui_fonts_valid());
    dinner_ui_battery(86);
    dinner_app_t a;
    dinner_init(&a, 41);
    capture("01-home", &a);
    dinner_key(&a, KEY_OK, 200);
    dinner_tick(&a, 800);
    capture("02-draw", &a);
    dinner_tick(&a, 2400);
    a.result = 0;
    capture("03-result", &a);
    a.state = SETTINGS;
    capture("04-settings", &a);
    a.state = SETTINGS_DISLIKE_FOOD;
    a.category = 0;
    a.focus = 5;
    capture("05-dislike", &a);
    a.state = ERROR_EMPTY_POOL;
    a.focus = 0;
    capture("06-empty", &a);
    unsigned count = objects(lv_screen_active());
    lv_mem_monitor_t warm = {0}, end = {0};
    for (int pass = 0; pass < 3; pass++) {
        for (int i = 0; i < 100; i++) {
            for (int s = HOME; s <= LAUNCHER; s++) {
                a.state = s;
                a.focus = dinner_rows(&a) ? i % dinner_rows(&a) : 0;
                a.result = i % 60;
                a.rolling = a.result;
                a.category = i % 12;
                a.frame = 1;
                capture(NULL, &a);
                assert(objects(lv_screen_active()) == count);
            }
        }
        if (pass == 0)
            lv_mem_monitor(&warm);
    }
    lv_mem_monitor(&end);
    assert(end.free_size + 256 >= warm.free_size);
    assert(lv_mem_test() == LV_RESULT_OK);
    printf("LVGL: glyph coverage PASS; 4500 state renders; %u fixed objects; free=%u largest=%u "
           "(24KB pool)\n",
           count, (unsigned)end.free_size, (unsigned)end.free_biggest_size);
}
