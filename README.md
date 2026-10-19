# esp32c6_screens

ESP-IDF 5.5.4 + LVGL 9.3.0 project for the Waveshare ESP32-C6-Touch-LCD-2.8.
Built on the Waveshare 02_2048 example's board support package. The board polls
your Quart app over HTTP; Postgres is the only source of truth, no MQTT.

## Layout

    main/                    app_main, app_config.h (Wi-Fi, API URL; git-ignored)
    components/
      esp32_c6_touch_lcd_2_8/  Waveshare BSP, copied unchanged from the example
      waveshare__esp_lcd_touch_cst3530/, pedrominatel__shtc3/   also from the example
      board/                 thin wrapper: board_init(), board_lvgl_lock/unlock
      wifi/                  station connect, auto-reconnect
      api_client/            api_get(path, buf, cap)
      ui/                    screen_t registry, swipe navigation, polling task
      ui/screens/            one file per screen

## Build

1. `cp main/app_config.example.h main/app_config.h` and edit Wi-Fi + API_BASE
   (Pi's IP address, not a .local name).
2. Activate ESP-IDF v5.5.4, then:
   `idf.py build flash monitor`

## Add a screen

1. Copy `components/ui/screens/screen_temperature.c`, rename, change `create`,
   `on_data`, `endpoint`, `poll_s`.
2. Add the file to `SRCS` in `components/ui/CMakeLists.txt`.
3. In `components/ui/ui.c` add `extern const screen_t screen_xxx;` and list it
   in `screens[]`.

Swipe left/right to move between screens. Only the visible screen polls.
