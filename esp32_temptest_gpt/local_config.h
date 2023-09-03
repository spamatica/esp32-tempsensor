
// Settings for this specific device

// DEVICE NAME
int numberOfSensors = 2;
const char* sensorNames[] = {
    "sensor1",  // first
    "sensor2",  // second
};

// adresses for multiple sensors
// ROM = 28 FF 64 18 2D D4 9D 69
// ROM = 28 FF 64 18 2D DB B8 6D
DeviceAddress sensorIds[2] = {
          { 0x28, 0xFF, 0x64, 0x18, 0x2D, 0xD4, 0x9D, 0x69 },
          { 0x28, 0xFF, 0x64, 0x18, 0x2D, 0xDB, 0xB8, 0x6D },
};

// ACCESS POINT
const char* ssid = "AP";
const char* password = "password";

// END POINT
const char* serverUrl = "http://example.com:80/temperature";

// UPDATE PERIOD IN SECONDS
int update_period_s = 600;
