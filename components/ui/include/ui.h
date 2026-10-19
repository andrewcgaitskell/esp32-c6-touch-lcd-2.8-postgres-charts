#pragma once
#include "lvgl.h"

/* LVGL 9.3.0 (pinned in main/idf_component.yml) */
#define UI_CHART_RANGE(c, mn, mx)  lv_chart_set_axis_range(c, LV_CHART_AXIS_PRIMARY_Y, mn, mx)
#define UI_CHART_CLEAR(c, s)       lv_chart_set_all_values(c, s, LV_CHART_POINT_NONE)

/* A screen is one of these. Add a new file under screens/, define one
 * screen_t, and list it in ui.c. Swipe left/right moves between screens. */
typedef struct {
    const char *name;
    const char *endpoint;                 /* GET path on the API, or NULL if static */
    unsigned    poll_s;                   /* refresh interval while visible */
    lv_obj_t  *(*create)(void);           /* build and return an lv_obj_t screen */
    void      (*on_data)(const char *json); /* called with the LVGL lock held */
} screen_t;

void ui_init(void);
void ui_show(int index);
