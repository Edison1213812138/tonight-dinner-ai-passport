#include "bsp_battery.h"
#include "bsp_button.h"
#include "bsp_display.h"
#include "dinner.h"
#include "dinner_storage.h"
#include "dinner_ui.h"
#include "esp_heap_caps.h"
#include "esp_log.h"
#include "esp_random.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "lvgl.h"
static dinner_app_t app;
static QueueHandle_t inputs;
static StaticQueue_t queue_control;
static uint8_t queue_data[8 * sizeof(dinner_key_t)];
static const char *TAG = "dinner";
static uint32_t milliseconds(void) {
    return (uint32_t)(esp_timer_get_time() / 1000);
}
/* BSP debounces ADC. Only bounded queue operations run in esp_timer context. */
static void button(bsp_btn_t btn, bsp_btn_ev_t event, void *user) {
    (void)user;
    dinner_key_t key;
    if (btn == BSP_BTN_OK && event == BSP_BTN_LONG)
        key = KEY_BACK;
    else if (event == BSP_BTN_CLICK || event == BSP_BTN_DOUBLE)
        key = btn == BSP_BTN_UP ? KEY_UP : btn == BSP_BTN_DOWN ? KEY_DOWN : KEY_OK;
    else
        return;
    xQueueSend(inputs, &key, 0);
}
static void animation(lv_timer_t *timer) {
    (void)timer;
    if (dinner_tick(&app, milliseconds()))
        dinner_ui_render(&app);
}
/* Slow optional I2C initialization must not stall navigation or NVS saves. */
static void battery_worker(void *arg) {
    (void)arg;
    vTaskDelay(pdMS_TO_TICKS(2000));
    bool ready = bsp_battery_init() == ESP_OK;
    for (;;) {
        int soc = ready ? bsp_battery_soc() : -1;
        if (bsp_lvgl_lock(100)) {
            dinner_ui_battery(soc);
            bsp_lvgl_unlock();
        }
        vTaskDelay(pdMS_TO_TICKS(30000));
    }
}
void app_main(void) {
    dinner_init(&app, esp_random() ^ (uint32_t)esp_timer_get_time());
    app.storage_failed = !dinner_storage_load(&app.settings);
    ESP_ERROR_CHECK(bsp_display_init());
    if (!bsp_lvgl_init()) {
        ESP_LOGE(TAG, "LVGL initialization failed");
        return;
    }
    inputs = xQueueCreateStatic(8, sizeof(dinner_key_t), queue_data, &queue_control);
    if (!bsp_lvgl_lock(1000))
        return;
    dinner_ui_create();
    if (!dinner_ui_fonts_valid())
        ESP_LOGE(TAG, "Missing font glyphs");
    dinner_ui_render(&app);
    lv_timer_t *timer = lv_timer_create(animation, 20, NULL);
    bsp_lvgl_unlock();
    if (!timer) {
        ESP_LOGE(TAG, "Animation timer allocation failed");
        return;
    }
    ESP_ERROR_CHECK(bsp_button_init(button, NULL));
    bsp_display_backlight(75);
    /* Battery initialization can take seconds; first frame and buttons already exist. */
    if (xTaskCreate(battery_worker, "dinner_battery", 3072, NULL, 1, NULL) != pdPASS)
        ESP_LOGW(TAG, "Battery worker unavailable");
    uint32_t active = milliseconds(), logged = 0;
    bool dim = false;
    for (;;) {
        dinner_key_t key;
        bool got = xQueueReceive(inputs, &key, pdMS_TO_TICKS(30)) == pdTRUE;
        uint32_t now = milliseconds();
        dinner_settings_t pending;
        bool save = false;
        if (got) {
            active = now;
            if (dim) {
                bsp_display_backlight(75);
                dim = false;
            }
        }
        if (!dim && now - active >= 60000) {
            bsp_display_backlight(8);
            dim = true;
        }
        if (bsp_lvgl_lock(100)) {
            if (got) {
                dinner_key(&app, key, now);
                dinner_ui_render(&app);
            }
            if (app.dirty) {
                pending = app.settings;
                app.dirty = false;
                save = true;
            }
            if (app.draws != logged && app.draws % 100 == 0) {
                logged = app.draws;
                ESP_LOGI(TAG, "draws=%lu heap=%u min=%u largest=%u", (unsigned long)logged,
                         (unsigned)esp_get_free_heap_size(),
                         (unsigned)esp_get_minimum_free_heap_size(),
                         (unsigned)heap_caps_get_largest_free_block(MALLOC_CAP_8BIT));
            }
            bsp_lvgl_unlock();
        }
        if (save) {
            bool ok = dinner_storage_save(&pending);
            if (bsp_lvgl_lock(1000)) {
                app.storage_failed = !ok;
                if (!ok) {
                    dinner_defaults(&app.settings);
                    ESP_LOGW(TAG, "NVS save failed; using defaults in RAM");
                }
                dinner_ui_render(&app);
                bsp_lvgl_unlock();
            }
        }
    }
}
