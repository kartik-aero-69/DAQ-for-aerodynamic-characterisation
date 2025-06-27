#include <Wire.h>
#include <Adafruit_BMP085.h>
#include <HX711_ADC.h>
#include <Stepper.h>
#include "BluetoothSerial.h"

// BMP180s
Adafruit_BMP085 bmp1;
Adafruit_BMP085 bmp2;
TwoWire I2CBMP = TwoWire(1);

// HX711 Load Cell
const int HX711_dout = 32;
const int HX711_sck = 33;
HX711_ADC LoadCell(HX711_dout, HX711_sck);

// Stepper Motor
const int stepsPerRevolution = 2048;
#define IN1 16
#define IN2 17
#define IN3 18
#define IN4 19
Stepper myStepper(stepsPerRevolution, IN1, IN3, IN2, IN4);

// Bluetooth Serial
BluetoothSerial SerialBT;

void setup() {
  Serial.begin(115200);
  SerialBT.begin("ESP32_SensorStepper"); // Bluetooth device name

  Wire.begin(21, 22);
  bmp1.begin(BMP085_ULTRAHIGHRES, &Wire);

  I2CBMP.begin(25, 27, 400000);
  bmp2.begin(BMP085_ULTRAHIGHRES, &I2CBMP);

  LoadCell.begin();
  LoadCell.start(1000, true);
  LoadCell.setCalFactor(696.0);

  myStepper.setSpeed(15);
}

void sendCSV(float t1, int32_t p1, float t2, int32_t p2, float loadcellVal) {
  String csv = String(t1) + "," + String(p1) + "," + String(t2) + "," + String(p2) + "," + String(loadcellVal);
  Serial.println(csv);
  SerialBT.println(csv);
}

void handleInput(String input) {
  input.trim();
  if (input == "t") {
    LoadCell.tareNoDelay();
  } else {
    int steps = input.toInt();
    if (steps != 0) {
      myStepper.step(steps);
    }
  }
}

void loop() {
  if (LoadCell.update()) {
    float loadcellVal = LoadCell.getData();
    float temp1 = bmp1.readTemperature();
    int32_t pressure1 = bmp1.readPressure();
    float temp2 = bmp2.readTemperature();
    int32_t pressure2 = bmp2.readPressure();

    sendCSV(temp1, pressure1, temp2, pressure2, loadcellVal);
  }

  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');
    handleInput(input);
  }

  if (SerialBT.available() > 0) {
    String input = SerialBT.readStringUntil('\n');
    handleInput(input);
  }

  if (LoadCell.getTareStatus() == true) {
    Serial.println("Tare complete");
    SerialBT.println("Tare complete");
  }
}
