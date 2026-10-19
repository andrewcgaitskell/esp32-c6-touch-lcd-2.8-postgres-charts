idf_component_register(SRCS "ui.c" "screens/screen_temperature.c"
                       INCLUDE_DIRS "include"
                       REQUIRES board api_client json)
