/*
 * 
 * This is a temperature and humidity sensor for the Candle project.
 * 
 * It uses the DHT22 sensor to measure temperature and humidity.
 * 
 * These values can be displayed on an OLED screen.
 * 
 * 
 * SETTINGS */ 


//#define HAS_DHT22_SENSOR                            // Have you attached a temperature sensor?

#define MEASUREMENT_INTERVAL 30                     // How many seconds do you want between each measurement? The minimum is 4 seconds.

#define HAS_DISPLAY                                 // Does the sensor have a little OLED display attached?

//#define MY_REPEATER_FEATURE                       // Act as a repeater? The devices can pass along messages to each other to increase the range of your network.

#define RF_NANO                                       // RF-Nano. Check this box if you are using the RF-Nano Arduino, which has a built in radio. The Candle project uses the RF-Nano.

/* END OF SETTINGS
 *
 *
 */


// PINS
#define SENSOR_PIN 4                                // At what pin is the temperature sensor connected?

#ifdef RF_NANO
// If you are using an RF-Nano, you have to switch CE and CS pins.
#define MY_RF24_CS_PIN 9                            // Used by the MySensors library.
#define MY_RF24_CE_PIN 10                           // Used by the MySensors library.
#endif

#define DEBUG
#define MY_DEBUG // MySensors debugging. Enable MySensors debug output to the serial monitor, so you can check if the radio is working ok.

// Enable and select the attached radio type
#define MY_RADIO_RF24                               // This is a common and simple radio used with MySensors. Downside is that it uses the same frequency space as WiFi.
//#define MY_RADIO_NRF5_ESB                         // This is a new type of device that is arduino and radio all in one. Currently not suitable for beginners yet.
//#define MY_RADIO_RFM69                            // This is an open source radio on the 433mhz frequency. Great range and built-in encryption, but more expensive and little more difficult to connect.
//#define MY_RADIO_RFM95                            // This is a LoRaWan radio, which can have a range of 10km.

// MySensors: Choose your desired radio power level. High power can cause issues on cheap Chinese NRF24 radio's.
//#define MY_RF24_PA_LEVEL RF24_PA_MIN
//#define MY_RF24_PA_LEVEL RF24_PA_LOW
#define MY_RF24_PA_LEVEL RF24_PA_HIGH
//#define MY_RF24_PA_LEVEL RF24_PA_MAX

// Mysensors security
#define MY_ENCRYPTION_SIMPLE_PASSWD "changeme"      // Be aware, the length of the password has an effect on memory use.
//#define MY_SECURITY_SIMPLE_PASSWD "changeme"        // Be aware, the length of the password has an effect on memory use.
//#define MY_SIGNING_SOFT_RANDOMSEED_PIN A7           // Setting a pin to pickup random electromagnetic noise helps make encryption more secure.

// Mysensors advanced settings
#define MY_TRANSPORT_WAIT_READY_MS 10000            // Try connecting for 10 seconds. Otherwise just continue.
//#define MY_RF24_CHANNEL 100                       // In EU the default channel 76 overlaps with wifi, so you could try using channel 100. But you will have to set this up on every device, and also on the controller.
#define MY_RF24_DATARATE RF24_1MBPS               // Slower datarate makes the network more stable?
//#define MY_NODE_ID 10                             // Giving a node a manual ID can in rare cases fix connection issues.
//#define MY_PARENT_NODE_ID 0                       // Fixating the ID of the gatewaynode can in rare cases fix connection issues.
//#define MY_PARENT_NODE_IS_STATIC                  // Used together with setting the parent node ID. Daking the controller ID static can in rare cases fix connection issues.
#define MY_SPLASH_SCREEN_DISABLED                   // Saves a little memory.
//#define MY_DISABLE_RAM_ROUTING_TABLE_FEATURE      // Saves a little memory.



// LIBRARIES (in the Arduino IDE go to Sketch -> Include Library -> Manage Libraries to add these if you don't have them installed yet.)
#include <MySensors.h>                              // MySensors library                  
#include <SimpleDHT.h>                              // DHT22 temperature and humidity sensor library


