/* Library for managing blinking LEDs
    Easy to create multiple blink patterns for the same LED.
    Uses a the millis() function to keep track of the state of the LED.
    Does not use delay() which will cause issues in more complex code.

  Example:
  To setup a new LED blink
    Blinker led1(4, 100, 100);  LED on pin 4 will blink 100ms ON and 100ms OFF
    Blinker led2(4, 500, 500);  LED on the same pin 4 will blink slower at 500ms ON and 500ms OFF

  In the loop use this syntax
    led1.Blink();
    led2.Blink();
*/

class Blinker
{
	// Class Member Variables
	// These are initialized at startup
	int ledPin;      // the number of the LED pin
	long OnTime;     // milliseconds of on-time
	long OffTime;    // milliseconds of off-time

	// These maintain the current state
	int ledState;             		// ledState used to set the LED
	unsigned long previousMillis;  	// will store last time LED was updated

  // Constructor - creates a Blinker
  // and initializes the member variables and state
  public:
  Blinker(int pin, long on, long off)
  {
	ledPin = pin;
	pinMode(ledPin, OUTPUT);

	OnTime = on;
	OffTime = off;

	ledState = LOW;
	previousMillis = 0;
  }

  void Blink()
  {
    // check to see if it's time to change the state of the LED
    unsigned long currentMillis = millis();

    if((ledState == HIGH) && (currentMillis - previousMillis >= OnTime))
    {
    	ledState = LOW;  // Turn it off
      previousMillis = currentMillis;  // Remember the time
      digitalWrite(ledPin, ledState);  // Update the actual LED
    }
    else if ((ledState == LOW) && (currentMillis - previousMillis >= OffTime))
    {
      ledState = HIGH;  // turn it on
      previousMillis = currentMillis;   // Remember the time
      digitalWrite(ledPin, ledState);	  // Update the actual LED
    }
  }
};
