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

#define SHOW_TRANSMISSION_DETAILS                 // Show transmission details. If you enable this, the receiver will shown as a separate device. It will have details about how many succesful and how many failed transmission are made. These values are updated every 5 minutes.

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
#define MY_RADIO_RF24                               // MySensors supports multiple radio modules. Candle uses NRF24.
//#define MY_RADIO_NRF5_ESB
//#define MY_RADIO_RFM69
//#define MY_RADIO_RFM95

// Set LOW transmit power level as default, if you have an amplified NRF-module and
// power your radio separately with a good regulator you can turn up PA level.
//#define MY_RF24_PA_LEVEL RF24_PA_MIN
//#define MY_RF24_PA_LEVEL RF24_PA_LOW
//#define MY_RF24_PA_LEVEL RF24_PA_HIGH
#define MY_RF24_PA_LEVEL RF24_PA_MAX                // Sets the radio to transmit at maximum power, for optimum range.

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
#define MY_GATEWAY_SERIAL                           // This is the main function of this code. It tells the MySensors library to turn this device into a gateway/receiver for MySensors network.


#ifdef SHOW_TRANSMISSION_DETAILS
// Report how often there are wireless communication issues
#define MY_INDICATION_HANDLER
unsigned int txOK = 0;                              // Good transmissions counter
unsigned int txERR = 0;                             // Failed transmissions counter
#define REPORT_INTERVAL 300000                      // Report every 5 minutes
#define CHILD_ID_TX_OK 1                            // Child id for the counter of OK transmissions
#define CHILD_ID_TX_ERR 2                           // Child id for the counter of failed transmissions
#endif


#include <MySensors.h>                              // The MySensors library, which takes care of creating the wireless network.
#include <avr/wdt.h>                                // The watchdog timer - if the device becomes unresponsive and doesn't periodically reset the timer, then it will automatically reset once the timer reaches 0.

// Clock for the watchdog
#define INTERVAL 500                                // Every second we reset the watchdog timer. If the device freezes, the watchdog will not be reset, and the device will reboot.
#define RADIO_DELAY 100                             // Milliseconds of delay between sending transmissions, this keeps the radio module happy.


unsigned long current_time = 0;
byte loops_passed = 0;            // Used to schedule things on a timeline.


#ifdef SHOW_TRANSMISSION_DETAILS
MyMessage transmission_quality_message(CHILD_ID_TX_OK, V_CUSTOM);

void indication(indication_t ind)
{
  switch (ind)
  {
    case INDICATION_TX:
      txOK++;
      break;
    case INDICATION_ERR_TX:
      txERR++;
      break;
  }
}
#endif



void presentation()
{
#ifdef SHOW_TRANSMISSION_DETAILS
  sendSketchInfo(F("Candle receiver"), F("1.0"));
  present(CHILD_ID_TX_OK, S_CUSTOM, F("Good transmissions")); // Tell the controller about this property
  present(CHILD_ID_TX_ERR, S_CUSTOM,F("Bad transmissions")); // Tell the controller about this property
#endif
}



void setup()
{
  //Serial.println(F("Hello, I am a Candle receiver"));
#ifdef SHOW_TRANSMISSION_DETAILS
  send(transmission_quality_message.setSensor(CHILD_ID_TX_OK).set(txOK)); // Send the good transmissions value
  send(transmission_quality_message.setSensor(CHILD_ID_TX_ERR).set(txERR)); // Send the failed transmissions value
#endif
  wdt_enable(WDTO_2S);                              // Starts the watchdog timer. If it is not reset at least once every 2 seconds, then the entire device will automatically restart.                                 
}



void loop()
{
  static unsigned long previous_millis = 0;         // Used to run the internal clock
  current_time = millis();
  
  if( current_time - previous_millis >= INTERVAL ){ // Main loop, runs every second.
    previous_millis = current_time;                 // Store the current time as the previous measurement start time.
    wdt_reset();                                    // Reset the watchdog timer

    loops_passed++;
    if( loops_passed > 120 ){                       // If a minute has passed, send a heartbeat.
      sendHeartbeat();                              // Tells the controller we're still connected.
      loops_passed = 0;
    }                     
  }

#ifdef SHOW_TRANSMISSION_DETAILS
  static unsigned long last_send = 0;
  if( current_time - last_send >= REPORT_INTERVAL ){
    send(transmission_quality_message.setSensor(CHILD_ID_TX_OK).set(txOK)); // Send the good transmissions value
    send(transmission_quality_message.setSensor(CHILD_ID_TX_ERR).set(txERR)); // Send the failed transmissions value
    txOK = 0;                                       // Reset the good transmissions counter back to 0
    txERR = 0;                                      // Reset the error transmissions counter back to 0
    last_send = current_time;                       // Remember the time when the transmissions was done.
  }
#endif

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
