#ifndef TEMPSENS_NETWORK_HH
#define TEMPSENS_NETWORK_HH

#include "watchdog.h"

class Network {

public:
  void initWifi(Watchdog &watchdog);
  void initTime();

  void sendJsonToRestServer(float temperature, const char *sensorName);
  unsigned long get_time();

};



#endif // TEMPSENS_NETWORK_HH
