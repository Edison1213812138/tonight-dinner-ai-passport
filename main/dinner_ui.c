#include "dinner_ui.h"
#include "dinner_decoration.h"
#include "lvgl.h"
#include <stdio.h>
#include <string.h>
LV_FONT_DECLARE(dinner_font_14);
LV_FONT_DECLARE(dinner_font_20);
LV_FONT_DECLARE(dinner_font_30);
extern const uint32_t dinner_font_codepoints[];
extern const unsigned dinner_font_codepoint_count;
#define BG 0x0B1520
#define TEXT 0xFFF7E5
#define MUTED 0x8F9BA8
#define ACCENT 0xFFD447
static lv_obj_t *screen, *title, *hero, *hero_text, *sub, *note, *footer, *battery, *progress,
    *caption;
static lv_obj_t *rows[6], *row_text[6];
static const char *const home_notes[] = {"别纠结，让命运决定。", "选择困难症专用。",
                                         "想半天不如抽一下。", "小小的选择，大大的快乐。"};
static const char *const jokes[] = {"别反悔。",
                                    "命运已经替你决定了。",
                                    "今天适合放纵一下。",
                                    "纠结结束，出发。",
                                    "再想下去店都关门了。",
                                    "相信第一感觉。",
                                    "这个结果看起来很靠谱。"};
