//This is a sample program to be used for various devices
//it uses mqtt to communicate
//and is integrating these sensors: BME280 , Grove Multichannel Gas sensor , Grove voc and eCO2 sensor
#include <Arduino.h>

#include <ArduinoMqttClient.h>
#include <WiFiNINA.h> // for MKR1000 change to: #include <WiFi101.h>


WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);


const long interval = 60000;
unsigned long previousMillis = 0;

int count = 0;

#include <Wire.h>

#include <CAN.h>
short condition=0;

#include "MutichannelGasSensor.h"



#include "sensirion_common.h"
#include "sgp30.h"

#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BME280 bme; // I2C


/************************* WiFi Access Point *********************************/

char ssid[] = "$Your_SSID$";        // your network SSID (name)
char pass[] = "$Your_PSK$";    // your network password (use for WPA, or use as key for WEP)

/************************* Adafruit.io Setup *********************************/

#define MQTT_SERVER      "192.168.1.114"       //your brokers iP Adress
#define MQTT_SERVERPORT  1883                   // use 8883 for SSL
#define MQTT_USERNAME    ""     //username for your broker if set
#define MQTT_KEY         ""     //its password
#define PROJECT_NAME "project/" //not at all necessary

const char broker[] = MQTT_SERVER;
const char topic[]  = PROJECT_NAME;


/*************************** Sketch Code ************************************/

void setup() {
  Serial.begin(115200);
  delay(10);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(2000);
  Serial.println("Please input something to continue");
  while (!Serial.available());
  Serial.println("Program Started");
  gas.begin(0x4D); // your sensors i2c adress
  gas.powerOn();
  Serial.print("Firmware Version = ");
  Serial.println(gas.getVersion());

  s16 err;
  u16 scaled_ethanol_signal, scaled_h2_signal;
  while (sgp_probe() != STATUS_OK) {
    Serial.println("SGP failed");
    while (1);
  }
  /*Read H2 and Ethanol signal in the way of blocking*/
  err = sgp_measure_signals_blocking_read(&scaled_ethanol_signal,
                                          &scaled_h2_signal);
  if (err == STATUS_OK) {
    Serial.println("get ram signal!");
  } else {
    Serial.println("error reading signals");
  }
  err = sgp_iaq_init();

  bool status;

  // default settings
  // (you can also pass in a Wire library object like &Wire2)
  status = bme.begin();
  if (!status) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }
  
  Serial.println("CAN Sender");
  // start the CAN bus at 500 kbps
  if (!CAN.begin(500E3)) {
    Serial.println("Starting CAN failed!");
    while (1);
  }
  
  // attempt to connect to Wifi network:
  Serial.print("Attempting to connect to WPA SSID: ");
  Serial.println(ssid);
  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
    // failed, retry
    Serial.print(".");
    delay(5000);
  }

  Serial.println("You're connected to the network");
  Serial.println();



  // You can provide a unique client ID, if not set the library uses Arduino-millis()
  // Each client must have a unique client ID
  mqttClient.setId("MKRWiFi1010");
  // You can provide a username and password for authentication
  mqttClient.setUsernamePassword(MQTT_USERNAME, MQTT_KEY);

  Serial.print("Attempting to connect to the MQTT broker: ");
  Serial.println(broker);

  if (!mqttClient.connect(broker, + MQTT_SERVERPORT)) {
    Serial.print("MQTT connection failed! Error code = ");
    Serial.println(mqttClient.connectError());

    while (1);
  }

  Serial.println("You're connected to the MQTT broker!");
  Serial.println();

  digitalWrite(LED_BUILTIN, LOW);

}

uint32_t x = 0;

void loop() {
  // call poll() regularly to allow the library to send MQTT keep alives which
  // avoids being disconnected by the broker
  mqttClient.poll();


  Serial.print("Sending message to topic: ");


  // send message, the Print interface can be used to set the message contents
  //gmgs();
  gvcs();
  bmes();
  gmgs();
  ifs();
  //methods to check all things
  Serial.println();

  delay(interval);
}


