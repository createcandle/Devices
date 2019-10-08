/*
 * 
 * This is a CO2 sensor from the Candle project.
 * 
 * You can attach both a canbon monoxide and a carbon dioxide sensor.
 * 
 * Carbon monoxide is a dangerous, poisonous gas which is completely odourless. It is often formed when something is burning, but doesn't burn with enough oxygen.
 * 
 * Canbon dioxide is what we breathe out. Plants consume carbon dioxide to grow. High levels of carbon dioxide can influence how you feel.
 * 
 * Do not use this device as your sole carbon monoxide sensor! Use it as a support to your main carbon monoxide sensor only.
 * 
 * SETTINGS */ 



//#define HAS_CO_SENSOR                             // Have you attached a CO sensor?

#define HAS_CO2_SENSOR                              // Have you attached a CO2 sensor?

#define MEASUREMENT_INTERVAL 120                    // How many seconds do you want between each measurement? The minimum is 4 seconds.

#define HAS_DISPLAY                                 // Does the sensor have a little OLED display attached?

#define ALLOW_CONNECTING_TO_NETWORK                 // Connect wirelessly. Is this device allowed to connect to the network? For privacy or security reasons you may prefer a stand-alone device. If you do allow the device to connect, you can connect a toggle switch to pin 6 to change the connection state at any time.

//#define ALLOW_FAKE_DATA                           // Allow fake data to be sent? This is an experimental feature, and requires you to attach a switch to pin 5. It's designed to make the sensor less intrusive in some social situations, allowing you to pretend you are not smoking/cooking.

//#define MY_REPEATER_FEATURE                       // Act as a repeater? The devices can pass along messages to each other to increase the range of your network.

#define RF_NANO                                     // RF-Nano. Check this box if you are using the RF-Nano Arduino, which has a built in radio. The Candle project uses the RF-Nano.

/* END OF SETTINGS
 *
 */

#define JESSE                                       // If this is Jesse's Candle prototype

//#define DEBUG
//#define MY_DEBUG // MySensors debugging. Enable MySensors debug output to the serial monitor, so you can check if the radio is working ok.

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

// Mysensors security
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



// PINS
// Be aware, on the RF-Nano pins 9 through 13 are used by the radio.
#define CO_RX_PIN 3                                 // The RX (receive) pin for the CO sensor. This should be connected to the TX (transmit) pin on the sensor module.
#define CO_TX_PIN 4                                 // The TX (transmit) pin for the CO sensor. This should be connected to the RX (receive) pin on the sensor module.
#define CO2_RX_PIN 5                                // The RX (receive) pin for the CO2 sensor. This should be connected to the TX (transmit) pin on the sensor module.
#define CO2_TX_PIN 6                                // The TX (transmit) pin for the CO2 sensor. This should be connected to the RX (receive) pin on the sensor module.

#ifdef JESSE
#define TOGGLE_FAKE_DATA_PIN A3
#define TOGGLE1_MAIN_SWITCH_PIN A1
#define TOGGLE1_BOTTOM_SWITCH_PIN A2
#define TOGGLE1_MOTOR_FORWARD_PIN 7
#define TOGGLE1_MOTOR_BACKWARD_PIN 8
boolean current_toggle1_state = false;
#else
#define TOGGLE1_MAIN_SWITCH_PIN 8
#define TOGGLE_FAKE_DATA_PIN 7
#endif

#ifdef RF_NANO
// If you are using an RF-Nano, you have to switch CE and CS pins.
#define MY_RF24_CS_PIN 9                            // Used by the MySensors library.
#define MY_RF24_CE_PIN 10                           // Used by the MySensors library.
#endif




// LIBRARIES (in the Arduino IDE go to Sketch -> Include Library -> Manage Libraries to add these if you don't have them installed yet.)
#include <MySensors.h>                              // MySensors library                  
#include <SoftwareSerial.h>                         // Serial data connection to the sensor


#ifdef HAS_DISPLAY
  #define OLED_I2C_ADDRESS 0x3C
  #include <SSD1306Ascii.h>                         // Simple drivers for the screen.
  #include <SSD1306AsciiAvrI2c.h>                   // "SSD1306Ascii".
  SSD1306AsciiAvrI2c oled;
  byte screen_vertical_position = 3;                // Used to always show both CO and CO2 levels at the top of the screen.
#endif


#ifdef HAS_CO_SENSOR
// CO sensor variables
SoftwareSerial co_sensor(CO_RX_PIN, CO_TX_PIN);     // Receive (RX) and transmit (TX) pins. RX should always connect to TX on the opposite device.
int COValue = 0;
#endif


