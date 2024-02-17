#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <time.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <esp_task_wdt.h>

// device configuration
#include "local_config.h"

#include "esp32_temptest_gpt.h"

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, ntpServer, gmtOffset_sec, daylightOffset_sec);

// DS18B20 device
OneWire oneWire(oneWirePin);
DallasTemperature sensors(&oneWire);

void delayWithPatWatchdog(int ms)
{
  int incr = 0;
  while (incr != ms)
  {
    incr++;
    if (incr % 1000) // pat watchdog every second
    {
      esp_task_wdt_reset();
    }
    delay(1);
  }
}

void initWDT()
{
  esp_task_wdt_init(WDT_TIMEOUT, true);  // enable panic so ESP32 restarts
  esp_task_wdt_add(NULL);                // add current thread to WDT watch

  esp_reset_reason_t bootReason = esp_reset_reason();
  // TODO handle
  // ESP_RST_WDT == wdt
  // ESP_RST_POWERON == normal power on
}

void initWifi()
{
  // let the device be active a while before we try to connect to WIFI
  delayWithPatWatchdog(1001);

  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delayWithPatWatchdog(1001);

    Serial.println("Connecting to WiFi...");

    errorCount++;

    if (errorCount > 6)
    {
      ESP.restart();
    }
  }

  errorCount = 0;
  // Print WiFi information
  Serial.println("Connected to the WiFi network");
  Serial.println(WiFi.localIP());
}

void initTime()
{
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

void initSensors()
{
  uint8_t mac_address[8];
  esp_efuse_mac_get_default(mac_address);

  for (int i = 0; i<8; i++)
  {
      sprintf(&esp32_macaddress[i*2], "%X", mac_address[i]);
      Serial.printf("%X", mac_address[i]);
  }
  Serial.println();
  Serial.print("esp32_macaddress: ");
  //Serial.println(esp32_macaddress);
  for (int i = 0; i < 16; i++)
  {
    Serial.printf("%c", esp32_macaddress[i]);
  }
  Serial.println();

  byte i;
  DeviceAddress addr;

  // Initialize DS18B20 sensor
  sensors.begin();
  int cnt = sensors.getDeviceCount();
  int cnt2 = 0;

  oneWire.reset_search();

  while (oneWire.search(addr))
  {
//    Serial.println("got onewire device");
    memcpy(sensorIds[cnt2], addr, 8);

    for (int i = 0; i<8; i++)
    {
        sprintf(&sensorNames[cnt2][i*2], "%X", addr[i]);
        //Serial.printf("%X ", mac_address[i]);
    }
//    sprintf(sensorNames[cnt2],   "%1X%1X%1X%1X", addr[0], addr[1], addr[2], addr[3]);
//    sprintf(sensorNames[cnt2]+8, "%1X%1X%1X%1X", addr[4], addr[5], addr[6], addr[7]);

#define _ADDRESS
#ifdef PRINT_ADDRESS
    Serial.print(" ROM =");
    for (i = 0; i < 8; i++)
    {
      Serial.write(' ');
      Serial.print(addr[i], HEX);
    }
    Serial.println();

    Serial.println(sensorNames[cnt2]);
    cnt2++;
  }
  Serial.println(" No more addresses.");
  Serial.println();
#else
    cnt2++;
  }
#endif

  oneWire.reset_search();
  delay(250);

  if (cnt2 != cnt)
  {
    Serial.printf("Warning: number of devices incorrect! %d/%d", cnt,cnt2);
  }

  numberOfSensors = cnt;
}

void setup()
{
  initWDT();

  Serial.begin(115200);

  initWifi();

  initSensors();

  initTime();
}

void loop()
{
  esp_task_wdt_reset();            // Reset the Watch Dog Timer

  Serial.println("going through temp read loop");
  time_t currentTime = timeClient.getEpochTime();

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
      delayWithPatWatchdog(random(update_period_s * 1000));
      firstRun = false;
  }

  // Wait before sending the next reading
  delayWithPatWatchdog(60 * 1000);
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

//  char fullSensorName[34];
//  sprintf(fullSensorName, "%s_%s", esp32_macaddress, sensorName);

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
