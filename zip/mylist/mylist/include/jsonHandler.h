const char *WiFiSSID();
const char *WiFiPassword();
void jsonInit(fs::FS &fs,const char *path);
void jsonHandler(String raw);
unsigned getDelay();
const char *server();
const char *stationid();
const char *urlFormat();