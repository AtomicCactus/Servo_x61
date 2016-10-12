/*
 Servo_x61.h - Interrupt driven servo library for ATTiny x61
 This library uses Timer1 to drive up to 8 servos using interrupts 
 so no refresh activity is required from within the sketch.
 
 The usage and method naming is similar to the Arduino software 
 servo library http://www.arduino.cc/playground/ComponentLib/Servo
 except that pulse widths can be in microseconds or degrees.

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin St, Fifth Floor, 
 Boston, MA 02110-1301 USA

 Ported to ATTiny x61 series by Yuri Brigance 2016
*/

#ifndef Servo_x61_h
#define Servo_x61_h

#include <inttypes.h>
#include <Arduino.h>

/** Min servo position (in degrees). */
#define MIN_POS   0

/** Max servo position (in degrees). */
#define MAX_POS   180

/** Pulse width (in microseconds) when the servo is at 0 degrees. */
#define MIN_PW      1000

/** Pulse width (in microseconds) when the servo is at 0 degrees. */
#define MAX_PW      5500

/** Default pulse width (in microseconds) when the servo is attached. */
#define DEFAULT_PW  (MAX_PW - MIN_PW) / 2 + MIN_PW

/** Total frame duration (in microseconds). */
#define FRAME_SYNC_PERIOD   20000

/** Maximum number of channels. Don't change this. */
#define NBR_CHANNELS        8

/** Defines a servo pin. Stores pin number and status. */
typedef struct
{
    /** Pin number from 0 to 31. */
    uint8_t number      :5;
    
    /** Pin is pulsed only when this is set to True. */
    uint8_t isActive    :1;
} ServoPin_t;


/** Defines a servo. Stores pin information and status. */
typedef struct
{
    /** The pin responsible for this servo. */
    ServoPin_t pin;
    
    /** Current counter. */
    byte counter;
    
    /** Remainder of the counter. */
    byte remainder;

} Servo_t;

/** Defines all servo methods. */
class Servo_x61
{
  public:
    
	/** Constructor. */
	Servo_x61();

    /**
     Attach the given pin to the next free channel. Sets pinMode(..).
     The attached servo is pulsed with the DEFAULT_PW value.
     @param int Pin number to which the servo is attached.
     @return channel number or 0 if there was a failure.
     */
	uint8_t attach(int);
    
    /**
     Deactivates the servo. This cuts power to the servo motor.
     */
    void detach();
    
    /**
     Set the new position value or pulse width for this servo.
     If parameter value is < MAX_POS then it is treated as a position.
     Otherwise it is treated as pulse width in microseconds.
     @param int Position or pulse width.
     */
    void write(int);
    
    /**
     Returns the current pulse width for this servo.
     @return Current pulse width in microseconds.
     */
    int read();
    
    /**
     Checks if this servo is attached and active.
     @return True if the servo is attached, False otherwise.
     */
	boolean attached();
    
 private:
    
    /**
     Index into the channel data for this servo.
     */
    uint8_t channelIndex;
};

#endif
