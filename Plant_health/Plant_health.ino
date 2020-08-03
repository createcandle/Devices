/*
 *
 * DESCRIPTION
 * 
 * This device can measure the moisture level of five different plants. It uses the cheap 'capacitive analog moisture sensor' that you can get for about a dollar a piece online.
 * 
 * Each plant's moisture value can also be reacted to. If the moisture level goes below a set threshold, then a pin on the device is set to 'on'. This can be used to turn on a LED or, if you want, per-plant automated irrigation by connecting a little water pump or solenoid.
 * 
 * SETTINGS */

#define NUMBER_OF_SENSORS 3                         // Sensor count. How many moisture sensors have you connected? The maximum is five.

//#define HAS_DISPLAY                               // Has OLED display. Did you attach an OLED display?

#define WATERING                                    // Thirsty triggering. Enabling this will allow you to set the minimum moisture level or each plant. When the moisture level drops to this level, a pin on the device will be turned on. You can use this to turn on a LED for each plant when that plant is thirsty. You can also use this to automatically trigger irrigation, for example by turning on a small pump or opening a valve to water the thirsty plant.

#define MEASUREMENT_INTERVAL 3600                   // Measurement interval. How many seconds should pass between checking on the plants and sending the data? Don't make this less than 15. The default is 3600 (one hour)

//#define MY_REPEATER_FEATURE                       // Act as repeater. Do you want this node to also be act as repeater for other devices?

#define RF_NANO                                     // RF-Nano. Check this box if you are using the RF-Nano Arduino, which has a built in radio. The Candle project uses the RF-Nano.


/* END OF SETTINGS
  *  
  *  
  *  
  */


//#define DEBUG
//#define MY_DEBUG                                  // Enable MySensors debug prints to serial monitor


// Enable and select radio type attached
#define MY_RADIO_RF24
//#define MY_RADIO_NRF5_ESB
//#define MY_RADIO_RFM69
//#define MY_RADIO_RFM95

// Set LOW transmit power level as default, if you have an amplified NRF-module and power your radio separately with a good regulator you can turn up PA level. Choose one:
//#define MY_RF24_PA_LEVEL RF24_PA_MIN
//#define MY_RF24_PA_LEVEL RF24_PA_LOW
//#define MY_RF24_PA_LEVEL RF24_PA_HIGH
#define MY_RF24_PA_LEVEL RF24_PA_MAX

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
#ifdef RF_NANO
// If you are using an RF-Nano, you have to switch CE and CS pins.
#define MY_RF24_CS_PIN 9                            // Used by the MySensors library.
#define MY_RF24_CE_PIN 10                           // Used by the MySensors library.
#endif



//
// FURTHER SETTINGS
//

#define IRRIGATION_RELAYS 0                         // How many irrigation relays are connected?
#define LOOPDURATION 1000                           // The main loop runs every x milliseconds. This main loop starts the modem, and from then on periodically requests the password.

#define RADIO_DELAY 100                             // Milliseconds betweeen radio signals during the presentation phase.

#define ALLOW_CONNECTING_TO_NETWORK


//
// Do not change below this line
// 


#ifdef HAS_DISPLAY
  #define OLED_I2C_ADDRESS 0x3C
  #include <SSD1306Ascii.h>                         // Simple drivers for the screen.
  #include <SSD1306AsciiAvrI2c.h>                   // "SSD1306Ascii".
  SSD1306AsciiAvrI2c oled;
#endif

unsigned long lastLoopTime = 0;
unsigned long loopCounter = 0;                      // Counts the loops until the MEASUREMENT_INTERVAL value has been reached. Then new data is sent to the controller.
boolean send_all_values = true;
boolean may_transmit = true;
boolean previous_may_transmit = true;
boolean may_water = false;
boolean previous_may_water = false;
boolean actually_connected = false;
boolean first_run = true;

static const uint8_t analog_pins[] = {A0,A1,A2,A3,A6,A7}; // The pins to which moisture sensors are connected
static const uint8_t trigger_pins[] = {2,3,4,5,6,7}; // The pins used to trigger an output, such as an LED or a water pump. Pins 0 and 1 are reserved for the USB serial connection, so they are skipped.
byte moistureLevels[6] = {50, 50, 50, 50, 50, 50};  // Initial moisture levels, before any actual sensor values replace these.
byte moistureThresholds[6] = {35, 35, 35, 35, 35, 35}; // for each plant we can have a unique moisture level to compare against.


#define TRANSMIT_CHILD_ID 100                       // Is the device allowed to transmit data?
#define WATERING_CHILD_ID 101                       // Is the device allowed to automatically trigger things? (LED or water pump). Sometimes you may want to disable this.

#ifdef ALLOW_CONNECTING_TO_NETWORK

#include <MySensors.h>


