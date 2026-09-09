/****************************************************************************************************************************
ConfigStorage.h
for ESP32 and ESP8266 boards

Store configuration parameters as a JSON document in the file system (LittleFS or SPIFFS).

Version Modified By   Date        Comments
------- -----------   ----------  -----------
0.0.1   Tom           20/04/2022  Initial coding
0.1.0   Tom           09/09/2026  ArduinoJson 7 (JsonDocument), begin() API, LittleFS as real default,
                                  CONFIGSTORAGE_DEBUG controls all serial output (idea from PR #3 by ReinaldoAF)
*****************************************************************************************************************************/

#pragma once

#ifndef ConfigStorage_H
#define ConfigStorage_H

#if defined(ARDUINO) && (ARDUINO >= 100)
#include <Arduino.h>
#else
#include <WProgram.h>
#endif

#include <ArduinoJson.h>

/*--------------------------------------------------------------------------------------------------------------------------
  Version
--------------------------------------------------------------------------------------------------------------------------*/
#ifndef CONFIG_STORAGE_VERSION
#define CONFIG_STORAGE_VERSION        "ConfigStorage v0.1.0"
#define CONFIG_STORAGE_VERSION_MAJOR  0
#define CONFIG_STORAGE_VERSION_MINOR  1
#define CONFIG_STORAGE_VERSION_PATCH  0
#define CONFIG_STORAGE_VERSION_INT    0001000
#endif

#define ConfigStorage_VERSION CONFIG_STORAGE_VERSION

/*--------------------------------------------------------------------------------------------------------------------------
  Debug output
  Define CONFIGSTORAGE_DEBUG true in your sketch *before* including ConfigStorage.h to get messages on Serial.
  (ConfigStorage_DEBUG is accepted as well for backwards compatibility.)
--------------------------------------------------------------------------------------------------------------------------*/
#if !defined(CONFIGSTORAGE_DEBUG) && defined(ConfigStorage_DEBUG)
#define CONFIGSTORAGE_DEBUG ConfigStorage_DEBUG
#endif

#ifndef CONFIGSTORAGE_DEBUG
#define CONFIGSTORAGE_DEBUG false
#endif

#ifndef CONFIGSTORAGE_DEBUG_PORT
#define CONFIGSTORAGE_DEBUG_PORT Serial
#endif

#if CONFIGSTORAGE_DEBUG
#define CS_LOG(x)  CONFIGSTORAGE_DEBUG_PORT.println(F(x))
#else
#define CS_LOG(x)  do {} while (0)
#endif

/*--------------------------------------------------------------------------------------------------------------------------
  File system selection
  Default is LittleFS. To use SPIFFS instead, define CS_USE_SPIFFS true before including ConfigStorage.h.
  (SPIFFS is deprecated on both platforms and kept only for existing images.)
--------------------------------------------------------------------------------------------------------------------------*/
#ifndef CS_USE_SPIFFS
#define CS_USE_SPIFFS false
#endif

#ifndef CS_USE_LITTLEFS
#if CS_USE_SPIFFS
#define CS_USE_LITTLEFS false
#else
#define CS_USE_LITTLEFS true
#endif
#endif

#if CS_USE_LITTLEFS && CS_USE_SPIFFS
#error "ConfigStorage: define either CS_USE_LITTLEFS or CS_USE_SPIFFS, not both"
#endif

#include <FS.h>

#if defined(ESP32)
#if CS_USE_LITTLEFS
// LittleFS is part of the ESP32 Arduino core since 2.0.0 (1.0.6 shipped it as LITTLEFS)
#include <LittleFS.h>
#define FileFS  LittleFS
#define FS_Name "LittleFS"
#else
#include <SPIFFS.h>
#define FileFS  SPIFFS
#define FS_Name "SPIFFS"
#endif
#elif defined(ESP8266)
#if CS_USE_LITTLEFS
// LittleFS is part of the ESP8266 Arduino core since 2.7.1
#include <LittleFS.h>
#define FileFS  LittleFS
#define FS_Name "LittleFS"
#else
// SPIFFS is declared in FS.h on the ESP8266 core (deprecated)
#define FileFS  SPIFFS
#define FS_Name "SPIFFS"
#endif
#else
#error "ConfigStorage: only ESP32 and ESP8266 boards are supported"
#endif

/*--------------------------------------------------------------------------------------------------------------------------
  JSON document
  ArduinoJson 7 has a single, self-growing JsonDocument. With ArduinoJson 6 a DynamicJsonDocument of
  CONFIGSTORAGE_JSON_CAPACITY bytes is used instead.
--------------------------------------------------------------------------------------------------------------------------*/
#ifndef CONFIGSTORAGE_JSON_CAPACITY
#define CONFIGSTORAGE_JSON_CAPACITY 1024
#endif

