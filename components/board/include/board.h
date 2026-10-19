#pragma once
#include <stdbool.h>

/* Brings up LCD, touch and LVGL. The only board-specific code in the project. */
void board_init(void);

/* Wrap any LVGL call made outside the LVGL task. */
bool board_lvgl_lock(int timeout_ms);
void board_lvgl_unlock(void);
