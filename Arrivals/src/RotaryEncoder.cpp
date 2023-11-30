#include <Arduino.h>
#include <SimpleRotary.h>
#include "jsonHandler.h"
#include "display.h"

SimpleRotary rotary(RECLK,REDT,RESW); // CLK DT SW

void displayStation(int index) {
    Serial.println(getStationName(index));
    displayClear();
    displayLine(0,0,getStationName(index));
}
int selectStation() {
    int selected = -1;
    int display = 0;
    int numOfStations = getNumberOfStations();
    displayStation(display);
    while (selected < 0) {
        switch (rotary.rotate()) {
            case 1: // clockwise
                display = ++display %= numOfStations;
                displayStation(display);
                break;
            case 2: // anticlockwise
                if (--display < 0)
                    display = numOfStations-1;
                displayStation(display);
                break;
        }
        if (rotary.push())
            selected = display;
    }
    displayLine(0,1,"Selected");
    return selected;
}