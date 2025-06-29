#include <Wire.h>
#include <Adafruit_BMP085.h>
#include <HX711_ADC.h>
#include <Stepper.h>

// BMP180s
Adafruit_BMP085 bmp1;
Adafruit_BMP085 bmp2;
TwoWire I2CBMP = TwoWire(1);

// HX711 Load Cells (4 units)
HX711_ADC LoadCell1(34, 26); // DOUT, SCK
HX711_ADC LoadCell2(14, 15);
HX711_ADC LoadCell3(13, 23);
HX711_ADC LoadCell4(32, 33); // Existing

// Stepper Motor
const int stepsPerRevolution = 2048;
#define IN1 16
#define IN2 17
#define IN3 18
#define IN4 19
Stepper myStepper(stepsPerRevolution, IN1, IN3, IN2, IN4);

void setup() {
  Serial.begin(115200);

  // BMP180s
  Wire.begin(21, 22); // Default I2C
  bmp1.begin(BMP085_ULTRAHIGHRES, &Wire);

  I2CBMP.begin(25, 27, 400000); // Second I2C port
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

  LoadCell1.setCalFactor(696.0); // Adjust as needed
  LoadCell2.setCalFactor(696.0);
  LoadCell3.setCalFactor(696.0);
  LoadCell4.setCalFactor(696.0);

  // Stepper Motor
  myStepper.setSpeed(15);
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

    Serial.print(temp1); Serial.print(",");
    Serial.print(pressure1); Serial.print(",");
    Serial.print(temp2); Serial.print(",");
    Serial.print(pressure2); Serial.print(",");
    Serial.print(val1); Serial.print(",");
    Serial.print(val2); Serial.print(",");
    Serial.print(val3); Serial.print(",");/////Serial.println(val4);
    Serial.print(val4);Serial.print(",,,,");
Serial.println(val1+val2+val3+val4);
  }

  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');
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

  if (LoadCell1.getTareStatus() || LoadCell2.getTareStatus() ||
      LoadCell3.getTareStatus() || LoadCell4.getTareStatus()) {
    Serial.println("Tare complete");
  }
}
