#include <OneWire.h>
#include <DallasTemperature.h>
#include "tempsensor.h"

#include "esp32_temptest_gpt.h"


// DS18B20 device
OneWire oneWire(oneWirePin);
DallasTemperature sensors(&oneWire);

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
