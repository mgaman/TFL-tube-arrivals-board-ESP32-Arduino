/*
    Tool  to estimate json object size
*/
#include <Arduino.h>
#include <ArduinoJson.h>
#include <FS.h>
#include <SPIFFS.h>

#define DO_FILTER 

//const char *path = "/bakerstreet.json"; // needs 25000 without filter, 3200 with
//const char *path = "/actontown.json";   // 4000 , 512
const char *path = "/earlscourt.json";  // 9000 1200
//const char *path = "/config.json";        // 1300
StaticJsonDocument<25000> doc;
#ifdef DO_FILTER
StaticJsonDocument<100> filter;
const char *keys[] = { "platformName", "destinationName", "timeToStation","towards"};
#endif

void setup()
{
  Serial.begin(115200);
  delay(1000);

#ifdef DO_FILTER
  for (int i=0;i<sizeof(keys)/sizeof(const char *);i++)
    filter[0][keys[i]] = true;
  serializeJsonPretty(filter,Serial);
  Serial.printf("Mem filter %d\r\n",filter.memoryUsage());
#endif

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
#ifdef DO_FILTER
    DeserializationError derr = deserializeJson(doc, buff, DeserializationOption::Filter(filter));
#else
    DeserializationError derr = deserializeJson(doc, buff);
#endif
    if (!derr)
    {
      serializeJsonPretty(doc, Serial);
      Serial.printf("doc %d bytes\n", doc.memoryUsage());
    }
    else
    {
      Serial.print("config ");
      Serial.println(derr.c_str());
    }
    free(buff);
  }
}

void loop()
{
  // put your main code here, to run repeatedly:
}
