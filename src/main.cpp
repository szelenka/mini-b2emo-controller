#include <SPI.h>
#include <RH_RF95.h>
#include <Adafruit_SleepyDog.h>

// Desired LoRa frequency in MHz
// 433.0, 868.0, 915.0
#define RF95_FREQ           900.0
#define RF95_TX_POWER       23 // dBm
#define RF95_PACKET_SIZE    100
// Radio Pins
// ref: https://github.com/adafruit/Adafruit-Feather-M0-RFM-LoRa-PCB/blob/master/Adafruit%20Feather%20M0%20RFM9x%20Pinout.pdf
#define PIN_RFM95_CS        8
#define PIN_RFM95_INT       3
#define PIN_RFM95_RST       4
// Button Pins
#define PIN_MECANUM_BTN     13
// Potentiometer Pins
#define PIN_EXTEND_BODY     A0
#define PIN_EXTEND_WHEELS   A1
#define PIN_DRIVE_X_AXIS    A2
#define PIN_DRIVE_Y_AXIS    A3
#define PIN_HEAD_Y_AXIS     A4
#define PIN_HEAD_X_AXIS     A5

/*
  Driver to send and receive unaddressed, unreliable datagrams via a LoRa capable radio transceiver
  ref: https://www.airspayce.com/mikem/arduino/RadioHead/classRH__RF95.html
*/
RH_RF95 rf95(PIN_RFM95_CS, PIN_RFM95_INT);

int extBody = 0;
int extWheels = 0;
int driveX = 0;
int driveY = 0;
int headY = 0;
int headX = 0;
int btnPress = 0;
char radiopacket[RF95_PACKET_SIZE] = {0};
std::size_t sendLen = 0;

/**
 * @brief Reads the analog value from a specified pin and scales it to a percentage.
 * 
 * This function reads the analog input from the given pin, which is expected to
 * return a value in the range of 0 to 1023. The value is then scaled to a percentage
 * (0 to 99) and returned. 
 * 
 * Integer division in C++ truncates the result, discarding any fractional part.
 *  - The maximum value of `analogRead(pin)` is `1023`. Substituting this into the formula:
 *    ```cpp
 *    value = (1023 * 100) / 1024;
 *    ```
 *  - This evaluates to `99`, not `100`, because of the truncation in integer division.
 * 
 * @param pin The analog pin number to read from.
 * @return int The scaled percentage value (0 to 99) based on the analog input.
 */
int readPotentiometer(int pin) {
  int value = (analogRead(pin) * 100) / 1024; 
  return value;
}

void setup() {
  pinMode(PIN_RFM95_RST, OUTPUT);
  pinMode(PIN_MECANUM_BTN, INPUT_PULLUP);
  digitalWrite(PIN_RFM95_RST, HIGH);

  Serial.begin(115200);
  delay(1500);
  // manual reset
  // digitalWrite(PIN_RFM95_RST, LOW);
  // delay(10);
  // digitalWrite(PIN_RFM95_RST, HIGH);
  // delay(10);
  while (!rf95.init()) {
    Serial.println("LoRa radio init failed");
    Serial.println("Uncomment '#define SERIAL_DEBUG' in RH_RF95.cpp for detailed debug info");
    Watchdog.sleep();
  }
  // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM
  if (!rf95.setFrequency(RF95_FREQ)) {
    Serial.println("setFrequency failed");
    Watchdog.sleep();
  }
  rf95.setTxPower(RF95_TX_POWER, false);
  Serial.println(String("Mini B2EMO Controller w/LoRa at ") + RF95_FREQ + String(" MHz ") + RF95_TX_POWER + String(" dBm"));
}

void loop() {
  // Read the button and potentiometer values
  btnPress = digitalRead(PIN_MECANUM_BTN);
  extBody = readPotentiometer(PIN_EXTEND_BODY);
  extWheels = readPotentiometer(PIN_EXTEND_WHEELS);
  driveX = readPotentiometer(PIN_DRIVE_X_AXIS);
  driveY = readPotentiometer(PIN_DRIVE_Y_AXIS);
  headY = readPotentiometer(PIN_HEAD_Y_AXIS);
  headX = readPotentiometer(PIN_HEAD_X_AXIS);

  // Format the radiopacket
  snprintf(radiopacket, RF95_PACKET_SIZE, "%d,%d,%d,%d,%d,%d,%d", extBody, extWheels, driveX, driveY, headY, headX, btnPress);
  Serial.println(radiopacket);
  sendLen = strlen(radiopacket);

  // Send the packet
  rf95.send((uint8_t *)radiopacket, sendLen);
  rf95.waitPacketSent();
  delay(10);
}
