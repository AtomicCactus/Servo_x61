# Servo_x61
Timer1 interrupt driven servo library for Atmel ATTiny x61 series microcontrollers. Adapted from ServoTimer2 library. This library uses Timer1 hardware interrupt to control up to 8 servos using an ATTiny x61 series microcontroller. It is compatible with [ATTinyCore](https://github.com/SpenceKonde/ATTinyCore).

## Usage

Usage is similar to the usual Arduino Servo library. 

The `write(int)` function allows you to pass in either a position (in degrees form 0 to 180) or pulse width (in microseconds between 1000 and 5500). 

```
#include "Servo_x61.h"

// Attach servo on pin 6 (PB3).
int servoPin = 6;

Servo_x61 servo;

void setup() {
  servo.attach(servoPin);
}

void loop() {

  // Rotate the servo from 0 to 180 degrees.
  for (int i = 0; i < 180; i++) 
  {
      servo.write(i);
      delay(500);
  }
}
```