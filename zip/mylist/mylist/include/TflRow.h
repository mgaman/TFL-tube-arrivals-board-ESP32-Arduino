class TflRow {
  public:
    TflRow(int at, const char *direction, const char *dest);
 //   ~TflRow();
    int getArriveTime();
    bool getDirection();
    const char *getDestination() ;
};

// utilities
int compare2(const void *l,const void *r);
void printItem(TflRow *pI);