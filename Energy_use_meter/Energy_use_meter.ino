/*
 * 
 * This devices counts LED blinks from your power company's measument device. This allows it to calculate how many watts your home is currently using. By measuring this over time, it becomes possible to keep track of how many kwh your home is using.
 * 
 * 
 *
 * SETTINGS */ 

//#define HAS_DISPLAY                               // Did you connect an OLED display on pins A4 (SDA) and A5 (SCK)?

#define PULSE_FACTOR 1000                           // Blinks per kilowatt. Set the number of blinks per kilowatt of the electricity meter in your home. In most cases this is 1000.

#define ALLOW_CONNECTING_TO_NETWORK                 // Connect wirelessly. Is this device allowed to connect to the network? For privacy or security reasons you may prefer a stand-alone device. If you do allow the device to connect, you can connect a toggle switch to pin 8 to switch the transmission of data or off.

#define MY_REPEATER_FEATURE                         // Act as signal repeater. Should this sensor act as a repeater for your other devices? This can help the signal spread further.

#define RF_NANO                                     // RF-Nano. Check this box if you are using the RF-Nano Arduino, which has a built in radio. The Candle project uses the RF-Nano.


 /* END OF SETTINGS
 *
 *
 */


//#define DEBUG                                     // General debug option, give extra information via the serial output when enabled.
//#define MY_DEBUG                                  // Enable MySensors debug output to the serial monitor, so you can check if the radio is working ok.


// Enable and select the attached radio type
#define MY_RADIO_RF24                               // This is a common and simple radio used with MySensors. Downside is that it uses the same frequency space as WiFi.
//#define MY_RADIO_NRF5_ESB                         // This is a new type of device that is arduino and radio all in one. Currently not suitable for beginners yet.
//#define MY_RADIO_RFM69                            // This is an open source radio on the 433mhz frequency. Great range and built-in encryption, but more expensive and little more difficult to connect.
//#define MY_RADIO_RFM95                            // This is a LoRaWan radio, which can have a range of 10km.

// MySensors: Choose your desired radio power level. High power can cause issues on cheap Chinese NRF24 radio's.
//#define MY_RF24_PA_LEVEL RF24_PA_MIN
//#define MY_RF24_PA_LEVEL RF24_PA_LOW
//#define MY_RF24_PA_LEVEL RF24_PA_HIGH
#define MY_RF24_PA_LEVEL RF24_PA_MAX

// Mysensors advanced security
#define MY_ENCRYPTION_SIMPLE_PASSWD "changeme"      // Be aware, the length of the password has an effect on memory use.
//#define MY_SECURITY_SIMPLE_PASSWD "changeme"      // Be aware, the length of the password has an effect on memory use.
//#define MY_SIGNING_SOFT_RANDOMSEED_PIN A7         // Setting a pin to pickup random electromagnetic noise helps make encryption more secure.

// Mysensors advanced settings
#define MY_TRANSPORT_WAIT_READY_MS 10000            // Try connecting for 10 seconds. Otherwise just continue.
//#define MY_RF24_CHANNEL 100                       // In EU the default channel 76 overlaps with wifi, so you could try using channel 100. But you will have to set this up on every device, and also on the controller.
#define MY_RF24_DATARATE RF24_1MBPS                 // Slower datarate makes the network more stable?
//#define MY_NODE_ID 10                             // Giving a node a manual ID can in rare cases fix connection issues.
//#define MY_PARENT_NODE_ID 0                       // Fixating the ID of the gatewaynode can in rare cases fix connection issues.
//#define MY_PARENT_NODE_IS_STATIC                  // Used together with setting the parent node ID. Daking the controller ID static can in rare cases fix connection issues.
#define MY_SPLASH_SCREEN_DISABLED                   // Saves a little memory.
//#define MY_DISABLE_RAM_ROUTING_TABLE_FEATURE      // Saves a little memory.

#define RADIO_DELAY 100                              // Gives the radio some space to catch a breath


// PINS
#define LIGHT_PULSE_SENSOR_PIN 2                    // The digital input you attached your light sensor.  (Only 2 and 3 generate interrupt)
#define NETWORK_TOGGLE_PIN 8                        // The digital input you attached your light sensor.  (Only 2 and 3 generate interrupt)


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


