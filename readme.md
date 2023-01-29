# SDM120 --> ESP32 --> InfluxDB

The sketch SDM120.ino can be used to get data from an Eastron SDM120 Modbus Meter using an ESP32 Microcontroller and then sending the data to InfluxDB. You need a TTL to RS485 Module converter with Automatic Flow Control for this example sketch.

InfluxDB can then easy be used for example with Grafana, see Screenshot:

![Screenshot](Screenshot.png)

Thanks to [Reaper7](https://github.com/reaper7/SDM_Energy_Meter)