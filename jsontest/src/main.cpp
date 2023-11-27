#include <Arduino.h>
#include <ArduinoJson.h>
#include <FS.h>
#include <SPIFFS.h>

StaticJsonDocument<1000> doc;
StaticJsonDocument<1500> config;
char json[] = "{\"sensor\":\"gps\",\"time\":1351824120,\"data\":[48.756080,2.302038,5]}";
void setup()
{
  Serial.begin(115200);
  delay(1000);
  DeserializationError derr = deserializeJson(doc, json);
  if (derr)
  {
    Serial.println(derr.c_str());
  }
  else
  {
    Serial.println(doc["data"].size());
  }
  if (!SPIFFS.begin(true))
  {
    Serial.println("SPIFFS Mount Failed");
    return;
  }

  File file = SPIFFS.open("/config.json");
  if (!file || file.isDirectory())
  {
    Serial.println("− failed to open file for reading");
    return;
  }
  else
    Serial.println("File opened");

  int bsize = 10 + file.size();
  Serial.print("File size "); Serial.println(file.size());
  uint8_t *buff = (uint8_t *)calloc(1,bsize);
  
  if (buff)
  {
    int fr = file.read(buff, bsize);
    Serial.print("Read ");Serial.println(fr);
  //  Serial.println((char *)buff);

    DeserializationError derr = deserializeJson(config, buff);
    if (!derr)
    {
      serializeJsonPretty(config, Serial);
      Serial.printf("config %d bytes\n", config.memoryUsage());
      Serial.println((const char *)config["stations"][config["defaultStation"]]["ID"]);
    }
    else
    {
      Serial.print("config ");
      Serial.println(derr.c_str());
    }
  }
}

  void loop()
  {
    // put your main code here, to run repeatedly:
  }
