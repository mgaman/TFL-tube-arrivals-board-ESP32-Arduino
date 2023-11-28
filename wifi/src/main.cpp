/*
   Select wifi credential from list
*/
#include <Arduino.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include <List.hpp>
#include <FS.h>
#include <SPIFFS.h>
#include "AP.h"

const char *path = "/config.json"; 
StaticJsonDocument<2000> config;

List<AP> aplist;

/// @brief
/// @param l
/// @param r
/// @return 0 if equal 1 if greater -1 if less
int compareRSSI(const void *l, const void *r)
{
  int lv = ((AP *)l)->getRSSI();
  int rv = ((AP *)r)->getRSSI();
  if (lv == rv)
    return 0;
  else
    return lv < rv ? 1 : -1;
}

void setup() {
  Serial.begin(115200);
  delay(1000);
#if 1
  // open file system & load config file
  if (!SPIFFS.begin(false))
  {
    Serial.println("SPIFFS Mount Failed");
    return;
  }
  File file = SPIFFS.open(path);
  if (!file || file.isDirectory())
  {
    Serial.println("− failed to open file for reading");
    return;
  }
  int bsize = 10 + file.size();
  uint8_t *buff = (uint8_t *)malloc(bsize);
  if (buff)
  {
    file.read(buff, bsize);
    DeserializationError derr = deserializeJson(config, buff);
    if (!derr)
    {
      serializeJsonPretty(config, Serial);
      Serial.printf("config %d bytes\n", config.memoryUsage());
    }
    else
    {
      Serial.print("config ");
      Serial.println(derr.c_str());
      return;
    }
    //free(buff); // DO NOT FREE - the data is all in here
  }
#endif
#if 1
  // scan local wifi providers
  int numSsid = WiFi.scanNetworks();
  while (numSsid == -1) {
    Serial.print("+");
    delay(50);
    numSsid = WiFi.scanNetworks();
  }
  for (int i = 0; i<numSsid; i++) {
    Serial.print(WiFi.SSID(i));
    Serial.print(": ");
    Serial.println(WiFi.RSSI(i));
    aplist.add(AP(WiFi.SSID(i).c_str(),WiFi.RSSI(i)));
  }
#endif
#if 1
  // now sort the list, the lower the RSSI the stronger the signal
  aplist.sort(compareRSSI);
  Serial.println("-------------------");
  // now print the sorted list
  for (int i=0;i<aplist.getSize();i++) {
    const char *ssid = aplist[i].getSSID();
    Serial.printf("%s %d \r\n",ssid,aplist[i].getRSSI());
    // check if this ssid in config, if so use it to connect to wifi
    if (config["WiFi"].containsKey(ssid)) {
      Serial.printf("Connect using %s %s\r\n",ssid,(const char *)config["WiFi"][ssid]);
          // get wifi connection
    WiFi.disconnect();
    WiFi.mode(WIFI_STA); // Optional
    WiFi.begin(ssid, (const char *)config["WiFi"][ssid]);
    Serial.println("\nConnecting");
 
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(100);
    }
    Serial.println("\nConnected to the WiFi network");
    Serial.print("Local ESP32 IP: ");
    Serial.println(WiFi.localIP());
    WiFi.disconnect();

  }
  else
    Serial.printf("No key %s\r\n",ssid);
  }
#endif
}

void loop() {
}

