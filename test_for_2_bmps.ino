#include <Wire.h>
#include <Adafruit_BMP085.h>

// Create sensor objects
Adafruit_BMP085 bmp1;
Adafruit_BMP085 bmp2;

// Create a second I2C bus on pins 25 (SDA) and 27 (SCL)
TwoWire I2CBMP = TwoWire(1);

void setup() {
  Serial.begin(115200);

  // Initialize first BMP180 on default I2C pins (GPIO21 SDA, GPIO22 SCL)
  Wire.begin(21, 22);
  if (!bmp1.begin(BMP085_ULTRAHIGHRES, &Wire)) {
    Serial.println("BMP180 #1 not found!");
    while (1);
  }

  // Initialize second BMP180 on custom I2C pins (GPIO25 SDA, GPIO27 SCL)
  I2CBMP.begin(25, 27, 100000); // 100kHz I2C speed
  if (!bmp2.begin(BMP085_ULTRAHIGHRES, &I2CBMP)) {
    Serial.println("BMP180 #2 not found!");
    while (1);
  }
}

void loop() {
  // Read temperature and pressure from BMP180 #1
  float temp1 = bmp1.readTemperature();
  int32_t pressure1 = bmp1.readPressure();

  // Read temperature and pressure from BMP180 #2
  float temp2 = bmp2.readTemperature();
  int32_t pressure2 = bmp2.readPressure();

  // Print results
  Serial.print("BMP180 #1: Temp = ");
  Serial.print(temp1);
  Serial.print(" C, Pressure = ");
  Serial.print(pressure1);
  Serial.println(" Pa");

  Serial.print("BMP180 #2: Temp = ");
  Serial.print(temp2);
  Serial.print(" C, Pressure = ");
  Serial.print(pressure2);
  Serial.println(" Pa");

  Serial.println("---------------------------");
  delay(1000);
}
