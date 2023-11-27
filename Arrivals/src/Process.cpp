#include <Arduino.h>
#include <List.hpp>
#include <Wire.h>
#include <FS.h>
#include <SPIFFS.h>
//#include <LiquidCrystal_I2C.h>
#include "jsonHandler.h"
#include "Item.h"
#include "display.h"

/***
 * Use config to filter data by platform number
 * Not that platform name is an inconsistent format, may be "Platform 4" or "Eastbound Platform 4"
 * We need a reliable way to extract the number only
 */
void processAll()
{
  // make a list for each platform
  int numItems = getNumberOfPlatforms();
  List<Item> li[numItems];
#ifdef DEBUGxx
  Serial.print("List size:");
  Serial.println(numItems);
#endif
  // read line by line & populate the appropriate list
  for (int row = 0; row < getNumberOfRows(); row++)
  {
    int pn = getArrivalPlatformNumber(row);
    // check for match of platform number & place in config PlatformsToDisplay
    for (int itemIndex = 0; itemIndex < numItems; itemIndex++)
    {
      if (pn == getPlatformToDisplay(itemIndex))
      {
        // Have noticed cases where destinationName is missing
        if (rowContainsKey(row,"destinationName")) {
#ifdef DEBUGxx
          Serial.printf("Add index %d %d %s %d\r\n", itemIndex, getETA(row), getDestinationName(row), pn);
#endif
          li[itemIndex].add(Item(getETA(row), getDestinationName(row), pn));
        }
        else {
#ifdef DEBUGxx
          Serial.println("Missing destination name");
#endif          
        }
      }
      else {
#ifdef DEBUGxx
        Serial.printf("no pn match %d %d %d\r\n",itemIndex,pn,getPlatformToDisplay(itemIndex));   
#endif  
      }
    }
  }

  displayClear();
  // send unsorted list (per platform)
  for (int item = 0; item < numItems; item++)
    displayItem(&li[item]);
}
