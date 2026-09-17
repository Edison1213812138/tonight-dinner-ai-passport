#include "dinner_decoration.h"

/* Small native-vector stickers: no image/canvas buffers and no extra LVGL objects.
 * All drawing occurs in LVGL's draw event, beneath the screen's children. */
#define INK 0x0B1520
#define CREAM 0xFFF7E5
#define GRAY 0x8F9BA8
#define GOLD 0xFFD447
static dinner_state_t s_state;
static lv_obj_t *s_screen;
typedef struct {
    lv_layer_t *layer;
    int x, y, scale;
} pen_t;
static int px(const pen_t *p, int n) { return n * p->scale / 100; }
static void rect(pen_t p, int x, int y, int w, int h, int radius, uint32_t color) {
    lv_draw_rect_dsc_t d;
    lv_draw_rect_dsc_init(&d);
    d.bg_color = lv_color_hex(color);
    d.bg_opa = LV_OPA_COVER;
    d.radius = px(&p, radius);
    lv_area_t a = {p.x + px(&p, x), p.y + px(&p, y), p.x + px(&p, x + w) - 1,
                   p.y + px(&p, y + h) - 1};
    lv_draw_rect(p.layer, &d, &a);
}
static void line(pen_t p, int x1, int y1, int x2, int y2, int width, uint32_t color) {
    lv_draw_line_dsc_t d;
    lv_draw_line_dsc_init(&d);
    d.p1 = (lv_point_precise_t){p.x + px(&p, x1), p.y + px(&p, y1)};
    d.p2 = (lv_point_precise_t){p.x + px(&p, x2), p.y + px(&p, y2)};
    d.width = LV_MAX(1, px(&p, width));
    d.color = lv_color_hex(color);
    d.round_start = d.round_end = true;
    lv_draw_line(p.layer, &d);
}
static void star(pen_t p, int x, int y, int r) {
    line(p, x - r, y, x + r, y, 2, GOLD);
    line(p, x, y - r, x, y + r, 2, GOLD);
}
static void burger(pen_t p) {
    rect(p, 0, 1, 48, 23, 13, GOLD);
    rect(p, 1, 22, 46, 5, 2, CREAM);
    rect(p, 0, 30, 48, 9, 4, GOLD);
    /* Sesame dots and a little face on the bun. */
    line(p, 12, 7, 15, 6, 2, CREAM);
    line(p, 29, 6, 32, 7, 2, CREAM);
    rect(p, 17, 14, 3, 4, 2, INK);
    rect(p, 29, 14, 3, 4, 2, INK);
    line(p, 22, 18, 24, 20, 2, INK);
    line(p, 24, 20, 26, 18, 2, INK);
}
static void fries(pen_t p) {
    rect(p, 2, 5, 6, 27, 2, GOLD);
    rect(p, 10, 0, 6, 30, 2, CREAM);
    rect(p, 18, 4, 6, 28, 2, GOLD);
    rect(p, 26, 2, 6, 30, 2, CREAM);
    rect(p, 0, 23, 36, 28, 5, GOLD);
    /* Twin arches echo the fast-food motif in the supplied wireframe. */
    line(p, 9, 43, 11, 32, 3, INK);
    line(p, 11, 32, 15, 31, 3, INK);
    line(p, 15, 31, 18, 40, 3, INK);
    line(p, 18, 40, 21, 31, 3, INK);
    line(p, 21, 31, 25, 32, 3, INK);
    line(p, 25, 32, 27, 43, 3, INK);
}
static void drink(pen_t p) {
    line(p, 18, 0, 15, 15, 3, GOLD);
    rect(p, 1, 16, 26, 32, 5, CREAM);
    rect(p, 0, 11, 28, 7, 3, GOLD);
    rect(p, 7, 27, 3, 4, 2, INK);
    rect(p, 18, 27, 3, 4, 2, INK);
    line(p, 11, 34, 14, 36, 2, INK);
    line(p, 14, 36, 17, 34, 2, INK);
}
static void snow_king(pen_t p) {
    line(p, 8, 46, 0, 39, 4, CREAM);
    line(p, 42, 46, 50, 36, 4, CREAM);
    rect(p, 6, 36, 39, 34, 17, CREAM);
    rect(p, 5, 13, 40, 34, 17, CREAM);
    /* Three-point crown, scarf, happy face and golden cheeks. */
    line(p, 13, 12, 11, 3, 4, GOLD);
    line(p, 11, 3, 22, 8, 4, GOLD);
    line(p, 22, 8, 25, 0, 4, GOLD);
    line(p, 25, 0, 29, 8, 4, GOLD);
    line(p, 29, 8, 40, 3, 4, GOLD);
    line(p, 40, 3, 37, 12, 4, GOLD);
    rect(p, 12, 10, 26, 5, 2, GOLD);
    rect(p, 14, 26, 4, 5, 2, INK);
    rect(p, 33, 26, 4, 5, 2, INK);
    rect(p, 8, 33, 6, 3, 2, GOLD);
    rect(p, 37, 33, 6, 3, 2, GOLD);
    line(p, 22, 33, 25, 36, 2, INK);
    line(p, 25, 36, 29, 32, 2, INK);
    rect(p, 9, 44, 32, 6, 3, GOLD);
    rect(p, 31, 46, 6, 13, 2, GOLD);
    rect(p, 23, 54, 4, 4, 2, INK);
}
static void draw(lv_event_t *event) {
    lv_layer_t *layer = lv_event_get_layer(event);
    if (s_state == HOME) {
        fries((pen_t){layer, 25, 94, 90});
        burger((pen_t){layer, 77, 107, 90});
        snow_king((pen_t){layer, 161, 76, 100});
        drink((pen_t){layer, 136, 108, 65});
        star((pen_t){layer, 0, 0, 100}, 83, 84, 4);
        star((pen_t){layer, 0, 0, 100}, 215, 111, 4);
    } else if (s_state == DRAW_CATEGORY || s_state == DRAW_FOOD || s_state == EASTER) {
        snow_king((pen_t){layer, 29, 225, 75});
        fries((pen_t){layer, 180, 224, 70});
        star((pen_t){layer, 0, 0, 100}, 116, 239, 5);
        star((pen_t){layer, 0, 0, 100}, 141, 225, 3);
    } else if (s_state == RESULT) {
        burger((pen_t){layer, 24, 72, 57});
        snow_king((pen_t){layer, 193, 64, 45});
        star((pen_t){layer, 0, 0, 100}, 175, 83, 3);
    } else if (s_state == SETTINGS) {
        snow_king((pen_t){layer, 25, 33, 52});
        drink((pen_t){layer, 195, 40, 56});
    } else if (s_state == CONFIRMED || s_state == LAUNCHER || s_state == ERROR_EMPTY_POOL) {
        burger((pen_t){layer, 26, 69, 62});
        snow_king((pen_t){layer, 179, 58, 57});
    }
}
void dinner_decoration_attach(lv_obj_t *screen) {
    s_screen = screen;
    lv_obj_add_event_cb(screen, draw, LV_EVENT_DRAW_MAIN, NULL);
}
void dinner_decoration_state(dinner_state_t state) {
    if (s_state != state) {
        s_state = state;
        lv_obj_invalidate(s_screen);
    }
}
