//Your own CAN device
//I have used Arduino Leonardo. Your pin definations may be different
//I have just changed the default sketch, to my requirement.
// you may change it as per your requirements
//Dont forget to added 120E Resistor on both sides of CAN Bus if using it.

#include <CAN.h> //library by sandeep mistry
//link to git: https://github.com/sandeepmistry/arduino-CAN

#define fan_pin 13
#define red_pin 3
#define green_pin 5
#define blue_pin 6
short data = 0;

void setup() {
  Serial.begin(115200);
  //while (!Serial);
  pinMode(fan_pin, OUTPUT);
  pinMode(red_pin, OUTPUT);
  pinMode(blue_pin, OUTPUT);
  pinMode(green_pin, OUTPUT);

  Serial.println("CAN Receiver");

  // start the CAN bus at 500 kbps
  if (!CAN.begin(500E3)) {
    Serial.println("Starting CAN failed!");
    while (1);
  }
}

void loop() {
  // try to parse packet
  int packetSize = CAN.parsePacket();
  if (packetSize) {
    // received a packet
    Serial.print("Received ");
    if (CAN.packetExtended()) {
      Serial.print("extended ");
    }
    if (CAN.packetRtr()) {
      // Remote transmission request, packet contains no data
      Serial.print("RTR ");
    }
    Serial.print("packet with id 0x");
    //int pkt_id = CAN.packetId(), HEX;
    if (CAN.packetRtr()) {
      Serial.print(" and requested length ");
      Serial.println(CAN.packetDlc());
    } else {
      Serial.print(" and length ");
      Serial.println(packetSize);

      // only print packet data for non-RTR packets
      while (CAN.available()) {
        data = CAN.read();
      }
      Serial.println(data);
    }
    if (data < 1) {
      green();
      speed_(75);
    }
    else if (data < 5) {
      yellow();
      speed_(160);
    }
    else {
      red();
      speed_(255);
    }
    Serial.println();
  }
}
void speed_(int r) {
  analogWrite(fan_pin, r);
}
void green() {
  analogWrite(red_pin, 0);
  analogWrite(green_pin, 175);
  analogWrite(blue_pin, 0);
  //0, 175, 0
}
void red() {
  analogWrite(red_pin, 135);
  analogWrite(green_pin, 0);
  analogWrite(blue_pin, 0);
  // 135, 0, 0
}
void yellow() {
  analogWrite(red_pin, 215);
  analogWrite(green_pin, 215);
  analogWrite(blue_pin, 95);
  //215, 215, 95
}
