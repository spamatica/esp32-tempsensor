#ifndef TEMPSENS_TEMPTEST_GPT_HH
#define TEMPSENS_TEMPTEST_GPT_HH

#include <DallasTemperature.h>

//#define DRY_RUN
#define WDT_TIMEOUT_S               10     // Watch Dog Timer timeout in seconds
#define MAX_NUM_SENSORS             5
#define UPDATE_PERIOD_S             600    // UPDATE PERIOD IN SECONDS
#define ONEWIRE_USED_PIN            4

class TemperatureConfig {
  public:
// NTP server and time zone´
    char      ntpServer[40];
    long      gmtOffset_sec;
    int       daylightOffset_sec;

    bool            firstRun;

    int             errorCount;
    int             pollCounter;

    char            esp32_macaddress[17];
    int8_t          numberOfSensors;

    char            sensorNames[MAX_NUM_SENSORS][18];

    DeviceAddress   sensorIds[MAX_NUM_SENSORS];

    time_t          lastSendTime[MAX_NUM_SENSORS];

    float           smoothedTemperature[MAX_NUM_SENSORS];

};

extern TemperatureConfig cfg;

#endif // TEMPSENS_TEMPTEST_GPT_HH