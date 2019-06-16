/*
 * Candle Example
 * 
 * This is an example Arduino Sketch that shows how you can integrate your Arduino code with the Candle Controller.
 * 
 * This commented out part at the top of the sketch, which you are reading now, will be shown in the 'explanation' section of the Candle Manager interface.
 * You can use it to explain in more detail how the device works, and that the settings do.
 * 
 * If you upload this code to an Arduino it will simply blink its LED.
 * 
 * Learn more at https://github.com/createcandle/Candle-manager-addon or https://www.createcandle.com.
 * 
 * 
 * 
 * 
 * SETTINGS */ 

// When users create new code from a template like this, the settings in this SETTINGS area are turned into an easy to use interface.
// it will only do this for settings in between the "SETTINGS */" and "/* END OF SETTINGS" lines.

// The first sentence of the comment will become the title, while the second and later sentences are turned into the comment text beneath the input field.

int blinkSeconds = 2;                               // Blink delay. How many seconds should a LED blink on-off take?

char character_array_example[14] = "+31123456789";  // Phone number. This is an example of a string (character array) variable.

#define STRING_EXAMPLE "passwordje"                 // Define a value. Defines are also possible. They are turned into a settings interface automatically too.

#define NUMBER_EXAMPLE 5                            // This first sentence will become the title. This second sentence will become the explanation comment underneath the input field. Try it.

#define BOOLEAN_EXAMPLE                             // A boolean value. A basic #define without a value is turned into a checkbox. This will toggle whether or not the define is commented out.


 /* END OF SETTINGS
 * 
 * 
 */



// The Candle Manager will also try to download any required Arduino libraries:

// This is a simple example. Here the name that the Candle manager will search for is taken from the include 
// value. In this case it will search for and try to install "SoftwareSerial". The name will be taken from inbetween < > or " ".
#include <SoftwareSerial.h>

// This is a more complex example. Often the filename and the library name will differ. 
// To tell the Candle Manager specificall which library should be downloaded you can add the proper name 
// as a comment, in between two quotation marks:
#include "Seeed_BME280.h"                           // "Grove - Barometer Sensor BME280". A relatively new library (as of 2018), works well with cheap BME280 sensors from China.


// Finally, libraries with "avr/" at the beginning of their name will be ignored.
#include <avr/wdt.h>                                // The watchdog timer - if the device becomes unresponsive and doesn't periodically reset the timer, then it will automatically reset once the timer reaches 0.



// The setup function runs once when you press reset or power the board
void setup()
{
  Serial.begin(115200);                               // For serial debugging over USB. Always use 115200 baud.
  Serial.println("Hello");
  
  pinMode(LED_BUILTIN, OUTPUT);

  if(blinkSeconds <= 0){                              // This should not be set to 0 (or less).
    blinkSeconds = 1;
  }
}


// The loop function runs over and over again forever
void loop() {
  
  digitalWrite(LED_BUILTIN, HIGH);                    // Turn the LED on (HIGH is the voltage level).
  delay( (blinkSeconds / 2)*1000 );                   // Wait before we continue.
  
  digitalWrite(LED_BUILTIN, LOW);                     // Turn the LED off by making the voltage LOW.
  delay( (blinkSeconds / 2)*1000 );                   // Wait before we continue.
  
  Serial.println("I just blinked my LED");
  
}
