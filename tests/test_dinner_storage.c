#include "dinner_storage.h"
#include "nvs.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
static uint8_t bytes[16];
static size_t length;
static int failure;
static unsigned commits;
esp_err_t nvs_flash_init(void) {
    return failure == 1 ? 2 : ESP_OK;
}
esp_err_t nvs_open(const char *ns, int mode, nvs_handle_t *h) {
    assert(!strcmp(ns, "dinner"));
    *h = 1;
    if (failure == 2)
        return 2;
    return !length && mode == NVS_READONLY ? ESP_ERR_NVS_NOT_FOUND : ESP_OK;
}
esp_err_t nvs_get_blob(nvs_handle_t h, const char *key, void *out, size_t *n) {
    (void)h;
    assert(!strcmp(key, "settings"));
    assert(*n >= length);
    memcpy(out, bytes, length);
    *n = length;
    return length ? ESP_OK : ESP_ERR_NVS_NOT_FOUND;
}
esp_err_t nvs_set_blob(nvs_handle_t h, const char *key, const void *data, size_t n) {
    (void)h;
    (void)key;
    if (failure == 3)
        return 2;
    assert(n == 16);
    memcpy(bytes, data, n);
    length = n;
    return ESP_OK;
}
esp_err_t nvs_commit(nvs_handle_t h) {
    (void)h;
    commits++;
    return failure == 4 ? 2 : ESP_OK;
}
void nvs_close(nvs_handle_t h) {
    (void)h;
}
int main(void) {
    dinner_settings_t s;
    assert(dinner_storage_load(&s));
    assert(commits == 1 && length == 16 && s.excluded == 0);
    s.mode = PARTY;
    s.budget = 2;
    s.diet = 21;
    s.categories = 2049;
    s.excluded = (UINT64_C(1) << 59) | 1;
    assert(dinner_storage_save(&s));
    dinner_settings_t restored;
    assert(dinner_storage_load(&restored));
    assert(restored.mode == PARTY && restored.budget == 2 && restored.diet == 21 &&
           restored.categories == 2049 && restored.excluded == s.excluded);
    for (int field = 0; field < 16; field++) {
        uint8_t backup = bytes[field];
        bytes[field] = 255;
        if (field == 0 || field == 1 || field == 2 || field == 3 || field == 5 || field == 13 ||
            field >= 14) {
            assert(!dinner_storage_load(&restored));
            assert(restored.excluded == 0 && restored.mode == ANY);
        }
        bytes[field] = backup;
    }
    length = 15;
    assert(!dinner_storage_load(&restored));
    length = 16;
    for (failure = 1; failure <= 2; failure++) {
        assert(!dinner_storage_load(&restored));
        assert(restored.mode == ANY);
    }
    failure = 0;
    assert(dinner_storage_load(&restored));
    for (failure = 3; failure <= 4; failure++)
        assert(!dinner_storage_save(&s));
    puts("Dinner storage: first boot, restart, version/length/range corruption, "
         "initialization/write/commit failure PASS");
}
