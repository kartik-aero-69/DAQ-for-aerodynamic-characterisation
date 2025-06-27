#include <Wire.h>
#include <Adafruit_BMP085.h>
#include <HX711_ADC.h>
#if defined(ESP8266)|| defined(ESP32) || defined(AVR)
#include <EEPROM.h>
#endif

// BMP180s
Adafruit_BMP085 bmp1;
Adafruit_BMP085 bmp2;
TwoWire I2CBMP = TwoWire(1);

// HX711 Load Cell
const int HX711_dout = 32; // DT pin
const int HX711_sck = 33;  // SCK pin
HX711_ADC LoadCell(HX711_dout, HX711_sck);

void setup() {
  Serial.begin(115200);

  Wire.begin(21, 22);
  bmp1.begin(BMP085_ULTRAHIGHRES, &Wire);

  I2CBMP.begin(25, 27, 400000);
  bmp2.begin(BMP085_ULTRAHIGHRES, &I2CBMP);

  LoadCell.begin();
  LoadCell.start(1000, true);
  LoadCell.setCalFactor(696.0);
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
    char inByte = Serial.read();
    if (inByte == 't') LoadCell.tareNoDelay();
  }

  if (LoadCell.getTareStatus() == true) {
    Serial.println("Tare complete");
  }
}
