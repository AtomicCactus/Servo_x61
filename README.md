# Servo_x61
Timer1 (8 bit) interrupt driven servo library for Atmel ATTiny x61 series microcontrollers. Adapted from [ServoTimer2](https://github.com/nabontra/ServoTimer2) library. This library uses Timer1 hardware interrupt to control up to 8 servos using an ATTiny x61 series microcontroller. It is compatible with [ATTinyCore](https://github.com/SpenceKonde/ATTinyCore).

## Usage

The public interface is meant to mirror the [Arduino Servo library](https://www.arduino.cc/en/Reference/Servo).

### Example

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
