#include <Arduino.h>
#include <ArduinoJson.h>
#include <FS.h>
#include <SPIFFS.h>
#include "jsonHandler.h"

StaticJsonDocument<DOC_SIZE> doc;
StaticJsonDocument<CONFIG_SIZE> config;
StaticJsonDocument<FILTER_SIZE> filter;

int defaultStation = -1;

void processAll();
#ifdef USE_ROTARY
int selectStation();
#endif

void jsonHandler(String raw)
{
    DeserializationError derr =
        deserializeJson(doc, raw.c_str(), DeserializationOption::Filter(filter));
    //    deserializeJson(doc, jsonData);
    if (!derr)
    {
#ifdef LOGRAW
        serializeJsonPretty(doc, Serial);
        Serial.printf("doc %d bytes\n", doc.memoryUsage());
#endif
        processAll();
    }
    else
    {
        Serial.print("doc ");
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
    int bsize = 10 + file.size();
    uint8_t *buff = (uint8_t *)malloc(bsize);
    if (buff)
    {
        file.read(buff, bsize);
        DeserializationError derr = deserializeJson(config, buff);
        if (!derr)
        {
#ifdef LOGRAW
            serializeJsonPretty(config, Serial);
            Serial.printf("config %d bytes\n", config.memoryUsage());
#endif
        }
        else
        {
            Serial.print("config ");
            Serial.println(derr.c_str());
        }
        // build the filter json object
        for (int i = 0; i < config["filterKeys"].size(); i++)
        {
            filter[0][(const char *)config["filterKeys"][i]] = true;
        }
#ifdef LOGRAW
        serializeJsonPretty(filter, Serial);
        Serial.printf("filter %d bytes\n", filter.memoryUsage());
#endif
        //   free(buff);  // DO NOT free memory, config data is here
#ifdef USE_ROTARY
        defaultStation = selectStation();
#else
        defaultStation = config["defaultStation"];
#endif
    }
}
unsigned getDelay()
{
    return (unsigned)config["refreshDelaySeconds"];
}
const char *server()
{
    return (const char *)config["server"];
}
const char *stationid()
{
    const char *stationid = (const char *)config["stations"][defaultStation]["ID"];
#ifdef DEBUG
    Serial.printf("Station ID %s\r\n", stationid);
#endif
    return stationid;
}

int getNumberOfStations()
{
    return config["stations"].size();
}

const char *getStationName(int index)
{
    const char *stationname = (const char *)config["stations"][index]["Name"];
#ifdef DEBUG
    Serial.printf("Station ID %s\r\n", stationname);
#endif
    return stationname;
}
const char *urlFormat()
{
    return (const char *)config["URL"]["formatC"];
}
/// @brief Check if one of the paramaters has a shortened version
/// @param destinationName Note, may be zero length
/// @param towards   Note may be zero length
/// @return Shortene
const char *getShortName(const char *destinationName, const char *towards)
{
    const char *shortName = NULL;
    // use towards if present
    const char *longName = strlen(towards) > 0 ? towards : destinationName;
#ifdef DEBUG
    Serial.printf("D %s T %s L %s\r\n", destinationName, towards, longName);
#endif
    if (config["substitutes"].containsKey(longName))
    {
        shortName = config["substitutes"][longName];
#ifdef DEBUG
        Serial.printf("long %s short %s\r\n", longName, shortName);
#endif
    }
    else
    {
        shortName = longName;
#ifdef DEBUG
        Serial.printf("No sub %s \r\n", longName);
#endif
    }
    return shortName;
}

int rowsPerPlatform()
{
    //    return config["rowsPerPlatform"];
    int rpp = config["stations"][defaultStation]["rowsPerPlatform"];
#ifdef DEBUG
    Serial.printf("rows per platform %d\r\n", rpp);
#endif
    return rpp;
}

int getPlatformToDisplay(int index)
{
    //    return config["PlatformsToDisplay"][index];
    int ptd = config["stations"][defaultStation]["PlatformsToDisplay"][index];
    return ptd;
}

int getNumberOfPlatforms()
{
    //    return config["PlatformsToDisplay"].size();
    int nop = config["stations"][defaultStation]["PlatformsToDisplay"].size();
#ifdef DEBUG
    Serial.printf("Number of platforms %d\r\n", nop);
#endif
    return nop;
}

// Arrival Data API
int getArrivalPlatformNumber(int rowNumber)
{
    // make an assumption here that text is xxxxxxnnn where x is non-digit and n is digit
    const char *c = doc[rowNumber]["platformName"];
#ifdef DEBUG
    //    Serial.printf("row %d %s\r\n",rowNumber,(const char *)doc[rowNumber]["platformName"]);
    Serial.printf("Row %d ", rowNumber);
    Serial.printf("P %s\r\n", (const char *)doc[rowNumber]["platformName"]);
#endif
    while (!isdigit(*c))
        c++; // skip non-digits
    return atoi(c);
}

int getETA(int rowNumber)
{
    return doc[rowNumber]["timeToStation"];
}

const char *getDestinationName(int rowNumber)
{
    return doc[rowNumber]["destinationName"];
}

const char *getTowardsName(int rowNumber)
{
    return doc[rowNumber]["towards"];
}
int getNumberOfRows()
{
    int nor = doc.size();
#ifdef DEBUG
    Serial.printf("NOR %d\r\n", nor);
#endif
    return nor;
}

bool rowContainsKey(int row, const char *key)
{
    return doc[row].containsKey(key);
}
