/*
 * Energy use sensor
 * 
 * Your home probably has a central electricity meter. On it, you may find a blinking LED. How frequently it blinks indicates how much power you are using. If you use more, it will blink faster. By measuring how often it blinks, this device can calculate how much electricity your home is currently using (in Watt). It also calculates how many electricity (in KiloWattHour) your home is consuming over time. It will give you live values, as well as the averages of the past hour and past day. If you disable live data transmission, it will only transmit these hourly and daily averages. You can always still see the live values on the display. 
 * 
 * 
 *
 * SETTINGS */ 

//#define HAS_DISPLAY                               // Did you connect an OLED display on pins A4 (SDA) and A5 (SCK)?

#define PULSE_FACTOR 1000                           // LED flashes per Kwh. Set the number of LED flashes that corresponds to the consumption of one Khw. In most cases this is 1000.

#define ALLOW_CONNECTING_TO_NETWORK                 // Connect wirelessly. Is this device allowed to connect to the network? For privacy or security reasons you may prefer a stand-alone device.

//#define MY_REPEATER_FEATURE                       // Act as signal repeater. Should this sensor act as a repeater for your other devices? This can help your network reach further.

#define RF_NANO                                     // RF-Nano. Check this box if you are using the RF-Nano Arduino, which has a built in radio. The Candle project uses the RF-Nano.


 /* END OF SETTINGS
 *
 *
 */


//#define DEBUG                                       // General debug option, give extra information via the serial output when enabled.
//#define MY_DEBUG                                    // Enable MySensors debug output to the serial monitor, so you can check if the radio is working ok.


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
#define MY_ENCRYPTION_SIMPLE_PASSWD "smarthome"      // Be aware, the length of the password has an effect on memory use.
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

#define RADIO_DELAY 100                             // Gives the radio some space to catch breath


// PINS
#define LIGHT_PULSE_SENSOR_PIN 2                    // The digital input you attached your light sensor.  (Only 2 and 3 generate interrupt)
//#define TRANSMISSION_TOGGLE_PIN 8                   // A psychical switch to disable data transmission. This feature is currently disabled. In future, a small push button might be used to toggle data transmission state instead.


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
boolean transmission_state = true;
boolean previous_transmission_state = true;
volatile boolean connected_to_network = false;      // Whether the device has actually been able to succesfully connect to the network.
volatile boolean waiting_for_first_pulse = true;
volatile uint32_t pulse_count = 0;                  // uint32_t = unsigned long
volatile uint32_t last_blink = 0;
volatile uint32_t watt = 0;
uint32_t previous_pulse_count = 0;
uint32_t old_watt = 0;
float kwh = 0;                                      // the total kwh so far during this day
float kwh_hour = 0;                                 // The total kwh so far during this hour
float previous_kwh_hour_total = 0;                  // Stores what the KWH value was up to when the previous hour ended. It's used to calculate how much was used in the current hour.
float kwh_hour_total = 0;                           // How much electricity was used in the previous hour.


unsigned long seconds_left_in_the_day = 86400;      // When this counts down to 60, it will automatically trigger a re-request of the current time from the controller.
unsigned int seconds_left_in_the_hour = 0;
unsigned long epoch_time = 0;                       // Holds the universal epoch time, which is requested from the gateway once in a while. 

unsigned long lastLoopTime = 0;                     // Holds the last time the main loop ran.

#define millis_period = 3600000;                    // By default, the Kwh measurement works per day.

// REQUIRED LIBRARIES
#ifdef ALLOW_CONNECTING_TO_NETWORK
#include <MySensors.h>                              // The MySensors library. Hurray!
#endif
#include <avr/wdt.h>                                // The watch dog timer resets the device if it becomes unresponsive.

#ifdef HAS_DISPLAY
#define INCLUDE_SCROLLING 0                         // Text scrolling for the OLED screen is not necessary
#define TEXT_STRING_LENGTH 26                       // How long the strings are that store the text to display on the screen
#define OLED_I2C_ADDRESS 0x3C                       // A technical requirement. All I2C OLED screens have an 'address'
#include <SSD1306Ascii.h>                           // Simple drivers for the screen.
#include <SSD1306AsciiAvrI2c.h>                     // "SSD1306Ascii"
SSD1306AsciiAvrI2c oled;                            // Creating the display object
byte screen_vertical_position = 3;                  // Used to always show both output at the top of the screen.
//#define F_POSITION 66                             // Horizontal position of the "F" icon, indicating it is allowed to generate fake data.
#define T_POSITION 72                               // Horizontal position of the "T" icon, indicating it is allowed to transmit data.
#define W_POSITION 80                               // Horizontal position of the "W" icon, indicating a wireless connection.
#endif // End of has display



