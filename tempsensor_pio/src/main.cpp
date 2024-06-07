#include <Arduino.h>

// device configuration
#include "esp32_temptest_gpt.h"
#include "watchdog.h"
#include "network.h"
#include "tempsensor.h"


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
  resetWDT();            // Reset the Watch Dog Timer

  Serial.println("going through temp read loop");
  time_t currentTime = network_get_time();

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

    if (firstRun || (currentTime - lastSendTime[i] > UPDATE_PERIOD_S))
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
      delayWithPatWatchdog(random(UPDATE_PERIOD_S * 1000));
      firstRun = false;
  }

  // Wait before sending the next reading
  delayWithPatWatchdog(60 * 1000);
  pollCounter++;
}


