#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <List.hpp>
#include "Item.h"
#include "jsonHandler.h"

LiquidCrystal_I2C lcd(0x3F, 20, 4); // set the LCD address to 0x27 for a 16 chars and 2 line display

#define INCREMENT true

void displayInit()
{
  lcd.init();
  lcd.backlight();
  lcd.clear();
}

/// @brief
/// @param l
/// @param r
/// @return 0 if equal 1 if greater -1 if less

int compare(const void *l, const void *r)
{
  int lv = ((Item *)l)->getArrivalTime();
  int rv = ((Item *)r)->getArrivalTime();
  if (lv == rv)
    return 0;
  else if (INCREMENT)
    return lv > rv ? 1 : -1;
  else
    return lv < rv ? 1 : -1;
}

int rowNumber = 0;
void displayClear()
{
  lcd.clear();
  rowNumber = 0;
}

void displayItem(List<Item> *pLI)
{
  // check if any arrivals on desired platform(s)
  if (pLI->getSize() > 0)
  {
    pLI->sort(compare);
    for (int j = 0; j < rowsPerPlatform(); j++)
    {
     lcd.setCursor(0, rowNumber);
      Item *pI = pLI->getPointer(j);
      // Arrival time round down seconds to minutes
      int aTime = pI->getArrivalTime() / 60;
      const char *dest = getShortName(pI->getDestination(),pI->getTowards());
      if (dest == NULL)
        dest = (char *)pI->getDestination();
      lcd.printf("%d %s", pI->getPlatform(), dest);
      lcd.setCursor(18, rowNumber);
      lcd.printf("%2d", aTime);
#ifdef DEBUG
      //      Serial.printf("platform %d ETA %d dest %s\r\n",pI->getPlatform(),pI->getArrivalTime(),pI->getDestination());
      Serial.print("platform ");
      Serial.print(pI->getPlatform());
      Serial.print(" ETA ");
      Serial.print(pI->getArrivalTime());
      Serial.print(" dest ");
      Serial.println(pI->getDestination());
#endif
      rowNumber++;
    }
  }
}

void displayNoData() {
  lcd.clear();
  lcd.print("Missing data");
}

void displayLine(int col,int row,const char *text) {
  lcd.setCursor(col,row);
  lcd.print(text);
}
