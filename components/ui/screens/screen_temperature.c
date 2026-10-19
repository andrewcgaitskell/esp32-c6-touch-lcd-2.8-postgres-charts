/* Temperature screen: one line per thermometer, hourly averages, last 24 h.
 * Data: GET /api/temperatures?hours=24
 *   {"hours":24,"devices":[{"name":"...","age":123,"t10":[225,null,...]}]}
 * t10 = tenths of a degree, oldest first, null = no readings that hour. */
#include <stdio.h>
#include "ui.h"
#include "cJSON.h"

#define N_HOURS 24
#define MAX_DEV 4

static lv_obj_t *chart;
static lv_obj_t *legend[MAX_DEV];
static lv_chart_series_t *ser[MAX_DEV];
static const lv_palette_t pal[MAX_DEV] = {
    LV_PALETTE_RED, LV_PALETTE_BLUE, LV_PALETTE_GREEN, LV_PALETTE_ORANGE };

static lv_obj_t *create(void)
{
    lv_obj_t *scr = lv_obj_create(NULL);

    lv_obj_t *title = lv_label_create(scr);
    lv_label_set_text(title, "Temperature, last 24 h");
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 6);

    chart = lv_chart_create(scr);
    lv_obj_set_size(chart, lv_pct(92), 190);
    lv_obj_align(chart, LV_ALIGN_TOP_MID, 0, 32);
    lv_chart_set_type(chart, LV_CHART_TYPE_LINE);
    lv_chart_set_point_count(chart, N_HOURS);
    lv_chart_set_div_line_count(chart, 4, 5);

    for (int i = 0; i < MAX_DEV; i++) {
        ser[i] = lv_chart_add_series(chart, lv_palette_main(pal[i]),
                                     LV_CHART_AXIS_PRIMARY_Y);
        legend[i] = lv_label_create(scr);
        lv_label_set_text(legend[i], "");
        lv_obj_set_style_text_color(legend[i], lv_palette_main(pal[i]), 0);
        lv_obj_align(legend[i], LV_ALIGN_TOP_LEFT, 10, 232 + i * 18);
    }
    return scr;
}

static void on_data(const char *json)
{
    cJSON *root = cJSON_Parse(json);
    cJSON *devs = root ? cJSON_GetObjectItem(root, "devices") : NULL;
    if (!cJSON_IsArray(devs)) { cJSON_Delete(root); return; }

    /* Y range from the data, with a little headroom */
    int lo = 100000, hi = -100000;
    cJSON *d, *v;
    cJSON_ArrayForEach(d, devs) {
        cJSON_ArrayForEach(v, cJSON_GetObjectItem(d, "t10")) {
            if (!cJSON_IsNumber(v)) continue;
            if (v->valueint < lo) lo = v->valueint;
            if (v->valueint > hi) hi = v->valueint;
        }
    }
    if (hi >= lo) UI_CHART_RANGE(chart, lo - 10, hi + 10);

    for (int i = 0; i < MAX_DEV; i++) {
        UI_CHART_CLEAR(chart, ser[i]);
        lv_label_set_text(legend[i], "");
    }

    int i = 0;
    cJSON_ArrayForEach(d, devs) {
        if (i >= MAX_DEV) break;
        int last = 0, have = 0, j = 0;
        cJSON_ArrayForEach(v, cJSON_GetObjectItem(d, "t10")) {
            if (j >= N_HOURS) break;
            if (cJSON_IsNumber(v)) {
                lv_chart_set_series_value_by_id(chart, ser[i], j, v->valueint);
                last = v->valueint;
                have = 1;
            }
            j++;
        }

        const char *name = cJSON_GetObjectItem(d, "name")
                         ? cJSON_GetObjectItem(d, "name")->valuestring : "?";
        int age = cJSON_GetObjectItem(d, "age")
                ? cJSON_GetObjectItem(d, "age")->valueint : 0;
        char stale[16] = "";
        if (age >= 3600) snprintf(stale, sizeof(stale), "  %dh ago", age / 3600);

        if (have) lv_label_set_text_fmt(legend[i], "%.16s  %.1f C%s", name, last / 10.0f, stale);
        else      lv_label_set_text_fmt(legend[i], "%.16s  no data", name);
        i++;
    }
    lv_chart_refresh(chart);
    cJSON_Delete(root);
}

const screen_t screen_temperature = {
    .name = "temperature",
    .endpoint = "/api/temperatures?hours=24",
    .poll_s = 300,
    .create = create,
    .on_data = on_data,
};