void gmgs()
{
  float c;

  c = gas.measure_NH3();
  Serial.print("The concentration of NH3 is ");
  if (c >= 0) Serial.print(c);
  else Serial.print("invalid");
  Serial.println(" ppm");
  // send message, the Print interface can be used to set the message contents
  mqttClient.beginMessage("project/nh3");
  mqttClient.print(c);
  mqttClient.endMessage();


  c = gas.measure_CO();
  Serial.print("The concentration of CO is ");
  if (c >= 0) Serial.print(c);
  else Serial.print("invalid");
  Serial.println(" ppm");
  mqttClient.beginMessage("project/co");
  mqttClient.print(c);
  mqttClient.endMessage();
  if(c>175)
    condition++;

  c = gas.measure_NO2();
  Serial.print("The concentration of NO2 is ");
  if (c >= 0) Serial.print(c);
  else Serial.print("invalid");
  Serial.println(" ppm");
  mqttClient.beginMessage("project/no2");
  mqttClient.print(c);
  mqttClient.endMessage();
  if(c>230)
    condition++;

  c = gas.measure_C3H8();
  Serial.print("The concentration of C3H8 is ");
  if (c >= 0) Serial.print(c);
  else Serial.print("invalid");
  Serial.println(" ppm");
  mqttClient.beginMessage("project/c3h8");
  mqttClient.print(c);
  mqttClient.endMessage();

  c = gas.measure_C4H10();
  Serial.print("The concentration of C4H10 is ");
  if (c >= 0) Serial.print(c);
  else Serial.print("invalid");
  Serial.println(" ppm");
  mqttClient.beginMessage("project/c4h10");
  mqttClient.print(c);
  mqttClient.endMessage();

  c = gas.measure_CH4();
  Serial.print("The concentration of CH4 is ");
  if (c >= 0) Serial.print(c);
  else Serial.print("invalid");
  Serial.println(" ppm");
  mqttClient.beginMessage("project/ch4");
  mqttClient.print(c);
  mqttClient.endMessage();
  if(c>150)
    condition++;

  c = gas.measure_H2();
  Serial.print("The concentration of H2 is ");
  if (c >= 0) Serial.print(c);
  else Serial.print("invalid");
  Serial.println(" ppm");
  mqttClient.beginMessage("project/h2");
  mqttClient.print(c);
  mqttClient.endMessage();

  c = gas.measure_C2H5OH();
  Serial.print("The concentration of C2H5OH is ");
  if (c >= 0) Serial.print(c);
  else Serial.print("invalid");
  Serial.println(" ppm");
  mqttClient.beginMessage("project/c2h50h");
  mqttClient.print(c);
  mqttClient.endMessage();

  Serial.println("...");
}
void gvcs() {
  s16 err = 0;
  u16 tvoc_ppb, co2_eq_ppm;
  err = sgp_measure_iaq_blocking_read(&tvoc_ppb, &co2_eq_ppm);
  if (err == STATUS_OK) {
    Serial.print("tVOC  Concentration:");
    Serial.print(tvoc_ppb);
    Serial.println("ppb");
    mqttClient.beginMessage("project/tvoc");
    mqttClient.print((float)tvoc_ppb);
    mqttClient.endMessage();
    
    Serial.print("CO2eq Concentration:");
    Serial.print(co2_eq_ppm);
    Serial.println("ppm");
    mqttClient.beginMessage("project/eco2");
    mqttClient.print((float)co2_eq_ppm);
    mqttClient.endMessage();
    if(co2_eq_ppm>750)
    condition++;
    
  } else {
    Serial.println("error reading IAQ values\n");
  }

}


void bmes() {
  Serial.print("Temperature = ");
  Serial.print(bme.readTemperature());
  Serial.println(" *C");
  float vb = bme.readTemperature();
  if(vb>35)
    condition++;
  mqttClient.beginMessage("project/temp");
  mqttClient.print(vb);
  mqttClient.endMessage();
  
  Serial.print("Pressure = ");
  Serial.print(bme.readPressure() / 100000.0F);
  Serial.println(" hPa");
  vb = (bme.readPressure() / 100000.0F);
  mqttClient.beginMessage("project/p");
  mqttClient.print(vb);
  mqttClient.endMessage();

  Serial.print("Humidity = ");
  Serial.print(bme.readHumidity());
  Serial.println(" %");
  vb = (bme.readHumidity());
  mqttClient.beginMessage("project/h");
  mqttClient.print(vb);
  mqttClient.endMessage();

}
void ifs(){
  // send packet: id is 11 bits, packet can contain up to 8 bytes of data
  Serial.print("Sending packet ... ");

  CAN.beginPacket(0x19);
  CAN.write(condition);
  condition=0;
  CAN.endPacket();

  Serial.println("done");
}