MyMessage moisture_message(0, V_LEVEL);             // Used to send moisture level data to the gateway. Should be V_LEVEL.
MyMessage threshold_message(1, V_PERCENTAGE);       // Used to create a dimmer on the controller that controls the mosture threshold;
MyMessage relay_message(TRANSMIT_CHILD_ID, V_STATUS); // Used to manage the data transission toggle  



void before()
{
  // Setup pins for input
  for (int i = 0; i < NUMBER_OF_SENSORS; i++) {
    pinMode(analog_pins[i], INPUT);
    pinMode(trigger_pins[i], OUTPUT);
  }
}


void presentation()
{
  // Send the sketch version information to the gateway and Controller
  sendSketchInfo(F("Plant Health"), F("1.0")); wait(RADIO_DELAY);

  present(TRANSMIT_CHILD_ID, S_BINARY, F("Data transmission")); wait(RADIO_DELAY);
  present(WATERING_CHILD_ID, S_BINARY, F("Triggering")); wait(RADIO_DELAY);

  // Present the sensors

  // For now, it uses S_MOISTURE instead of S_MOISTURE.
  //for (byte i=0; i<NUMBER_OF_SENSORS ; i=i+1) {
  present(0, S_MOISTURE, F("Sensor 1")); wait(RADIO_DELAY);       // present all the sensors
#ifdef WATERING
  present(10, S_DIMMER, F("Threshold 1")); wait(RADIO_DELAY);       // present the dimmers to set the level with.
#endif
  if(NUMBER_OF_SENSORS > 1){
    present(1, S_MOISTURE, F("Sensor 2")); wait(RADIO_DELAY);       // present all the sensors
#ifdef WATERING
    present(11, S_DIMMER, F("Threshold 2")); wait(RADIO_DELAY);       // present the dimmers to set the level with.
#endif
    if(NUMBER_OF_SENSORS > 2){
      present(2, S_MOISTURE, F("Sensor 3")); wait(RADIO_DELAY);       // present all the sensors
#ifdef WATERING
      present(12, S_DIMMER, F("Threshold 3")); wait(RADIO_DELAY);       // present the dimmers to set the level with.
#endif
      if(NUMBER_OF_SENSORS > 3){
        present(3, S_MOISTURE, F("Sensor 4")); wait(RADIO_DELAY);       // present all the sensors
#ifdef WATERING
        present(13, S_DIMMER, F("Threshold 4")); wait(RADIO_DELAY);       // present the dimmers to set the level with.
#endif
        if(NUMBER_OF_SENSORS > 4){
          present(4, S_MOISTURE, F("Sensor 5")); wait(RADIO_DELAY);       // present all the sensors
#ifdef WATERING
          present(14, S_DIMMER, F("Threshold 5")); wait(RADIO_DELAY);       // present the dimmers to set the level with.
#endif
          if(NUMBER_OF_SENSORS > 5){
            present(5, S_MOISTURE, F("Sensor 6")); wait(RADIO_DELAY);       // present all the sensors
#ifdef WATERING
            present(15, S_DIMMER, F("Threshold 6")); wait(RADIO_DELAY);       // present the dimmers to set the level with.
#endif
          }
        }
      }
    }
  }

  send_all_values = true;  
}
#endif


void setup()
{
  Serial.begin(115200);                             // Start serial output of data.
  delay(1000);                                      // Wait for serial connection to be initiated
  Serial.println(F("Hello, I am a plant health device"));

#ifdef HAS_DISPLAY
  // Initiate the display
  oled.begin(&Adafruit128x64, OLED_I2C_ADDRESS);
  oled.setFont(Adafruit5x7);
  oled.ssd1306WriteCmd(SSD1306_DISPLAYON);
  oled.setScroll(false);
  oled.setCursor(0,0);
  oled.print(F("PLANT HEALTH"));
  oled.setCursor(0,2);
  oled.print(F("Wait..."));
#endif

#ifdef ALLOW_CONNECTING_TO_NETWORK
  if( isTransportReady() ){                         // Check if a network connection has been established
    Serial.println(F("Connected to gateway!"));
    actually_connected = true;
#ifdef HAS_DISPLAY    
    // Show connection icon on the display
    oled.setCursor(85,0);
    oled.print(F("W"));
#endif

  }
  else {
     Serial.println(F("! NO CONNECTION")); 
     actually_connected = false;
  }
#endif 

  if( loadState(TRANSMIT_CHILD_ID) < 2 ){
    previous_may_transmit = may_transmit = loadState(TRANSMIT_CHILD_ID);
  }
  else{
     previous_may_transmit = may_transmit = true;   // By default, data transmission of moisture levels is enabled.
    saveState(TRANSMIT_CHILD_ID, true);
  }

  if( loadState(WATERING_CHILD_ID) < 2 ){
    previous_may_water = may_water = loadState(WATERING_CHILD_ID);
  }
  else{
    previous_may_water = may_water = false;         // To avoid water damage, by default watering is disabled. 
    saveState(WATERING_CHILD_ID, false);
  }

  // load the threshold level from the built-in EEPROM memory.
  for (int i=0; i<NUMBER_OF_SENSORS ; i++) {
    moistureThresholds[i] = loadState(i);
    if( moistureThresholds[i] > 99 ){
      moistureThresholds[i] = 35;                   // If the EEPROM value is not set, reset it to default
    }
    Serial.print(F("Loaded: ")); Serial.println(moistureThresholds[i]);
    //request((i * 2) + 1, V_PERCENTAGE ); wait(RADIO_DELAY);
  }
  
  Serial.println(F("Warming up the sensors (15 seconds).")); // To avoid weird measurements
  wait(15000);

  wdt_enable(WDTO_2S);                              // Starts the watchdog timer. If it is not reset once every 2 seconds, then the entire device will automatically restart.                                
}



