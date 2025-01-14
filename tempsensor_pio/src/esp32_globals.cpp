#include "esp32_temptest_gpt.h"

TemperatureConfig cfg;

// NTP server and time zone
void initGlobals()
{
  strcpy(cfg.ntpServer,"pool.ntp.org");

  cfg.gmtOffset_sec               = 3600;
  cfg.daylightOffset_sec          = 3600;
  cfg.firstRun                    = true;

  cfg.errorCount                  = 0;
  cfg.pollCounter                 = 0;

  cfg.esp32_macaddress[17]        = {0};
  cfg.numberOfSensors             = 2;

  cfg.sensorNames[MAX_NUM_SENSORS][18]      = { 0 };

  // @todo Fix initialization
  //cfg.sensorIds[MAX_NUM_SENSORS]            = { 0 };

  cfg.lastSendTime[MAX_NUM_SENSORS]         = {0};

  cfg.smoothedTemperature[MAX_NUM_SENSORS]  = {0.0};
}