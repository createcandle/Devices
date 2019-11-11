/*
* 
* The Candle receiver acts as the bridge between the Candle devices and the Candle Controller. 
* 
* It only allows communication with other Candle devices that use the same encryption password as it uses itself. 
* When you install the Candle Manager, a random password is generated for you. If you ever want to change the encryption password used by your network, this can be done in the Candle Manager settings. 
* Be warned that you will have to re-create this receiver as well as all your devices, since they will all need to have new code with the new password in it.
* 
* If you have already installed the MySensors add-on, please temporarily disable it before creating this receiver. Otherwise the MySensors add-on may try to connect to it during the creation process, and thus disrupt it.
* 
*
* SETTINGS */ 

// You can enable and disable the settings below by adding or removing double slashes ( // ) in front of a line.

#define RF_NANO                                     // RF-Nano. Enable this if you are using the RF-Nano Arduino, which has a built in radio. The Candle project uses the RF-Nano.

/* END OF SETTINGS
*
*
*
*/


// Enable MySensors debug output to the serial monitor, so you can check if the radio is working ok.
//#define MY_DEBUG 

#ifdef RF_NANO
// If you are using an RF-Nano, you have to switch CE and CS pins.
#define MY_RF24_CS_PIN 9                            // Used by the MySensors library.
#define MY_RF24_CE_PIN 10                           // Used by the MySensors library.
#endif

// Enable and select radio type attached
#define MY_RADIO_RF24
//#define MY_RADIO_NRF5_ESB
//#define MY_RADIO_RFM69
//#define MY_RADIO_RFM95

// Set LOW transmit power level as default, if you have an amplified NRF-module and
// power your radio separately with a good regulator you can turn up PA level.
//#define MY_RF24_PA_LEVEL RF24_PA_MIN
//#define MY_RF24_PA_LEVEL RF24_PA_LOW
//#define MY_RF24_PA_LEVEL RF24_PA_HIGH
#define MY_RF24_PA_LEVEL RF24_PA_MAX

// Mysensors advanced security
#define MY_ENCRYPTION_SIMPLE_PASSWD "changeme"      // The Candle Manager add-on will change this into the actual password your network uses.
//#define MY_SECURITY_SIMPLE_PASSWD "changeme"      // Be aware, the length of the password has an effect on memory use.
//#define MY_SIGNING_SOFT_RANDOMSEED_PIN A7         // Setting a pin to pickup random electromagnetic noise helps make encryption more secure.

// Mysensors advanced settings
//#define MY_RF24_CHANNEL 100                       // In EU the default channel 76 overlaps with wifi, so you could try using channel 100. But you will have to set this up on every device, and also on the controller. You can even try 115.
//#define MY_RF24_DATARATE RF24_250KBPS             // Slower datarate increases the range, but the RF-Nano does not support this slow speed.
#define MY_RF24_DATARATE RF24_1MBPS                 // This datarate is supported by pretty much all NRF24 radios, including the RF-Nano.
#define MY_SPLASH_SCREEN_DISABLED                   // Saves a little memory.

// Enable serial gateway
#define MY_GATEWAY_SERIAL                           // This is the main function of this code. It tells the MySensors library to turn this device into a gateway for MySensors network.

#include <MySensors.h>                              // The MySensors library, which takes care of creating the wireless network.
#include <avr/wdt.h>                                // The watchdog timer - if the device becomes unresponsive and doesn't periodically reset the timer, then it will automatically reset once the timer reaches 0.

// Clock for the watchdog
#define INTERVAL 1000                               // Every second we reset the watchdog timer. If the device freezes, the watchdog will not re reset, and the device will reboot.
unsigned long previousMillis = 0;                   // Used to run the internal clock


void setup()
{
  wdt_enable(WDTO_2S);                              // Starts the watchdog timer. If it is not reset at least once every 2 seconds, then the entire device will automatically restart.                                 
}


void presentation()
{
	// The receiver does not have any extra children.
}


void loop()
{
  if(millis() - previousMillis >= INTERVAL){        // Main loop, runs every second.
    previousMillis = millis();                      // Store the current time as the previous measurement start time.
    wdt_reset();                                    // Reset the watchdog timer
  }
}


/**
* The MySensors Arduino library handles the wireless radio link and protocol
* between your home built sensors/actuators and HA controller of choice.
* The sensors forms a self healing radio network with optional repeaters. Each
* repeater and gateway builds a routing tables in EEPROM which keeps track of the
* network topology allowing messages to be routed to nodes.
*
* Created by Henrik Ekblad <henrik.ekblad@mysensors.org>
* Copyright (C) 2013-2018 Sensnology AB
* Full contributor list: https://github.com/mysensors/MySensors/graphs/contributors
*
* Documentation: http://www.mysensors.org
* Support Forum: http://forum.mysensors.org
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* version 2 as published by the Free Software Foundation.
*/
