/****************************************************************************************************************************
  ConfigStorage_ESP32.ino
  for ESP32 boards
  built by Tost69 https://github.com/Tost69/ConfigStorage
  changed by ReinaldoAF: 17/01/2022 https://github.com/Tost69/ConfigStorage/issues/2
  licensed under MIT license
 *****************************************************************************************************************************/
#include <FS.h>
#include <ArduinoJson.h> // https://github.com/bblanchon/ArduinoJson

// by default, LittleFS is used; to use SPIFFS, uncomment the following line
//#define CS_USE_SPIFFS true
#define CS_USE_LITTLEFS true
#define CONFIGSTORAGE_DEBUG true // show debug messages

#include <ConfigStorage.h> // https://github.com/Tost69/ConfigStorage

// set parameter defaults
int exampleInt = 1234;
float exampleFloat = 3.14;
String exampleString = "Welcome";

void setup()
{

  Serial.begin(115200);
  while (!Serial)
      ;

  delay(200);
  
  ConfigStorage CS("/config.dat");

  //if mount failed try format it... when I change FS type (SPIFFS/LITTLEFS)
  if(!FileFS.begin()){
    if(!FileFS.format())
      Serial.println("FS Mount Failed");
    ESP.restart();
    while(true);
  }
  
  DynamicJsonDocument configDoc = CS.get();
  
  // change parameters
  exampleInt = (int)configDoc["ParaInt"] + 10;
  exampleFloat = (float)configDoc["ParaFloat"] + 0.314;
  String exampleText = configDoc["ParaText"];

  //check if it is first time and get null  inside string.
  if(exampleText.indexOf("null")>-1) 
    exampleText = exampleString + String(" and hello again");
  else
    exampleText += String(" and hello again");

  Serial.print("Int Value: ");
  Serial.println(exampleInt);
  Serial.print("Float Value: ");    
  Serial.println(exampleFloat);
  Serial.print("String Value: ");
  Serial.println(exampleText.c_str());

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