// VARIABLES
#define SLEEP_MODE false                            // You could run this device on battery power, which means "sleep mode" is enabled. Watt value can only be reported when sleep mode is false.
#define MAX_WATT 10000                              // Max watt value to report. This filters outliers.

uint32_t SEND_FREQUENCY = 10000;                    // Minimum time between send (in milliseconds). We don't want to spam the gateway.
float ppwh = ((float)PULSE_FACTOR)/1000;            // Pulses per watt hour
boolean pulse_count_received = false;
boolean connected = false;                          // Whether the device has actually been able to succesfully connect to the network.
volatile uint32_t pulse_count = 0;
volatile uint32_t last_blink = 0;
volatile uint32_t watt = 0;
uint32_t old_pulse_count = 0;
uint32_t old_watt = 0;
float kwh = 0;
float old_kwh = 0;
float previous_kwh_hour_total = 0;                  // Stores what the KWH value was up to when the previous hour was complete. It's used to calculate how much was used in the current hour.
float kwh_hour_total = 0;                           // How much electricity was used in the last hour.
float kwh_day_total = 0;
//uint32_t last_send;

unsigned long seconds_left_in_the_day = 1;          // When this counts down to 60, it will automatically trigger a re-request of the current time from the controller.
int seconds_left_in_the_hour = 1;
//unsigned long last_epoch_time = 0;                // Holds the universal epoch time, which is requested from the gateway once in a while. 
unsigned long epoch_time = 0;                       // Holds the universal epoch time, which is requested from the gateway once in a while. 

unsigned long lastLoopTime = 0;                     // Holds the last time the main loop ran.


//long millisecond_counter = 0;
#define millis_period = 3600000;                    // By default, the Kwh measurement works per day.

// REQUIRED LIBRARIES
#include <MySensors.h>                              // The MySensors library. Hurray!
#include <avr/wdt.h>                                // The watch dog timer resets the device if it becomes unresponsive.

#ifdef HAS_DISPLAY
#define INCLUDE_SCROLLING 0                         // Text scrolling for the OLED screen is not necessary
#define TEXT_STRING_LENGTH 26                       // How long the strings are that store the text to display on the screen
#define OLED_I2C_ADDRESS 0x3C                       // A technical requirement. All I2C OLED screens have an 'address'
#include <SSD1306Ascii.h>                           // Simple drivers for the screen.
#include <SSD1306AsciiAvrI2c.h>                     // "SSD1306Ascii"
SSD1306AsciiAvrI2c oled;                            // Creating the display object
byte screen_vertical_position = 3;                  // Used to always show both output at the top of the screen.
#endif

// MySensors
#define WATT_CHILD_ID 1                             // The current wattage being used.
#define KWH_PER_DAY_CHILD_ID 2                      // Every hour we update how much total electricity is being used as the day progresses.
#define KWH_PER_HOUR_CHILD_ID 3                     // Every hour we update how much electricity was used in the past hour.

MyMessage watt_message(WATT_CHILD_ID,V_WATT);
MyMessage kwh_message(KWH_PER_HOUR_CHILD_ID,V_KWH);
//MyMessage pulse_count_message(WATT_PULSES_CHILD_ID,V_VAR1);


// Other
boolean send_all_values = true;                     // When this is true, all current values will be (re)-sent to the controller.
boolean connecting_to_network = false;
boolean old_connecting_to_network = false;

void presentation()
{
#ifdef ALLOW_CONNECTING_TO_NETWORK
  // Send the sketch version information to the gateway and Controller
  sendSketchInfo("Energy meter", "1.0");

  // Register this device as power sensor
  present(WATT_CHILD_ID, S_POWER,F("Watt"));
  present(KWH_PER_DAY_CHILD_ID, S_POWER,F("Daily use"));
  present(KWH_PER_HOUR_CHILD_ID, S_POWER,F("Hourly use"));

  send_all_values = true;
#endif
}


