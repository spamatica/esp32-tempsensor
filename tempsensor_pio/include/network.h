#ifndef TEMPSENS_NETWORK_HH
#define TEMPSENS_NETWORK_HH

void initWifi();
void initTime();

void sendJsonToRestServer(float temperature, const char *sensorName);
unsigned long network_get_time();


#endif // TEMPSENS_NETWORK_HH
