/****************************************************************************************************************************
ConfigStorage.h
for ESP32 and ESP8266 boards

Version Modified By   Date      Comments
------- -----------  ---------- -----------
0.0.1   Tom          20/04/2022 Initial coding
0.0.2   ReinaldoAF   17/01/2023 Include #ifdef CONFIGSTORAGE_DEBUG in Serial printing informations 
															  Removed useless commented lines
*****************************************************************************************************************************/

#pragma once

#ifndef ConfigStorage_H
#  define ConfigStorage_H

#  ifndef ConfigStorage_DEBUG
#    define ConfigStorage_DEBUG false
#  endif // #ifndef ConfigStorage_DEBUG

#  if defined(ARDUINO) && (ARDUINO >= 100)
#    include <Arduino.h>
#  else  // defined(ARDUINO) && (ARDUINO >= 100)
#    include <WProgram.h>
#  endif // #defined(ARDUINO) && (ARDUINO >= 100)

#  ifndef CONFIG_STORAGE_VERSION
#    define CONFIG_STORAGE_VERSION             "ConfigStorage v0.0.2"
#    define CONFIG_STORAGE_VERSION_MAJOR       0
#    define CONFIG_STORAGE_VERSION_MINOR       0
#    define CONFIG_STORAGE_VERSION_PATCH       2
#    define CONFIG_STORAGE_VERSION_INT         0000002
#  endif // #ifndef CONFIG_STORAGE_VERSION

#  define ConfigStorage_VERSION CONFIG_STORAGE_VERSION

#  ifdef ESP32
#    include <ArduinoJson.h>
#    if (!CS_USE_SPIFFS && !CS_USE_LITTLEFS)

#      if (ConfigStorage_DEBUG)
#        warning Neither LittleFS nor SPIFFS selected. Default to LittleFS!
#      endif // #if (ConfigStorage_DEBUG)

#      ifdef CS_USE_LITTLEFS
#        undef CS_USE_LITTLEFS
#        define CS_USE_LITTLEFS true
#      endif // #ifdef CS_USE_LITTLEFS

#    endif // #if (!CS_USE_SPIFFS && !CS_USE_LITTLEFS)
#  endif // #ifdef ESP32

#  ifdef ESP8266
#    include <ArduinoJson.h>
#    if (!CS_USE_SPIFFS && !CS_USE_LITTLEFS)

#      if (ConfigStorage_DEBUG)
#        warning Neither LittleFS nor SPIFFS selected. Default to LittleFS!
#      endif // #if (ConfigStorage_DEBUG)

#      ifdef CS_USE_LITTLEFS
#        undef CS_USE_LITTLEFS
#        define CS_USE_LITTLEFS	true
#      endif // #ifdef CS_USE_LITTLEFS

#    endif // #if (!CS_USE_SPIFFS && !CS_USE_LITTLEFS)
#  endif // #ifdef ESP8266

// default to use LITTLEFS (higher priority), then SPIFFS
#  if ( CS_USE_LITTLEFS || CS_USE_SPIFFS )
#    include <FS.h>
#    ifdef ESP32
#      if CS_USE_LITTLEFS
//       check cores/esp32/esp_arduino_version.h and cores/esp32/core_version.h
#        if ( defined(ESP_ARDUINO_VERSION_MAJOR) && (ESP_ARDUINO_VERSION_MAJOR >= 2) )
#          if (ConfigStorage_DEBUG)
#            warning Using ESP32 Core 1.0.6 or 2.0.0+
#          endif // #if (ConfigStorage_DEBUG)

//         the library has been merged into esp32 core from release 1.0.6
#          include <LittleFS.h>
#          define FileFS	LittleFS

#        else // #if ( defined(ESP_ARDUINO_VERSION_MAJOR) && (ESP_ARDUINO_VERSION_MAJOR >= 2) )

#          if (ConfigStorage_DEBUG)
#            warning Using ESP32 Core 1.0.5-. You must install LITTLEFS library
#          endif // #if (ConfigStorage_DEBUG)

//         the library has been merged into esp32 core from release 1.0.6
#          include <LITTLEFS.h>             // https://github.com/lorol/LITTLEFS
#          define FileFS	LITTLEFS

#        endif // #if ( defined(ESP_ARDUINO_VERSION_MAJOR) && (ESP_ARDUINO_VERSION_MAJOR >= 2) )

#      else // #if CS_USE_LITTLEFS

//       ESP32 core 1.0.4 still uses SPIFFS
#        include "SPIFFS.h"
#        define FileFS	SPIFFS

#      endif // #if CS_USE_LITTLEFS

#    else // #ifdef ESP32

//     from ESP8266 core 2.7.1
#      include <LittleFS.h>

#      if CS_USE_LITTLEFS
#        define FileFS    LittleFS
#      else // #if CS_USE_LITTLEFS
#        define FileFS   SPIFFS
#      endif // #if CS_USE_LITTLEFS

#    endif // #ifdef ESP32   
#  endif // #if ( CS_USE_LITTLEFS || CS_USE_SPIFFS )   

class ConfigStorage
{
public:
  ConfigStorage(const char *path)
  {
    if (!FileFS.begin())
    {
		#ifdef CONFIGSTORAGE_DEBUG
      Serial.println("LittleFS failed");
		#endif
    }
    else
    {
		#ifdef CONFIGSTORAGE_DEBUG
			Serial.println("LittleFS started");
		#endif
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
				#ifdef CONFIGSTORAGE_DEBUG
        Serial.println("Config file exists");
				#endif
        File configFile = FileFS.open(fileName, "r");
        if (!configFile)
        {
					#ifdef CONFIGSTORAGE_DEBUG
          Serial.println("Reading config file failed");
					#endif
        }
        else
        {
					#ifdef CONFIGSTORAGE_DEBUG
          Serial.println("Reading config file OK");
					#endif
          deserializeJson(this->configDoc, configFile);
          configFile.close();
        }
      }
      else
      {
				#ifdef CONFIGSTORAGE_DEBUG
        Serial.println("Config file missing");
				#endif
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
				#ifdef CONFIGSTORAGE_DEBUG
        Serial.println("Saving config file OK");
				#endif
      }
      else
      {
				#ifdef CONFIGSTORAGE_DEBUG
        Serial.println("Saving config file failed");
				#endif
      }
    }
  };

  void removeConfigFile()
  {
    if (FileFS.begin())
    {
      if (FileFS.exists(fileName))
      {
				#ifdef CONFIGSTORAGE_DEBUG
        Serial.println("Config file exists");
				#endif
        if (FileFS.remove(fileName))
        {
					#ifdef CONFIGSTORAGE_DEBUG
          Serial.println("Removing config file OK");
					#endif
        }
        else
        {
					#ifdef CONFIGSTORAGE_DEBUG
          Serial.println("Removing config file failed");
					#endif
        }
      }
      else
      {
				#ifdef CONFIGSTORAGE_DEBUG
        Serial.println("Config file missing");
				#endif
      }
    }
  };
};
#endif // ConfigStorage_H