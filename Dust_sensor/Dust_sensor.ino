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


#define MEASUREMENT_INTERVAL 60                    // Seconds between measurements. How many seconds between measurements? Recommended is at least 120 seconds. The minimum is 15 seconds.

#define HAS_DISPLAY                                 // Did you connect an OLED display? If you have connected a small OLED dislay it will show the latest measurements.

//#define SHOW_DATAVIZ                              // Show a basic datavizualisation on the display? This vizualisation is experimental and far from perfect, but might be fun to try.

#define ALLOW_CONNECTING_TO_NETWORK                 // Connect wirelessly. Is this device allowed to connect to the network? For privacy or security reasons you may prefer a stand-alone device. If you do allow the device to connect, you can connect a toggle switch to pin 6 to change the connection state at any time.

//#define ALLOW_FAKE_DATA                           // Allow fake data to be sent? This is an experimental feature, and requires you to attach a switch to pin 5. It's designed to make the sensor less intrusive in some social situations, allowing you to pretend you are not smoking/cooking.

//#define MY_REPEATER_FEATURE                       // Act as signal repeater. Should this sensor act as a repeater for your other devices? This can help the signal spread further.

#define RF_NANO                                     // RF-Nano. Check this box if you are using the RF-Nano Arduino, which has a built in radio. The Candle project uses the RF-Nano.


/* END OF SETTINGS
*
*
*
*/

//#define MY_DEBUG                                  // Enable MySensors debug output to the serial monitor? This will help you check if the radio is working ok.


#define AVERAGE_HOURS 1                             // Averaging period. Over how many hours should the average be calculated? If the device has been on shorter than this period, it will show the average until then.


// PINS
#define DUST_SENSOR_RX_PIN 3                        // Dust sensor RX pin. Connect this to the TX pin on the sensor.
#define DUST_SENSOR_TX_PIN 4                        // Dust sensor TX pin. Connect this to the RX pin on the sensor.
#define TOGGLE_FAKE_DATA_PIN 5                      // Pin where the toggle switch to send fake data is connected.   
#define CONNECT_TO_NETWORK_PIN 6                    // Pin where the toggle switch to allow connecting to the network is connected.
#define RANDOM_SEED_PIN A7                          // Pin to use to create more random variables.

#ifdef RF_NANO
// If you are using an RF-Nano, you have to switch CE and CS pins.
#define MY_RF24_CS_PIN 9                            // Used by the MySensors library.
#define MY_RF24_CE_PIN 10                           // Used by the MySensors library.
#endif


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



#define RADIO_DELAY 150                                // Milliseconds between sensing radio signals during the presentation phase. Gives the radio some time to breathe in between working, and time to listen to a response.
#define LOOPDURATION 1000                           // The main loop runs every x milliseconds. It's like a second counter on a clock.


// LIBRARIES (in the Arduino IDE go to Sketch -> Include Library -> Manage Libraries to add these if you don't have them installed yet.)
#include <MySensors.h>                              // MySensors library                  
#include <SDS011.h>                                 // "SDS011 sensor Library". Makes it easy to talk to the fine dust sensor.
#include <avr/wdt.h>                                // Watchdog library. Resets the device if it becomes unresponsive.


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
float average_p10 = 0;
float average_p25 = 0;

SDS011 my_sds;


// Mysensors settings.
#define CHILD_ID_DUST_PM10 0
#define CHILD_ID_DUST_PM25 1
#define ACTIVATED_CHILD_ID 2
MyMessage message_dust(CHILD_ID_DUST_PM10, V_LEVEL); // Sets up the message format for actual dust messages.
MyMessage message_prefix(CHILD_ID_DUST_PM10, V_UNIT_PREFIX); // Sets up the MySensors prefix message
MyMessage relaymsg(ACTIVATED_CHILD_ID, V_STATUS);   // Toggle message



// Other
unsigned long lastLoopTime = 0;                     // Holds the last time the main loop ran.
int loopCounter = MEASUREMENT_INTERVAL;             // Count how many loops have passed.
int measurements_counter = 0;                       // Used by averaging function.
byte vizPosition = 30;                              // Used by the experimenal data vizualisation option.
boolean send_all_values = true;                     // If the controller asks the devive to re-present itself, then this is used to also resend all the current sensor values.
boolean received_echo = false;                      // If we get a response from the controller, then this is set to true.

