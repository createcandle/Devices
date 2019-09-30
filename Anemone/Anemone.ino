
/**
 * 
 * DESCRIPTION
 * 
 * The Anemone can disconnect your home from the internet when an internet connect is not required. For example, when you are sleeping, away or on holiday.
 * 
 * If your home is not physically connected to the internet, it becomes impossible for non-targetted hacking attempts to affect you.
 * 
 * SETTINGS */ 

#define CHECK_CONNECTION                            // Fail safe. If the Anemone loses its connection to the Candle controller, should it re-enable the internet connection?

//#define MY_REPEATER_FEATURE                       // Act as signal repeater. Should this device act as a repeater for your other Candle devices? This can help the signal spread further.

#define RF_NANO                                     // RF-Nano. Check this box if you are using the RF-Nano Arduino, which has a built in radio. The Candle project uses the RF-Nano.


 /* END OF SETTINGS
  *  
  *  
  *  TODO
  *  - Use desiredState and only toggle in one position, once per second.
  *  - Check if the last toggle came from the controller or from the button, and depending on that let it override to either position or not. So the button can turn it on and off during a non-safe situation, and then the security value will respect that.
  *  
  *  OTHER ADVANCED SETTINGS
  */


#define JESSE                                       // If this is Jesse's Candle prototype

#define DEBUG                                       // Show general debugging data from the device
#define MY_DEBUG                                    // Debug the MySensors network activity


// MySensors radio type
#define MY_RADIO_NRF24                              // A 2.4Ghz transmitter and receiver, often used with MySensors. The RF-Nano has this built in.
//#define MY_RADIO_RFM69                            // 433Mhz transmitter and reveiver.
//#define MY_RADIO_NRF5_ESB                         // NRF5 devices
//#define MY_RADIO_RFM95

// MySensors: Choose your desired radio power level. High and max power can cause issues on cheap Chinese NRF24 radios.
//#define MY_RF24_PA_LEVEL RF24_PA_MIN
#define MY_RF24_PA_LEVEL RF24_PA_LOW
//#define MY_RF24_PA_LEVEL RF24_PA_HIGH
//#define MY_RF24_PA_LEVEL RF24_PA_MAX

// Mysensors advanced settings
#define MY_TRANSPORT_WAIT_READY_MS 10000            // Try connecting for 10 seconds. Otherwise just continue.
//#define MY_RF24_CHANNEL 100                       // In EU the default channel 76 overlaps with wifi, so you could try using channel 100. But you will have to set this up on every device, and also on the controller.
#define MY_RF24_DATARATE RF24_1MBPS                 // The RF-Nano requires a 1Mbps datarate.
//#define MY_NODE_ID 10                             // Giving a node a manual ID can in rare cases fix connection issues.
//#define MY_PARENT_NODE_ID 0                       // Fixating the ID of the gatewaynode can in rare cases fix connection issues.
//#define MY_PARENT_NODE_IS_STATIC                  // Used together with setting the parent node ID. Daking the controller ID static can in rare cases fix connection issues.
#define MY_SPLASH_SCREEN_DISABLED                   // Saves a little memory.
//#define MY_DISABLE_RAM_ROUTING_TABLE_FEATURE      // Saves a little memory.

// MySensors security
#define MY_ENCRYPTION_SIMPLE_PASSWD "changeme"
//#define MY_SECURITY_SIMPLE_PASSWD "changeme"
//#define MY_SIGNING_SOFT_RANDOMSEED_PIN A7         // Setting a pin to pickup noise makes signing more secure.


// PINS
#define MAIN_PUSH_BUTTON_PIN 4
#define RELAY_PIN 5                                 // The pin where the relay is connected
#define STATUS_LED 6                                // The pin where an optional status LED is connected. It will light up if the connection is allowed.

#ifdef JESSE
int top_switch_pin = 2;
int bottom_switch_pin = 3;
int motor_forward_pin = 7;
int motor_backward_pin = 8;
#endif

#ifdef RF_NANO
// If you are using an RF-Nano, you have to switch CE and CS pins.
#define MY_RF24_CS_PIN 9                            // Used by the MySensors library.
#define MY_RF24_CE_PIN 10                           // Used by the MySensors library.
#endif




// STATES
#define NORMAL 0
#define LOST_CONNECTION 11

byte state = NORMAL;


