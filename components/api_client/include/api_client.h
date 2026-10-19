#pragma once
#include <stddef.h>
#include "esp_err.h"

void      api_set_base(const char *base_url);   /* "http://host:port" */
/* GET base+path into buf (NUL-terminated). ESP_OK only on HTTP 200. */
esp_err_t api_get(const char *path, char *buf, size_t cap);
