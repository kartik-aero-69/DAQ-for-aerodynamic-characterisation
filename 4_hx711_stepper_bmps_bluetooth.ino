#include <Wire.h>
#include <Adafruit_BMP085.h>
#include <HX711_ADC.h>
#include <Stepper.h>
#include "BluetoothSerial.h"

// BMP180s
Adafruit_BMP085 bmp1;
Adafruit_BMP085 bmp2;
TwoWire I2CBMP = TwoWire(1);

// HX711 Load Cells (4 units, unique pins, none on GPIO 2)
HX711_ADC LoadCell1(32, 33); // DT, SCK
HX711_ADC LoadCell2(26, 27);
HX711_ADC LoadCell3(14, 12);
HX711_ADC LoadCell4(13, 15); // Changed from (4,2) to (13,15)

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
  SerialBT.begin("ESP32_SensorStepper");

  Wire.begin(21, 22);
  bmp1.begin(BMP085_ULTRAHIGHRES, &Wire);

  I2CBMP.begin(25, 23, 400000);
  bmp2.begin(BMP085_ULTRAHIGHRES, &I2CBMP);

  LoadCell1.begin(); LoadCell1.start(1000, true); LoadCell1.setCalFactor(696.0);
  LoadCell2.begin(); LoadCell2.start(1000, true); LoadCell2.setCalFactor(696.0);
  LoadCell3.begin(); LoadCell3.start(1000, true); LoadCell3.setCalFactor(696.0);
  LoadCell4.begin(); LoadCell4.start(1000, true); LoadCell4.setCalFactor(696.0);

  myStepper.setSpeed(15);
}

void sendCSV(float t1, int32_t p1, float t2, int32_t p2, float v1, float v2, float v3, float v4) {
  String csv = String(t1) + "," + String(p1) + "," + String(t2) + "," + String(p2) + "," +
               String(v1) + "," + String(v2) + "," + String(v3) + "," + String(v4);
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
  bool ready1 = LoadCell1.update();
  bool ready2 = LoadCell2.update();
  bool ready3 = LoadCell3.update();
  bool ready4 = LoadCell4.update();

  if (ready1 && ready2 && ready3 && ready4) {
    float v1 = LoadCell1.getData();
    float v2 = LoadCell2.getData();
    float v3 = LoadCell3.getData();
    float v4 = LoadCell4.getData();
    float temp1 = bmp1.readTemperature();
    int32_t pressure1 = bmp1.readPressure();
    float temp2 = bmp2.readTemperature();
    int32_t pressure2 = bmp2.readPressure();

    sendCSV(temp1, pressure1, temp2, pressure2, v1, v2, v3, v4);
  }

  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');
    handleInput(input);
  }

  if (SerialBT.available() > 0) {
    String input = SerialBT.readStringUntil('\n');
    handleInput(input);
  }

  if (LoadCell1.getTareStatus() || LoadCell2.getTareStatus() || LoadCell3.getTareStatus() || LoadCell4.getTareStatus()) {
    Serial.println("Tare complete");
    SerialBT.println("Tare complete");
  }
}
