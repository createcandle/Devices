/*
 *
 * DESCRIPTION
 * 
 * This device can measure the moisture of 5 different plants. It uses the cheap 'capacitive analog 
 * moisture sensor' that you can get for about 3 dollars an Aliexpress or eBay. For example:
 * https://www.aliexpress.com/item/Analog-Capacitive-Soil-Moisture-Sensor-V1-2-Corrosion-Resistant-Z09-Drop-ship/32858273308.html
 * 
 * In a future version, each plant' moisture value can also be responded to individually, either by turning on an LED (wire that to the plan, and you can see which one is thirsty) or, if you want, per-plant automated irrigation by connecting a little solenoid..
 * 
 * SETTINGS */

#define NUMBER_OF_SENSORS 2                         // Sensor count. How many moisture sensors have you connected?

#define SLEEPTIME 100                               // Measurement interval. How many seconds should pass between checking on the plants and sending the data? Don't make this less than 15 or more than 255.

//#define MY_REPEATER_FEATURE                         // ACt as repeater. Do you want this node to also be act as repeater for other devices?

#define RF_NANO                                     // RF-Nano. Check this box if you are using the RF-Nano Arduino, which has a built in radio. The Candle project uses the RF-Nano.


/* END OF SETTINGS
  *  
  *  
  *  
  */

// Enable MySensors debug prints to serial monitor
//#define MY_DEBUG 

// Enable and select radio type attached
#define MY_RADIO_NRF24
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
#define SLEEPTIME 100                               // In seconds, how often should a measurement be made and sent to the server? The maximum delay between measurements is once every 254 seconds, but if you change "byte" to "int" further down in the code you could create more time between each loop.
#define LOOPDURATION 1000                           // The main loop runs every x milliseconds. This main loop starts the modem, and from then on periodically requests the password.

#define RADIO_DELAY 100                                // Milliseconds betweeen radio signals during the presentation phase.

#define ALLOW_CONNECTING_TO_NETWORK

//
// Do not change below this line
// 

#include <MySensors.h>



#ifdef HAS_SCREEN
#include <SoftwareSerial.h>
SoftwareSerial mySerial(7,6);                       // RX, TX
#endif


boolean send_all_values = true;

static const uint8_t analog_pins[] = {A0,A1,A2,A3,A4,A5};
byte moistureLevels[6] = {1, 2, 3, 4, 5, 6};
byte moistureThresholds[6] = {35, 35, 35, 35, 35, 35}; // for each plant we can have a unique moisture level to compare against.
MyMessage msg(0, V_LEVEL);                          // used to send moisture level data to the gateway. Should be V_LEVEL.
MyMessage thresholdMsg(1, V_PERCENTAGE);            // used to create a dimmer on the controller that controls the mosture threshold;



void presentation()
{
  // Send the sketch version information to the gateway and Controller
  sendSketchInfo(F("Plant Health"), F("0.1"));  wait(RADIO_DELAY);

  // Present the sensors

  // For now, it uses S_MOISTURE instead of S_MOISTURE.
  //for (byte i=0; i<NUMBER_OF_SENSORS ; i=i+1) {
  present(0, S_MOISTURE, "Sensor 1");  wait(RADIO_DELAY);       // present all the sensors
  present(1, S_DIMMER, "Threshold 1");  wait(RADIO_DELAY);       // present the dimmers to set the level with.
  if(NUMBER_OF_SENSORS > 1){
    present(2, S_MOISTURE, "Sensor 2");  wait(RADIO_DELAY);       // present all the sensors
    present(3, S_DIMMER, "Threshold 2");  wait(RADIO_DELAY);       // present the dimmers to set the level with.
    if(NUMBER_OF_SENSORS > 2){
      present(4, S_MOISTURE, "Sensor 3");  wait(RADIO_DELAY);       // present all the sensors
      present(5, S_DIMMER, "Threshold 3");  wait(RADIO_DELAY);       // present the dimmers to set the level with.
      if(NUMBER_OF_SENSORS > 3){
        present(6, S_MOISTURE, "Sensor 4");  wait(RADIO_DELAY);       // present all the sensors
        present(7, S_DIMMER, "Threshold 4");  wait(RADIO_DELAY);       // present the dimmers to set the level with.
        if(NUMBER_OF_SENSORS > 4){
          present(8, S_MOISTURE, "Sensor 5");  wait(RADIO_DELAY);       // present all the sensors
          present(9, S_DIMMER, "Threshold 5");  wait(RADIO_DELAY);       // present the dimmers to set the level with.
        }
      }
    }
  }

  send_all_values = true;  
}

