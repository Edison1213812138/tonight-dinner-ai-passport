#include "dinner.h"
#include <string.h>
void dinner_defaults(dinner_settings_t *s) {
    memset(s, 0, sizeof(*s));
}
bool dinner_settings_valid(const dinner_settings_t *s) {
    return s->mode < MODE_COUNT && s->budget <= 3 && !(s->diet & ~31u) &&
           !(s->categories & ~4095u) && !(s->excluded & ~FOOD_MASK);
}
