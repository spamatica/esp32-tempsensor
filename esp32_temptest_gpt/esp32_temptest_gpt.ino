#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <time.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// device configuration
#include "local_config.h"

// NTP server and time zone
const char* ntpServer = "pool.ntp.org"                                                                                                                                            ;
const long gmtOffset_sec = 3600;
const int daylightOffset_sec = 3600;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, ntpServer, gmtOffset_sec, daylightOffset_sec);

// DS18B20 device
const int oneWirePin = 4;
OneWire oneWire(oneWirePin);
DallasTemperature sensors(&oneWire);

bool firstRun = true;

void setup()
{
  Serial.begin(115200);

  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  // Initialize NTP client
  timeClient.begin();

  // Initialize DS18B20 sensor
  sensors.begin();

  // Print WiFi information
  Serial.println("Connected to the WiFi network");
  Serial.println(WiFi.localIP());
  
  timeClient.update();
}

void loop() {  
  
  // Read temperature from sensor
  float temperature = readTemperature();

  char currentTimeString[80];
  struct tm ts;
  time_t rawTime = timeClient.getEpochTime();
  ts = *localtime(&rawTime);
   
  strftime(currentTimeString, sizeof(currentTimeString), "%Y-%m-%dT%H:%M:%S-01:00", &ts);

  // Create JSON object with temperature reading
  StaticJsonDocument<200> doc;
  doc["sensor_id"] = sensorName;
  doc["temperature"] = temperature;
  doc["timestamp"] = currentTimeString;

  // Convert JSON object to string
  String jsonString;
  serializeJson(doc, jsonString);

  // Create HTTP client and send POST request
  HTTPClient http;
  http.begin(serverUrl);
  http.addHeader("Content-Type", "application/json");
  int httpCode = http.POST(jsonString);

  // Print result
  if (httpCode > 0) {
    Serial.printf("Response code: %d\n", httpCode);
    Serial.println(http.getString());
  } else {
    Serial.printf("Error sending POST request: %s\n", http.errorToString(httpCode).c_str());
  }

  // first run? make a random delay before continuing
  if (firstRun == true)
  {
      sleep(random(update_period_s));
      firstRun = false;
  }

  // Wait before sending the next reading
  delay(update_period_s);
}

float readTemperature()
{
    // Read temperature from DS18B20 sensor
    // TODO: support more sensors
  sensors.requestTemperatures();
  return sensors.getTempCByIndex(0);
}