#ifdef ALLOW_CONNECTING_TO_NETWORK
//
//  This function get run whenever the controller asks the device for up to date data. This happens, for example, after the controller or the mysensors addon has restarted itself.
//
void send_values()
{
  send( relay_message.setSensor(TRANSMIT_CHILD_ID).set(may_transmit) ); wait(RADIO_DELAY);
  send( relay_message.setSensor(WATERING_CHILD_ID).set(may_water) ); wait(RADIO_DELAY);
  
  for (int i=0; i<NUMBER_OF_SENSORS; i++) {
    if( may_transmit && first_run == false){
      send(moisture_message.setSensor(i).set(moistureLevels[i]),1); wait(RADIO_DELAY); // Ask for a receipt, to make sure the data was sent.
    }
#ifdef WATERING
    send( threshold_message.setSensor(10 + i).set( moistureThresholds[i] ) );
#endif
  }
}
#endif



void loop()
{

#ifdef ALLOW_CONNECTING_TO_NETWORK
  // Send everything to the controller. This will initialise things there.
  if( send_all_values ){
    Serial.println(F("Sending all values"));
    send_all_values = false;
    send_values();
  }
  wait(20);
#endif

  if( may_transmit != previous_may_transmit ){
    previous_may_transmit = may_transmit;
    send(relay_message.setSensor(TRANSMIT_CHILD_ID).set(may_transmit));
    Serial.print(F(">> New transmission state: ")); Serial.println(may_transmit);
  }

  if( may_water != previous_may_water ){
    previous_may_water = may_water;
    send(relay_message.setSensor(WATERING_CHILD_ID).set(may_water));
    Serial.print(F(">> New triggering state: ")); Serial.println(may_water);
  }




  //
  // MAIN LOOP
  // Runs every second. By counting how often this loop has run, it becomes possible to schedule all kinds of things without using a lot of memory.
  // Maximum time that can be scheduled is 4s * 250 loops = 1000 seconds. So the maximum time between sending data can be 16 minutes.
  //


  if( millis() - lastLoopTime > LOOPDURATION ){
    lastLoopTime = millis();
    wdt_reset();                                    // Reset the watchdog timer

    // Check if values are below the threshold

    for (int i=0; i<NUMBER_OF_SENSORS ; i++) {
      //byte shiftedDigitalPin = i + 3;

      wait(10);                                     // This short delay is to avoid reading the analog value in quick succession.
      int16_t moistureLevel = analogRead(analog_pins[i]);
#ifdef DEBUG
      Serial.print(F("raw analog moisture value: "));
      Serial.println(moistureLevel);
#endif
      if( moistureLevel > 650 ){moistureLevel = 650;}
      moistureLevels[i] = map(moistureLevel,0,650,0,99); // The maximum voltage output of the capacitive sensor is 3V, so since we're measuring 0-5v about 614 is the theoretical highest value we'll ever get.
      Serial.print(i);
      Serial.print(F(" moisture level is "));
      Serial.print(moistureLevels[i]);
      Serial.println(F("%"));
      
      if( analogRead(analog_pins[i]) < moistureThresholds[i] ){
        Serial.print(F("Plant ")); Serial.print(i); Serial.print(F(" is below ")); Serial.println( moistureThresholds[i] );
        digitalWrite(trigger_pins[i], HIGH);
      }
      else{
        digitalWrite(trigger_pins[i], LOW);
      }

      



#ifdef HAS_DISPLAY

      oled.set1X();
      if( NUMBER_OF_SENSORS < 4 ){
        oled.setCursor(0,(i * 2) + 2);              // If there are more than 4 sensors, the text on the OLED display will get smaller to accomodate.
      } 
      else {
        oled.setCursor(0,i + 2);
      }
      oled.print(i + 1);                            // Internally, the sensors are labeled 0 through 5. On the OLED display they are called 1 through 6.
      oled.print(F(" "));
      if( NUMBER_OF_SENSORS < 4 ){
        oled.set2X();                               // Increase font size if less than 4 sensors are attached.
      }
      
      oled.print(moistureLevels[i]);
      oled.print(F("%"));

      if(moistureLevels[i] < 10){
        oled.print(F(" "));                         // Add an extra space after the numbers 0 through 9. This keeps things aligned on the display.
      }
      if( moistureLevels[i] < moistureThresholds[i] ){
        oled.print(F(" WATER"));
      }
      else{
        oled.print(" OK");
      }
      oled.clearToEOL(); 

#endif

    } // End of looping over all sensors
    
    first_run == false;
    
    if(loopCounter < NUMBER_OF_SENSORS){            // During the first few loops the script will send updated data.
#ifdef ALLOW_CONNECTING_TO_NETWORK
      if( may_transmit ){
        send(moisture_message.setSensor(loopCounter).set(moistureLevels[loopCounter]),1); // Ask for a receipt, to make sure the data was sent.
      }
#endif
    }

    
    loopCounter++;
    if(loopCounter >= MEASUREMENT_INTERVAL){        // If enough time has passed, the counter is reset, and new data is sent.
      loopCounter = 0;
    }

    if(loopCounter % 59 == 0){                      // Every 59 seconds, let the controller know the device is alive.
      //sendHeartbeat();                            // Lets the controller know the device is still alive, even is sensor data hasn't been transmitted in a while.
      actually_connected = false;                   // Whether the device is actually succesfully connected to the network.
      send( relay_message.setSensor(TRANSMIT_CHILD_ID).set(may_transmit),1 ); // We ask the controller to 'echo' the message back to the device. If this echo is not received, the connection must be down.
    }
    if(loopCounter % 59 == 1){                      // A second later, check if the controller ECHO'd the message back to the device.
#ifdef HAS_DISPLAY 
      oled.setCursor(85,0);
      if( actually_connected ){
        oled.print(F("W"));                         // Show connection icon on the display
      }
      else{
        oled.print(F(" "));                         // Hide connection icon on the display
      }
#endif
    }
    
    
#ifdef HAS_DISPLAY
    // Show countdown to measurement
    oled.set1X();
    oled.setCursor(100,0);
    if(MEASUREMENT_INTERVAL - loopCounter > 59){
      unsigned long minutes = (int) (MEASUREMENT_INTERVAL - loopCounter) / 60;
      oled.print(minutes);
      oled.print(F("m"));
    }
    else{
      oled.print(MEASUREMENT_INTERVAL - loopCounter);
    }
    oled.clearToEOL();
#endif

  }
}


