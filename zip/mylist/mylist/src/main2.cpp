#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <FS.h>
#include <SPIFFS.h>

//#include "config.h"
#include "jsonHandler.h"

/* You only need to format SPIFFS the first time you run a
   test or else use the SPIFFS plugin to create a partition
   https://github.com/me−no−dev/arduino−esp32fs−plugin */
#define FORMAT_SPIFFS_IF_FAILED true

//#include "TflRow.h"
HTTPClient client;
void getJSON()
{
    // create the URL
    char URL[100];
    sprintf(URL,urlFormat(),server(),stationid());
#ifdef DEBUG
    Serial.println(URL);
#endif
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
            delay(1000);  // try again
        }
    }
}

void setup()
{
    Serial.begin(115200);
    delay(1000);
    Serial.println();
      if(!SPIFFS.begin(FORMAT_SPIFFS_IF_FAILED)){
      Serial.println("SPIFFS Mount Failed");
      return;
    }

    jsonInit(SPIFFS,"/config.json");

    // get wifi connection
    WiFi.mode(WIFI_STA); // Optional
    WiFi.begin(WiFiSSID(), WiFiPassword());
    Serial.println("\nConnecting");
 
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(100);
    }
#ifdef DEBUG
    Serial.println("\nConnected to the WiFi network");
    Serial.print("Local ESP32 IP: ");
    Serial.println(WiFi.localIP());
#endif
}

void loop()
{
    getJSON();
    delay(getDelay()*1000);
}
