#include "dinner.h"
#include <limits.h>
#include <string.h>
static void go(dinner_app_t *a, dinner_state_t s) {
    a->state = s;
    a->focus = 0;
}
bool dinner_drawing(const dinner_app_t *a) {
    return a->state == DRAW_CATEGORY || a->state == DRAW_FOOD || a->state == EASTER;
}
void dinner_init(dinner_app_t *a, uint32_t seed) {
    memset(a, 0, sizeof(*a));
    a->seed = seed;
    a->result = a->previous = -1;
    a->state = HOME;
}
uint8_t dinner_rows(const dinner_app_t *a) {
    switch (a->state) {
    case HOME:
    case ERROR_EMPTY_POOL:
    case RESET_CONFIRM:
        return 2;
    case RESULT:
        return 3;
    case SETTINGS:
    case SETTINGS_MODE:
    case SETTINGS_DIET:
        return 5;
    case SETTINGS_BUDGET:
        return 4;
    case SETTINGS_DISLIKE_CATEGORY:
        return CATEGORY_COUNT;
    case SETTINGS_DISLIKE_FOOD:
        return 6;
    case LAUNCHER:
        return 1;
    default:
        return 0;
    }
}
static void draw(dinner_app_t *a, uint32_t now, bool retry) {
    a->count = dinner_pool(&a->settings, a->pool);
    if (!a->count) {
        go(a, ERROR_EMPTY_POOL);
        return;
    }
    if (retry && a->rerolls < UINT32_MAX)
        a->rerolls++;
    a->result = dinner_select(&a->settings, a->pool, a->count, a->previous, &a->seed);
    a->joke = dinner_random(&a->seed) % 7;
    a->rolling = a->pool[dinner_random(&a->seed) % a->count];
    a->started = now;
    a->next_frame = 0;
    a->frame = 0;
    go(a, retry && a->rerolls == 10 ? EASTER : DRAW_CATEGORY);
}
static void settings_enter(dinner_app_t *a, dinner_state_t parent) {
    a->settings_parent = parent;
    go(a, SETTINGS);
}
static void back(dinner_app_t *a) {
    switch (a->state) {
    case HOME:
        go(a, LAUNCHER);
        break;
    case LAUNCHER:
        break;
    case SETTINGS:
        go(a, a->settings_parent);
        break;
    case SETTINGS_MODE:
    case SETTINGS_BUDGET:
    case SETTINGS_DIET:
    case SETTINGS_DISLIKE_CATEGORY:
    case RESET_CONFIRM:
        go(a, SETTINGS);
        break;
    case SETTINGS_DISLIKE_FOOD:
        go(a, SETTINGS_DISLIKE_CATEGORY);
        a->focus = a->category;
        break;
    default:
        go(a, HOME);
        break;
    }
}
void dinner_key(dinner_app_t *a, dinner_key_t key, uint32_t now) {
    if (a->key_seen && (uint32_t)(now - a->last_key) < 90)
        return;
    a->last_key = now;
    a->key_seen = true;
    if (key == KEY_BACK) {
        back(a);
        return;
    }
    if (dinner_drawing(a) || a->state == CONFIRMED)
        return;
    uint8_t rows = dinner_rows(a);
    if (key == KEY_UP || key == KEY_DOWN) {
        if (rows)
            a->focus = (a->focus + rows + (key == KEY_UP ? -1 : 1)) % rows;
        return;
    }
    switch (a->state) {
    case LAUNCHER:
        go(a, HOME);
        break;
    case HOME:
        if (a->focus)
            settings_enter(a, HOME);
        else {
            a->rerolls = 0;
            draw(a, now, false);
        }
        break;
    case RESULT:
        if (a->focus == 0) {
            go(a, CONFIRMED);
            a->started = now;
            a->rerolls = 0;
        } else if (a->focus == 1)
            draw(a, now, true);
        else
            settings_enter(a, RESULT);
        break;
    case SETTINGS:
        if (a->focus == 0) {
            go(a, SETTINGS_MODE);
            a->focus = a->settings.mode;
        } else if (a->focus == 1) {
            go(a, SETTINGS_BUDGET);
            a->focus = a->settings.budget;
        } else if (a->focus == 2)
            go(a, SETTINGS_DIET);
        else if (a->focus == 3)
            go(a, SETTINGS_DISLIKE_CATEGORY);
        else
            go(a, RESET_CONFIRM);
        break;
    case SETTINGS_MODE:
        a->settings.mode = a->focus;
        a->dirty = true;
        go(a, SETTINGS);
        break;
    case SETTINGS_BUDGET:
        a->settings.budget = a->focus;
        a->dirty = true;
        go(a, SETTINGS);
        a->focus = 1;
        break;
    case SETTINGS_DIET:
        a->settings.diet ^= 1u << a->focus;
        a->dirty = true;
        break;
    case SETTINGS_DISLIKE_CATEGORY:
        a->category = a->focus;
        go(a, SETTINGS_DISLIKE_FOOD);
        break;
    case SETTINGS_DISLIKE_FOOD:
        if (!a->focus)
            a->settings.categories ^= 1u << a->category;
        else
            a->settings.excluded ^= UINT64_C(1) << (a->category * 5 + a->focus - 1);
        a->dirty = true;
        break;
    case RESET_CONFIRM:
        if (a->focus) {
            dinner_defaults(&a->settings);
            a->dirty = true;
        }
        go(a, SETTINGS);
        break;
    case ERROR_EMPTY_POOL:
        if (!a->focus) {
            dinner_defaults(&a->settings);
            a->dirty = true;
            go(a, HOME);
        } else
            settings_enter(a, HOME);
        break;
    default:
        break;
    }
}
bool dinner_tick(dinner_app_t *a, uint32_t now) {
    uint32_t elapsed = now - a->started;
    if (a->state == RESULT && !a->frame && elapsed >= 140) {
        a->frame = 1;
        return true;
    }
    if (a->state == CONFIRMED && elapsed >= 1500) {
        go(a, HOME);
        return true;
    }
    if (a->state == EASTER) {
        if (elapsed >= 1000) {
            go(a, DRAW_CATEGORY);
            a->started = now;
            a->next_frame = 0;
            return true;
        }
        unsigned frame = elapsed >= 500;
        if (a->frame != frame) {
            a->frame = frame;
            return true;
        }
        return false;
    }
    if (!dinner_drawing(a))
        return false;
    if (elapsed >= 2200) {
        a->previous = a->result;
        a->draws++;
        a->started = now;
        a->frame = 0;
        go(a, RESULT);
        return true;
    }
    if (elapsed >= 1150 && a->state == DRAW_CATEGORY) {
        go(a, DRAW_FOOD);
        a->next_frame = 1150;
    }
    if (elapsed < a->next_frame)
        return false;
    if (elapsed >= 800 && elapsed < 1150) {
        a->rolling = a->result;
        a->next_frame = 1150;
    } else if (elapsed >= 1950) {
        a->rolling = a->result;
        a->next_frame = 2200;
    } else {
        uint8_t candidates[5], n = 0;
        if (a->state == DRAW_FOOD) {
            for (unsigned i = 0; i < a->count; i++)
                if (dinner_foods[a->pool[i]].category == dinner_foods[a->result].category)
                    candidates[n++] = a->pool[i];
            a->rolling = candidates[dinner_random(&a->seed) % n];
        } else
            a->rolling = a->pool[dinner_random(&a->seed) % a->count];
        a->next_frame = elapsed + 55 + (a->state == DRAW_CATEGORY ? elapsed : elapsed - 1150) / 7;
    }
    a->frame++;
    return true;
}
