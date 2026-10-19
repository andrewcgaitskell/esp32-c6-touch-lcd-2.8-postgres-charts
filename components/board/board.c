#include "board.h"
#include "bsp/esp-bsp.h"
#include "bsp/display.h"

void board_init(void)
{
    bsp_display_start();           /* panel, touch and LVGL (esp_lvgl_adapter) */
    bsp_display_backlight_on();
}

bool board_lvgl_lock(int timeout_ms)
{
    return bsp_display_lock(timeout_ms) == ESP_OK;
}

void board_lvgl_unlock(void)
{
    bsp_display_unlock();
}
