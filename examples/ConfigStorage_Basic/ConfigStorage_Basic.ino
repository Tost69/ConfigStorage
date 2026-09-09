/****************************************************************************************************************************
  ConfigStorage_Basic.ino
  for ESP32 and ESP8266 boards
  built by Tost69 https://github.com/Tost69/ConfigStorage
  licensed under MIT license

  Keeps a boot counter and a few parameters in /config.json.
  Every reset increments the counter and appends to the text - open the serial monitor to watch it.
 *****************************************************************************************************************************/

#include <ArduinoJson.h> // https://github.com/bblanchon/ArduinoJson

#define CONFIGSTORAGE_DEBUG true // show what the library does on Serial
// By default the file system is LittleFS. To use SPIFFS instead, uncomment the next line.
// #define CS_USE_SPIFFS true

#include <ConfigStorage.h> // https://github.com/Tost69/ConfigStorage

ConfigStorage storage("/config.json");

void setup()
{
  Serial.begin(115200);
  delay(500);
  Serial.println();
  Serial.println(CONFIG_STORAGE_VERSION);

  // Mount the file system and read the file. true = format the file system if it cannot be mounted
  // (this happens on a fresh board or after switching between LittleFS and SPIFFS).
  if (!storage.begin(true))
  {
    Serial.println("File system not available - check the partition scheme / flash size settings");
    while (true)
    {
      delay(1000);
    }
  }

  JsonDocument &config = storage.get();

  if (storage.isLoaded())
  {
    Serial.println("Configuration loaded:");
  }
  else
  {
    Serial.println("No configuration yet - starting with an empty document");
  }
  serializeJsonPretty(config, Serial);
  Serial.println();

  // read parameters; the "| default" gives a fallback when the key does not exist yet
  int    bootCount = config["bootCount"] | 0;
  float  paraFloat = config["paraFloat"] | 3.14f;
  String paraText  = config["paraText"]  | "Welcome";

  // change parameters
  bootCount += 1;
  paraFloat += 0.314f;
  paraText  += " and hello again";

  // write them back into the document ...
  config["bootCount"] = bootCount;
  config["paraFloat"] = paraFloat;
  config["paraText"]  = paraText;

  // ... and save the document to the file system
  if (storage.save())
  {
    Serial.println("Configuration saved:");
    serializeJsonPretty(config, Serial);
    Serial.println();
  }
  else
  {
    Serial.println("Saving failed");
  }
}

void loop()
{
}