void setup()
{
  Serial.begin(115200);
  Serial.println(F("Hello, I am an energy use meter"));

#ifdef ALLOW_CONNECTING_TO_NETWORK
  connecting_to_network = !digitalRead(NETWORK_TOGGLE_PIN);

  if(isTransportReady()){
    Serial.println(F("Connected to gateway"));
  }else{
    Serial.println(F("! NOT CONNECTED TO GATEWAY"));  
  }
#endif
    

  // Use the internal pullup to be able to hook up this sketch directly to an energy meter with S0 output
  // If no pullup is used, the reported usage will be too high because of the floating pin
  pinMode(LIGHT_PULSE_SENSOR_PIN, INPUT_PULLUP);
  pinMode(NETWORK_TOGGLE_PIN, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(LIGHT_PULSE_SENSOR_PIN), onPulse, RISING);
  
#ifdef HAS_DISPLAY
  // Start the display (if there is one)
  oled.begin(&Adafruit128x64, OLED_I2C_ADDRESS);
  oled.setFont(Adafruit5x7);
  
  oled.ssd1306WriteCmd(SSD1306_DISPLAYON);
  oled.setScroll(false);
  oled.setCursor(0,0);
  oled.print(F("ENERGY USE"));

  oled.setCursor(0,2);
  oled.print(F("Watt:"));
  oled.setCursor(0,5);
  oled.print(F("Kwh:"));

#ifdef DEBUG
  Serial.println(F("I have a display"));
  oled.setCursor(0,1);
  oled.print(F("Cnct:"));
  oled.print(connecting_to_network);
#endif

#endif

  Serial.println(F("connecting_to_network: "));
  Serial.println(connecting_to_network);
}


void send_values()
{
}


void loop()
{
  // Send all the child states to the controller. This will initialise things there.
  if( send_all_values ){
#ifdef DEBUG
    Serial.println(F("Sending all values"));
#endif
    send_all_values = false;
    send_values();
  }

  unsigned long currentMillis = millis();

  if (currentMillis - lastLoopTime > 1000) {
    lastLoopTime = currentMillis;
    seconds_left_in_the_day--;
    seconds_left_in_the_hour = seconds_left_in_the_day % 3600;
    Serial.println(seconds_left_in_the_day);
    Serial.println(seconds_left_in_the_hour);

    //if( seconds_left_in_the_day < 1 ){
      // Calculate day value
      // Send day value.
   //  kwh_day_total = 0;
   //}

    // Send total kwh used in the previous hour.
    float kwh_hour_total = kwh - previous_kwh_hour_total;
    sendWithPermission(kwh_message.setSensor(KWH_PER_HOUR_CHILD_ID).set(kwh_hour_total, 4),0); // Send kwh value to gateway
    previous_kwh_hour_total = kwh;

    // If a full day has passed, reset the kwh counting.
    if( seconds_left_in_the_day == 0 ){
      Serial.println(F("End of the day"));
      seconds_left_in_the_day = 86400;
      pulse_count = 0;
      kwh = 0;
      requestTime();                                // Request the current time from the controller.
      wait(RADIO_DELAY);
    }
      
    //}

    wdt_reset();

    // Check if network connection button was toggled (if it exists)
    connecting_to_network = !digitalRead(NETWORK_TOGGLE_PIN);
    Serial.print("ppin read:");
    Serial.println(digitalRead(NETWORK_TOGGLE_PIN));
    if( connecting_to_network != old_connecting_to_network ){
      old_connecting_to_network = connecting_to_network;
      
  #ifdef HAS_DISPLAY                                // TODO temporary
      oled.set1X();
      oled.setCursor(0,1);
      oled.print(F("Cnct:"));
      oled.print(connecting_to_network);
  #endif
  
      Serial.println(F("Connecting to network changed to: "));
      Serial.println(connecting_to_network);
    }

#ifdef HAS_DISPLAY
    oled.set1X();
    oled.setCursor(100,0);
    oled.print(seconds_left_in_the_day % 3600);
    oled.clearToEOL();

    oled.set2X();
    oled.setCursor(50,2);
    oled.print(watt);
    oled.clearToEOL();
    oled.setCursor(50,5);
    oled.print(kwh);
    oled.clearToEOL();
#endif


    if( seconds_left_in_the_day % 60 == 0){
      Serial.println(F("60 seconds have passed"));
    }


    // Once every 10 seconds, update the values on the gateway. For example if there are 40 seconds left.
    if( seconds_left_in_the_day % 10 == 0){
      if( watt != 0 && watt != old_watt ){
        if (watt<((uint32_t)MAX_WATT)) {              // Check that we don't get unreasonable large watt value. This could happen when long wraps or false interrupt triggered.
          connected = false;                          // Will be turned back on if a response is received.
          send(watt_message.setSensor(WATT_CHILD_ID).set(watt),1); wait(RADIO_DELAY);
          sendWithPermission(watt_message.setSensor(WATT_CHILD_ID).set(watt),1);
          old_watt = watt;
        }
        Serial.print("Watt:");
        Serial.println(watt);
      }

      
    }

    else if( seconds_left_in_the_day % 10 == 9){ // A second later than the above. For example if there are 39 seconds left.
      // We asked the server to acknowledge that it has received the data. If it hasn't, remove the connection icon.
      if( connected ){ 
        Serial.println(F("Connection is ok"));
        // add W icon
#ifdef HAS_DISPLAY
        oled.set1X();
        oled.setCursor(90,0);
        oled.print(F("w"));
#endif
      }
      else {
        Serial.println(F("No connection"));
#ifdef HAS_DISPLAY
        // remove W icon
        oled.set1X();
        oled.setCursor(90,0);
        oled.print(F(" "));
#endif
      }
    }
       
    // Pulse count value has changed
    if( pulse_count != old_pulse_count ){
#ifdef HAS_DISPLAY
      oled.set1X();
      oled.setCursor(0,1);
      oled.print(F("pulses:"));
      oled.print(pulse_count);
      oled.clearToEOL();
#endif
      Serial.print(F("Pulse count: "));
      Serial.println(pulse_count);
      //send(pulse_count_message.set(pulse_count));   // Send pulse count value to gateway
      kwh = ((float)pulse_count/((float)PULSE_FACTOR));
      old_pulse_count = pulse_count;
      if( kwh != old_kwh ){ // We only send a new value if it changed.
        sendWithPermission(kwh_message.setSensor(KWH_PER_DAY_CHILD_ID).set(kwh, 4),0); // Send kwh value to gateway
        old_kwh = kwh;
      }
    }
    
  }
}

