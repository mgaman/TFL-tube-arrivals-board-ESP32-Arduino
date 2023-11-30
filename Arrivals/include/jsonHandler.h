void jsonHandler(String raw);
// configuration API
unsigned getDelay();
const char *server();
const char *stationid();
const char *getStationName(int index);
int getNumberOfStations();
const char *urlFormat();
const char *getShortName(const char *,const char *);
int rowsPerPlatform();
int getPlatformToDisplay(int);
int getNumberOfPlatforms();

// arrival data API
int getArrivalPlatformNumber(int);
int getETA(int);
const char *getDestinationName(int);
const char *getTowardsName(int);
int getNumberOfRows();
bool rowContainsKey(int,const char *);
