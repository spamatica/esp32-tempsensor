#ifndef TEMPSENS_TEMPSENSOR_HH
#define TEMPSENS_TEMPSENSOR_HH
#include <DallasTemperature.h>

float readTemperature(DeviceAddress sensorId);
void initSensors();

#endif // TEMPSENS_TEMPSENSOR_HH
