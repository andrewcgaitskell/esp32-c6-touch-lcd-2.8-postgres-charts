#include "ui.h"
#include "board.h"
#include "api_client.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "ui";

/* ---- register screens here ---- */
extern const screen_t screen_temperature;
static const screen_t *const screens[] = {
    &screen_temperature,
};
#define N_SCREENS ((int)(sizeof(screens) / sizeof(screens[0])))

static lv_obj_t *s_obj[N_SCREENS];
static volatile int  s_cur;
static volatile bool s_force = true;     /* fetch now (on start and on switch) */

static void on_gesture(lv_event_t *e)
{
    lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_active());
    if (dir == LV_DIR_LEFT)       ui_show(s_cur + 1);
    else if (dir == LV_DIR_RIGHT) ui_show(s_cur - 1);
}

/* Call with the LVGL lock held (gesture callbacks already do). */
void ui_show(int index)
{
    if (index < 0 || index >= N_SCREENS || index == s_cur) return;
    s_cur = index;
    s_force = true;
    lv_screen_load(s_obj[index]);
}

static void poll_task(void *arg)
{
    static char buf[4096];
    int64_t last = 0;

    for (;;) {
        const screen_t *s = screens[s_cur];
        int64_t now = esp_timer_get_time() / 1000;
        int64_t every = (int64_t)s->poll_s * 1000;

        if (s->endpoint && (s_force || now - last >= every)) {
            s_force = false;
            if (api_get(s->endpoint, buf, sizeof(buf)) == ESP_OK) {
                if (board_lvgl_lock(1000)) {
                    s->on_data(buf);
                    board_lvgl_unlock();
                }
                last = now;
            } else {
                ESP_LOGW(TAG, "fetch failed for %s, retry in 5 s", s->name);
                last = now - every + 5000;
            }
        }
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void ui_init(void)
{
    if (board_lvgl_lock(1000)) {
        for (int i = 0; i < N_SCREENS; i++) {
            s_obj[i] = screens[i]->create();
            lv_obj_add_event_cb(s_obj[i], on_gesture, LV_EVENT_GESTURE, NULL);
        }
        lv_screen_load(s_obj[0]);
        board_lvgl_unlock();
    }
    xTaskCreate(poll_task, "ui_poll", 8192, NULL, 5, NULL);
}
