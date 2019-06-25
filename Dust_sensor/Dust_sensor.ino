/*
* Candle fine dust sensor
* 
* Often the air in our homes is much dirtier than the air outside. This fine dust sensor can help you gain insight into the quality of he air inside your home.
*
* It is built around the SDS011 fine dust sensor. It's used here because we can directly connect to it without needing to solder anything onto it.
*
* Sensors like can output 'particle count per cubic meter' or 'micrograms per cubic meter. This sensor does the latter.
* 
* The most popular particle size to measure is 2,5 micrometers. These particles can enter deep inside your lungs. Some dust sensors can also mearure other 'bins', such as 1, 5 or 10 micrometers. This sensors can also give us the weight of the 10 micrometer particles. This weight includes the weight of the smaller sizes particles..
*
*
* SETTINGS */ 

#define MEASUREMENT_INTERVAL 240                      // Seconds between measurements. How many seconds between measurements? Recommended is at least 120 seconds. The minimum is 15 seconds.

#define HAS_DISPLAY                                   // Did you connect an OLED display? If you have connected a small OLED dislay it will show the latest measurements.

//#define SHOW_DATAVIZ                                // Show a basic datavizualisation on the display? This vizualisation is experimental and far from perfect, but might be fun to try.

// MySensors devices form a mesh network by passing along messages for each other. Do you want this node to also be a repeater?
//#define MY_REPEATER_FEATURE                         // Act as signal repeater. Should this sensor act as a repeater for your other devices? This can help the signal spread further.

#define RF_NANO                                       // RF-Nano. Check this box if you are using the RF-Nano Arduino, which has a built in radio. The Candle project uses the RF-Nano.


/* END OF SETTINGS
*
*
*
*/


#ifdef RF_NANO
// If you are using an RF-Nano, you have to switch CE and CS pins.
#define MY_RF24_CS_PIN 9                            // Used by the MySensors library.
#define MY_RF24_CE_PIN 10                           // Used by the MySensors library.
#endif

#define DUST_SENSOR_RX_PIN 3                        // Dust sensor RX pin. Connect this to the TX pin on the sensor.
#define DUST_SENSOR_TX_PIN 4                        // Dust sensor TX pin. Connect this to the RX pin on the sensor.

//#define MY_DEBUG                                  // Enable MySensors debug output to the serial monitor? This will help you check if the radio is working ok.


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



#define RF_DELAY 150                                // Milliseconds between sensing radio signals during the presentation phase. Gives the radio some time to breathe in between working, and time to listen to a response.
#define LOOPDURATION 1000                           // The main loop runs every x milliseconds. It's like a second counter on a clock.


// LIBRARIES (in the Arduino IDE go to Sketch -> Include Library -> Manage Libraries to add these if you don't have them installed yet.)
#include <MySensors.h>                              // MySensors library                  
#include <SDS011.h>                                 // "SDS011 sensor Library". Makes it easy to talk to the fine dust sensor.
#include <avr/wdt.h>


#ifdef HAS_DISPLAY
  #define INCLUDE_SCROLLING 0
  #define OLED_I2C_ADDRESS 0x3C
  #include <SSD1306Ascii.h>                         // Simple drivers for the screen.
  #include <SSD1306AsciiAvrI2c.h>                   // "SSD1306Ascii". Simple drivers for the screen.
  SSD1306AsciiAvrI2c oled;
#endif


// SDS011 dust sensor details
float p10 = 0;
float p25 = 0;
SDS011 my_sds;


// Mysensors settings.
#define CHILD_ID_DUST_PM10 0
#define CHILD_ID_DUST_PM25 1
MyMessage message_dust(CHILD_ID_DUST_PM10, V_LEVEL); // Sets up the message format for actual dust messages.
MyMessage message_prefix(CHILD_ID_DUST_PM10, V_UNIT_PREFIX); // Sets us the MySensors


// Other
static unsigned long lastLoopTime = 0;              // Holds the last time the main loop ran.
int loopCounter = MEASUREMENT_INTERVAL;             // Count how many loops have passed.
byte vizPosition = 30;                              // Used by the experimenal data vizualisation option.
boolean send_all_values = true;                     // If the controller asks the devive to re-present itself, then this is used to also resend all the current sensor values.
boolean received_echo = false;                      // If we get a response from the controller, then this is set to true.

void presentation()
{
  // Send the sketch version information to the gateway and Controller
  sendSketchInfo(F("Fine dust sensor"), F("1.1"));
  
  // Register all child sensors with the gateway 
  present(CHILD_ID_DUST_PM10, S_DUST, F("10 micrometers & smaller")); delay(RF_DELAY);
  present(CHILD_ID_DUST_PM25, S_DUST, F("2.5 micrometers")); delay(RF_DELAY);

  send_all_values = true;     
}