#ifdef ALLOW_CONNECTING_TO_NETWORK

// MySensors
#define WATT_CHILD_ID 1                             // The current wattage being used.
#define KWH_PER_DAY_CHILD_ID 2                      // Every hour we update how much total electricity is being used as the day progresses.
#define KWH_PER_HOUR_CHILD_ID 3                     // Every hour we update how much electricity was used in the past hour.
#define DATA_TRANSMISSION_CHILD_ID 4                // The child ID of the data transmission switch.
#define KWH_PER_DAY_TOTAL_CHILD_ID 5                // Every hour we update how much total electricity is being used as the day progresses.
#define KWH_PER_HOUR_TOTAL_CHILD_ID 6               // Every hour we update how much electricity was used in the past hour.

MyMessage relay_message(DATA_TRANSMISSION_CHILD_ID, V_STATUS); // A generic boolean state message.
MyMessage watt_message(WATT_CHILD_ID,V_WATT);
MyMessage kwh_message(KWH_PER_HOUR_CHILD_ID,V_KWH);
//MyMessage pulse_count_message(WATT_PULSES_CHILD_ID,V_VAR1); // This is currently not used, but in future could be used to ask the controller to remember the total amount of pulses to far. This value could then be requested back whenever this device reboots.


// Other
boolean send_all_values = true;                     // When this is true, all current values will be (re)-sent to the controller.

void presentation()
{
  // Send the sketch version information to the gateway and Controller
  sendSketchInfo(F("Energy meter"), F("1.0")); wait(RADIO_DELAY);

  // Register this device with the controller
  present(WATT_CHILD_ID, S_POWER,F("Wattage")); wait(RADIO_DELAY);
  present(KWH_PER_DAY_CHILD_ID, S_POWER,F("Daily use")); wait(RADIO_DELAY);
  present(KWH_PER_DAY_TOTAL_CHILD_ID, S_POWER,F("Yesterday")); wait(RADIO_DELAY);
  present(KWH_PER_HOUR_CHILD_ID, S_POWER,F("Hourly use")); wait(RADIO_DELAY);
  present(KWH_PER_HOUR_TOTAL_CHILD_ID, S_POWER,F("Last hourly total")); wait(RADIO_DELAY);
  present(DATA_TRANSMISSION_CHILD_ID, S_BINARY, F("Data transmission")); wait(RADIO_DELAY);

  send_all_values = true;
}
#endif // End of allow connecting to network


void setup()
{
  Serial.begin(115200);
  Serial.println(F("Hello, I am an energy use meter"));


  // Use the internal pullup to be able to hook up this sketch directly to an energy meter with S0 output
  // If no pullup is used, the reported usage will be too high because of the floating pin
  pinMode(LIGHT_PULSE_SENSOR_PIN, INPUT_PULLUP);
  //pinMode(TRANSMISSION_TOGGLE_PIN, INPUT_PULLUP);


#ifdef ALLOW_CONNECTING_TO_NETWORK
  transmission_state = loadState(DATA_TRANSMISSION_CHILD_ID);

#ifdef DEBUG
  Serial.print(F("transmission_state loaded from eeprom: "));
  Serial.println(transmission_state);
#endif

  requestTime();                                // Request the current time from the controller.
  wait(1000);

  //connecting_to_network = !digitalRead(TRANSMISSION_TOGGLE_PIN);

  if(isTransportReady()){
    Serial.println(F("Connected to gateway"));
    //connected_to_network = true; // connected_to_network is only set to true once a watt reading exists, to avoid sending a zero value to the controller.
  }else{
    Serial.println(F("! NOT CONNECTED TO GATEWAY"));  
  }
#endif // End of allow connecting to network
  
#ifdef HAS_DISPLAY
  // Start the display (if there is one)
  oled.begin(&Adafruit128x64, OLED_I2C_ADDRESS);
  oled.setFont(Adafruit5x7);
  
  oled.ssd1306WriteCmd(SSD1306_DISPLAYON);
  oled.setScrollMode(false);
  oled.setCursor(0,0);
  oled.print(F("ENERGY USE"));

  oled.setCursor(0,2);
  oled.print(F("Watt:"));
  oled.setCursor(0,5);
  oled.print(F("Kwh:"));

#ifdef DEBUG
  Serial.println(F("I have a display"));
#endif

#endif // End of has display

  wdt_enable(WDTO_2S);                              // Starts the watchdog timer. If it is not reset once every few seconds, then the entire device will automatically restart.

  attachInterrupt(digitalPinToInterrupt(LIGHT_PULSE_SENSOR_PIN), onPulse, RISING);   // Start listening to light pulses
}