#ifdef ALLOW_CONNECTING_TO_NETWORK
void receive(const MyMessage &message)
{
  Serial.print(F("Incoming message for child #")); Serial.println(message.sensor);

  if (message.isAck()) {
    Serial.println(F("-Got ECHO"));
    actually_connected = true;
    return;
  }

  else if( message.type == V_STATUS && message.sensor == TRANSMIT_CHILD_ID ){
    may_transmit = message.getBool();
    saveState(TRANSMIT_CHILD_ID, may_transmit);
  }

  else if( message.type == V_STATUS && message.sensor == WATERING_CHILD_ID ){
    may_water = message.getBool();
    saveState(WATERING_CHILD_ID, may_water);
  }

#ifdef WATERING
  else if (message.type == V_PERCENTAGE) {

    //  Retrieve the power or dim level from the incoming request message
    int requestedLevel = atoi( message.data );
    Serial.print(F("Requested level is "));
    Serial.println( requestedLevel );
    
    byte sensorID = (message.sensor - 10); // Children ID's of thresholds are the sensor child ID + 10

#ifdef DEBUG
    Serial.print(F("Before clipping requested level: "));
    Serial.println( requestedLevel );
#endif

    // Clip incoming level to valid range of 0 to 100
    if(requestedLevel > 100){ requestedLevel = 100;}
    if(requestedLevel < 0){ requestedLevel = 0;}

    if( requestedLevel < 1 || requestedLevel > 99 ){
      // Erroneous value, since it cannot act as a treshold at these extreme values.
    }
    else{
      Serial.print(F("Changing threshold from "));
      Serial.println( moistureThresholds[sensorID] );
      Serial.print(F(" to "));
      Serial.print( byte(requestedLevel) );
      moistureThresholds[sensorID] = byte(requestedLevel);
      saveState(sensorID, moistureThresholds[sensorID]);
    }
  }
#endif // End of automatic watering
}
#endif // end of allow connecting to network





/* THANKS TO
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