#if ARDUINOJSON_VERSION_MAJOR >= 7
#define CS_JSON_DOCUMENT_INIT()   configDoc()
#elif ARDUINOJSON_VERSION_MAJOR == 6
#define CS_JSON_DOCUMENT_INIT()   configDoc(CONFIGSTORAGE_JSON_CAPACITY)
#else
#error "ConfigStorage: ArduinoJson 6.x or 7.x required"
#endif

class ConfigStorage
{
public:
  /// path: file name in the file system, e.g. "/config.json"
  explicit ConfigStorage(const char *path) : CS_JSON_DOCUMENT_INIT(), fileName(path), mounted(false)
  {
  }

  /// Mount the file system and load the configuration file. Call from setup().
  /// formatOnFail: format the file system if it cannot be mounted (erases all files!)
  /// Returns true if the file system is mounted (regardless of whether a configuration file existed).
  bool begin(bool formatOnFail = false)
  {
    mounted = mountFS(formatOnFail);

    if (!mounted)
    {
      CS_LOG(FS_Name " mount failed");
      return false;
    }

    CS_LOG(FS_Name " mounted");
    readConfigFile();
    return true;
  }

  /// The configuration document. Read and modify it directly, then call save().
  JsonDocument &get()
  {
    return configDoc;
  }

  /// Replace the configuration document with a copy of another one.
  void set(const JsonDocument &document)
  {
    configDoc.set(document);
  }

  /// Discard all parameters in memory (the file is untouched until save()).
  void clear()
  {
    configDoc.clear();
  }

  /// Discard changes in memory and reload the file.
  bool revert()
  {
    clear();
    return readConfigFile();
  }

  /// Write the document to the file.
  bool save()
  {
    return writeConfigFile();
  }

  /// Delete the file (the document in memory is kept).
  bool remove()
  {
    return removeConfigFile();
  }

  /// true if the file system is mounted (begin() succeeded)
  bool isMounted() const
  {
    return mounted;
  }

  /// true if the last begin()/revert() found and parsed a configuration file
  bool isLoaded() const
  {
    return loaded;
  }

  const char *path() const
  {
    return fileName;
  }

  // ---- kept for sketches written against v0.0.x ----
  void initialize()
  {
    clear();
  }

private:
#if ARDUINOJSON_VERSION_MAJOR >= 7
  JsonDocument configDoc;
#else
  DynamicJsonDocument configDoc;
#endif
  const char *fileName;
  bool mounted;
  bool loaded = false;

  bool mountFS(bool formatOnFail)
  {
#if defined(ESP32)
    return FileFS.begin(formatOnFail);
#else
    if (FileFS.begin())
    {
      return true;
    }
    if (formatOnFail)
    {
      CS_LOG("Formatting " FS_Name);
      return FileFS.format() && FileFS.begin();
    }
    return false;
#endif
  }

  bool readConfigFile()
  {
    loaded = false;

    if (!mounted)
    {
      CS_LOG(FS_Name " not mounted");
      return false;
    }

    if (!FileFS.exists(fileName))
    {
      CS_LOG("Config file missing");
      return false;
    }

    File configFile = FileFS.open(fileName, "r");
    if (!configFile)
    {
      CS_LOG("Opening config file failed");
      return false;
    }

    DeserializationError error = deserializeJson(configDoc, configFile);
    configFile.close();

    if (error)
    {
      CS_LOG("Parsing config file failed");
#if CONFIGSTORAGE_DEBUG
      CONFIGSTORAGE_DEBUG_PORT.println(error.c_str());
#endif
      configDoc.clear();
      return false;
    }

    CS_LOG("Reading config file OK");
    loaded = true;
    return true;
  }

  bool writeConfigFile()
  {
    if (!mounted)
    {
      CS_LOG(FS_Name " not mounted");
      return false;
    }

    File configFile = FileFS.open(fileName, "w");
    if (!configFile)
    {
      CS_LOG("Saving config file failed");
      return false;
    }

    size_t written = serializeJson(configDoc, configFile);
    configFile.close();

    if (written == 0)
    {
      CS_LOG("Saving config file failed (nothing written)");
      return false;
    }

    CS_LOG("Saving config file OK");
    return true;
  }

  bool removeConfigFile()
  {
    if (!mounted)
    {
      CS_LOG(FS_Name " not mounted");
      return false;
    }

    if (!FileFS.exists(fileName))
    {
      CS_LOG("Config file missing");
      return false;
    }

    if (!FileFS.remove(fileName))
    {
      CS_LOG("Removing config file failed");
      return false;
    }

    CS_LOG("Removing config file OK");
    return true;
  }
};

#endif // ConfigStorage_H