// DHT sensor
//SimpleDHT11 dht22(SENSOR_PIN); // Use this one if you want to use an older DHT11 sensor instead
SimpleDHT22 dht22(SENSOR_PIN);
float temperature_value = -100;
float humidity_value = -100;
int err = SimpleDHTErrSuccess;

#ifdef HAS_DISPLAY
  #define OLED_I2C_ADDRESS 0x3C
  #include <SSD1306Ascii.h>                         // Simple drivers for the screen.
  #include <SSD1306AsciiAvrI2c.h>                   // "SSD1306Ascii".
  SSD1306AsciiAvrI2c oled;
#endif



// Mysensors settings
#define CHILD_ID_TEMPERATURE 1                      // The child ID of the sensor that will be presented to the controller.
#define CHILD_ID_HUMIDITY 2                         // The child ID of the sensor that will be presented to the controller.


const byte RF_DELAY = 150;                          // A few milliseconds delay between sending makes the radio happy.

MyMessage temperature_message(CHILD_ID_TEMPERATURE, V_TEMP); // Sets up the message format that we'll be sending to the MySensors gateway later.
MyMessage humidity_message(CHILD_ID_HUMIDITY, V_HUM); // Sets up the message format that we'll be sending to the MySensors gateway later.


// Other
#define LOOPDURATION 1000                           // The main loop runs every x milliseconds.  Normally this sensor has a 'heartbeat' of once every second.
boolean send_all_values = true;                     // If the controller asks the devive to re-present itself, then this is used to also resend all the current sensor values.
boolean received_echo = false;                      // If we get a response from the controller, then this is set to true.


void presentation()
{
  // Send the sketch version information to the gateway and Controller
  sendSketchInfo(F("Temperature sensor"), F("1.0"));
  
  // Register all sensors to gateway: 
  present(CHILD_ID_TEMPERATURE, S_TEMP, F("Temperature")); wait(RF_DELAY);
  present(CHILD_ID_HUMIDITY, S_HUM, F("Humidity")); wait(RF_DELAY);

  send_all_values = true;
}


void setup() {
  Serial.begin(115200);
  while (!Serial) {}
  
  Serial.println(F("Hello, I am a temperature and humidity sensor"));

#ifdef HAS_DISPLAY
  // Initiate the display
  oled.begin(&Adafruit128x64, OLED_I2C_ADDRESS);
  oled.setFont(Adafruit5x7);
  oled.ssd1306WriteCmd(SSD1306_DISPLAYON);
  oled.setScroll(false);
  oled.setCursor(0,0);
  oled.print(F("TEMPERATURE"));
#endif

  
  if( isTransportReady() ){                         // Check if a network connection has been established
    Serial.println(F("Connected to gateway!"));
  }
  else {
    Serial.println(F("! NO CONNECTION"));
  }
  

#ifdef HAS_DISPLAY
  //oled.setCursor(0,2);                              // The labels are shown slightly above the values.
  //oled.print(F("Temperature:"));
  oled.setCursor(0,5);                              // The labels are shown slightly above the values.
  oled.print(F("Humidity:"));
#endif

  wdt_enable(WDTO_8S);                              // Starts the watchdog timer. If it is not reset once every few seconds, then the entire device will automatically restart.
}


void send_values()
{
  if( temperature_value != -100){
    send(temperature_message.setSensor(CHILD_ID_TEMPERATURE).set(temperature_value,1),1);
  }
  if( humidity_value != -100){
    send(humidity_message.setSensor(CHILD_ID_HUMIDITY).set(humidity_value,1),1);
  }
}


