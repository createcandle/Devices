/*
 * Candle cleaner
 * 
 * This script is required by the Candle Manager add-on for the Mozilla WebThings Gateway, as it is used during the 'test' phase. 
 * 
 * It functions as a 'factory reset. It clears the memory (eeprom) of the Arduino that you upload this code to. Afterwards it simply blinks its LED.
 * 
 * Wiping the memory removes old security/encryption settings, as well as any old network ID that the device may have had.
 * 
 * SETTINGS */ 

int blink_seconds = 2;                                        // Blink duration. How many seconds should a LED blink take?

 /* END OF SETTINGS
 * 
 * 
 */

#include <EEPROM.h>                                           // Import the required library to work with the EEPROM (storage memory) of the Arduino


// The setup function runs once when you press reset or power the board
void setup()
{
  Serial.begin(115200);                                       // Allows for serial debugging over USB.
  Serial.println("Hello");
  
  Serial.println("Starting memory wipe.");
  for (int i = 0 ; i < EEPROM.length() ; i++) {
    EEPROM.write(i, 255);
  }
  Serial.println("My memory (eeprom) has been wiped.");

  if(blink_seconds <= 0){                                     // Blink time should not be set to 0 (or less).
    blink_seconds = 1;
  }
  pinMode(LED_BUILTIN, OUTPUT);                               // Set the LED pin to output mode.
}


//The main loop function runs over and over again, forever.
void loop() {
  
  digitalWrite(LED_BUILTIN, HIGH);                            // Turn the LED on (HIGH is the voltage level).
  delay( blink_seconds*1000 );                          // Wait before we continue.
  
  digitalWrite(LED_BUILTIN, LOW);                             // Turn the LED off by making the voltage LOW.
  delay( blink_seconds*1000 );                          // Wait before we continue.
  
  Serial.println(F("I just blinked my LED"));
}
