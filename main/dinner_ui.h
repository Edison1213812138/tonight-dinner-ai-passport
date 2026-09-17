#pragma once
#include "dinner.h"
void dinner_ui_create(void);
void dinner_ui_render(const dinner_app_t *a);
void dinner_ui_battery(int soc);
bool dinner_ui_fonts_valid(void);
