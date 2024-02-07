#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <time.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#include "esp32_temptest_gpt.h"

// device configuration
#include "local_config.h"

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, ntpServer, gmtOffset_sec, daylightOffset_sec);

// DS18B20 device
OneWire oneWire(oneWirePin);
DallasTemperature sensors(&oneWire);

void setup()
{
  Serial.begin(115200);

  delay(1000);

  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);

    Serial.println("Connecting to WiFi...");

    errorCount++;

    if (errorCount > 6)
    {
      ESP.restart();
    }
  }

  errorCount = 0;

  // Initialize DS18B20 sensor
  sensors.begin();

  // Print WiFi information
  Serial.println("Connected to the WiFi network");
  Serial.println(WiFi.localIP());
  
  // Initialize NTP client and update time
  timeClient.begin();

  while (timeClient.update() == false)
  {
    // We failed to get a valid time from NTP
    errorCount++;

    Serial.println("Failed to get NTP time");
  }

  Serial.println("got NTP time");
}

void loop()
{
  Serial.println("going through temp read loop");
  time_t currentTime = timeClient.getEpochTime();

  // Uncomment to find sensor id's
  //debugEnumerateDevices();

  for (int i = 0; i < numberOfSensors; i++)
  {
    // Read temperature from sensor
    float temperature = readTemperature(sensorIds[i]);
    Serial.printf("got temperature %f\n", temperature);

    // Smooth temperature reading
    if (firstRun == true)
    {
      smoothedTemperature[i] = temperature;
      lastSendTime[i] = currentTime;
    }
    else
    {
      smoothedTemperature[i] = smoothedTemperature[i] * 0.95 + temperature * 0.05;
    }

    if (firstRun || (currentTime - lastSendTime[i] > update_period_s))
    {
      Serial.println("time to send");
      lastSendTime[i] = currentTime;
      sendJsonToRestServer(smoothedTemperature[i], sensorNames[i]);
    }
  }

  // first run? make a random delay before continuing
  // This is done so not all sensors that are started at the same time
  // - maybe due to a power-out - will not report at the same time.
  if (firstRun == true)
  {
      Serial.println("first run - add a random delay");
      delay(random(update_period_s * 1000));
      firstRun = false;
  }

  // Wait before sending the next reading
  delay(60 * 1000);
  pollCounter++;
}

float readTemperature(DeviceAddress sensorId)
{
  // Read temperature from DS18B20 sensor
  sensors.requestTemperatures();
  // int n = sensors.getDeviceCount();
  // for (int i = 0; i < n; i++)
  // {
  //   Serial.printf("temp %d %f\n", i, sensors.getTempCByIndex(i));
  // }
  return sensors.getTempC(sensorId);
}

void sendJsonToRestServer(float temperature, const char *sensorName)
{
  // Create JSON document  // format current time for inserting into json data
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
  doc["sensor_uptime"] = pollCounter;

  // Convert JSON object to string
  String jsonString;

  serializeJson(doc, jsonString);

  Serial.println(jsonString);

  do
  {
    // Create HTTP client and send POST request with temperature reading
    HTTPClient http;
    http.begin(serverUrl);
    http.addHeader("Content-Type", "application/json");
    int httpCode = 200;

    // comment for head-less testing
    httpCode = http.POST(jsonString);

    // Print result
    if (httpCode > 0)
    {
      Serial.printf("Response code: %d\n", httpCode);
      Serial.println(http.getString());
      errorCount = 0;
    }
    else
    {
      Serial.printf("Error sending POST request: %s\n", http.errorToString(httpCode).c_str());
      errorCount++;
      delay(1000);
    }

    if (errorCount > 5)
    {
      ESP.restart();
    }

  } while (errorCount > 0);

  Serial.println("Send json successful");
}

void debugEnumerateDevices()
{
  byte i;
  byte addr[8];
  
  while (oneWire.search(addr)) {
    Serial.print(" ROM =");
    for (i = 0; i < 8; i++) {
      Serial.write(' ');
      Serial.print(addr[i], HEX);
    }
    Serial.println();
  }
  Serial.println(" No more addresses.");
  Serial.println();
  oneWire.reset_search();
  delay(250);

}