#include "dinner_storage.h"
#include "nvs.h"
#include "nvs_flash.h"
/* Fixed versioned wire format: never persist compiler padding or pointers. */
static bool ready;
bool dinner_storage_save(const dinner_settings_t *s) {
    if (!ready)
        return false;
    uint8_t data[16] = {1, s->mode, s->budget, s->diet};
    data[4] = s->categories;
    data[5] = s->categories >> 8;
    for (int i = 0; i < 8; i++)
        data[6 + i] = s->excluded >> (8 * i);
    nvs_handle_t h;
    if (nvs_open("dinner", NVS_READWRITE, &h) != ESP_OK)
        return false;
    esp_err_t e = nvs_set_blob(h, "settings", data, sizeof(data));
    if (e == ESP_OK)
        e = nvs_commit(h);
    nvs_close(h);
    return e == ESP_OK;
}
bool dinner_storage_load(dinner_settings_t *s) {
    dinner_defaults(s);
    ready = nvs_flash_init() == ESP_OK;
    if (!ready)
        return false; /* Never erase unrelated NVS. */
    nvs_handle_t h;
    esp_err_t e = nvs_open("dinner", NVS_READONLY, &h);
    if (e == ESP_ERR_NVS_NOT_FOUND)
        return dinner_storage_save(s);
    if (e != ESP_OK)
        return false;
    uint8_t data[16];
    size_t n = sizeof(data);
    e = nvs_get_blob(h, "settings", data, &n);
    nvs_close(h);
    if (e == ESP_ERR_NVS_NOT_FOUND)
        return dinner_storage_save(s);
    if (e != ESP_OK || n != sizeof(data) || data[0] != 1 || data[14] || data[15])
        return false;
    dinner_settings_t value = {.mode = data[1],
                               .budget = data[2],
                               .diet = data[3],
                               .categories = data[4] | (data[5] << 8)};
    for (int i = 0; i < 8; i++)
        value.excluded |= (uint64_t)data[6 + i] << (8 * i);
    if (!dinner_settings_valid(&value))
        return false;
    *s = value;
    return true;
}