void setup()
{
  Serial.begin(115200);                             // Start serial output of data.
  while (!Serial) {}                                // Wait for serial connection to be initiated
  Serial.println(F("Hello, I am a plant health device"));

  //Serial.print("ids-"); Serial.println(ids[1]);

  // Setup pins for input
  //for (byte i = 15; i < 21; i++) {
  //}

  if(isTransportReady()){
    Serial.println(F("Connected to gateway"));
  }else{
    Serial.println(F("! NOT CONNECTED TO GATEWAY"));  
  }

  // Setup pins for input
  for (int i = 0; i < NUMBER_OF_SENSORS; i++) { //or i <= 5
    pinMode(analog_pins[i], INPUT); // experimental new: added the pullup.
     wait(1);
  }



  // load the threshold level from the built-in EEPROM memory.
  for (byte i=0; i<NUMBER_OF_SENSORS ; i++) {
    moistureThresholds[i] = loadState(i);
    if(moistureThresholds[i] > 99){moistureThresholds[i] = 35;}
    Serial.print(F("Loaded: ")); Serial.println(moistureThresholds[i]);
    request((i * 2) + 1, V_PERCENTAGE ); wait(RADIO_DELAY);
  }
  
  Serial.println(F("Warming up the sensors (15 seconds).")); // to avoid weird measurements
  wait(15000);

  wdt_enable(WDTO_8S);                              // Starts the watchdog timer. If it is not reset once every 2 seconds, then the entire device will automatically restart.                                
}




