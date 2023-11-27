#include <Arduino.h>
#include <ArduinoJson.h>
// #include "TflRow.h"
#include <FS.h>
#include <SPIFFS.h>

StaticJsonDocument<3000> doc;
StaticJsonDocument<400> config;
StaticJsonDocument<100> filter;
void displayAll();

void jsonHandler(String raw)
{
    DeserializationError  derr = 
        deserializeJson(doc, raw.c_str(), DeserializationOption::Filter(filter));
    //    deserializeJson(doc, jsonData);
    if (!derr) {
    #ifdef DEBUG
        serializeJsonPretty(doc, Serial);
    #endif
        displayAll();
    }
    else {
        Serial.println(derr.c_str());
    }
}

void jsonInit(fs::FS &fs, const char *path)
{
    File file = fs.open(path);
    if (!file || file.isDirectory())
    {
        Serial.println("− failed to open file for reading");
        return;
    }
    int bsize = 10+file.size();
    uint8_t *buff = (uint8_t *)malloc(bsize);
    if (buff)
    {
        file.read(buff, bsize);
        DeserializationError derr = deserializeJson(config, buff);
        if (!derr)
        {
#ifdef DEBUG
            serializeJsonPretty(config, Serial);
            Serial.printf("config %d bytes\n", config.memoryUsage());
#endif
        }
        else
        {
            Serial.println(derr.c_str());
        }
        // build the filter json object
        for (int i=0; i<config["filterKeys"].size();i++) {
            filter[0][(const char *)config["filterKeys"][i]] = true;
        }
#ifdef DEBUG
        serializeJsonPretty(filter, Serial);
        Serial.printf("filter %d bytes\n", filter.memoryUsage());
#endif
     //   free(buff);
    }
}
const char *WiFiSSID()
{
#ifdef DEBUG
    Serial.println((const char *)config["WiFi"]["ssid"]);
#endif
    return (const char *)config["WiFi"]["ssid"];
}
const char *WiFiPassword()
{
#ifdef DEBUG
    Serial.println((const char *)config["WiFi"]["password"]);
#endif
    return (const char *)config["WiFi"]["password"];
}

unsigned getDelay()
{
    return (unsigned)config["refreshDelaySeconds"];
}
const char *server() {
    return (const char *)config["server"];
}
const char *stationid() {
    return (const char *)config["station"]["ID"];
}
const char *urlFormat() {
    return (const char *)config["URL"]["formatC"];    
}