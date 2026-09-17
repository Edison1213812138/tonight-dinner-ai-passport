#include "dinner.h"
uint32_t dinner_random(uint32_t *seed) {
    uint32_t x = *seed ? *seed : 0x92aeb173u;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    return *seed = x;
}
/* Rejection sampling avoids modulo bias; never needs radio entropy. */
static uint32_t below(uint32_t *seed, uint32_t n) {
    uint32_t r, limit = UINT32_MAX - (UINT32_MAX % n);
    do {
        r = dinner_random(seed) - 1u;
    } while (r >= limit);
    return r % n;
}
size_t dinner_pool(const dinner_settings_t *s, uint8_t out[FOOD_COUNT]) {
    size_t n = 0;
    for (unsigned i = 0; i < FOOD_COUNT; i++) {
        const food_t *f = &dinner_foods[i];
        if ((s->excluded & (UINT64_C(1) << i)) || (s->categories & (1u << f->category)) ||
            (f->tags & s->diet) || (s->budget && f->budget > s->budget) ||
            (s->mode == LIGHT && (f->tags & (SPICY | OILY | FRIED))))
            continue;
        out[n++] = i;
    }
    return n;
}
static unsigned weight(const dinner_settings_t *s, int id) {
    const food_t *f = &dinner_foods[id];
    if (s->mode == CHEAP)
        return f->budget == 1 ? 4 : 1;
    if (s->mode == STRONG)
        return (f->category == 0 || f->category == 1 || f->category == 3 || (f->tags & SPICY)) ? 4
                                                                                               : 1;
    if (s->mode == PARTY)
        return f->tags & SHARE ? 4 : 1;
    return 1;
}
int dinner_select(const dinner_settings_t *s, const uint8_t *pool, size_t n, int previous,
                  uint32_t *seed) {
    if (!n)
        return -1;
    unsigned category_weights[CATEGORY_COUNT] = {0}, total = 0;
    for (size_t i = 0; i < n; i++) {
        if (n > 1 && pool[i] == previous)
            continue;
        unsigned w = weight(s, pool[i]);
        category_weights[dinner_foods[pool[i]].category] += w;
        total += w;
    }
    unsigned r = below(seed, total), category = 0;
    while (r >= category_weights[category])
        r -= category_weights[category++];
    r = below(seed, category_weights[category]);
    for (size_t i = 0; i < n; i++) {
        int id = pool[i];
        if (dinner_foods[id].category != category || (n > 1 && id == previous))
            continue;
        unsigned w = weight(s, id);
        if (r < w)
            return id;
        r -= w;
    }
    return -1;
}
