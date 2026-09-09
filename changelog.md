## ConfigStorage

[![GitHub](https://img.shields.io/github/license/mashape/apistatus.svg)](https://github.com/Tost69/ConfigStorage/blob/main/LICENSE)

---
---

## Table of Contents

* [Changelog](#changelog)
  * [Releases v0.1.0](#releases-v010)
  * [Releases v0.0.1](#releases-v001)

---
---

## Changelog

### Releases v0.1.0

**Breaking change:** the constructor no longer touches the file system. Call `begin()` from `setup()` and use the
`JsonDocument&` returned by `get()` instead of copying documents around.

1. Support ArduinoJson 7 (`JsonDocument`); ArduinoJson 6.21+ still works (`DynamicJsonDocument` of `CONFIGSTORAGE_JSON_CAPACITY` bytes)
2. New `begin(bool formatOnFail = false)` - mounts the file system and loads the file; returns `false` if the file system is not available
3. `get()` returns a reference to the document, `set()` copies a document into it
4. `save()`, `revert()`, `remove()` return `bool`; new `clear()`, `isMounted()`, `isLoaded()`, `path()`
5. LittleFS is now really the default - previously nothing compiled unless `CS_USE_LITTLEFS` or `CS_USE_SPIFFS` was defined by the sketch
6. All serial output is controlled by `CONFIGSTORAGE_DEBUG` (default off), output port by `CONFIGSTORAGE_DEBUG_PORT` - idea from [PR #3](https://github.com/Tost69/ConfigStorage/pull/3)
7. Dropped the obsolete `LittleFS_esp32` dependency; LittleFS is part of the ESP32 core since 2.0.0
8. Examples rewritten so they compile in the Arduino IDE on ESP32 and ESP8266 ([#2](https://github.com/Tost69/ConfigStorage/issues/2), [PR #4](https://github.com/Tost69/ConfigStorage/pull/4)); new `ConfigStorage_Defaults` example ([#5](https://github.com/Tost69/ConfigStorage/issues/5))
9. GitHub Actions now compile both examples for ESP32 and ESP8266

### Releases v0.0.1

1. Add LittleFS support for ESP8266
2. Add LittleFS support for ESP32
