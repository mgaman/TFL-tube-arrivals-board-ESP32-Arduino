#include <Arduino.h>

//YWROBOT
//Compatible with the Arduino IDE 1.0
//Library version:1.1
/*
  Wiring notes
  Can connect 3.3v SCL/SDA to I2C
  ESp32 5V not enough power for screen, I use external 5V
*/
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x3F,20,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display
//LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display
//             01234567890123456789
const char *lines[] = {
              "Heathrow T4     8min",
              "Heathrow T2-3  15min",
              "Paddington      4min",
              "Abbey Wood     12min"
};

void setup()
{
  Serial.begin(115200);
  delay(1000);
  lcd.init();                      // initialize the lcd 
  lcd.init();
  lcd.backlight();

  for (int i=0; i<4; i++) {
    lcd.setCursor(0,i);
    lcd.print(lines[i]);  
  }

}


void loop()
{
}
