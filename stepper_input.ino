#include <Stepper.h>

// Number of steps per revolution for 28BYJ-48
const int stepsPerRevolution = 2048;

// Define safe GPIOs for ESP32 38-pin board
#define IN1 16
#define IN2 17
#define IN3 18
#define IN4 19

// Initialize the stepper library on pins IN1, IN3, IN2, IN4
Stepper myStepper(stepsPerRevolution, IN1, IN3, IN2, IN4);

int stepsToMove = 0;

void setup() {
  Serial.begin(115200);
  Serial.println("Enter number of steps (positive or negative):");
  myStepper.setSpeed(15); // 15 RPM for reliable operation
}

void loop() {
  if (Serial.available() > 0) {
    stepsToMove = Serial.parseInt();
    Serial.print("Moving ");
    Serial.print(stepsToMove);
    Serial.println(" steps.");
    myStepper.step(stepsToMove);
    Serial.println("Done. Enter new number of steps:");
  }
}
