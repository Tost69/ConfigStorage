/****************************************************************************************************************************
  ConfigStorage_ESP8266.ino
  for ESP8266 boards
  built by Tost69 https://github.com/Tost69/ConfigStorage
  licensed under MIT license
 *****************************************************************************************************************************/

#if defined(ESP8266)
#  define USE_LITTLEFS true
#  define ESP_DRD_USE_LITTLEFS true
#else
#  error This code is intended to run on the ESP8266 or ESP32 platform! Please check your Tools->Board setting.
#endif

#include <ArduinoJson.h>   // https://github.com/bblanchon/ArduinoJson
#include <ConfigStorage.h> // https://github.com/Tost69/ConfigStorage

void setup()
{
}

void loop()
{
}
