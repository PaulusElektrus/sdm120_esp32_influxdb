// W-Lan & InfluxDB
#include <WiFi.h>
#include <InfluxDbClient.h>

// SDM --> https://github.com/reaper7/SDM_Energy_Meter
#include <SDM.h>
#include <SoftwareSerial.h>

// Definitionen
#define DEVICE "ESP32"

// WLAN Zugangsdaten
#define WIFI_SSID "+++"
#define WIFI_PASSWORD "+++"

// InfluxDB
#define INFLUXDB_URL "+++"
#define INFLUXDB_DB_NAME "energy"
#define INFLUXDB_USER "+++"
#define INFLUXDB_PASSWORD "+++"

// InfluxDB & SDM laden
InfluxDBClient client(INFLUXDB_URL, INFLUXDB_DB_NAME);
SoftwareSerial swSerSDM;
SDM sdm(swSerSDM, 9600, NOT_A_PIN, SWSERIAL_8N1, 16, 17);
///////(////////, Baud Rate ///////////////////, RX, TX)

// Variablen
float voltage, current, power_active, power_apparent, power_reactive, power_factor, phase_angle, frequency, total_energy_active, total_energy_reactive, import_energy_active, import_energy_reactive, export_energy_active, export_energy_reactive;
Point sensor("energy");

 
//////////////////////////// SETUP

void setup() 
{
  // serieller Monitor
  Serial.begin(9600);
  Serial.println("Booting...");

  Serial.println("Verbinden mit ");
  Serial.println(WIFI_SSID);

  // W-Lan Verbindung
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  // W-Lan prüfen 
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println("");
  Serial.println("WiFi verbunden..!");
  Serial.print("IP= ");  Serial.println(WiFi.localIP());

  // InfluxDB V 1.0
  client.setConnectionParamsV1(INFLUXDB_URL, INFLUXDB_DB_NAME, INFLUXDB_USER, INFLUXDB_PASSWORD);

  // SDM starten
  sensor.addTag("device", DEVICE);
  sensor.addTag("SSID", WiFi.SSID());
  sdm.begin();

  // Check InfluxDB connection
  if (client.validateConnection()) {
    Serial.print("Connected to InfluxDB: ");
    Serial.println(client.getServerUrl());
  } else {
    Serial.print("InfluxDB connection failed: ");
    Serial.println(client.getLastErrorMessage());
  }

  Serial.println("Start des Hauptprogramms");
}


//////////////////////////// HAUPTPROGRAMM

void loop() {
  sensor.clearFields();
  
  // SDM auslesen
  voltage = sdm.readVal(SDM_PHASE_1_VOLTAGE);
  current = sdm.readVal(SDM_PHASE_1_CURRENT);
  power_active = sdm.readVal(SDM_PHASE_1_POWER);
  power_apparent = sdm.readVal(SDM_PHASE_1_APPARENT_POWER);
  power_reactive = sdm.readVal(SDM_PHASE_1_REACTIVE_POWER);
  power_factor = sdm.readVal(SDM_PHASE_1_POWER_FACTOR);
  phase_angle = sdm.readVal(SDM_PHASE_1_ANGLE);
  frequency = sdm.readVal(SDM_FREQUENCY);
  total_energy_active = sdm.readVal(SDM_TOTAL_ACTIVE_ENERGY);
  total_energy_reactive = sdm.readVal(SDM_TOTAL_REACTIVE_ENERGY);
  import_energy_active = sdm.readVal(SDM_IMPORT_ACTIVE_ENERGY);
  import_energy_reactive = sdm.readVal(SDM_EXPORT_ACTIVE_ENERGY);
  export_energy_active = sdm.readVal(SDM_IMPORT_REACTIVE_ENERGY);
  export_energy_reactive = sdm.readVal(SDM_EXPORT_REACTIVE_ENERGY);
  Serial.println("Sensoren ausgelesen");

  sensor.addField("WIFI", WiFi.RSSI());
  sensor.addField("voltage", voltage);
  sensor.addField("current", current);
  sensor.addField("power_active", power_active);
  sensor.addField("power_apparent", power_apparent);
  sensor.addField("power_reactive", power_reactive);
  sensor.addField("power_factor", power_factor);
  sensor.addField("phase_angle", phase_angle);
  sensor.addField("frequency", frequency);
  sensor.addField("total_energy_active", total_energy_active);
  sensor.addField("total_energy_reactive", total_energy_reactive);
  sensor.addField("import_energy_active", import_energy_active);
  sensor.addField("import_energy_reactive", import_energy_reactive);
  sensor.addField("export_energy_active", export_energy_active);
  sensor.addField("export_energy_reactive", export_energy_reactive);

  
  // Print & Check
  Serial.print("Writing: ");
  Serial.println(client.pointToLineProtocol(sensor));
  // If no Wifi signal, try to reconnect it
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Wifi connection lost");
  }
  
  // Write point
  if (!client.writePoint(sensor)) {
    Serial.print("InfluxDB write failed: ");
    Serial.println(client.getLastErrorMessage());
  }
  
  //Wait 1s
  Serial.println("Wait 1s");
  delay(1000);
}
