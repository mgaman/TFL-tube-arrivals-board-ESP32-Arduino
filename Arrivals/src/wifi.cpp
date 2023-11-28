#include <Arduino.h>
#include <WiFi.h>
#include <List.hpp>
#include <ArduinoJson.h>
#include "AP.h"
#include "display.h"

List<AP> aplist;
extern StaticJsonDocument<CONFIG_SIZE> config;

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

/**
 * @brief  Scan all access points and make a list sorted by RSSI
 *
 */
bool wifiInit()
{
    // scan local wifi providers
    bool rc = false;
    int numSsid = WiFi.scanNetworks();
    if (numSsid != -1) {
#ifdef DEBUG
        Serial.print(numSsid); Serial.println(" networks");
#endif
        for (int i = 0; i < numSsid; i++)
        {
#ifdef DEBUG
            Serial.print(WiFi.SSID(i));
            Serial.print(": ");
            Serial.println(WiFi.RSSI(i));
#endif
            aplist.add(AP(WiFi.SSID(i).c_str(), WiFi.RSSI(i)));
        }
        // now sort the list, the lower the RSSI the stronger the signal
        aplist.sort(compareRSSI);
        rc = true;
    }
    return rc;
}

/**
 * @brief Using list created in wifiInit look for matching kei in config["iFi"]
 *
 */
bool wifiConnect()
{
    bool rc = false;
    for (int i = 0; i < aplist.getSize(); i++)
    {
        const char *ssid = aplist[i].getSSID();
        Serial.printf("%s %d \r\n", ssid, aplist[i].getRSSI());
        // check if this ssid in config, if so use it to connect to wifi
        if (config["WiFi"].containsKey(ssid))
        {
            Serial.printf("Connect with SSID %s\r\n", ssid);
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
            displayClear();
            displayLine(0,0,WiFi.localIP().toString().c_str());
            rc = true;
        }
        else
            Serial.printf("No key %s\r\n", ssid);
    }
    return rc;
}