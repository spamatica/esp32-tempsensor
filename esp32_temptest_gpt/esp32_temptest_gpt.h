#ifndef ESP32_TEMPTEST_GPT_HH
#define ESP32_TEMPTEST_GPT_HH

// NTP server and time zone´
const char* ntpServer = "pool.ntp.org"                                                                                                                                            ;
const long gmtOffset_sec = 3600;
const int daylightOffset_sec = 3600;

const int oneWirePin = 4;

bool firstRun = true;

int errorCount = 0;
int pollCounter = 0;

#define WDT_TIMEOUT 3     // define a 3 seconds WDT (Watch Dog Timer)
#define MAX_NUM_SENSORS 5

int8_t numberOfSensors = 2;
char sensorNames[MAX_NUM_SENSORS][17] = { 0 };

DeviceAddress sensorIds[MAX_NUM_SENSORS] = { 0 };

time_t lastSendTime[MAX_NUM_SENSORS] = {0};

float smoothedTemperature[MAX_NUM_SENSORS] = {0.0};

#endif // ESP32_TEMPTEST_GPT_HH
