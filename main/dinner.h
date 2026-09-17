#pragma once
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#define FOOD_COUNT 60
#define CATEGORY_COUNT 12
#define FOOD_MASK ((UINT64_C(1) << FOOD_COUNT) - 1)
enum { SPICY = 1, SEAFOOD = 2, BEEF_LAMB = 4, RAW = 8, FRIED = 16, OILY = 32, SHARE = 64 };
typedef enum { ANY, CHEAP, LIGHT, STRONG, PARTY, MODE_COUNT } dinner_mode_t;
typedef struct {
    const char *name;
    uint8_t category, budget, tags;
} food_t;
extern const food_t dinner_foods[FOOD_COUNT];
extern const char *const dinner_categories[CATEGORY_COUNT];
extern const char *const dinner_modes[MODE_COUNT];
extern const char *const dinner_budgets[4];
extern const char *const dinner_diets[5];
typedef struct {
    uint64_t excluded;
    uint16_t categories;
    uint8_t mode, budget, diet;
} dinner_settings_t;
void dinner_defaults(dinner_settings_t *s);
bool dinner_settings_valid(const dinner_settings_t *s);
size_t dinner_pool(const dinner_settings_t *s, uint8_t out[FOOD_COUNT]);
uint32_t dinner_random(uint32_t *seed);
int dinner_select(const dinner_settings_t *s, const uint8_t *pool, size_t n, int previous,
                  uint32_t *seed);
typedef enum {
    HOME,
    DRAW_CATEGORY,
    DRAW_FOOD,
    RESULT,
    SETTINGS,
    SETTINGS_MODE,
    SETTINGS_BUDGET,
    SETTINGS_DIET,
    SETTINGS_DISLIKE_CATEGORY,
    SETTINGS_DISLIKE_FOOD,
    CONFIRMED,
    ERROR_EMPTY_POOL,
    EASTER,
    RESET_CONFIRM,
    LAUNCHER
} dinner_state_t;
typedef enum { KEY_UP, KEY_DOWN, KEY_OK, KEY_BACK } dinner_key_t;
typedef struct {
    dinner_settings_t settings;
    dinner_state_t state, settings_parent;
    uint32_t seed, started, next_frame, last_key, rerolls, draws;
    uint8_t focus, category, pool[FOOD_COUNT], count, frame, joke;
    int result, previous, rolling;
    bool dirty, storage_failed, key_seen;
} dinner_app_t;
void dinner_init(dinner_app_t *a, uint32_t seed);
void dinner_key(dinner_app_t *a, dinner_key_t key, uint32_t now);
bool dinner_tick(dinner_app_t *a, uint32_t now);
uint8_t dinner_rows(const dinner_app_t *a);
bool dinner_drawing(const dinner_app_t *a);
