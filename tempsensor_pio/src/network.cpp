#include <NTPClient.h>
#include <WiFi.h>
#include <ArduinoJson.h>

#include "network.h"
#include "esp32_temptest_gpt.h"
#include "watchdog.h"
#include "local_config.h"
#include <HTTPClient.h>


WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, cfg.ntpServer, cfg.gmtOffset_sec, cfg.daylightOffset_sec);

void Network::initWifi(Watchdog &watchdog)
{
  // let the device be active a while before we try to connect to WIFI
  watchdog.delayWithPatWatchdog(1001);

  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    watchdog.delayWithPatWatchdog(1001);

    Serial.println("Connecting to WiFi...");

    cfg.errorCount++;

    if (cfg.errorCount > 6)
    {
      ESP.restart();
    }
  }

  cfg.errorCount = 0;
  // Print WiFi information
  Serial.println("Connected to the WiFi network");
  Serial.println(WiFi.localIP());
}


void Network::initTime()
{
  // Initialize NTP client and update time
  timeClient.begin();

  while (timeClient.update() == false)
  {
    // We failed to get a valid time from NTP
    cfg.errorCount++;

    Serial.println("Failed to get NTP time");
  }

  Serial.println("got NTP time");
}

void Network::sendJsonToRestServer(float temperature, const char *sensorName)
{
  // Create JSON document  // format current time for inserting into json data
  char currentTimeString[80];
  struct tm ts;
  time_t rawTime = timeClient.getEpochTime();
  ts = *localtime(&rawTime);
   
  strftime(currentTimeString, sizeof(currentTimeString), "%Y-%m-%dT%H:%M:%S-01:00", &ts);

//  char fullSensorName[34];
//  sprintf(fullSensorName, "%s_%s", esp32_macaddress, sensorName);

  // Create JSON object with temperature reading
  StaticJsonDocument<200> doc;
  doc["sensor_id"] = sensorName;
  doc["temperature"] = temperature;
  doc["timestamp"] = currentTimeString;
  doc["sensor_uptime"] = cfg.pollCounter;

  // Convert JSON object to string
  String jsonString;

  serializeJson(doc, jsonString);

  Serial.println(jsonString);

#ifdef DRY_RUN
  Serial.println("DRYRUN");
  return;
#endif

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
      cfg.errorCount = 0;
    }
    else
    {
      Serial.printf("Error sending POST request: %s\n", http.errorToString(httpCode).c_str());
      cfg.errorCount++;
      delay(1000);
    }

    if (cfg.errorCount > 5)
    {
      ESP.restart();
    }

  } while (cfg.errorCount > 0);

  Serial.println("Send json successful");
}

unsigned long Network::get_time()
{
    return timeClient.getEpochTime();
}