// Fake data feature
boolean sending_fake_data = false;                  // Experimental. Will allow a user to send fake data for a while. Useful in some social situations.
boolean desired_sending_fake_data = false;          // If the user wants to change the state of sending fake data.
float p25_fakeness_range = 0;                       // How far of the last average the value can meander.   
float p10_fakeness_range = 0;                       // How far of the last average the value can meander.   
float p25_addition = 0;                             // Holds how much will actually be deviated from the average when generating a fake value.
float fakeness_proportion = 0;                      // How these two values relate. if one goes up, the other should also go up, but in proportion to it's own fakeness range.

// Connection toggle feature
boolean desired_connecting_to_network = false;
boolean connecting_to_network = true;
boolean may_send_data = true;

void presentation()
{
#ifdef ALLOW_CONNECTING_TO_NETWORK
  // Send the sketch version information to the gateway and Controller
  sendSketchInfo(F("Fine dust sensor"), F("1.1"));
  
  // Register all child sensors with the gateway 
  present(CHILD_ID_DUST_PM10, S_DUST, F("10 micrometers & smaller")); delay(RADIO_DELAY);
  present(CHILD_ID_DUST_PM25, S_DUST, F("2.5 micrometers")); delay(RADIO_DELAY);
  present(ACTIVATED_CHILD_ID, S_BINARY, F("data transmission"));

  send_all_values = true;
#endif   
}