#ifdef ALLOW_CONNECTING_TO_NETWORK
void send_values()
{
  send(relay_message.setSensor(DATA_TRANSMISSION_CHILD_ID).set(transmission_state));

  if(waiting_for_first_pulse == false){
    if( transmission_state ){  
      send(watt_message.setSensor(WATT_CHILD_ID).set(watt,0),1); wait(RADIO_DELAY);
      send(kwh_message.setSensor(KWH_PER_HOUR_CHILD_ID).set(kwh_hour, 4),0); // Send kwh value to gateway
      send(kwh_message.setSensor(KWH_PER_DAY_CHILD_ID).set(kwh, 4),0); // Send kwh value to gateway
  
      
    }
    /*
    if(previous_kwh_hour_total != 0){
      send(kwh_message.setSensor(KWH_PER_HOUR_CHILD_ID).set(previous_kwh_hour_total, 4),0); // Send previous kwh value from the previous hour to the controller
    }
    if(previous_kwh_day_total != 0){
      send(kwh_message.setSensor(KWH_PER_HOUR_CHILD_ID).set(previous_kwh_day_total, 4),0); // Send previous kwh value from the previous hour to the controller
    }
    */
  }
}
#endif



void loop()
{
#ifdef ALLOW_CONNECTING_TO_NETWORK
  // Send all the child states to the controller. This will initialise things there.
  if( send_all_values ){
#ifdef DEBUG
    Serial.println(F("Sending all values"));
#endif
    send_all_values = false;
    send_values();
  }


  if ( transmission_state != previous_transmission_state ){
    previous_transmission_state = transmission_state;
    saveState(DATA_TRANSMISSION_CHILD_ID, transmission_state);
    Serial.print(F("Sending new data transmission state: ")); Serial.println(transmission_state);
    send(relay_message.setSensor(DATA_TRANSMISSION_CHILD_ID).set(transmission_state),0);
  }
#endif // End of allow connecting to network

  unsigned long currentMillis = millis();

#ifdef DEBUG
  delay(200);
  Serial.print(".");
#endif

  if (currentMillis - lastLoopTime > 1000) {
    lastLoopTime = currentMillis;

    wdt_reset();
    
    seconds_left_in_the_day--;
    seconds_left_in_the_hour = seconds_left_in_the_day % 3600;
    Serial.print(F("Seconds left in the hour: ")); Serial.println(seconds_left_in_the_hour);
    

    //
    //  CHECK IF A PULSE WAS RECEIVED
    //
    
    if( waiting_for_first_pulse == false ){
      if( watt != old_watt ){
        Serial.print(F("Watt changed to: "));
        Serial.println(watt);
        if (watt<((uint32_t)MAX_WATT)) {          // Check that we don't get unreasonable large watt value. This could happen when long wraps or false interrupt triggered.
          Serial.println(F("Watt is reasonable"));
          old_watt = watt;
#ifdef ALLOW_CONNECTING_TO_NETWORK
          if(transmission_state){                 // If it is allowed to update the live values, do so.
            Serial.println(F("Transmitting Watt value"));
            connected_to_network = false;         // Will be set back to 'true' if a response is received from the controller.
            send(watt_message.setSensor(WATT_CHILD_ID).set(watt),1); wait(RADIO_DELAY);
          }
#endif
        }
      }


      // Pulse count value has changed.
      if( pulse_count != previous_pulse_count ){
#ifdef HAS_DISPLAY
        oled.set1X();
        oled.setCursor(0,1);
        oled.print(F("pulses:"));
        oled.print(pulse_count);
        oled.clearToEOL();
#endif
        Serial.print(F("Pulse count: "));
        Serial.println(pulse_count);
        //send(pulse_count_message.set(pulse_count)); // Send pulse count value to gateway
        kwh = ((float)pulse_count/((float)PULSE_FACTOR)); // Get the live kwh consumption so far this day
        previous_pulse_count = pulse_count;
        
        Serial.print(F("Kwh so far today: "));
        Serial.println(kwh);

        kwh_hour = kwh - previous_kwh_hour_total; // Get the live kwh consumption so far this hour

        Serial.print(F("Kwh so far this hour: "));
        Serial.println(kwh_hour);
#ifdef ALLOW_CONNECTING_TO_NETWORK
        if(transmission_state){                   // If it is allowed to update the live values, do so.
          send(kwh_message.setSensor(KWH_PER_HOUR_CHILD_ID).set(kwh_hour, 4),0);  // Hour
          send(kwh_message.setSensor(KWH_PER_DAY_CHILD_ID).set(kwh, 4),0);        // Day
        }
#endif
      }
      
    }
    else{
      Serial.println("Still waiting to receive the first light pulse");
    }



    //
    //  UPDATING THE DISPLAY
    //

#ifdef HAS_DISPLAY
    oled.set1X();
    oled.setCursor(100,0);
    oled.print(seconds_left_in_the_hour);
    oled.clearToEOL();

    oled.set2X();
    oled.setCursor(50,2);
    oled.print(watt);
    oled.clearToEOL();
    oled.setCursor(50,5);
    oled.print(kwh);
    oled.clearToEOL();
#endif



#ifdef ALLOW_CONNECTING_TO_NETWORK
    if( seconds_left_in_the_day % 10 == 9){ // A second later than the above. For example if there are 39 seconds left.
      // We asked the server to acknowledge that it has received the data. If it hasn't, remove the connection icon.
      if( connected_to_network ){ 
        Serial.println(F("Connection is ok"));
        // add W icon
#ifdef HAS_DISPLAY
        oled.set1X();
        oled.setCursor(W_POSITION,0);
        oled.print(F("W"));
#endif
      }
      else {
        Serial.println(F("No connection"));
#ifdef HAS_DISPLAY
        // remove W icon
        oled.set1X();
        oled.setCursor(W_POSITION,0);
        oled.print(F("."));
#endif
      }
    }

    // Set whether data transmission is allowed
#ifdef HAS_DISPLAY
    oled.setCursor(T_POSITION,0);
    oled.set1X();
    if( transmission_state ){
      oled.print(F("T"));
    }
    else{
      oled.print(F("."));
    }
#endif

#endif // end of allow connecting to network
    
       


    //
    //  UPDATING THE AVERAGE TOTAL VALUES
    //
    
    if( seconds_left_in_the_hour == 0){
      Serial.println(F(""));
      // Send total kwh used in the previous hour.
      kwh_hour_total = kwh - previous_kwh_hour_total;
      Serial.print(F("A new hour is starting. Kwh used over the last hour: "));
      Serial.println(kwh_hour_total);

#ifdef ALLOW_CONNECTING_TO_NETWORK
      send(kwh_message.setSensor(KWH_PER_HOUR_TOTAL_CHILD_ID).set(kwh_hour_total, 4),0); wait(RADIO_DELAY);// Send kwh value for the past hour to the controller
#endif
      previous_kwh_hour_total = kwh;
    }

    // If a full day has passed, reset the kwh counting.
    if( seconds_left_in_the_day == 0 ){
      Serial.print(F("A new day is starting. Kwh used over the last day: "));
      Serial.println(kwh);

#ifdef ALLOW_CONNECTING_TO_NETWORK
      send(kwh_message.setSensor(KWH_PER_DAY_TOTAL_CHILD_ID).set(kwh, 4),0); wait(RADIO_DELAY);// Send kwh value for the past day to the controller
      requestTime();                                // Try to synchronize the current time with the controller.
#endif
      seconds_left_in_the_day = 86400;
      pulse_count = 0;
      kwh = 0;
      previous_kwh_hour_total = 0;
    }
    
    
  } // End of main loop that runs every second
}


