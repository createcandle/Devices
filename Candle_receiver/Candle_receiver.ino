/*
* 
* The Candle Serial Gateway acts as the brige between the Candle devices and the main Candle Controller.
* 
* This device in the only one that does NOT output any test data.
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

// Enable serial gateway
#define MY_GATEWAY_SERIAL


// Mysensors advanced security
#define MY_ENCRYPTION_SIMPLE_PASSWD "changeme"
//#define MY_SECURITY_SIMPLE_PASSWD "changeme"        // Be aware, the length of the password has an effect on memory use.
//#define MY_SIGNING_SOFT_RANDOMSEED_PIN A7           // Setting a pin to pickup random electromagnetic noise helps make encryption more secure.

// Mysensors advanced settings
//#define MY_RF24_CHANNEL 100                       // In EU the default channel 76 overlaps with wifi, so you could try using channel 100. But you will have to set this up on every device, and also on the controller. You can even try 115.
//#define MY_RF24_DATARATE RF24_250KBPS               // Slower datarate makes the network more stable?
#define MY_RF24_DATARATE RF24_1MBPS               // Slower datarate makes the network more stable?
#define MY_SPLASH_SCREEN_DISABLED                   // Saves a little memory.

#include <MySensors.h>

void setup()
{
	// Setup locally attached sensors
}

void presentation()
{
	// Present locally attached sensors
}

void loop()
{
	// Send locally attached sensor data here
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
