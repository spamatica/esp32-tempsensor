# esp32-tempsensor

## Temperature sensor and temperature data collector

Both are based to a large extent on code provided by Chat GPT. :-o

### Server
The server is written in golang and receives temperature readings on a REST interface and writes them down to a sqlite database defined by the create_database.sql script.

### Sensor
The sensor part relies on an esp32 module coupled with a Dallas DS18B20 sensor for getting temperature readings. 
The esp32 uses NTP to create a proper timestamp.

Temperature readings are pushed to the defined rest server in Json format. Multiple sensors can be connected to the same server by giving them unique identifications.