void send_values()
{
  for (byte i=0; i<NUMBER_OF_SENSORS ; i++) {
    send( thresholdMsg.setSensor(i*2 + 1).set( moistureThresholds[i] ) );
  }
}




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


  //
  // MAIN LOOP
  // Runs every few seconds. By counting how often this loop has run (and resetting that counter back to zero after 250 loops), it becomes possible to schedule all kinds of things without using a lot of memory.
  // Maximum time that can be scheduled is 4s * 250 loops = 1000 seconds. So the maximum time between sending data can be 16 minutes.
  //

  static byte loopCounter = 0;                      // Counts the loops until the SLEEPTIME value has been reached. Then new data is sent to the controller.
  static boolean loopDone = false;                  // used to make sure the 'once every millisecond' things only run once every millisecond (or 2.. sometimes the millis() function skips a millisecond.);

  // Avoid the loop running at the speed of the processor (multiple times per millisecond). This entire construction saves memory by not using a long to store the last time the loop ran.
  if( (millis() % LOOPDURATION) > LOOPDURATION - 4 && loopDone == true ) {
    loopDone = false;  
  }

  // Main loop to time actions.
  if( (millis() % LOOPDURATION) < 4 && loopDone == false ) { // the 4 is just a precaution: sometimes the milli() function skips a millisecond. This ensure the loop code still runs in that rare case.
    loopDone = true;
    wdt_reset(); // Reset the watchdog timer

    byte selectedSensor = loopCounter % NUMBER_OF_SENSORS; // Each loop one of the sensors is checked

    Serial.print(F("loopcounter: ")); Serial.println(loopCounter);
    Serial.print(F("selectedSensor: ")); Serial.println(selectedSensor);
    
      int16_t moistureLevel = analogRead(analog_pins[selectedSensor]);
      Serial.print(F(" moisture level (pre): "));
      Serial.println(moistureLevel);
      if(moistureLevel > 700){moistureLevel = 700;}
      moistureLevel = map(moistureLevel,0,700,0,99); // The maximum voltage output of the capacitive sensor is 3V, so since we're measuring 0-5v about 614 is the highest value we'll ever get.
      Serial.print(selectedSensor);
      Serial.print(F(" moisture level %: "));
      Serial.println(moistureLevel);

      moistureLevels[selectedSensor] = moistureLevel;   


      // This is some experimental code to add watering funtionality (actuator). Will be expanded upon later.
      /*
      //}
      //if(digitalRead(shiftedDigitalPin) == HIGH){  // outputs the LED/irrigation status via serial. This code can be removed.
      //  Serial.print(F("- currently watering until "));
      //  Serial.println(moistureThresholds[selectedSensor] + 10);
      //}
      // byte shiftedDigitalPin = selectedSensor + 3;
      
      if( moistureLevel < moistureThresholds[selectedSensor] ){   // if the plant doesn' have enough water, turn on the LED/water.
        Serial.print(F("- moisture level is below ")); Serial.println(moistureThresholds[selectedSensor]);
        //digitalWrite(shiftedDigitalPin, HIGH);
      }else if (moistureLevel >= moistureThresholds[selectedSensor] + 10){  // turn of the water/led if the plant is wet enough.
        //digitalWrite(shiftedDigitalPin, LOW);
      }
      */

      if(loopCounter == 0){
        Serial.println("send test");                       
        // Whole dealing with the first sensor we also do a check if the server is responding ok. It it doesn't respond, remove the connection icon.
        if(send(msg.setSensor(selectedSensor*2).set(moistureLevel),1)){ // Ask for a transmission receipt
          Serial.println(F("Connection is ok"));
        }else {
          Serial.println(F("Connection lost")); 
        }
        
      } else if(loopCounter < NUMBER_OF_SENSORS){       // During the first few loops the script will send updated data.
        if(loopCounter == selectedSensor){                           // It sends sensor 0 at second 0. Sensor 1 at second 1, etc. This keeps the radio happy.
          Serial.println(F("- sending data."));
          byte calculated_child_id = selectedSensor*2;
          Serial.println(F("calculated_child_id: ")); Serial.println(calculated_child_id);
          send(msg.setSensor(calculated_child_id).set(moistureLevel),1);  // 0, 2, 4 etc
        }
      }
    
    loopCounter++;
    if(loopCounter >= SLEEPTIME){                       // If enough time has passed, the counter is reset, and new data is sent.
      loopCounter = 0;
    }
  }
}



void receive(const MyMessage &message)
{
  Serial.print(F("<- message for child #")); Serial.println(message.sensor);

  if (message.isAck()) {
    Serial.println(F("-Got ACK"));
    return;
  }
  
  if (message.type == V_PERCENTAGE) {

    //  Retrieve the power or dim level from the incoming request message
    int requestedLevel = atoi( message.data );
    Serial.print(F("Requested level is "));
    Serial.println( requestedLevel );
    
    byte sensorID = (message.sensor - 1) / 2;
    // Adjust incoming level if this is a V_LIGHT variable update [0 == off, 1 == on]
    // requestedLevel *= ( message.type == V_LIGHT ? 100 : 1 );

    Serial.print(F("Before clipping requested level: "));
    Serial.println( requestedLevel );

    // Clip incoming level to valid range of 0 to 100
    //requestedLevel = requestedLevel > 100 ? 100 : requestedLevel;
    //requestedLevel = requestedLevel < 0   ? 0   : requestedLevel;
    if(requestedLevel > 100){ requestedLevel = 100;}
    if(requestedLevel < 0){ requestedLevel = 0;}

    if(requestedLevel < 1 ||  requestedLevel > 99){
      // Nothing
    }else{
      Serial.print(F("Changing level to "));
      Serial.print( byte(requestedLevel) );
      Serial.print(F(", from "));
      Serial.println( moistureThresholds[sensorID] );
      moistureThresholds[sensorID] = byte(requestedLevel);
      saveState(sensorID, moistureThresholds[sensorID]);
    }
    
    // Inform the gateway of the current DimmableLED's SwitchPower1 and LoadLevelStatus value...
    //send(lightMsg.set(currentLevel > 0));

    // Hek comment: Is this really nessesary?
    //send( dimmerMsg.set(currentLevel) );

  }
}


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
