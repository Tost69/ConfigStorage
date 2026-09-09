## ConfigStorage

ConfigStorage is a small library for the ESP32/ESP8266 Arduino platform to store configuration parameters in the file system using LittleFS or SPIFFS.
Using the [ArduinoJson](https://arduinojson.org/) library the parameters are kept in memory as a JSON document and saved as a JSON file.

[![GitHub](https://img.shields.io/github/license/mashape/apistatus.svg)](https://github.com/Tost69/ConfigStorage/blob/main/LICENSE)
[![Build](https://github.com/Tost69/ConfigStorage/actions/workflows/build.yml/badge.svg)](https://github.com/Tost69/ConfigStorage/actions/workflows/build.yml)

---
---

## Table of Contents

* [Changelog](changelog.md)
* [Prerequisites](#prerequisites)
* [Installation](#installation)
  * [Use Arduino Library Manager](#use-arduino-library-manager)
  * [Manual Install](#manual-install)
  * [VS Code & PlatformIO](#vs-code--platformio)
* [HOWTO Usage](#howto-usage)
  * [Minimal sketch](#minimal-sketch)
  * [API](#api)
  * [Compile-time options](#compile-time-options)
  * [Migrating from v0.0.x](#migrating-from-v00x)
* [Examples](#examples)
* [Debug](#debug)
* [Troubleshooting](#troubleshooting)
* [Issues](#issues)
* [License](#license)

---
---

## Prerequisites

1. [`Arduino IDE 1.8.19+` or `Arduino IDE 2.x`](https://www.arduino.cc/en/software), or [PlatformIO](https://platformio.org/)
2. [`ESP32 Core 2.0.0+`](https://github.com/espressif/arduino-esp32) for ESP32-based boards (tested against 3.x). LittleFS is part of the core.
3. [`ESP8266 Core 3.0.0+`](https://github.com/esp8266/Arduino) for ESP8266-based boards. LittleFS is part of the core; SPIFFS is deprecated since 2.7.1.
4. [`ArduinoJson 7.x`](https://github.com/bblanchon/ArduinoJson) (6.21+ still works, see [Compile-time options](#compile-time-options)). To install, check [![arduino-library-badge](https://www.ardu-badge.com/badge/ArduinoJson.svg?)](https://www.ardu-badge.com/ArduinoJson).

The `LittleFS_esp32` library that older versions of ConfigStorage required is **not** needed anymore.

---
---

## Installation

### Use Arduino Library Manager
Search for `ConfigStorage` in the Arduino Library Manager and install the latest version, or use this link [![arduino-library-badge](https://www.ardu-badge.com/badge/ConfigStorage.svg?)](https://www.ardu-badge.com/ConfigStorage) for more detailed instructions.

### Manual Install

1. Navigate to the [ConfigStorage](https://github.com/Tost69/ConfigStorage) page.
2. Download the latest release or `ConfigStorage-main.zip`.
3. Extract the zip file to a `ConfigStorage` directory.
4. Copy the whole `ConfigStorage` folder to the Arduino libraries directory such as `~/Arduino/libraries/`.

### VS Code & PlatformIO

1. Install [VS Code](https://code.visualstudio.com/)
2. Install [PlatformIO](https://platformio.org/platformio-ide)
3. Add the library to your `platformio.ini`:

```ini
lib_deps =
  bblanchon/ArduinoJson@^7
  https://github.com/Tost69/ConfigStorage
```

The [platformio/platformio.ini](platformio/platformio.ini) in this repository builds the examples for an ESP32 and an ESP8266 board and can be used as a starting point.

---
---

## HOWTO Usage

### Minimal sketch

```cpp
#include <ArduinoJson.h>
#include <ConfigStorage.h>

ConfigStorage storage("/config.json");   // file name in the file system

void setup()
{
  Serial.begin(115200);

  // mount the file system and read the file; true = format if the file system cannot be mounted
  if (!storage.begin(true))
  {
    Serial.println("file system not available");
    return;
  }

  JsonDocument &config = storage.get();  // the document lives inside the library

  int  bootCount = config["bootCount"] | 0;        // read with a default
  config["bootCount"] = bootCount + 1;             // change
  storage.save();                                  // write to the file
}

void loop() {}
```

`begin()` must be called from `setup()` (or later) - not from a constructor of a global object, because the file system
and `Serial` are not initialised before `setup()` runs.

### API

| Method | Description |
|---|---|
| `ConfigStorage(const char *path)` | Creates the object; nothing is touched yet. `path` is the file name, e.g. `"/config.json"`. |
| `bool begin(bool formatOnFail = false)` | Mounts the file system and loads the file. Returns `false` if the file system cannot be mounted. With `formatOnFail = true` an unmountable file system is formatted first (erases all files - needed once after switching between LittleFS and SPIFFS). |
| `JsonDocument &get()` | The configuration document. Read and write it directly with the ArduinoJson API. |
| `void set(const JsonDocument &doc)` | Replaces the document with a copy of `doc`. |
| `bool save()` | Writes the document to the file. |
| `bool revert()` | Discards the document in memory and reloads the file. |
| `void clear()` | Discards the document in memory (the file is untouched until `save()`). |
| `bool remove()` | Deletes the file (the document in memory is kept). |
| `bool isMounted()` | `true` after a successful `begin()`. |
| `bool isLoaded()` | `true` if the last `begin()`/`revert()` found and parsed a file. |
| `const char *path()` | The file name. |

### Compile-time options

Define these in your sketch **before** `#include <ConfigStorage.h>`:

| Define | Default | Description |
|---|---|---|
| `CS_USE_SPIFFS` | `false` | Use SPIFFS instead of LittleFS. |
| `CS_USE_LITTLEFS` | `true` | Use LittleFS (the default). |
| `CONFIGSTORAGE_DEBUG` | `false` | Print what the library does to `CONFIGSTORAGE_DEBUG_PORT`. |
| `CONFIGSTORAGE_DEBUG_PORT` | `Serial` | Where debug messages go. |
| `CONFIGSTORAGE_JSON_CAPACITY` | `1024` | Only with ArduinoJson 6: size of the `DynamicJsonDocument` in bytes. With ArduinoJson 7 the document grows as needed. |

### Migrating from v0.0.x

* Call `storage.begin()` in `setup()` - the constructor no longer mounts the file system.
* `get()` now returns a `JsonDocument&` instead of a copy. Write `JsonDocument &doc = storage.get();` and work on `doc`; `save()` writes exactly that document.
* `initialize()` still exists but is now called `clear()`.
* The debug define is `CONFIGSTORAGE_DEBUG` (the old `ConfigStorage_DEBUG` is still accepted).

---
---

## Examples

1. [ConfigStorage_Basic](examples/ConfigStorage_Basic) - boot counter and a few parameters; shows loading, reading with defaults, changing and saving.
2. [ConfigStorage_Defaults](examples/ConfigStorage_Defaults) - the pattern for a real project: a settings struct with defaults, plus a small serial menu to save, revert and delete the configuration.

Both examples run unchanged on ESP32 and ESP8266.

---
---

## Debug

```cpp
#define CONFIGSTORAGE_DEBUG true   // before including ConfigStorage.h
#include <ConfigStorage.h>
```

prints messages like `LittleFS mounted`, `Config file missing`, `Saving config file OK` and the ArduinoJson error text if a file cannot be parsed.

---
---

## Troubleshooting

* **`begin()` returns false** - the file system could not be mounted. On a fresh board, or after switching between LittleFS and SPIFFS, call `begin(true)` once to format it. On the ESP32 also check that the selected partition scheme contains a SPIFFS/LittleFS partition.
* **`isLoaded()` is false although the file exists** - the file is not valid JSON. Enable `CONFIGSTORAGE_DEBUG` to see the parser error, or `remove()` the file.
* **Values are missing after `save()` with ArduinoJson 6** - the document is full. Increase `CONFIGSTORAGE_JSON_CAPACITY`. (ArduinoJson 7 does not have this limit.)
* **Compiler complains about `SPIFFS` being deprecated on the ESP8266** - that is a warning from the core; use LittleFS unless you have to read an existing SPIFFS image.

---
---

## Issues

Submit issues to: [ConfigStorage issues](https://github.com/Tost69/ConfigStorage/issues)

---
---

## License

- The library is licensed under [MIT](https://github.com/Tost69/ConfigStorage/blob/main/LICENSE)

Copyright (c) 2022- Tost69
