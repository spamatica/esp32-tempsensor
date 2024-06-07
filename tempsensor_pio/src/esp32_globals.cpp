#include <DallasTemperature.h>
#include "esp32_temptest_gpt.h"

// NTP server and time zone
const char*     ntpServer                   = "pool.ntp.org";
const long      gmtOffset_sec               = 3600;
const int       daylightOffset_sec          = 3600;

const int       oneWirePin                  = 4;

bool            firstRun                    = true;

int             errorCount                  = 0;
int             pollCounter                 = 0;

char            esp32_macaddress[17]        = {0};
int8_t          numberOfSensors             = 2;

char            sensorNames[MAX_NUM_SENSORS][18]      = { 0 };

DeviceAddress   sensorIds[MAX_NUM_SENSORS]            = { 0 };

time_t          lastSendTime[MAX_NUM_SENSORS]         = {0};

float           smoothedTemperature[MAX_NUM_SENSORS]  = {0.0};
