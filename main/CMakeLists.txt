idf_component_register(
    SRCS "main.c" "dinner_food_database.c" "dinner_settings.c"
         "dinner_random_selector.c" "dinner_navigation.c" "dinner_storage.c" "dinner_ui.c" "dinner_decoration.c"
         "../assets/fonts/dinner_font_14.c" "../assets/fonts/dinner_font_20.c"
         "../assets/fonts/dinner_font_30.c" "../assets/fonts/dinner_font_inventory.c"
    INCLUDE_DIRS "."
    REQUIRES bsp esp_timer esp_hw_support nvs_flash
)
