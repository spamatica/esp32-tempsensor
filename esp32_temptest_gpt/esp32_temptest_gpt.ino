#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <time.h>
#include <OneWire.h>
#include <DallasTemperature.h>

const char* ssid = "AP";
const char* password = "password";
const char* serverUrl = "http://192.1.1.1:8080/temperature";

// NTP server and time zone
const char* ntpServer = "pool.ntp.org"                                                                                                                                            ;
const long gmtOffset_sec = 3600;
const int daylightOffset_sec = 3600;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, ntpServer, gmtOffset_sec, daylightOffset_sec);

// Pin for DS18B20 data line
const int oneWirePin = 4;

// DS18B20 device object
OneWire oneWire(oneWirePin);
DallasTemperature sensors(&oneWire);

void setup() {
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
  
  // Update time from NTP server
//  timeClient.update();

  // Read temperature from sensor
  float temperature = readTemperature();

  char currentTimeString[80];
  struct tm ts;
  time_t rawTime = timeClient.getEpochTime();
  ts = *localtime(&rawTime);
   
//  strftime(currentTimeString, sizeof(currentTimeString), "%a %Y-%m-%d %H:%M:%S %Z", &ts);
  strftime(currentTimeString, sizeof(currentTimeString), "%Y-%m-%dT%H:%M:%S-01:00", &ts);


  // Create JSON object with temperature reading
  StaticJsonDocument<200> doc;
  doc["sensor_id"] = "esp32_sensor";
  doc["temperature"] = temperature;
  doc["timestamp"] = currentTimeString;
//  doc["timestamp"] = String(millis());

  // Convert JSON object to string
  String jsonString;
  serializeJson(doc, jsonString);

  Serial.println(jsonString);
  
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

  // Wait for some time before sending the next reading
  delay(10000);
}

// Replace this function with code to read the temperature from your sensor
float readTemperature() {
    // Read temperature from DS18B20 sensor
  sensors.requestTemperatures();
  return sensors.getTempCByIndex(0);
}

