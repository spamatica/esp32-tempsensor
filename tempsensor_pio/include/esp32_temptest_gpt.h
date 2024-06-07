#ifndef TEMPSENS_TEMPTEST_GPT_HH
#define TEMPSENS_TEMPTEST_GPT_HH

#include <DallasTemperature.h>

//#define DRY_RUN
#define WDT_TIMEOUT_S               10     // Watch Dog Timer timeout in seconds
#define MAX_NUM_SENSORS             5
#define UPDATE_PERIOD_S             600    // UPDATE PERIOD IN SECONDS
#define ONEWIRE_USED_PIN            4

// NTP server and time zone´
extern const char*     ntpServer;
extern const long      gmtOffset_sec;
extern const int       daylightOffset_sec;

extern bool            firstRun;

extern int             errorCount;
extern int             pollCounter;

extern char            esp32_macaddress[17];
extern int8_t          numberOfSensors;

extern char            sensorNames[MAX_NUM_SENSORS][18];

extern DeviceAddress   sensorIds[MAX_NUM_SENSORS];

extern time_t          lastSendTime[MAX_NUM_SENSORS];

extern float           smoothedTemperature[MAX_NUM_SENSORS];

#endif // TEMPSENS_TEMPTEST_GPT_HH