#ifdef HAS_CO2_SENSOR
// CO2 sensor variables
SoftwareSerial co2_sensor(CO2_RX_PIN, CO2_TX_PIN);  // Receive (RX) and transmit (TX) pins. RX should always connect to TX on the opposite device.
int co2_value = 0;
float average_co2 = 400;                            
#endif


// Mysensors settings
#define CHILD_ID_CO  1                              // The child ID of the sensor that will be presented to the controller.
#define CHILD_ID_CO2 2                              // The child ID of the sensor that will be presented to the controller.


const byte RF_DELAY = 150;                          // A few milliseconds delay between sending makes the radio happy.

MyMessage prefix_message(CHILD_ID_CO, V_UNIT_PREFIX);    // Tell the controller what to display along with the value.

#ifdef HAS_CO_SENSOR
MyMessage msgCO(CHILD_ID_CO, V_LEVEL);              // Sets up the message format that we'll be sending to the MySensors gateway later.
#endif

#ifdef HAS_CO2_SENSOR
MyMessage msgCO2(CHILD_ID_CO2, V_LEVEL);            // Sets up the message format that we'll be sending to the MySensors gateway later.
#endif


// Other
#define LOOPDURATION 1000                           // The main loop runs every x milliseconds.  Normally this sensor has a 'heartbeat' of once every second.
boolean send_all_values = true;                     // If the controller asks the devive to re-present itself, then this is used to also resend all the current sensor values.
boolean received_echo = false;                      // If we get a response from the controller, then this is set to true.


// Variable for Jesse's candle prototype
#ifdef JESSE
int motorSpeed = 100;
#endif


// Fake data feature
boolean sending_fake_data = false;                  // Experimental. Will allow a user to send fake data for a while. Useful in some social situations.
boolean desired_sending_fake_data = false;          // If the user wants to change the state of sending fake data.
float co2_fakeness_range = 0;                       // How far of the last average the value can meander.   
float p10_fakeness_range = 0;                       // How far of the last average the value can meander.   
float co2_addition = 0;                             // Holds how much will actually be deviated from the average when generating a fake value.
float fakeness_proportion = 0;                      // How these two values relate. if one goes up, the other should also go up, but in proportion to it's own fakeness range.

// Connection toggle feature
boolean desired_connecting_to_network = false;
boolean connecting_to_network = true;


void presentation()
{
#ifdef ALLOW_CONNECTING_TO_NETWORK
  // Send the sketch version information to the gateway and Controller
  sendSketchInfo(F("Carbon sensor"), F("1.0"));
  
  // Register all sensors to gateway: 
#ifdef HAS_CO_SENSOR
  present(CHILD_ID_CO, S_AIR_QUALITY, F("Carbon monoxide")); wait(RF_DELAY);
#endif
#ifdef HAS_CO2_SENSOR
  present(CHILD_ID_CO2, S_AIR_QUALITY, F("Carbon dioxide")); wait(RF_DELAY);
#endif

  send_all_values = true;
#endif
}


