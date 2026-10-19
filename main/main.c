#include "nvs_flash.h"
#include "board.h"
#include "wifi.h"
#include "api_client.h"
#include "ui.h"
#include "app_config.h"

void app_main(void)
{
    ESP_ERROR_CHECK(nvs_flash_init());

    board_init();                      /* display, touch, LVGL (from the 2048 example) */
    api_set_base(API_BASE);
    ui_init();                         /* builds screens, starts the polling task */
    wifi_start(WIFI_SSID, WIFI_PASS);  /* polling retries until the network is up */
}
