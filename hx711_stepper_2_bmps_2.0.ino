#include <Wire.h>
#include <Adafruit_BMP085.h>
#include <HX711_ADC.h>
#include <Stepper.h>

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

void setup() {
  Serial.begin(115200);

  Wire.begin(21, 22);
  bmp1.begin(BMP085_ULTRAHIGHRES, &Wire);

  I2CBMP.begin(25, 27, 400000);
  bmp2.begin(BMP085_ULTRAHIGHRES, &I2CBMP);

  LoadCell.begin();
  LoadCell.start(1000, true);
  LoadCell.setCalFactor(696.0);

  myStepper.setSpeed(15);
}

void loop() {
  if (LoadCell.update()) {
    float loadcellVal = LoadCell.getData();
    float temp1 = bmp1.readTemperature();
    int32_t pressure1 = bmp1.readPressure();
    float temp2 = bmp2.readTemperature();
    int32_t pressure2 = bmp2.readPressure();

    Serial.print(temp1); Serial.print(",");
    Serial.print(pressure1); Serial.print(",");
    Serial.print(temp2); Serial.print(",");
    Serial.print(pressure2); Serial.print(",");
    Serial.println(loadcellVal);
  }

  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');
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

  if (LoadCell.getTareStatus() == true) {
    Serial.println("Tare complete");
  }
}
