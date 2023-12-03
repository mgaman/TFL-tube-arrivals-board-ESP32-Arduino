#include <Arduino.h>
#include <ArduinoJson.h>
#include <FS.h>
#include <SPIFFS.h>

/*
===============================================================================================================
SimpleRotary.h Library CW / CCW Example Sketch
Learn more at [https://github.com/mprograms/SimpleRotary]

This example shows how to get the rotational direction.

===============================================================================================================
Release under the GNU General Public License v3
[https://www.gnu.org/licenses/gpl-3.0.en.html]
===============================================================================================================
*/
#include <SimpleRotary.h>

const char *path = "/config.json";        // 1300
StaticJsonDocument<25000> doc;

// Pin A, Pin B, Button Pin
SimpleRotary rotary(5,18,19); // CLK DT SW

int numStations = -1;
int chosenStation = 0;
void displayStation(int index) {
  Serial.printf("%d %s\r\n",index,(const char *)doc["stations"][index]["Name"]);
}

void setup() {
  Serial.begin(115200);
  delay(1000);
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
//    free(buff);  DO NOT free this buffer
  }
  // enumerate stations
  numStations = doc["stations"].size();
  for (int i=0; i<numStations;i++) {
    displayStation(i);
  }
  displayStation(chosenStation);
}

void loop() {
  //return;
    // 0 = not turning, 1 = CW, 2 = CCW
#if 1
  switch (rotary.rotate()) {
    case 1:
      chosenStation++;
      chosenStation %= numStations;
      displayStation(chosenStation);
      break;
    case 2:
      chosenStation--;
      if (chosenStation < 0)
        chosenStation = numStations -1;
      displayStation(chosenStation);
      break;
    default:
      break;
  }
#endif
  if (rotary.push())
    Serial.printf("%d selected\r\n",chosenStation);
}