void setup()
{
  my_sds.begin(DUST_SENSOR_RX_PIN, DUST_SENSOR_TX_PIN);
  Serial.begin(115200);
  
#ifdef HAS_DISPLAY
  // Start the display (if there is one)
  oled.begin(&Adafruit128x64, OLED_I2C_ADDRESS);
  oled.setFont(Adafruit5x7);
  
  oled.ssd1306WriteCmd(SSD1306_DISPLAYON);
  oled.setScroll(false);
  oled.setCursor(0,0);
  oled.print(F("FINE DUST"));
  //delay(1000);
#endif

  // Check if there is a network connection
  if(isTransportReady()){
    Serial.println(F("Connected to gateway!"));

#ifdef HAS_DISPLAY    
    // Show connection icon on the display
    oled.setCursor(80,0);
    oled.print(F("W"));
#endif
  }else{
    Serial.println(F("! NOCONNECTION"));  
  }

  // Place labels on the screen
#ifdef HAS_DISPLAY
  oled.setCursor(0,2);
  oled.print(F("1.0:")); 
  oled.setCursor(0,5);
  oled.print(F("2.5:"));
#endif  

  wdt_enable(WDTO_8S);                              // Starts the watchdog timer. If it is not reset once every few seconds, then the entire device will automatically restart.                                
}



    
void send_values()
{
  send(message_prefix.setSensor(CHILD_ID_DUST_PM10).set( F("ug/m3") )); delay(RF_DELAY);
  send(message_dust.setSensor(CHILD_ID_DUST_PM10).set(p10,1)); delay(RF_DELAY);
  
  send(message_prefix.setSensor(CHILD_ID_DUST_PM25).set( F("ug/m3") )); delay(RF_DELAY);
  send(message_dust.setSensor(CHILD_ID_DUST_PM25).set(p25,1)); delay(RF_DELAY);
}


void loop() {
  // Send all the child states to the controller. This will initialise things there.
  if( send_all_values ){
#ifdef DEBUG
    Serial.println(F("Sending all values"));
#endif
    send_all_values = false;
    send_values();
  }


  //
  // HEARTBEAT LOOP
  // runs every second (or as long as you want). By counting how often this loop has run (and resetting that counter back to zero after a number of loops), it becomes possible to schedule all kinds of things without using a lot of memory.
  // The maximum time that can be scheduled is 255 * the time that one loop takes. So usually 255 seconds.
  //
  unsigned long currentMillis = millis();

  if (currentMillis - lastLoopTime > LOOPDURATION) {
    lastLoopTime = currentMillis;
    loopCounter++;
    if(loopCounter > MEASUREMENT_INTERVAL){
      Serial.println(); Serial.println(F("__starting__"));  
      loopCounter = 0;
    }
    wdt_reset();

#ifdef HAS_DISPLAY
    // Update the second countdown on the display.
    oled.set1X();
    oled.setCursor(100,0);
    oled.print(MEASUREMENT_INTERVAL - loopCounter); oled.print(F("  "));
#endif
      

    // schedule
    switch (loopCounter) {
      
      case 1:                                             // On the first second
        if(MEASUREMENT_INTERVAL > 29){                    // Only uses the sleep-and-wake functionality if there is at least 30 seconds between measurements.
          Serial.println(F("Sensor waking up"));
          my_sds.wakeup();
        }
        break;

      case 10:
        Serial.println(F("Asking for fresh data"));
        my_sds.read(&p25, &p10);
        break;
        
      case 11:                                             // On the 11th second (after the fan has been spinning for 10 seconds)
        
        while (!my_sds.read(&p25, &p10))
        {
          Serial.println(F("Waiting for data"));
          delay(10);
        }
        
#ifdef HAS_DISPLAY
        // update the display
        oled.set2X();
        oled.setCursor(30,3);
        oled.print(p10); oled.println(F("   ")); 
        oled.setCursor(30,6);
        oled.print(p25); oled.println(F("   "));
#endif
        if(MEASUREMENT_INTERVAL > 29){ // Only goes to sleep if there is a long enough interval between desired measurements.
          Serial.println(F("Sensor going to sleep."));
          my_sds.sleep();
        }
        break;

      case 12:                                             // on the 12th second we send the first bit of data
        Serial.print(F("-> sending 1.0: ")); Serial.println(p10);
        send(message_dust.setSensor(CHILD_ID_DUST_PM10).set(p10,1));
        received_echo = false;                             // If all goes well this will be reset to 'true' when the controller acknowledges that it has received the first message.
        break;

      case 13:                                             // On the 13th second we send the second bit of data, and check the network connection.
        Serial.print(F("-> sending 2.5: ")); Serial.println(p25);
        send(message_dust.setSensor(CHILD_ID_DUST_PM25).set(p25,1),1); // This message asks the controller to send and acknowledgement it was received.
        
        if( received_echo == true ){
          Serial.println(F("Connection to controller is ok."));
#ifdef HAS_DISPLAY
          // add W icon
          oled.set1X();
          oled.setCursor(80,0);
          oled.print(F("W"));
#endif
        }
        else {
          Serial.println(F("No connection to controller!"));
#ifdef HAS_DISPLAY
          // remove W icon
          oled.set1X();
          oled.setCursor(80,0);
          oled.print(F(" "));
#endif          
        }
        break;

#if defined(HAS_DISPLAY) && defined(SHOW_DATAVIZ)
      case 14:

        // This is an experimental way to show a basic datavizualisation using only the characters available in this display library.
        if(vizPosition > 60 ){vizPosition = 30;}
        //Serial.print(F("Dataviz x position: ")); Serial.println(vizPosition);
        oled.set1X();
        oled.setCursor(vizPosition,1);
        if(p25 < 1 || vizPosition == 1){oled.print(F(" "));}
        else if(p25 < 2){oled.print(F("_"));}
        else if(p25 < 3){oled.print(F("/"));}
        else if(p25 < 4){oled.print(F("4"));}           // This one isn't perfect..
        else if(p25 < 5){oled.print(F("+"));}
        else if(p25 < 6){oled.print(F("t"));}
        else if(p25 < 7){oled.print(F("~"));}
        else {oled.print(F("'"));}
        vizPosition++;
        oled.setCursor(vizPosition,1);
        oled.print(F(" "));      

       break;
#endif 

    }
  }
}

void receive(const MyMessage &message)
{
  if (message.isAck()) {
    Serial.println(F(">> Received acknowledgement"));
    received_echo = true;
  }
}


/** This device uses the MySensors library:
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
 */
