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

time_t lastSendTime = 0;

float smoothedTemperature = 0.0;

// example adresses for multiple sensors
DeviceAddress sensor1 = { 0x28, 0xFF, 0x77, 0x62, 0x40, 0x17, 0x4, 0x31 };
DeviceAddress sensor2 = { 0x28, 0xFF, 0xB4, 0x6, 0x33, 0x17, 0x3, 0x4B };
DeviceAddress sensor3= { 0x28, 0xFF, 0xA0, 0x11, 0x33, 0x17, 0x3, 0x96 };

#endif // ESP32_TEMPTEST_GPT_HH