// VARIABLES
#define RADIO_DELAY 150
#define MINILOOP_DURATION 1000                      // Every few seconds we do the main loop. Don't make this bigger than 7900 (almost 8 seconds), since it resets the watchdog.
#define SAFETY_CHECK_ONCE_EVERY_X_LOOPS 60          // Once a minute (12 x 5 seconds = 60 seconds) we do a safety check.
#define MAXIMUM_CONNECTION_LOSS_COUNT 5             // If fail-safe is active, and no connection to the controller could be made for 5 minutes, then reconnect the network.

unsigned long currentMillis = 0;                    // The millisecond clock in the main loop.
unsigned long lastMiniLoop = 0;                     // Used to remember the time of the last temperature measurement.

byte connection_loss_counter = 0;
byte mini_loop_counter = 0;
boolean relay_state = true;
boolean desired_relay_state = true;
boolean send_all_values = true;
boolean is_there_danger = false;
boolean button_pressed = false;

// MySensors child ID's
#define ACTIVATED_CHILD_ID        1                 // Is the thermostat on or off?
#define STATUS_CHILD_ID           2                 // for MySensors. Within this node each sensortype should have its own ID number.


//RELAY VARIABLES
#define RELAY_ON 1                                  // GPIO value to write to turn on attached relay
#define RELAY_OFF 0                                 // GPIO value to write to turn off attached relay


// LIBRARIES
#include <MySensors.h>


// Mysensors settings
MyMessage txtmsg(STATUS_CHILD_ID, V_TEXT);          // Sets up the message format that we'll be sending to the MySensors gateway later. The first part is the ID of the specific sensor module on this node. The second part tells the gateway what kind of data to expect.
MyMessage relaymsg(ACTIVATED_CHILD_ID, V_STATUS);           


// Variable for Jesse's candle prototype
#ifdef JESSE
int motorSpeed = 100;
#endif


void before()
{
  
  // BUTTON
  pinMode(MAIN_PUSH_BUTTON_PIN, INPUT_PULLUP);      // Set another digital pin as power input

  // LED
  pinMode(STATUS_LED, OUTPUT);                      // Set the LED pin as output.

  // RELAY
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, RELAY_OFF);               // Start with the relay off

  if( loadState(ACTIVATED_CHILD_ID) != 255 ){
    relay_state = (boolean)loadState(ACTIVATED_CHILD_ID);
    Serial.print(F("Loaded relay_state from eeprom: ")); Serial.println(relay_state);
  }
}


void presentation()
{
  sendSketchInfo(F("Anemone"), F("1.0"));           // Send the sketch version information to the gateway and Controller
  present(ACTIVATED_CHILD_ID, S_BINARY, F("Internet connection"));
  present(STATUS_CHILD_ID, S_INFO, F("Status"));

  send_all_values = true;    
}


void send_values()
{
  send(relaymsg.setSensor(ACTIVATED_CHILD_ID).set(relay_state)); wait(RADIO_DELAY);
  send(txtmsg.setSensor(STATUS_CHILD_ID).set(F("Hello"))); wait(RADIO_DELAY); 
}


void setup()
{
  Serial.begin(115200);                             // For serial debugging.
  wait(100);  
  Serial.println(F("Hello world"));

  if( isTransportReady() ){
    Serial.println(F("Connected to gateway!"));
  }
  else {
    Serial.println(F("! NO CONNECTION"));
  }

#ifdef JESSE
  pinMode(motor_forward_pin, OUTPUT);
  pinMode(motor_backward_pin, OUTPUT);
  pinMode(top_switch_pin, INPUT_PULLUP);
  pinMode(bottom_switch_pin, INPUT_PULLUP);
#endif
  
  wdt_enable(WDTO_8S);                              // Starts the watchdog timer. If it is not reset once every few seconds, then the entire device will automatically restart.                                
}



