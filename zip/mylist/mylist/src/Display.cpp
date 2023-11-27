#include <Arduino.h>
#include <List.hpp>
#include <ArduinoJson.h>

#define INCREMENT true
/**
 * Our raw data is in JSON format which is not convenient to sort.
 * We convert each line of JSON into an Item object and build a list of them
 * THis list can be sorted by a chosen field and then displayed
*/
class Item {
  public:
    Item(int at, const char *dest,int pn) {
      estArrivTime = at;
    platform = pn;
      destination = dest;
    };
    ~Item() {
      //Serial.println("Item destroyed"); 
    }
    int getArrivalTime() {
      return estArrivTime;
    }
    const char *getDestination() {
      return destination;
    }
    int getPlatform() {
        return platform;
    }
  private:
    int estArrivTime; // seconds
    int platform;
    const char *destination;
};

extern StaticJsonDocument<3000> doc;
extern StaticJsonDocument<400> config;

int extract(const char * source) {
  while (!isdigit(source[0])){  // skip over non digits
   // Serial.print(source[0]);
    source++;
  }
  return atoi(source);
}

/// @brief 
/// @param l 
/// @param r 
/// @return 0 if equal 1 if greater -1 if less

int compare(const void *l,const void *r) {
  int lv = ((Item*)l)->getArrivalTime();
  int rv = ((Item*)r)->getArrivalTime();
  if ( lv == rv)
    return 0;
  else if (INCREMENT)
    return lv > rv ? 1 : -1;
  else
    return lv < rv ? 1 : -1;
}

void printItem(Item *pI) {
  Serial.print(pI->getArrivalTime());
  Serial.print(":");
  Serial.print(pI->getPlatform());
  Serial.print(":");
  Serial.println(pI->getDestination());
}

/***
 *  Use config to filter data by platform number
 * Not that platform name is an inconsistent format, may be "Platform 4" or "Eastbound Platform 4"
 * We need a reliable way to extract the number only
*/
void displayAll() {
   // serializeJsonPretty(doc, Serial);
    // make 1 list for each platform
    List<Item> li[config["PlatformsToDisplay"].size()];
    // read line by line
    for (int row=0;row<doc.size();row++) {
        int pn = extract((const char *)doc[row]["platformName"]);
#ifdef DEBUG
        Serial.println(pn);
#endif
        // check for match of platform number & place in apprpriate queue
        for (int i=0; i<config["PlatformsToDisplay"].size();i++) {
#ifdef DEBUG
          Serial.printf("Compare %d %d\n",pn,(int)config["PlatformsToDisplay"][i]);
#endif
          if (pn == config["PlatformsToDisplay"][i]) {
#ifdef DEBUG
            Serial.printf("Add to list %d value %d\n",i,pn);
#endif
            li[i].add(Item(doc[row]["timeToStation"],doc[row]["destinationName"],pn));
          }
        }
    }
    // now sort & print each list
    for (int i=0; i<config["PlatformsToDisplay"];i++) {
      //li[i].sort(compare);
      for (int j=0;j<li[i].getSize();j++)
        printItem(li[i].getPointer(j));
    }
}