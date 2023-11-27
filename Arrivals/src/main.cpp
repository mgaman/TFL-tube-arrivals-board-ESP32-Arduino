#include <Arduino.h>
//#include <WiFi.h>
#include <HTTPClient.h>
#include <FS.h>
#include <SPIFFS.h>
#include "jsonHandler.h"
#include "wifi.h"

void jsonInit(fs::FS &fs,const char *path);
void displayInit();
void displayNoData();


#define FORMAT_SPIFFS_IF_FAILED true
char URL[100];

HTTPClient client;
void getJSON()
{
    client.begin(URL);
    // http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    int httpCode = HTTP_CODE_OK + 1;  // i.e. not OK
    while (httpCode != HTTP_CODE_OK) {
        httpCode = client.GET();
        if (httpCode == HTTP_CODE_OK) {
            String payload = client.getString();
            jsonHandler(payload);
            client.end();
        }
        else
        {
            Serial.printf("[HTTP] GET... failed, error: %s\n", client.errorToString(httpCode).c_str());
            displayNoData();
            delay(1000);  // try again
        }
    }
}

void setup()
{
    Serial.begin(115200);
    delay(1000);
    Serial.println();
    displayInit();
    if(!SPIFFS.begin(FORMAT_SPIFFS_IF_FAILED)){
        Serial.println("SPIFFS Mount Failed");
        return;
    }
    jsonInit(SPIFFS,"/config.json");
    // create the URL
    sprintf(URL,urlFormat(),server(),stationid());
#ifdef DEBUG
    Serial.println(URL);
#endif
    wifiInit();
    if (!wifiConnect()) {
        Serial.println("Failed to connect to WiFi");
        while (true);
    }
}

void loop()
{
    getJSON();
    delay(getDelay()*1000);
}