void loop()
{
  // Send everything to the controller. This will initialise things there.
  if( send_all_values ){
    //Serial.println(F("Sending all values"));
    send_all_values = false;
    send_values();
  }
  
  currentMillis = millis();                         // The time since the sensor started, counted in milliseconds. This script tries to avoid using the Sleep function, so that it could at the same time be a MySensors repeater.


  wait(20);

  // ON DEVICE TOGGLE BUTTON
  if( digitalRead(MAIN_PUSH_BUTTON_PIN) == LOW ){
    button_pressed = true;
#ifdef DEBUG
    Serial.print(F("button pushed"));
#endif
  }

  // Every few seconds...
  if( currentMillis - lastMiniLoop >= MINILOOP_DURATION ){
    lastMiniLoop = currentMillis;
    wdt_reset();

    if( button_pressed == true ){
      button_pressed = false;
      desired_relay_state = !relay_state;           // Switch the device to the other position.
      Serial.print(F("-Button was pressed."));
      send(txtmsg.setSensor(STATUS_CHILD_ID).set(F("BUTTON PRESSED")));
    }

    if( desired_relay_state != relay_state ){
      relay_state = desired_relay_state;
      saveState(ACTIVATED_CHILD_ID, relay_state);
      send(relaymsg.setSensor(ACTIVATED_CHILD_ID).set(relay_state)); // We acknowledge to the controller that we are now in the new state.
      Serial.print(F("-New relay state: ")); Serial.println(relay_state);

#ifdef JESSE
      if( relay_state = true ){
        switch_on();
      }
      else {
        switch_off();
      }
    }
#endif

    mini_loop_counter++;
#ifdef DEBUG
    Serial.println(mini_loop_counter);
#endif

    // Do a safety check - are we still connected to the controller?
    if( mini_loop_counter >= SAFETY_CHECK_ONCE_EVERY_X_LOOPS ){
#ifdef DEBUG
      Serial.println(F("Checking connection"));
#endif
      mini_loop_counter = 0;

#ifdef CHECK_CONNECTION
      connection_loss_counter++;
#endif
      send(relaymsg.setSensor(ACTIVATED_CHILD_ID).set(relay_state),1); wait(RADIO_DELAY);

      is_there_danger = safety_check();
    }



    // If everything is ok, the relay may be toggled. Otherwise return to the fail-safe position.
    if( is_there_danger == false ){
    // Set the device LED to show the current state of the internet connection.
      digitalWrite(STATUS_LED, relay_state);
      
      digitalWrite(RELAY_PIN, relay_state?RELAY_ON:RELAY_OFF);
    }
    else{
      digitalWrite(STATUS_LED, LOW);                // Go to fail-safe position.
      digitalWrite(STATUS_LED, RELAY_OFF);          // Go to fail-safe position.
    }
    
  }
}



boolean safety_check()
{

  // CHECKING - Is everything ok?

  // Is the controller up?
  if(connection_loss_counter > MAXIMUM_CONNECTION_LOSS_COUNT){
    Serial.println(F("LOST CONNECTION TO SERVER"));
    if(state != LOST_CONNECTION){      
      send(txtmsg.setSensor(STATUS_CHILD_ID).set(F("CONNECTION LOST")));
    }
    state = LOST_CONNECTION;
    return true;                                    // Danger spotted!
  }

  return false;                                     // No issue spotted.
}


void receive(const MyMessage &message)
{
  Serial.println(F(">> receiving message"));
  
  connection_loss_counter = 0;
  
  if (message.isAck()) {
    Serial.println(F("-Got ACK"));
    return;
  }
  
  if (message.type == V_STATUS && message.sensor == ACTIVATED_CHILD_ID ){
    desired_relay_state = message.getBool(); //?RELAY_ON:RELAY_OFF;
    //send(relaymsg.setSensor(ACTIVATED_CHILD_ID).set(relay_state));
    Serial.print(F("-New desired relay state: ")); Serial.println(desired_relay_state);
  }
}


#ifdef JESSE
void switch_on() {
  analogWrite(motor_forward_pin, motorSpeed);
  digitalWrite(motor_backward_pin, LOW);
  while (digitalRead(top_switch_pin) == 1);
  stop();
}

void switch_off() {
  digitalWrite(motor_forward_pin, LOW);
  analogWrite(motor_backward_pin, motorSpeed);
  while (digitalRead(bottom_switch_pin) == 1);
  stop();
}

void stop() {
  digitalWrite(motor_forward_pin, LOW);
  digitalWrite(motor_backward_pin, LOW);
}
#endif



/**
 * 
 * The MySensors Arduino library handles the wireless radio link and protocol
 * between your home built sensors/actuators and HA controller of choice.
 * The sensors forms a self healing radio network with optional repeaters. Each
 * repeater and gateway builds a routing tables in EEPROM which keeps track of the
 * network topology allowing messages to be routed to nodes.
 *
 * Created by Henrik Ekblad <henrik.ekblad@mysensors.org>
 * Copyright (C) 2013-2015 Sensnology AB
 * Full contributor list: https://github.com/mysensors/Arduino/graphs/contributors
 *
 * Documentation: http://www.mysensors.org
 * Support Forum: http://forum.mysensors.org
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 *
 */
