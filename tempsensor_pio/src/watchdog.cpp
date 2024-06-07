#include <esp_task_wdt.h>
#include <esp32-hal.h>

#include "esp32_temptest_gpt.h"

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
  esp_task_wdt_init(WDT_TIMEOUT_S, true);  // enable panic so ESP32 restarts
  esp_task_wdt_add(NULL);                // add current thread to WDT watch

  esp_reset_reason_t bootReason = esp_reset_reason();
  // TODO handle
  // ESP_RST_WDT == wdt
  // ESP_RST_POWERON == normal power on
}

void resetWDT()
{
	esp_task_wdt_reset();
}