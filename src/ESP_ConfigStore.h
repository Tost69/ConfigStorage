/****************************************************************************************************************************
ESP_ConfigStore.h
For ESP32 and ESP8266 boards

Version Modified By   Date      Comments
------- -----------  ---------- -----------
0.0.1   Tom          20/04/2022 Initial coding
*****************************************************************************************************************************/

#pragma once

#ifndef ESP_ConfigStore_H
#define ESP_ConfigStore_H

#ifndef CONFIGSTORE_DEBUG
#define CONFIGSTORE_DEBUG       false
#endif // #ifndef CONFIGSTORE_DEBUG

#if defined(ARDUINO) && (ARDUINO >= 100)
#include <Arduino.h>
#else  // defined(ARDUINO) && (ARDUINO >= 100)
#include <WProgram.h>
#endif // #defined(ARDUINO) && (ARDUINO >= 100)

#ifndef ESP_CONFIG_STORE_VERSION
#define ESP_CONFIG_STORE_VERSION             "ESP_ConfigStore v0.0.1"
#define ESP_CONFIG_STORE_VERSION_MAJOR       0
#define ESP_CONFIG_STORE_VERSION_MINOR       0
#define ESP_CONFIG_STORE_VERSION_PATCH       1
#define ESP_CONFIG_STORE_VERSION_INT         0000001
#endif // #ifndef ESP_CONFIG_STORE_VERSION

#define ESP_CONFIGSTORE_VERSION         ESP_CONFIG_STORE_VERSION

//#define ESP_CS_USE_LITTLEFS    true
//#define ESP_CS_USE_SPIFFS      false

#ifdef ESP32
#if (!ESP_CS_USE_SPIFFS && !ESP_CS_USE_LITTLEFS)

#if (CONFIGSTORE_DEBUG)
#warning Neither LittleFS nor SPIFFS selected. Default to LittleFS
#endif // #if (CONFIGSTORE_DEBUG)

#ifdef ESP_CS_USE_LITTLEFS
#undef ESP_CS_USE_LITTLEFS
#define ESP_CS_USE_LITTLEFS      true
#endif // #ifdef ESP_CS_USE_LITTLEFS

#endif // #if (!ESP_CS_USE_SPIFFS && !ESP_CS_USE_LITTLEFS)
#endif // #ifdef ESP32

#ifdef ESP8266
#if (!ESP_CS_USE_SPIFFS && !ESP_CS_USE_LITTLEFS)

#if (CONFIGSTORE_DEBUG)
#warning Neither LittleFS nor SPIFFS selected. Default to LittleFS
#endif // #if (CONFIGSTORE_DEBUG)

#ifdef ESP_CS_USE_LITTLEFS
#undef ESP_CS_USE_LITTLEFS
#define ESP_CS_USE_LITTLEFS	true
#endif // #ifdef ESP_CS_USE_LITTLEFS

#endif // #if (!ESP_CS_USE_SPIFFS && !ESP_CS_USE_LITTLEFS)
#endif // #ifdef ESP8266

//default to use LITTLEFS (higher priority), then SPIFFS
#if ( ESP_CS_USE_LITTLEFS || ESP_CS_USE_SPIFFS )

#include <FS.h>

#ifdef ESP32

#if ESP_CS_USE_LITTLEFS

// Check cores/esp32/esp_arduino_version.h and cores/esp32/core_version.h
//#if ( ESP_ARDUINO_VERSION >= ESP_ARDUINO_VERSION_VAL(2, 0, 0) )  //(ESP_ARDUINO_VERSION_MAJOR >= 2)
#if ( defined(ESP_ARDUINO_VERSION_MAJOR) && (ESP_ARDUINO_VERSION_MAJOR >= 2) )

#if (CONFIGSTORE_DEBUG)
#warning Using ESP32 Core 1.0.6 or 2.0.0+
#endif // #if (CONFIGSTORE_DEBUG)

// The library has been merged into esp32 core from release 1.0.6
#include <LittleFS.h>

#define FileFS	LittleFS

#else // #if ( defined(ESP_ARDUINO_VERSION_MAJOR) && (ESP_ARDUINO_VERSION_MAJOR >= 2) )

#if (CONFIGSTORE_DEBUG)
#warning Using ESP32 Core 1.0.5-. You must install LITTLEFS library
#endif // #if (CONFIGSTORE_DEBUG)

// The library has been merged into esp32 core from release 1.0.6
#include <LITTLEFS.h>             // https://github.com/lorol/LITTLEFS

#define FileFS	LITTLEFS

#endif // #if ( defined(ESP_ARDUINO_VERSION_MAJOR) && (ESP_ARDUINO_VERSION_MAJOR >= 2) )

#else // #if ESP_CS_USE_LITTLEFS

#include "SPIFFS.h"
// ESP32 core 1.0.4 still uses SPIFFS
#define FileFS	SPIFFS

#endif // #if ESP_CS_USE_LITTLEFS

#else // #ifdef ESP32

// From ESP8266 core 2.7.1
#include <LittleFS.h>

#if ESP_CS_USE_LITTLEFS
#define FileFS    LittleFS
#else // #if ESP_CS_USE_LITTLEFS
#define FileFS   SPIFFS
#endif // #if ESP_CS_USE_LITTLEFS

#endif // #ifdef ESP32   

#endif // #if ( ESP_CS_USE_LITTLEFS || ESP_CS_USE_SPIFFS )   


//#include <Arduino.h>
//#include <ArduinoJson.h>
//#include "FS.h"
//#include <LITTLEFS.h>

//#define FileFS LITTLEFS
//#define FS_Name "LittleFS"

class ConfigStore
{
public:
  ConfigStore(const char *path)
  {
    if (!FileFS.begin())
    {
      Serial.println("LittleFS failed");
    }
    else
    {
      Serial.println("LittleFS started");

      fileName = path;
      readConfigFile();
    }
  };

  StaticJsonDocument<512> get()
  {
    return configDoc;
  };

  void set(DynamicJsonDocument Document)
  {
    configDoc = Document;
  };

  void initialize()
  {
    configDoc.clear();
  };

  void revert()
  {
    initialize();
    readConfigFile();
  };

  void save()
  {
    writeConfigFile();
  };

  void remove()
  {
    removeConfigFile();
  };

private:
  StaticJsonDocument<512> configDoc;
  const char *fileName;

  void readConfigFile()
  {
    if (FileFS.begin())
    {
      if (FileFS.exists(fileName))
      {
        Serial.println("Config file exists");

        File configFile = FileFS.open(fileName, "r");
        if (!configFile)
        {
          Serial.println("Reading config file failed");
        }
        else
        {
          Serial.println("Reading config file OK");

          deserializeJson(this->configDoc, configFile);
          configFile.close();
        }
      }
      else
      {
        Serial.println("Config file missing");
      }
    }
  };

  void writeConfigFile()
  {
    if (FileFS.begin())
    {
      File configFile = FileFS.open(fileName, "w");

      if (configFile)
      {
        serializeJson(configDoc, configFile);
        configFile.close();

        Serial.println("Saving config file OK");
      }
      else
      {
        Serial.println("Saving config file failed");
      }
    }
  };

  void removeConfigFile()
  {
    if (FileFS.begin())
    {
      if (FileFS.exists(fileName))
      {
        Serial.println("Config file exists");

        if (FileFS.remove(fileName))
        {
          Serial.println("Removing config file OK");
        }
        else
        {
          Serial.println("Removing config file failed");
        }
      }
      else
      {
        Serial.println("Config file missing");
      }
    }
  };
};
