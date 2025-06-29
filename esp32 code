#include <Wire.h>
#include <Adafruit_BMP085.h>
#include <HX711_ADC.h>
#include <Stepper.h>
#include "BluetoothSerial.h"

// --- BMP180s ---
Adafruit_BMP085 bmp1;
Adafruit_BMP085 bmp2;
TwoWire I2CBMP = TwoWire(1);

// --- HX711 Load Cells (4 units) ---
HX711_ADC LoadCell1(34, 26); // DOUT, SCK
HX711_ADC LoadCell2(14, 15);
HX711_ADC LoadCell3(13, 23);
HX711_ADC LoadCell4(32, 33); // Original

// --- Stepper Motor ---
const int stepsPerRevolution = 2048;
#define IN1 16
#define IN2 17
#define IN3 18
#define IN4 19
Stepper myStepper(stepsPerRevolution, IN1, IN3, IN2, IN4);

// --- Bluetooth Serial ---
BluetoothSerial SerialBT;

void setup() {
  Serial.begin(115200);
  SerialBT.begin("ESP32_SensorStepper"); // BT device name

  // BMP180s
  Wire.begin(21, 22);
  bmp1.begin(BMP085_ULTRAHIGHRES, &Wire);

  I2CBMP.begin(25, 27, 400000);
  bmp2.begin(BMP085_ULTRAHIGHRES, &I2CBMP);

  // HX711s
  LoadCell1.begin();
  LoadCell2.begin();
  LoadCell3.begin();
  LoadCell4.begin();

  LoadCell1.start(1000, true);
  LoadCell2.start(1000, true);
  LoadCell3.start(1000, true);
  LoadCell4.start(1000, true);

  LoadCell1.setCalFactor(696.0);
  LoadCell2.setCalFactor(696.0);
  LoadCell3.setCalFactor(696.0);
  LoadCell4.setCalFactor(696.0);

  // Stepper
  myStepper.setSpeed(15);
}

void sendCSV(float t1, int32_t p1, float t2, int32_t p2,
             float lc1, float lc2, float lc3, float lc4) {
  String csv = String(t1) + "," + String(p1) + "," +
               String(t2) + "," + String(p2) + "," +
               String(lc1) + "," + String(lc2) + "," +
               String(lc3) + "," + String(lc4);
  Serial.println(csv);
  SerialBT.println(csv);
}

void handleInput(String input) {
  input.trim();
  if (input == "t") {
    LoadCell1.tareNoDelay();
    LoadCell2.tareNoDelay();
    LoadCell3.tareNoDelay();
    LoadCell4.tareNoDelay();
  } else {
    int steps = input.toInt();
    if (steps != 0) {
      myStepper.step(steps);
    }
  }
}

void loop() {
  bool newDataReady = false;

  if (LoadCell1.update()) newDataReady = true;
  if (LoadCell2.update()) newDataReady = true;
  if (LoadCell3.update()) newDataReady = true;
  if (LoadCell4.update()) newDataReady = true;

  if (newDataReady) {
    float val1 = LoadCell1.getData();
    float val2 = LoadCell2.getData();
    float val3 = LoadCell3.getData();
    float val4 = LoadCell4.getData();

    float temp1 = bmp1.readTemperature();
    int32_t pressure1 = bmp1.readPressure();
    float temp2 = bmp2.readTemperature();
    int32_t pressure2 = bmp2.readPressure();

    sendCSV(temp1, pressure1, temp2, pressure2, val1, val2, val3, val4);
  }

  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');
    handleInput(input);
  }

  if (SerialBT.available() > 0) {
    String input = SerialBT.readStringUntil('\n');
    handleInput(input);
  }

  if (LoadCell1.getTareStatus() || LoadCell2.getTareStatus() ||
      LoadCell3.getTareStatus() || LoadCell4.getTareStatus()) {
    Serial.println("Tare complete");
    SerialBT.println("Tare complete");
  }
}
