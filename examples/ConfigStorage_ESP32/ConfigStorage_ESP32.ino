/****************************************************************************************************************************
  ConfigStorage_ESP32.ino
  for ESP32 boards
  built by Tost69 https://github.com/Tost69/ConfigStorage
  licensed under MIT license
 *****************************************************************************************************************************/

#include <ArduinoJson.h> // https://github.com/bblanchon/ArduinoJson

// by default, LittleFS is used; to use SPIFFS, uncomment the following line
// #define CS_USE_SPIFFS true
#define CONFIGSTORAGE_DEBUG true // show debug messages

#include <ConfigStorage.h> // https://github.com/Tost69/ConfigStorage

ConfigStorage CS("/config.dat");
DynamicJsonDocument configDoc(1024) = CS.get();

// set parameter defaults
int exampleInt = 1234;
float exampleFloat = 3.14;
string exampleString = "Welcome";

void setup()
{
    Serial.begin(115200);
    while (!Serial)
        ;

    delay(200);

    // change parameters
    exampleInt = configDoc["ParaInt"] + 10;
    exampleFloat = configDoc["ParaFloat"] + 0.314;
    exampleText = configDoc["ParaText"] + " and hello again";

    // set parameters
    configDoc["ParaInt"] = exampleInt;
    configDoc["ParaFloat"] = exampleFloat;
    configDoc["ParaText"] = exampleText;

    // save configuration as file
    CS.set(configDoc);
    CS.save();
}

void loop()
{
}