void receive(const MyMessage &message)
{
#ifdef ALLOW_CONNECTING_TO_NETWORK
  connected = true;
  if (message.type==V_VAR1) {
    pulse_count = old_pulse_count = message.getLong();
    Serial.print("Received last pulse count from gateway:");
    Serial.println(pulse_count);
    pulse_count_received = true;
  }
#endif
}


boolean sendWithPermission(MyMessage &msg, boolean require_echo)
{
#ifdef ALLOW_CONNECTING_TO_NETWORK
  Serial.println("in sendWithPermission");
  // Check permission and send here
  Serial.println(connecting_to_network);
  if( connecting_to_network ){
    Serial.println(F("Sending"));
    boolean send_ok = send(msg, require_echo);
    Serial.println(send_ok);
    return send_ok;
  }
#endif
  return false;
}


void onPulse()
{
  if (!SLEEP_MODE) {
    uint32_t new_blink = micros();
    uint32_t interval = new_blink - last_blink;
    if (interval<300000L) {                          // Sometimes we get interrupt on RISING
        return;
    }
    watt = (3600000000.0 /interval) / ppwh;
    last_blink = new_blink;
  }
  pulse_count++;
  Serial.println("x");
}


void receiveTime(unsigned long controller_time) {
  Serial.print(F("Received time: ")); Serial.println(controller_time);
  epoch_time = controller_time;
  seconds_left_in_the_day = 86400 - (controller_time % 86400);
  Serial.print("Second left in the day: ");
  Serial.println(seconds_left_in_the_day);
  connected = true;
}






/*
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
 *
 *******************************
 *
 * REVISION HISTORY
 * Version 1.0 - Henrik Ekblad
 *
 * DESCRIPTION
 * This sketch provides an example how to implement a LM393 PCB
 * Use this sensor to measure kwh and Watt of your house meter
 * You need to set the correct pulsefactor of your meter (blinks per kwh).
 * The sensor starts by fetching current kwh value from gateway.
 * Reports both kwh and Watt back to gateway.
 *
 * Unfortunately millis() won't increment when the Arduino is in
 * sleepmode. So we cannot make this sensor sleep if we also want
 * to calculate/report watt value.
 * http://www.mysensors.org/build/pulse_power
 */
