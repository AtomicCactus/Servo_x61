extern "C" {
  // AVR LibC Includes
  #include <inttypes.h>
  #include <avr/interrupt.h>

}
#include <Arduino.h>
#include "Servo_x61.h"

/** Frame sync delay is the first entry in the channel array. */
#define FRAME_SYNC_INDEX    0

/** Number of iterations of the ISR to get the desired frame rate. */
#define FRAME_SYNC_DELAY    ((FRAME_SYNC_PERIOD - (NBR_CHANNELS * DEFAULT_PW))/ 128)

/** Number of microseconds of calculation overhead to be subtracted from pulse timings. */
#define DELAY_ADJUST        8

/** Static array holding servo data for all channels. */
static Servo_t servos[NBR_CHANNELS+1];

/** Counter holding the number of attached channels. */
static uint8_t channelCount = 0;

/** Counter holding the channel being pulsed. */
static volatile uint8_t currentChannel;

/** ISR iteration counter used in the interrupt routines. */
static volatile uint8_t ISRCount;

/** Flag to indicate if the ISR has been initialised. */
static boolean isStarted = false;       // flag to indicate if the ISR has been initialised

/**
 Sets the new pulse width for a given channel.
 @param channel Channel for which to set the pusle width.
 @param pw Pulse width value.
 */
static void writeChannel(uint8_t channel, int pw)
{
    // Calculate and store the values for the given channel.
    if( (channel > 0) && (channel <= NBR_CHANNELS) )   // Ensure channel is valid
    {
        // Ensure pulse width is valid.
        if (pw < MIN_PW)
        {
            pw = MIN_PW;
        }
        else if (pw > MAX_PW)
        {
            pw = MAX_PW;
        }
        
        // Subtract the time it takes to process the start and end pulses (mostly from digitalWrite)
        pw -= DELAY_ADJUST;
        
        // Write the values.
        servos[channel].counter = pw / 128;
        servos[channel].remainder = 255 - (2 * (pw - (pw * 128)));  // Number of 0.5us ticks for timer overflow
    }
}

/** 
 Sets up the Timer1 overflow interrupt and does other relevant setup.
 */
static void initISR()
{
    // NOTE: Channels start from index 1.
    for (uint8_t i = 1; i <= NBR_CHANNELS; i++)
    {
        // Store default PWM value.
        writeChannel(i, DEFAULT_PW);
    }
    servos[FRAME_SYNC_INDEX].counter = FRAME_SYNC_DELAY;   // store the frame sync period
    
    currentChannel = 0;     // Clear the channel index.
    ISRCount = 0;           // Clear the value of the ISR counter.
    TIMSK |= _BV(TOIE1);    // Enable Timer1 overflow interrupt.
    TCCR1B = _BV(CS10);     // Set prescaler to match CPU frequency.
    TIFR |= _BV(TOV1);      // Clear Timer1 overflow flag (pending interrupts).
    isStarted = true;       // Initialization complete flag.
}

/**
 Interrupt. Called when Timer1 overflows.
 */
ISR(TIMER1_OVF_vect)
{
    // Increment the overflow counter.
    ++ISRCount;
    
    // Is this the final iteration for this channel?
    if (ISRCount == servos[currentChannel].counter)
    {
        // Set counter for overflow to the remaining number of ticks.
        TCNT1 = servos[currentChannel].remainder;
        servos[FRAME_SYNC_INDEX].counter = FRAME_SYNC_DELAY - (servos[currentChannel].counter) * NBR_CHANNELS;
    }
    else if(ISRCount >  servos[currentChannel].counter)
    {
        // Finished timing the channel so set it to LOW and move on.
        ServoPin_t pin = servos[currentChannel].pin;
        if(pin.isActive == true)
        {
            digitalWrite(pin.number, LOW);
        }
        
        // Go to the next channel.
        currentChannel++;    // increment to the next channel
        
        // Reset the ISR iteration counter.
        ISRCount = 0;
        
        // Reset the clock counter register.
        TCNT1 = 0;
        
        // Check if the next channel should be set to HIGH.
        if ((currentChannel != FRAME_SYNC_INDEX) && (currentChannel <= NBR_CHANNELS))
        {
            ServoPin_t pin = servos[currentChannel].pin;
            if (pin.isActive == true)
            {
                digitalWrite(pin.number, HIGH);
            }
        }
        
        // All done, start over.
        else if (currentChannel > NBR_CHANNELS)
        {
            currentChannel = 0;
        }
    }
}

Servo_x61::Servo_x61()
{
    // If there are available channels.
    if (channelCount < NBR_CHANNELS)
    {
        // Assign a channel number to this instance.
        this->channelIndex = ++channelCount;
    }
    else
    {
        // Too many channels.
        // Assigning 0 prevents this instance from functioning.
        this->channelIndex = 0;
    }
}

uint8_t Servo_x61::attach(int pinNumber)
{
    // If this is the first servo to be attached, set up Timer1 ISR.
    if (isStarted == false)
    {
        initISR();
    }
	if(this->channelIndex > 0)
	{
        pinMode(pinNumber, OUTPUT);
        servos[this->channelIndex].pin.number = pinNumber;
        servos[this->channelIndex].pin.isActive = true;
	} 
	return this->channelIndex ;
}

void Servo_x61::detach()  
{
    digitalWrite(servos[this->channelIndex].pin.number, LOW);
    servos[this->channelIndex].pin.isActive = false;
}

void Servo_x61::write(int pw)
{
    int adjustedPw = pw;
    if (pw <= MAX_POS) {
        adjustedPw = map(pw, MIN_POS, MAX_POS, MIN_PW, MAX_PW);
    }
    writeChannel(this->channelIndex, adjustedPw);
}

int Servo_x61::read()
{
    unsigned int pw = 0;
    if (this->channelIndex > 0)
    {
        pw = servos[this->channelIndex].counter * 128 + ((255 - servos[this->channelIndex].remainder) / 2) + DELAY_ADJUST;
    }
    return pw;
}
 
boolean Servo_x61::attached()
{
    return servos[this->channelIndex].pin.isActive ;
}



 