//
//  HANDLE INCOMING MESSAGES
//

#ifdef ALLOW_CONNECTING_TO_NETWORK
void receive(const MyMessage &message)
{
  Serial.println(F(">> receiving message"));
  connected_to_network = true;

  if( message.isAck() ){
    Serial.println(F("-Got echo"));
    return;
  }
  if (message.type == V_STATUS && message.sensor == DATA_TRANSMISSION_CHILD_ID ){
    transmission_state = message.getBool(); //?RELAY_ON:RELAY_OFF;
    Serial.print(F("-New desired transmission state: ")); Serial.println(transmission_state);
  }  

  /*
  else if (message.type==V_VAR1) {
    pulse_count = previous_pulse_count = message.getLong();
    Serial.print("Received last pulse count from gateway:");
    Serial.println(pulse_count);
    pulse_count_received = true;
  }
  */
}

void receiveTime(unsigned long controller_time) {
  Serial.print(F("Received time: ")); Serial.println(controller_time);
  epoch_time = controller_time;
  seconds_left_in_the_day = 86400 - (controller_time % 86400);
  Serial.print("Second left in the day: ");
  Serial.println(seconds_left_in_the_day);
  connected_to_network = true;
}
#endif // End of allow connecting to network




//
//  HANDLE LED PULSE
//

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
    pulse_count++;
    if(waiting_for_first_pulse){
      waiting_for_first_pulse = false;
    }
  }
#ifdef DEBUG
  Serial.println("x");
#endif
}






/* This code builds on the MySensors project.
 *
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
 */
