#include <stdio.h>
#include <string.h>
#include "api_client.h"
#include "esp_http_client.h"
#include "esp_log.h"

static const char *TAG = "api";
static char s_base[96];

typedef struct { char *buf; size_t cap, len; } acc_t;

void api_set_base(const char *base_url)
{
    strlcpy(s_base, base_url, sizeof(s_base));
}

static esp_err_t on_http(esp_http_client_event_t *e)
{
    if (e->event_id == HTTP_EVENT_ON_DATA) {
        acc_t *a = e->user_data;
        if (a->len + e->data_len < a->cap) {
            memcpy(a->buf + a->len, e->data, e->data_len);
            a->len += e->data_len;
        } else {
            ESP_LOGW(TAG, "response larger than buffer, truncated");
        }
    }
    return ESP_OK;
}

esp_err_t api_get(const char *path, char *buf, size_t cap)
{
    char url[192];
    snprintf(url, sizeof(url), "%s%s", s_base, path);

    acc_t a = { buf, cap, 0 };
    esp_http_client_config_t cfg = {
        .url = url,
        .event_handler = on_http,
        .user_data = &a,
        .timeout_ms = 8000,
    };
    esp_http_client_handle_t c = esp_http_client_init(&cfg);
    esp_err_t err = esp_http_client_perform(c);
    int status = esp_http_client_get_status_code(c);
    esp_http_client_cleanup(c);

    buf[a.len] = 0;
    if (err == ESP_OK && status != 200) {
        ESP_LOGW(TAG, "HTTP %d for %s", status, path);
        err = ESP_FAIL;
    }
    return err;
}
