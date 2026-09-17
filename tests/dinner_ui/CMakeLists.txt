cmake_minimum_required(VERSION 3.16)
project(dinner_ui_test C)
set(CMAKE_C_STANDARD 11)
set(LV_CONF_PATH "${CMAKE_CURRENT_SOURCE_DIR}/lv_conf.h" CACHE STRING "" FORCE)
set(LV_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(LV_BUILD_DEMOS OFF CACHE BOOL "" FORCE)
set(ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../..")
add_subdirectory("${ROOT}/managed_components/lvgl__lvgl" lvgl)
add_executable(dinner_ui_test test_ui.c
 "${ROOT}/main/dinner_ui.c" "${ROOT}/main/dinner_decoration.c" "${ROOT}/main/dinner_food_database.c"
 "${ROOT}/main/dinner_navigation.c" "${ROOT}/main/dinner_settings.c" "${ROOT}/main/dinner_random_selector.c"
 "${ROOT}/assets/fonts/dinner_font_14.c" "${ROOT}/assets/fonts/dinner_font_20.c"
 "${ROOT}/assets/fonts/dinner_font_30.c" "${ROOT}/assets/fonts/dinner_font_inventory.c")
target_include_directories(dinner_ui_test PRIVATE "${ROOT}/main")
target_link_libraries(dinner_ui_test PRIVATE lvgl)