void setup() {
  Serial.begin(115200);
  while (!Serial) {}
  
  Serial.println(F("Hello, I am a carbon sensor"));

#ifdef JESSE
  pinMode(TOGGLE1_MOTOR_FORWARD_PIN, OUTPUT);
  pinMode(TOGGLE1_MOTOR_BACKWARD_PIN, OUTPUT);
  pinMode(TOGGLE1_MAIN_SWITCH_PIN, INPUT_PULLUP);
  pinMode(TOGGLE1_BOTTOM_SWITCH_PIN, INPUT_PULLUP);

  current_toggle1_state = digitalRead(TOGGLE1_MAIN_SWITCH_PIN); // Sets the intial state of the motorized toggle switch.
#endif


#ifdef ALLOW_FAKE_DATA
  pinMode(TOGGLE_FAKE_DATA_PIN, INPUT_PULLUP);
  Serial.print(F("Toggle fake-data-mode using a switch on pin ")); Serial.println(TOGGLE_FAKE_DATA_PIN);
#endif


#ifdef HAS_DISPLAY
  // Initiate the display
  oled.begin(&Adafruit128x64, OLED_I2C_ADDRESS);
  oled.setFont(Adafruit5x7);
  oled.ssd1306WriteCmd(SSD1306_DISPLAYON);
  oled.setScroll(false);
  oled.setCursor(0,0);
  oled.print(F("CARBON SENSOR"));
#endif

#ifdef ALLOW_CONNECTING_TO_NETWORK
  if( isTransportReady() ){                         // Check if a network connection has been established
    Serial.println(F("Connected to gateway!"));
    
#ifdef HAS_DISPLAY    
    // Show connection icon on the display
    oled.setCursor(85,0);
    oled.print(F("W"));
#endif

  }
  else {
     Serial.println(F("! NO CONNECTION")); 
  }
#endif 
  
#ifdef HAS_CO_SENSOR
  co_sensor.begin(9600);
#ifdef HAS_DISPLAY
  oled.setCursor(0,screen_vertical_position - 1);   // The labels are shown slightly above the values.
  oled.print(F("CO PPM:")); 
  screen_vertical_position = screen_vertical_position + 3;
#endif
#endif


#ifdef HAS_CO2_SENSOR
  co2_sensor.begin(9600);
#ifdef HAS_DISPLAY
  oled.setCursor(0,screen_vertical_position - 1);
  oled.print(F("CO2 PPM:")); 
#endif
#endif  

#if not defined(HAS_CO_SENSOR) && not defined(HAS_CO2_SENSOR)
  Serial.println(F("Please enable at least one sensor!"));
#ifdef HAS_DISPLAY
  oled.setCursor(0,3);
  oled.print(F("NO SENSORS ENABLED")); 
#endif
  while(1);
#endif

  wdt_enable(WDTO_8S);                              // Starts the watchdog timer. If it is not reset once every few seconds, then the entire device will automatically restart.
}


#ifdef ALLOW_CONNECTING_TO_NETWORK
void send_values()
{
#ifdef HAS_CO_SENSOR
  send(prefix_message.setSensor(CHILD_ID_CO).set( F("ppm") )); delay(RF_DELAY);
  send(msgCO.setSensor(CHILD_ID_CO).set(COValue),1);
#endif
#ifdef HAS_CO2_SENSOR
  send(prefix_message.setSensor(CHILD_ID_CO2).set( F("ppm") )); delay(RF_DELAY);
  send(msgCO2.setSensor(CHILD_ID_CO2).set(co2_value),1);
#endif
}
#endif