void loop() {
  
  if( send_all_values ){
#ifdef DEBUG
    Serial.println(F("RESENDING VALUES"));
#endif
    send_all_values = 0;
    send_values();
  }
  
  //
  // MAIN LOOP
  // runs every second (or as long as you want). By counting how often this loop has run (and resetting that counter back to zero after a number of loops), it becomes possible to schedule all kinds of things without using a lot of memory.
  // The maximum time that can be scheduled is 255 * the time that one loop takes. So usually 255 seconds.
  //

  static unsigned long lastLoopTime = 0;            // Holds the last time the main loop ran.
  static byte loopCounter = 0;                      // Count how many loops have passed (reset to 0 after at most 254 loops).

  if (millis() - lastLoopTime > LOOPDURATION) {
    lastLoopTime = millis();
    loopCounter++;
    if(loopCounter >= MEASUREMENT_INTERVAL){
      loopCounter = 0;
    }

    wdt_reset();                                      // Reset the watchdog timer

#ifdef DEBUG
    Serial.println(loopCounter);
#endif

#ifdef HAS_DISPLAY    
    // Show second counter
    oled.set1X();
    oled.setCursor(100,0);
    oled.print(MEASUREMENT_INTERVAL - loopCounter); oled.print(F("  "));
#endif
      

    // Clock schedule
    switch (loopCounter) {


      case 1:                                       // If we are in the first second of the clock, get and display the data.

        if ((err = dht22.read2(&temperature_value, &humidity_value, NULL)) != SimpleDHTErrSuccess) {
          Serial.print(F("Reading sensor failed, error number: ")); Serial.println(err);

#ifdef HAS_DISPLAY
          oled.set2X();
          oled.setCursor(0,3);
          oled.print(F("ERROR"));
#endif

          break;
        }
        Serial.print(F("Asked sensor for data"));


      case 2:
      
        // TEMPERATURE
#ifdef HAS_DISPLAY
        // Show temperature level on the screen
        oled.set2X();
        oled.setCursor(0,2);

        // Display temperature value.
        oled.print(temperature_value); oled.println(F("      "));
#endif

        // HUMIDITY
#ifdef HAS_DISPLAY
        // Show humidity level on the screen
        oled.set2X();
        oled.setCursor(0,6);

        // Display humidity value.
        oled.print(humidity_value); oled.println(F("      "));

        // Show quality opinion on the screen.
        oled.setCursor(70,6);
        if (humidity_value > 0 && humidity_value < 30){       oled.print(F("DRY"));}
        else if (humidity_value < 65){  oled.print(F("GOOD "));}
        //else if (humidity_value >= 1000){ oled.print(F("OK   "));}
        else {
          oled.print(F("MOIST"));
        }
#endif
        break;


      case 3:                                       // Send the data

        received_echo = false;                      // If the controller responds that it has received the data ok, then this will be set back to true.
        
        if( temperature_value != -100 ){ // Avoid sending erroneous values
          Serial.print(F("Sending temperature value: ")); Serial.println(temperature_value); 
          send(temperature_message.setSensor(CHILD_ID_TEMPERATURE).set(temperature_value,1),1); // We ask the controller to acknowledge that it has received the data.  
        }

        if( humidity_value != 101 ){ // Avoid sending erroneous values
          Serial.print(F("Sending humidity value: ")); Serial.println(humidity_value); 
          send(humidity_message.setSensor(CHILD_ID_HUMIDITY).set(humidity_value,1),1); // We ask the controller to acknowledge that it has received the data.
        }
        break;


      case 4:                                       // Show the connection status on the display

#ifdef HAS_DISPLAY
        oled.set1X();
        oled.setCursor(85,0);
        
        if( received_echo ){                        // Add W icon to the top right corner of the screen, indicating a wireless connection.
          oled.print(F("W"));
        }else {
          oled.print(F(" "));                       // Remove W icon
        }
#endif
        break;
    }
  }
}


void receive(const MyMessage &message)
{
  if (message.isAck()) {
    Serial.println(F(">> Received acknowledgement"));
    received_echo = true;
  }
  else{
    Serial.println(F(">> Incoming message"));
  }
}



/* 
 *  
 * This code makes use of the MySensors library:
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
 *******************************
 *
 */