void setup()
{
  my_sds.begin(DUST_SENSOR_RX_PIN, DUST_SENSOR_TX_PIN);
  Serial.begin(115200);
  Serial.println(F("Hello, I am a dust sensor"));

#ifdef ALLOW_FAKE_DATA
  pinMode(TOGGLE_FAKE_DATA_PIN, INPUT_PULLUP);
  Serial.print(F("Toggle fake-data-mode using a switch on pin ")); Serial.println(TOGGLE_FAKE_DATA_PIN);
#endif

  
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


#ifdef ALLOW_CONNECTING_TO_NETWORK
  // Check if there is a network connection
  if(isTransportReady()){
    Serial.println(F("Connected to gateway!"));
    
#ifdef HAS_DISPLAY    
    // Show connection icon on the display
    oled.setCursor(80,0);
    oled.print(F("W"));
#endif

  }else{
     Serial.println(F("! NO CONNECTION"));  
  }

#ifdef HAS_DISPLAY    
    // Show data transmission icon on the display
    oled.setCursor(70,0);
    if(may_send_data){                              // A small "T" icon on the screen reflects that data transmission is currently allowed.
      oled.print(F("T"));
    }
    else{
      oled.print(F(" "));
    }
#endif

  
#else
  Serial.println("This device will not connect to the network.")
#endif

  // Place labels on the screen
#ifdef HAS_DISPLAY
  oled.setCursor(0,2);
  oled.print(F("2.5:")); 
  oled.setCursor(0,5);
  oled.print(F("10.0:"));
#endif  

  wdt_enable(WDTO_8S);                              // Starts the watchdog timer. If it is not reset once every few seconds, then the entire device will automatically restart.                                

  //my_sds.wakeup();
}

    
void send_values()
{
#ifdef ALLOW_CONNECTING_TO_NETWORK
  
  send(message_prefix.setSensor(CHILD_ID_DUST_PM10).set( F("ug/m3") ),0); delay(RADIO_DELAY);
  if(may_send_data){
    send(message_dust.setSensor(CHILD_ID_DUST_PM10).set(p10,1),0); delay(RADIO_DELAY);
  }
  send(message_prefix.setSensor(CHILD_ID_DUST_PM25).set( F("ug/m3") ),0); delay(RADIO_DELAY);
  if(may_send_data){
    send(message_dust.setSensor(CHILD_ID_DUST_PM25).set(p25,1),0); delay(RADIO_DELAY);
  }
  send(relaymsg.setSensor(ACTIVATED_CHILD_ID).set(may_send_data)); wait(RADIO_DELAY);

#endif
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




#ifdef ALLOW_FAKE_DATA
  boolean fake_data_pin_state = digitalRead(TOGGLE_FAKE_DATA_PIN);
  if( fake_data_pin_state != desired_sending_fake_data ){
    desired_sending_fake_data = fake_data_pin_state;
    Serial.print(F("FAKE DATA TOGGLED TO  ")); Serial.println(desired_sending_fake_data);
#ifdef HAS_DISPLAY
    oled.set1X();
    oled.setCursor(72,0);
    if( desired_sending_fake_data ){
      oled.print(F("F"));
    }
    else{
      oled.print(F(" "));
    }
#endif
    wait(20);
  }
#endif


#ifdef ALLOW_CONNECTING_TO_NETWORK
  if( digitalRead(CONNECT_TO_NETWORK_PIN) != connecting_to_network ){
    
    //connecting_to_network = !desired_connecting_to_network;
    wait(10);
    connecting_to_network = digitalRead(CONNECT_TO_NETWORK_PIN);
    Serial.print(F("NETWORK TOGGLED TO ")); Serial.println(connecting_to_network);

#ifdef HAS_DISPLAY
    if(!connecting_to_network){ // If we should not connect to the network, remove the W icon.
      oled.set1X();
      oled.setCursor(80,0);
      oled.print(F(" "));
    }
#endif
    wait(10); // Avoid bounce
  }
#endif


  //
  // HEARTBEAT LOOP
  // runs every second (or as long as you want). By counting how often this loop has run (and resetting that counter back to zero after a number of loops), it becomes possible to schedule all kinds of things without using a lot of memory.
  // The maximum time that can be scheduled is 255 * the time that one loop takes. So usually 255 seconds.
  //
  unsigned long currentMillis = millis();

  if (currentMillis - lastLoopTime > LOOPDURATION) {
    lastLoopTime = currentMillis;
    loopCounter++;
    Serial.print("loopcounter:"); Serial.println(loopCounter);
    if(loopCounter > MEASUREMENT_INTERVAL){
      Serial.println(); Serial.println(F("__starting__"));  
      loopCounter = 0;
    }
    wdt_reset();

#ifdef HAS_DISPLAY
    // Update the second countdown on the display.
    oled.set1X();
    oled.setCursor(100,0);
    oled.print(MEASUREMENT_INTERVAL - loopCounter);
    oled.clearToEOL();
#endif
      

    // schedule
    switch (loopCounter) {
      
      case 1:                                       // On the first second
        //if(MEASUREMENT_INTERVAL > 29){              // Only uses the sleep-and-wake functionality if there is at least 30 seconds between measurements.
          Serial.println(F("Sensor waking up"));
          my_sds.wakeup();
        //}
        break;

      case 10:
        Serial.println(F("Asking for fresh data"));
        my_sds.read(&p25, &p10);
        break;
        
      case 11:                                             // On the 11th second (after the fan has been spinning for 10 seconds)
        Serial.println(F("Asking for fresh data again"));
        while (!my_sds.read(&p25, &p10))
        {
          Serial.println(F("Waiting for data"));
          delay(10);
        }
        
#ifdef HAS_DISPLAY
        // update the display
        
        oled.set2X();
        oled.setCursor(0,3);
        oled.print(p25); oled.println(F("   "));        
        oled.setCursor(0,6);
        oled.print(p10); oled.println(F("   "));



        // PM2.5 levels based on opinions of Dutch scientists and the World Health Organization. Keep your yearly average below 10.
        oled.setCursor(70,3);
        if(sending_fake_data){oled.print(F("FAKED"));}
        else if (p25 == 0){   oled.clearToEOL();     }
        else if (p25 <= 3){   oled.print(F("GREAT"));}
        else if (p25 <= 5){   oled.print(F("GOOD "));}
        else if (p25 <= 8){   oled.print(F("OK   "));}
        else if (p25 <= 14){  oled.print(F("POOR "));}
        else if (p25 >  20){  oled.print(F("BAD  "));}
        
        // PM10 levels based on opinions of Dutch scientists and the World Health Organization. Keep your yearly average below 20.
        oled.setCursor(70,6);
        if(sending_fake_data){oled.print(F("FAKED"));}
        else if (p10 == 0){   oled.clearToEOL();     }
        else if (p10 <= 5){   oled.print(F("GREAT"));}
        else if (p10 <= 10){  oled.print(F("GOOD "));}
        else if (p10 <= 20){  oled.print(F("OK   "));}
        else if (p10 <= 30){  oled.print(F("POOR "));}
        else if (p10 >  30){  oled.print(F("BAD  "));}
#endif

        if( MEASUREMENT_INTERVAL > 29 ){                    // Only goes to sleep if there is a long enough interval between desired measurements.
          Serial.println(F("Sensor going to sleep."));
          my_sds.sleep();
        }
        break;

      case 12:                                            // On the 12th second we send the first bit of data
        received_echo = false;                            // If all goes well this will be reset to 'true' when the controller acknowledges that it has received the first message.
#ifdef ALLOW_CONNECTING_TO_NETWORK

        if( desired_sending_fake_data == true && sending_fake_data == false ){

          if( average_p25 != 0 && p25 != average_p25 && average_p10 != 0 && p10 != average_p10 ){
            // We have good enough measurements to create fake data.

            // Determine the fakeness range: how far from the last average newly generated values may meander.
            /*if( p25 > average_p25){
              p25_fakeness_range = p25 - average_p25; 
            }
            else if ( p25 < average_p25){
              p25_fakeness_range = average_p25 -  p25;
            }

            if( p10 > average_p10){
              p10_fakeness_range = p10 - average_p10; 
            }
            else if ( p10 < average_p10){
              p10_fakeness_range = average_p25 -  p25;
            }
            */
            p25_fakeness_range = p25 - average_p25; 
            p10_fakeness_range = p10 - average_p10;

            if( p25_fakeness_range < 0 ){
              p25_fakeness_range = -p25_fakeness_range;
            }
            if( p10_fakeness_range < 0 ){
              p10_fakeness_range = -p10_fakeness_range;
            }
            //fakeness_proportion = (float) p10_fakeness_range / p25_fakeness_range; // Usually the p10 values will be slightly bigger.
            fakeness_proportion = (float) p10 / p25; // Usually the p10 values will be slightly bigger.
            
            randomSeed(analogRead(RANDOM_SEED_PIN)); // Creates better random values.
            Serial.print(F("Starting sending fake data. \n-P25 range: ")); Serial.println(p25_fakeness_range);
            Serial.print(F("-p10 range: ")); Serial.println(p10_fakeness_range);
            Serial.print(F("-proportion: ")); Serial.println(fakeness_proportion);
            if(p25_fakeness_range != 0){          
              sending_fake_data = true;
            }
          }
#ifdef DEBUG
          else{
            Serial.println(F("Measurement not useful for intiating fake data."));
          }
#endif
        }
        if( desired_sending_fake_data == false && sending_fake_data == true ){
          Serial.print(F("Will send real data again."));
          sending_fake_data = false;
        }
        
        if(sending_fake_data){
          p25_addition = (float)random( p25_fakeness_range * 100000) / 100000;
          if( random(2) ){ p25_addition = -p25_addition;}
          Serial.print(F("2.5 Addition: ")); Serial.println(p25_addition);
          p25_addition = average_p25 + p25_addition;
          Serial.print(F("<< sending FAKE 2.5: ")); Serial.println(p25_addition);
          send(message_dust.setSensor(CHILD_ID_DUST_PM25).set(p25_addition,1),1); // This message asks the controller to send and acknowledgement it was received.
        }
        else{
          Serial.print(F("<< sending 2.5: ")); Serial.println(p25);
          send(message_dust.setSensor(CHILD_ID_DUST_PM25).set(p25,1),1); // This message asks the controller to send and acknowledgement it was received.
        }
#endif
        break;

      case 13:                                            // On the 13th second we send the second bit of data, and check the network connection.
#ifdef ALLOW_CONNECTING_TO_NETWORK
        if(sending_fake_data){ // Now creating fake data for P10. It should generally move in the same direction as the p25.
          float p10_addition = (float) (p10_fakeness_range * fakeness_proportion); // + (p10_fakeness_range / random(4,8));
          //float addition = (float)random( p25_fakeness_range * 100000) / 100000;
          //if( random(2) ){ p10_addition = p10_addition ; }
          Serial.print(F("p10_addition = ")); Serial.println(p10_addition);
          p10_addition = p10_addition + average_p10;
          Serial.print(F("<< sending FAKE 10.0: ")); Serial.println(p10_addition);
          if(may_send_data){
            send(message_dust.setSensor(CHILD_ID_DUST_PM25).set(p10_addition,1),1); // This message asks the controller to send and acknowledgement it was received.
          }
        }
        else{
          Serial.print(F("-> sending 10.0: ")); Serial.println(p10);
          if(may_send_data){
            send(message_dust.setSensor(CHILD_ID_DUST_PM10).set(p10,1),0);
          }
        }
#endif

#ifdef HAS_DISPLAY
          oled.set1X();
          oled.setCursor(80,0);
#endif

        if( received_echo == true ){
          Serial.println(F("Connection to controller is ok."));
#ifdef HAS_DISPLAY
          oled.print(F("W"));                       // Add W icon
#endif
        }
        else {
          Serial.println(F("No connection to controller!"));
#ifdef HAS_DISPLAY
          oled.print(F(" "));                       // Remove W icon
#endif          
        }

        
        break;

      case 14:                                      // Calculating averages
        // Still thinking how to optimally deal with outliers and/or early mis-measurements:
        if(measurements_counter * MEASUREMENT_INTERVAL < AVERAGE_HOURS * 3600){ // This limits how strongly the old values influence the new average.
           measurements_counter++;                    
        }

        if(!sending_fake_data){
          average_p25 = average(measurements_counter, average_p25, p25);
          average_p10 = average(measurements_counter, average_p10, p10);
        }
        else{
          Serial.print(F("Sending fake data, so average is not changing."));
        }

        Serial.print(F("Average p2.5: ")); Serial.println(average_p25);
        Serial.print(F("Average p10: ")); Serial.println(average_p10);
        
        // Data vizualisation experiment
#if defined(HAS_DISPLAY) && defined(SHOW_DATAVIZ)
        // This is an experimental way to show a basic datavizualisation using only the characters available in this display library.
        if(vizPosition > 60 ){vizPosition = 30;}
        //Serial.print(F("Dataviz x position: ")); Serial.println(vizPosition);
        oled.set1X();
        oled.setCursor(vizPosition,1);
        if(p25 < 1 || vizPosition == 1){oled.print(F(" "));}
        else if(p25 < 2){oled.print(F("_"));}
        else if(p25 < 3){oled.print(F("/"));}
        else if(p25 < 4){oled.print(F("4"));}       // This one isn't perfect..
        else if(p25 < 5){oled.print(F("+"));}
        else if(p25 < 6){oled.print(F("t"));}
        else if(p25 < 7){oled.print(F("~"));}
        else {oled.print(F("'"));}
        vizPosition++;
        oled.setCursor(vizPosition,1);
        oled.print(F(" "));      
#endif 
       break;
    }
  }
}