void loop() {
#ifdef ALLOW_CONNECTING_TO_NETWORK
  if( send_all_values ){
#ifdef DEBUG
    Serial.println(F("RESENDING VALUES"));
#endif
    send_all_values = 0;
    send_values();
  }
#endif



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
  if( digitalRead(TOGGLE1_MAIN_SWITCH_PIN) != connecting_to_network ){
    
    //connecting_to_network = !desired_connecting_to_network;
    wait(10);
    connecting_to_network = digitalRead(TOGGLE1_MAIN_SWITCH_PIN);
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


#ifdef JESSE

  // Toggle number one
  if( current_toggle1_state == 0 and digitalRead(TOGGLE1_MAIN_SWITCH_PIN) == 1 ){
    switch_on();
    current_toggle1_state = 1;
  }
  else if( current_toggle1_state == 1 and digitalRead(TOGGLE1_MAIN_SWITCH_PIN) == 0 ){
    switch_off(); 
    current_toggle1_state = 0;
  }


  
#endif



  //
  // HEARTBEAT LOOP
  // runs every second (or as long as you want). By counting how often this loop has run (and resetting that counter back to zero after a number of loops), it becomes possible to schedule all kinds of things without using a lot of memory.
  // The maximum time that can be scheduled is 255 * the time that one loop takes. So usually 255 seconds.
  //


  static unsigned long lastLoopTime = 0;            // Holds the last time the main loop ran.
  static byte loopCounter = 0;                      // Count how many loops have passed (reset to 0 after at most 254 loops).
  unsigned long currentMillis = millis();


  if( currentMillis - lastLoopTime > LOOPDURATION ){
    lastLoopTime = currentMillis;
    loopCounter++;
    Serial.print("loopcounter:"); Serial.println(loopCounter);
    if(loopCounter >= MEASUREMENT_INTERVAL){
      Serial.println(); Serial.println(F("__starting__"));  
      loopCounter = 0;
    }

    wdt_reset();                                      // Reset the watchdog timer

  /*
  // Used during development
  if(measurements_counter == 10 && desired_sending_fake_data == false){
    Serial.println(); Serial.println(F("INITIATING FAKENESS----------------------------------------"));
    desired_sending_fake_data = true;
  }
  */
  

#ifdef HAS_DISPLAY
    screen_vertical_position = 3;                   // If there is one sensor attached, then new data should be shown at line 3 of the screen. If there are two, then data is shown on line 3 and line 6.
    
    // Show second counter
    oled.set1X();
    oled.setCursor(100,0);
    oled.print(MEASUREMENT_INTERVAL - loopCounter);
    oled.clearToEOL();
#endif
      

    // schedule
    switch (loopCounter) {
      
      case 1:                                       // If we are in the first second of the clock, get and display the data.
        // CARBON MONIXODE
#ifdef HAS_CO_SENSOR
        COValue = readCOValue();                    // Get carbon monoxide level from sensor module
#ifdef HAS_DISPLAY
        // Show CO level on the screen
        oled.set1X();
        oled.setCursor(0,screen_vertical_position);
        
        if (COValue == -1){                         // -1 value means sensor probably not connected
          oled.print(F("CHECK WIRE"));
          oled.clearToEOL();                       
          break;
        }
        else if (COValue == -2){
          oled.print(F("DATA ERROR"));              // -2 value means we got data form the sensor module was was not a CO2 level. For example, a response to some other command.
          oled.clearToEOL();
        }
        else{
          // Display CO value.
          oled.print(COValue);
          oled.clearToEOL();
  
          // Show quality opinion the screen.
          oled.setCursor(60,screen_vertical_position);
          if (COValue > 0 && COValue < 450){       oled.print(F("GREAT"));}
          else if (COValue < 700){  oled.print(F("GOOD"));oled.clearToEOL();}
          else if (COValue < 1000){ oled.print(F("OK"));  oled.clearToEOL();}
          else if (COValue < 2000){ oled.print(F("POOR"));oled.clearToEOL();}
          else if (COValue < 4500){ oled.print(F("BAD")); oled.clearToEOL();}
          else {
            oled.print(F("Wait.."));
            oled.clearToEOL();
          }
        }
        screen_vertical_position = screen_vertical_position + 3; // If a CO sensor is attached, it's value will be displayed on top. The Co2 value will then be shown 3 lines below it.
#endif
#endif

        // CARBON DIOXIDE
#ifdef HAS_CO2_SENSOR
        int new_co2_value = readco2_value();                  // Get carbon dioxide level from sensor module
        average_co2 = co2_value + new_co2_value / 2;
        co2_value = new_co2_value;

#ifdef ALLOW_FAKE_DATA
        if( desired_sending_fake_data == true ){
          if( sending_fake_data == false ){ // On the first run of fake data, we try and figure out what the range to fake in is.
            if( average_co2 != 0 && co2 != average_co2 ){ // Make sure the range is not zero.
              sending_fake_data = true;
              co2_fakeness_range = co2 - average_co2; 
              if( co2_fakeness_range < 0 ){
                co2_fakeness_range = -co2_fakeness_range; // Turn it into a positive number.
              }
            }
#ifdef DEBUG
            else{
              Serial.println(F("Measurement not useful for intiating fake data."));
            }
#endif
          }
          else{ // We are now sending fake data.
            co2_addition = (float)random( co2_fakeness_range * 100000) / 100000;
            if( random(2) ){ co2_addition = -co2_addition;} // This randomly adds or subtracts the addition.
            Serial.print(F("Addition: ")); Serial.println(co2_addition);
            co2_value = average_co2 + co2_addition; // Create the final fake value
          }
        }
        else{
          sending_fake_data = false;
        }
#endif




#ifdef HAS_DISPLAY
        // Show CO2 level on the screen
        oled.set2X();
        oled.setCursor(0,screen_vertical_position);
        if (co2_value == -1){                         // -1 value means sensor probably not connected
          oled.print(F("CHECK WIRE        "));                        
          break;
        }
        else if (co2_value == -2){
          oled.print(F("DATA ERROR         "));     // -2 value means we got data form the sensor module was was not a CO2 level. For example, a response to some other command.
        }
        else{
          // Display CO value.
          oled.print(co2_value); oled.println(F("      "));
  
          // Show quality opinion the screen.
          oled.setCursor(60,screen_vertical_position);
          if (co2_value > 0 && co2_value < 450){       oled.print(F("GREAT"));}
          else if (co2_value < 700){  oled.print(F("GOOD "));}
          else if (co2_value < 1000){ oled.print(F("OK   "));}
          else if (co2_value < 2000){ oled.print(F("POOR "));}
          else if (co2_value < 4500){ oled.print(F("BAD  "));}
          else {
            oled.print(F("Wait..             "));
          }
        }

#endif
#endif
        break;

      case 2:                                       // Send the data
        received_echo = false;                      // If the controller responds that it has received the data ok, then this will be set back to true.
        
#ifdef HAS_CO_SENSOR
        if( COValue > 0 && COValue < 4500 ){ // Avoid sending erroneous values
          Serial.print(F("Sending CO value: ")); Serial.println(COValue); 
          send(msgCO.setSensor(CHILD_ID_CO).set(COValue),1); // We ask the controller to acknowledge that it has received the data.  
        }
#endif 

#ifdef HAS_CO2_SENSOR
        if( co2_value > 0 && co2_value < 4500 ){ // Avoid sending erroneous values
          Serial.print(F("Sending CO2 value: ")); Serial.println(co2_value); 
          send(msgCO2.setSensor(CHILD_ID_CO2).set(co2_value),1); // We ask the server to acknowledge that it has received the data.
        }
#endif     
        break;

      case 3:                                       // Show the connection status on the display

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



#ifdef HAS_CO_SENSOR
int readCOValue() {

  //while (co_sensor.read()!=-1) {};                  // Clear serial buffer  

  char response[9];                                   // Holds response from sensor
  byte requestReading[9] = {0xFF, 0x01, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79};
  
  Serial.println(F("Requesting data from CO sensor module"));
  co_sensor.write(requestReading, 9);                 // Request PPM CO 
  co_sensor.readBytes(response, 9);

  // Do some checks on the response:
  if (byte(response[0]) != 0xFF){
    Serial.println(F("! Sensor not connected?"));
    while (co_sensor.read()!=-1) {};                  // Empty the serial buffer, for a fresh start, just in case.
    return -1;
  }
  if (byte(response[1]) != 0x86){
    Serial.println(F("! Sensor did not send CO data"));
    return -2;
  }
  // Did the data get damaged along the way?
  char check = getCheckSum(response);
  if (response[8] != check) {
    Serial.println(F("ERROR: checksum did not match"));
    return -2;
  }  

  int high = response[2];
  int low = response[3];
  return high * 256 + low;
}
#endif


#ifdef HAS_CO2_SENSOR
int readco2_value() {

  while (co2_sensor.read()!=-1) {};                    // Clear serial buffer  

  char response[9];                                 // Holds response from sensor
  byte requestReading[9] = {0xFF, 0x01, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79};
  
  Serial.println(F("Requesting data from CO2 sensor module"));
  co2_sensor.write(requestReading, 9);                 // Request data from sensor module.

  co2_sensor.readBytes(response, 9);

  // Do some checks on the response:
  if (byte(response[0]) != 0xFF){
    Serial.println(F("! Is the CO2 sensor connected?"));
    return -1;
  }
  if (byte(response[1]) != 0x86){
    Serial.println(F("! Non-sensor data"));
    return -2;
  }
  // Did the data get damaged along the way?
  char check = getCheckSum(response);
  if (response[8] != check) {
    Serial.println(F("! Corrupted data"));
    return -2;
  }  

  int high = response[2];
  int low = response[3];
  return high * 256 + low;
}
#endif


void receive(const MyMessage &message)
{
  if( message.isAck() ){
    Serial.println(F(">> Received acknowledgement"));
    received_echo = true;
  }
}


// A helper function to check the integrity of a received sensor message.
byte getCheckSum(byte* packet) {
  byte i;
  unsigned char checksum = 0;
  for (i = 1; i < 8; i++) {
    checksum += packet[i];
  }
  checksum = 0xff - checksum;
  checksum += 1;
  return checksum;
}


#ifdef JESSE
void switch_on() {
  analogWrite(TOGGLE1_MOTOR_FORWARD_PIN, motorSpeed);
  digitalWrite(TOGGLE1_MOTOR_BACKWARD_PIN, LOW);
  while (digitalRead(TOGGLE1_MAIN_SWITCH_PIN) == 1);
  stop();
}

void switch_off() {
  digitalWrite(TOGGLE1_MOTOR_FORWARD_PIN, LOW);
  analogWrite(TOGGLE1_MOTOR_BACKWARD_PIN, motorSpeed);
  while (digitalRead(TOGGLE1_BOTTOM_SWITCH_PIN) == 1);
  stop();
}

void stop() {
  digitalWrite(TOGGLE1_MOTOR_FORWARD_PIN, LOW);
  digitalWrite(TOGGLE1_MOTOR_BACKWARD_PIN, LOW);
}
#endif


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
 * Copyright (C) 2013b-2015 Sensnology AB
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
