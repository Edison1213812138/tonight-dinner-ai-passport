#include "dinner.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
static void key(dinner_app_t *a, dinner_key_t k, uint32_t *t) {
    *t += 150;
    dinner_key(a, k, *t);
}
int main(void) {
    dinner_settings_t s;
    dinner_defaults(&s);
    uint8_t pool[60];
    assert(dinner_pool(&s, pool) == 60);
    unsigned counts[12] = {0};
    for (int i = 0; i < 60; i++) {
        assert(dinner_foods[i].category < 12);
        counts[dinner_foods[i].category]++;
        for (int j = 0; j < i; j++)
            assert(strcmp(dinner_foods[i].name, dinner_foods[j].name));
    }
    for (int i = 0; i < 12; i++)
        assert(counts[i] == 5);
    for (int mode = 0; mode < 5; mode++)
        for (int budget = 0; budget < 4; budget++)
            for (int diet = 0; diet < 32; diet++) {
                s.mode = mode;
                s.budget = budget;
                s.diet = diet;
                size_t n = dinner_pool(&s, pool);
                uint32_t seed = 1;
                int prev = -1;
                for (size_t j = 0; j < n; j++) {
                    const food_t *f = &dinner_foods[pool[j]];
                    assert(!(f->tags & diet));
                    assert(!budget || f->budget <= budget);
                    if (mode == LIGHT)
                        assert(!(f->tags & (SPICY | OILY | FRIED)));
                }
                for (int j = 0; j < 100; j++) {
                    int id = dinner_select(&s, pool, n, prev, &seed);
                    if (!n)
                        assert(id == -1);
                    else {
                        assert(id >= 0 && id < 60);
                        bool found = false;
                        for (size_t k = 0; k < n; k++)
                            if (pool[k] == id)
                                found = true;
                        assert(found);
                        if (n > 1)
                            assert(id != prev);
                        prev = id;
                    }
                }
            }
    dinner_defaults(&s);
    s.excluded = FOOD_MASK;
    assert(!dinner_pool(&s, pool));
    s.excluded ^= UINT64_C(1) << 42;
    assert(dinner_pool(&s, pool) == 1);
    uint32_t seed = 0;
    assert(dinner_select(&s, pool, 1, 42, &seed) == 42);
    dinner_defaults(&s);
    s.categories = 4095;
    assert(!dinner_pool(&s, pool));
    s.categories = 2;
    assert(dinner_pool(&s, pool) == 55);
    for (int i = 0; i < 55; i++)
        assert(dinner_foods[pool[i]].category != 1);
    dinner_app_t a;
    dinner_init(&a, 123);
    uint32_t t = 0;
    key(&a, KEY_DOWN, &t);
    key(&a, KEY_OK, &t);
    assert(a.state == SETTINGS);
    key(&a, KEY_OK, &t);
    assert(a.state == SETTINGS_MODE);
    key(&a, KEY_DOWN, &t);
    key(&a, KEY_OK, &t);
    assert(a.settings.mode == CHEAP && a.dirty);
    key(&a, KEY_BACK, &t);
    assert(a.state == HOME);
    for (int i = 0; i < 1000; i++) {
        key(&a, KEY_OK, &t);
        assert(a.state == DRAW_CATEGORY || a.state == EASTER);
        if (a.state == EASTER) {
            assert(a.rerolls == 10);
            assert(dinner_tick(&a, t + 500));
            assert(a.frame == 1);
            assert(dinner_tick(&a, t + 1000));
            t += 1000;
        }
        int previous = a.previous;
        uint32_t start = a.started;
        for (unsigned ms = 0; ms <= 2200; ms += 20) {
            dinner_tick(&a, start + ms);
            if (ms == 600) {
                dinner_key(&a, KEY_DOWN, start + ms);
                assert(a.focus == 0);
            }
        }
        t = start + 2200;
        assert(a.state == RESULT);
        assert(a.result != previous);
        assert(a.previous == a.result);
        key(&a, KEY_DOWN, &t);
        assert(a.focus == 1);
    }
    assert(a.draws == 1000);
    key(&a, KEY_BACK, &t);
    assert(a.state == HOME);
    key(&a, KEY_BACK, &t);
    assert(a.state == LAUNCHER);
    key(&a, KEY_OK, &t);
    assert(a.state == HOME);
    a.settings.excluded = FOOD_MASK;
    key(&a, KEY_OK, &t);
    assert(a.state == ERROR_EMPTY_POOL);
    key(&a, KEY_OK, &t);
    assert(a.state == HOME && a.settings.excluded == 0);
    key(&a, KEY_OK, &t);
    key(&a, KEY_BACK, &t);
    assert(a.state == HOME);
    dinner_tick(&a, t + 9000);
    assert(a.state == HOME);
    /* Debounce and unsigned millisecond rollover. */
    dinner_init(&a, 1);
    dinner_key(&a, KEY_DOWN, UINT32_MAX - 40);
    dinner_key(&a, KEY_DOWN, 10);
    assert(a.focus == 1);
    dinner_key(&a, KEY_UP, 100);
    assert(a.focus == 0);
    dinner_key(&a, KEY_OK, 250);
    dinner_tick(&a, 2450);
    assert(a.state == RESULT);
    dinner_key(&a, KEY_OK, 2600);
    assert(a.state == CONFIRMED);
    dinner_tick(&a, 4100);
    assert(a.state == HOME);
    printf("Dinner: 60 foods, 640 filter combinations, 1000 draws, navigation, debounce, empty "
           "pool PASS\n");
}