float average(int measurements, float old_average, float new_value){
  if(measurements < 2){
    return new_value;
  }
  else if(measurements < 4){
    return (old_average + new_value) / 2;
  }
  else if(measurements >= 4){
    float totally = (measurements - 1) * old_average;
    return (totally + new_value) / measurements;
  }
}

void receive(const MyMessage &message)
{
#ifdef ALLOW_CONNECTING_TO_NETWORK


  if (message.isAck()) {
    Serial.println(F(">> Received acknowledgement"));
    received_echo = true;
  }

  if (message.type == V_STATUS && message.sensor == ACTIVATED_CHILD_ID ){
    may_send_data = message.getBool(); //?RELAY_ON:RELAY_OFF;
    send(relaymsg.setSensor(ACTIVATED_CHILD_ID).set(may_send_data)); // We echo the new state to the controller, to say "we got the message".
    Serial.print(F("-New may_send_data state: ")); Serial.println(may_send_data);
#ifdef HAS_DISPLAY    
    // Show connection icon on the display
    oled.setCursor(70,0);
    if(may_send_data){                              // A small "T" icon on the screen reflects that data transmission is currently allowed.
      oled.print(F("T"));
    }
    else{
      oled.print(F(" "));
    }
#endif
    
  }
  
#endif
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