static lv_obj_t *label(lv_obj_t *parent, int x, int y, int w, const lv_font_t *font,
                       uint32_t color) {
    lv_obj_t *o = lv_label_create(parent);
    lv_obj_set_pos(o, x, y);
    lv_obj_set_width(o, w);
    lv_obj_set_style_text_font(o, font, 0);
    lv_obj_set_style_text_color(o, lv_color_hex(color), 0);
    lv_obj_set_style_text_align(o, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text(o, "");
    return o;
}
static lv_obj_t *box(int x, int y, int w, int h) {
    lv_obj_t *o = lv_obj_create(screen);
    lv_obj_remove_style_all(o);
    lv_obj_set_pos(o, x, y);
    lv_obj_set_size(o, w, h);
    lv_obj_remove_flag(o, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_radius(o, 14, 0);
    lv_obj_set_style_bg_color(o, lv_color_hex(BG), 0);
    lv_obj_set_style_bg_opa(o, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(o, 1, 0);
    lv_obj_set_style_border_color(o, lv_color_hex(MUTED), 0);
    return o;
}
static void visible(lv_obj_t *o, bool show) {
    if (show)
        lv_obj_remove_flag(o, LV_OBJ_FLAG_HIDDEN);
    else
        lv_obj_add_flag(o, LV_OBJ_FLAG_HIDDEN);
}
static void selected(lv_obj_t *o, lv_obj_t *text, bool on) {
    lv_obj_set_style_bg_color(o, lv_color_hex(on ? ACCENT : BG), 0);
    lv_obj_set_style_border_color(o, lv_color_hex(on ? ACCENT : MUTED), 0);
    lv_obj_set_style_text_color(text, lv_color_hex(on ? BG : TEXT), 0);
}
void dinner_ui_create(void) {
    screen = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(screen, lv_color_hex(BG), 0);
    lv_obj_remove_flag(screen, LV_OBJ_FLAG_SCROLLABLE);
    dinner_decoration_attach(screen);
    lv_obj_t *brand = label(screen, 22, 12, 138, &dinner_font_14, MUTED);
    lv_label_set_text(brand, "AI PASSPORT");
    lv_obj_set_style_text_align(brand, LV_TEXT_ALIGN_LEFT, 0);
    battery = label(screen, 170, 12, 48, &dinner_font_14, MUTED);
    dinner_ui_battery(-1);
    title = label(screen, 16, 42, 208, &dinner_font_20, TEXT);
    hero = box(16, 83, 208, 100);
    hero_text = label(hero, 4, 0, 200, &dinner_font_30, ACCENT);
    sub = label(screen, 18, 191, 204, &dinner_font_14, MUTED);
    note = label(screen, 18, 252, 204, &dinner_font_14, MUTED);
    caption = label(screen, 18, 157, 204, &dinner_font_14, TEXT);
    footer = label(screen, 20, 294, 200, &dinner_font_14, MUTED);
    progress = box(24, 179, 192, 3);
    lv_obj_set_style_border_width(progress, 0, 0);
    lv_obj_set_style_bg_color(progress, lv_color_hex(ACCENT), 0);
    for (int i = 0; i < 6; i++) {
        rows[i] = box(16, 80 + i * 34, 208, 30);
        row_text[i] = label(rows[i], 9, 3, 190, &dinner_font_14, TEXT);
        lv_obj_set_style_text_align(row_text[i], LV_TEXT_ALIGN_LEFT, 0);
    }
    lv_screen_load(screen);
}
void dinner_ui_battery(int soc) {
    if (soc < 0)
        lv_label_set_text(battery, "--%");
    else
        lv_label_set_text_fmt(battery, "%d%%", soc);
}
bool dinner_ui_fonts_valid(void) {
    const lv_font_t *fonts[] = {&dinner_font_14, &dinner_font_20, &dinner_font_30};
    for (unsigned f = 0; f < 3; f++)
        for (unsigned i = 0; i < dinner_font_codepoint_count; i++) {
            lv_font_glyph_dsc_t d = {0};
            if (!lv_font_get_glyph_dsc(fonts[f], &d, dinner_font_codepoints[i], 0) ||
                d.is_placeholder)
                return false;
        }
    lv_font_glyph_dsc_t d = {0};
    return !lv_font_get_glyph_dsc(fonts[0], &d, 0x9F98, 0) || d.is_placeholder;
}
static void row(int slot, int y, const char *text, bool focus) {
    visible(rows[slot], true);
    lv_obj_set_y(rows[slot], y);
    lv_label_set_text(row_text[slot], text);
    selected(rows[slot], row_text[slot], focus);
}
static void hero_show(const char *text, int y, int h, bool focus) {
    visible(hero, true);
    lv_obj_set_pos(hero, 16, y);
    lv_obj_set_size(hero, 208, h);
    lv_label_set_text(hero_text, text);
    lv_obj_set_style_text_font(hero_text, &dinner_font_30, 0);
    lv_obj_set_style_text_color(hero_text, lv_color_hex(ACCENT), 0);
    selected(hero, hero_text, focus);
    if (!focus)
        lv_obj_set_style_text_color(hero_text, lv_color_hex(ACCENT), 0);
    lv_obj_center(hero_text);
}
static unsigned exclusions(const dinner_settings_t *s) {
    unsigned n = 0;
    for (int i = 0; i < FOOD_COUNT; i++)
        if ((s->excluded & (UINT64_C(1) << i)) ||
            (s->categories & (1u << dinner_foods[i].category)))
            n++;
    return n;
}
static void render_settings(const dinner_app_t *a) {
    char text[100];
    const char *heading = "设置";
    switch (a->state) {
    case SETTINGS_MODE:
        heading = "模式";
        break;
    case SETTINGS_BUDGET:
        heading = "预算 · 上限";
        break;
    case SETTINGS_DIET:
        heading = "忌口 · 排除标签";
        break;
    case SETTINGS_DISLIKE_CATEGORY:
        heading = "不喜欢 · 选择分类";
        break;
    case SETTINGS_DISLIKE_FOOD:
        heading = dinner_categories[a->category];
        break;
    default:
        break;
    }
    lv_label_set_text(title, heading);
    unsigned n = dinner_rows(a), start = a->focus >= 6 ? a->focus - 5 : 0;
    for (unsigned i = start; i < n && i < start + 6; i++) {
        const char *name = "";
        bool checked = false;
        switch (a->state) {
        case SETTINGS:
            if (i == 0)
                snprintf(text, sizeof(text), "模式   %s", dinner_modes[a->settings.mode]);
            if (i == 1)
                snprintf(text, sizeof(text), "预算   %s", dinner_budgets[a->settings.budget]);
            if (i == 2)
                snprintf(text, sizeof(text), "忌口   已选 %u 项",
                         (unsigned)__builtin_popcount(a->settings.diet));
            if (i == 3)
                snprintf(text, sizeof(text), "不喜欢   已排除 %u 项", exclusions(&a->settings));
            if (i == 4)
                snprintf(text, sizeof(text), "重置设置");
            break;
        case SETTINGS_MODE:
            name = dinner_modes[i];
            checked = i == a->settings.mode;
            break;
        case SETTINGS_BUDGET:
            name = dinner_budgets[i];
            checked = i == a->settings.budget;
            break;
        case SETTINGS_DIET:
            name = dinner_diets[i];
            checked = a->settings.diet & (1u << i);
            break;
        case SETTINGS_DISLIKE_CATEGORY:
            name = dinner_categories[i];
            checked = a->settings.categories & (1u << i);
            break;
        case SETTINGS_DISLIKE_FOOD:
            if (!i) {
                name = "整类排除";
                checked = a->settings.categories & (1u << a->category);
            } else {
                unsigned id = a->category * 5 + i - 1;
                name = dinner_foods[id].name;
                checked = a->settings.excluded & (UINT64_C(1) << id);
            }
            break;
        default:
            break;
        }
        if (a->state != SETTINGS)
            snprintf(text, sizeof(text), "%s %s", checked ? "[x]" : "[ ]", name);
        row(i - start, 78 + (i - start) * 34, text, i == a->focus);
    }
    if (a->state == SETTINGS)
        lv_label_set_text(note,
                          a->storage_failed ? "保存失败，已恢复默认" : "设置自动保存 · 完全离线");
    else if (a->state == SETTINGS_DISLIKE_FOOD && (a->settings.categories & (1u << a->category)))
        lv_label_set_text(note, "整类已排除");
    lv_label_set_text(footer, "上/下选择  OK确认/切换");
    if (n >= 6) {
        lv_obj_set_y(note, 282);
        if (!lv_label_get_text(note)[0])
            lv_label_set_text(note, "长按OK返回");
        lv_label_set_text(footer, "");
    }
}
void dinner_ui_render(const dinner_app_t *a) {
    dinner_decoration_state(a->state);
    for (int i = 0; i < 6; i++)
        visible(rows[i], false);
    visible(hero, false);
    visible(progress, false);
    lv_label_set_text(caption, "");
    lv_label_set_text(sub, "");
    lv_label_set_text(note, "");
    lv_obj_set_y(sub, 191);
    lv_obj_set_y(note, 262);
    lv_label_set_text(footer, "上/下选择  长按OK返回");
    char text[100];
    switch (a->state) {
    case HOME:
        lv_label_set_text(title, "今晚吃什么？");
        hero_show("开始抽", 153, 56, a->focus == 0);
        lv_obj_set_y(sub, 214);
        lv_obj_set_y(note, 270);
        lv_label_set_text(sub, home_notes[a->draws % 4]);
        row(0, 240, "设置", a->focus == 1);
        snprintf(text, sizeof(text), "%s · 排除%u项", dinner_modes[a->settings.mode],
                 exclusions(&a->settings));
        lv_label_set_text(note, a->storage_failed ? "保存失败，已恢复默认" : text);
        lv_label_set_text(footer, "OK 开始  长按OK退出");
        break;
    case DRAW_CATEGORY:
    case DRAW_FOOD:
        lv_label_set_text(title, "正在为你抽取...");
        hero_show(a->state == DRAW_CATEGORY ? dinner_categories[dinner_foods[a->rolling].category]
                                            : dinner_foods[a->rolling].name,
                  99, 96, true);
        lv_obj_set_y(sub, 71);
        lv_label_set_text(sub, a->state == DRAW_CATEGORY ? "01 / 分类" : "02 / 具体食物");
        lv_label_set_text(note, "马上揭晓！");
        visible(progress, true);
        lv_obj_set_y(progress, 215);
        lv_obj_set_width(progress, 24 + (a->frame % 8) * 24);
        lv_label_set_text(footer, "长按OK取消");
        break;
    case RESULT:
        lv_label_set_text(title, "命运的选择");
        lv_obj_set_y(sub, 73);
        lv_label_set_text(sub, dinner_categories[dinner_foods[a->result].category]);
        hero_show(dinner_foods[a->result].name, 99, 56, a->frame == 0);
        lv_label_set_text(caption, "今晚就吃这个！");
        lv_obj_set_y(note, 179);
        lv_label_set_text(note, a->rerolls == 8 ? "还没选好吗？"
                                : (a->rerolls >= 3 && a->rerolls <= 5 && a->joke % 2)
                                    ? "你其实已经有答案了？"
                                    : jokes[a->joke]);
        row(0, 203, "就它了！", a->focus == 0);
        row(1, 237, "再抽一次", a->focus == 1);
        row(2, 271, "设置", a->focus == 2);
        lv_label_set_text(footer, "");
        break;
    case CONFIRMED:
        lv_label_set_text(title, "决定了！");
        hero_show(dinner_foods[a->result].name, 101, 88, true);
        lv_label_set_text(sub, "今晚就吃这个！");
        lv_label_set_text(note, "纠结结束，出发。");
        break;
    case ERROR_EMPTY_POOL:
        lv_label_set_text(title, "没东西可以吃啦");
        hero_show("空空如也", 96, 78, false);
        lv_label_set_text(sub, "检查一下你的筛选设置");
        row(0, 228, "恢复默认", a->focus == 0);
        row(1, 262, "返回设置", a->focus == 1);
        break;
    case EASTER:
        lv_label_set_text(title, "命运有话说");
        hero_show(a->frame ? "就吃这个。" : "别选了。", 99, 100, true);
        break;
    case RESET_CONFIRM:
        lv_label_set_text(title, "恢复默认设置？");
        lv_label_set_text(sub, "将清除全部筛选与排除");
        row(0, 228, "取消", a->focus == 0);
        row(1, 262, "确认重置", a->focus == 1);
        break;
    case LAUNCHER:
        lv_label_set_text(title, "AI PASSPORT");
        hero_show("今晚吃什么", 99, 92, true);
        lv_label_set_text(sub, "已退出 · 按 OK 再进入");
        lv_label_set_text(footer, "OK 进入");
        break;
    default:
        render_settings(a);
        break;
    }
}
