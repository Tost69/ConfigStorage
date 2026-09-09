/****************************************************************************************************************************
  ConfigStorage_Defaults.ino
  for ESP32 and ESP8266 boards
  built by Tost69 https://github.com/Tost69/ConfigStorage
  licensed under MIT license

  Shows the typical pattern for a real project:
    - a settings struct with defaults in the sketch
    - load the file, fill in whatever is missing, save it back
    - a tiny serial "menu" to change, revert and delete the configuration

  Type one of the following characters into the serial monitor:
    p  print the current settings
    +  increase the interval and save
    r  revert to the saved file
    d  delete the file and restore the defaults
 *****************************************************************************************************************************/

#include <ArduinoJson.h> // https://github.com/bblanchon/ArduinoJson
#include <ConfigStorage.h> // https://github.com/Tost69/ConfigStorage

struct Settings
{
  String   deviceName;
  String   wifiSsid;
  uint16_t intervalSec;
  bool     ledEnabled;
};

// defaults used when the file does not exist or a key is missing
const Settings defaults = {"esp-device", "MyWiFi", 60, true};

ConfigStorage storage("/settings.json");
Settings settings;

void loadSettings()
{
  JsonDocument &doc = storage.get();

  settings.deviceName  = doc["deviceName"]  | defaults.deviceName;
  settings.wifiSsid    = doc["wifiSsid"]    | defaults.wifiSsid;
  settings.intervalSec = doc["intervalSec"] | defaults.intervalSec;
  settings.ledEnabled  = doc["ledEnabled"]  | defaults.ledEnabled;
}

bool saveSettings()
{
  JsonDocument &doc = storage.get();

  doc["deviceName"]  = settings.deviceName;
  doc["wifiSsid"]    = settings.wifiSsid;
  doc["intervalSec"] = settings.intervalSec;
  doc["ledEnabled"]  = settings.ledEnabled;

  return storage.save();
}

void printSettings()
{
  Serial.println("--- settings ---");
  Serial.print("deviceName : "); Serial.println(settings.deviceName);
  Serial.print("wifiSsid   : "); Serial.println(settings.wifiSsid);
  Serial.print("intervalSec: "); Serial.println(settings.intervalSec);
  Serial.print("ledEnabled : "); Serial.println(settings.ledEnabled ? "yes" : "no");
  Serial.print("file       : "); Serial.println(storage.path());
  Serial.println("----------------");
}

void setup()
{
  Serial.begin(115200);
  delay(500);
  Serial.println();

  if (!storage.begin(true))
  {
    Serial.println("File system not available");
    while (true)
    {
      delay(1000);
    }
  }

  loadSettings();

  if (!storage.isLoaded())
  {
    Serial.println("No settings file found - writing defaults");
    saveSettings();
  }

  printSettings();
  Serial.println("Commands: p = print, + = interval +10 and save, r = revert, d = delete file");
}

void loop()
{
  if (!Serial.available())
  {
    return;
  }

  char command = Serial.read();

  switch (command)
  {
    case 'p':
      printSettings();
      break;

    case '+':
      settings.intervalSec += 10;
      Serial.println(saveSettings() ? "saved" : "save failed");
      printSettings();
      break;

    case 'r':
      Serial.println(storage.revert() ? "reverted to file" : "no file to revert to");
      loadSettings();
      printSettings();
      break;

    case 'd':
      Serial.println(storage.remove() ? "file deleted" : "no file to delete");
      storage.clear();
      loadSettings();
      printSettings();
      break;

    default:
      break;
  }
}
