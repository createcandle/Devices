/*
 * DESCRIPTION
 * 
 * This device can toggle two relays, and thus two electric locks. These relays can be controlled via the Candle Controller, but also via SMS (if you want).
 * The SMS function is password protected: you have to send a password to switch the relay. Access can also be limited to only certain mobile phone numbers, and only certain moments (by toggling the Mobile control switch).
 * 
 * If you send an SMS without the password, it will be shown as the Incoming SMS. This can be used to automate other things in your home. For example, you could create a rule that when the system receices an SMS with "purifier on" in it, the air purifier will turn on.
 * 
 * This way you can control smart devices in your home even if it/Candle is not connected to the internet.
 * 
 * If data transmission is disabled, you can change the state of the lock via SMS or the pushbutton without the new state being sent to the controller.
 * 
 * SETTINGS */ 

// It's important to change these to your own phone number(s). When the device starts up it will always start with these values.
// You will also be able to change this via your controller later, but in case no connection to the controller can be established it's a good idea to enter your phonenumber here.
char phone1[15] = "+031612345678";                  // Phone number of user #1. This is the main number to which status updates will be sent.
char phone2[15] = "+031612345678";                  // Phone number of user #2 (optional). If there is no second user, set it the same number as user #1.

char rotatingPassword1[26] = "door1";               // Door 1 password. If the device is powered up and can't reconnect to the home network, then this will be the fallback password. Maximum length is 25 characters.
#define DOOR1_SELF_LOCKING                          // Self locking? Should door number one automatically re-lock itself after a short amount of time?
char rotatingPassword2[26] = "door2";               // Door 2 password. If the device is powered up and can't reconnect to the home network, then this will be the fallback password. Maximum length is 25 characters.
//#define DOOR2_SELF_LOCKING                        // Self locking? Should door number two automatically re-lock itself after a short amount of time?

#define SELF_LOCKING_DELAY 5                        // Self locking delay. If a door is self-locking, how many seconds before it should re-lock?

//#define HAS_BUTTONS                                 // Have you attached buttons? You can toggle the state of each lock with a push button if you want. If you want this feature, even if you only want it for one, you have to attach two buttons, one for each door.

#define APN_URL "internet"                          // APN Name. The APN URL from your mobile provider. Check your provider website for the precise settings.
#define APN_USERNAME ""                             // APN Login. The APN username from your mobile provider. You can try leaving this and the password empty.
#define APN_PASSWORD ""                             // APN Password. The APN password from your mobile provider.

//#define SEND_SMS_EVERY_49_DAYS                    // Send SMS every 49 days. If you are using a prepaid simcard, you may have to send an SMS once in a while to stop your simcard from getting disabled. This function tries sending an SMS to you once every 49 days.

#define ALLOW_CONNECTING_TO_NETWORK                 // Connect wirelessly. Is this device allowed to connect to the network? For privacy or security reasons you may prefer a stand-alone device. If you do allow the device to connect, you can connect a button to switch the transmission of data or off.

#define RF_NANO                                     // RF-Nano. Check this box if you are using the RF-Nano Arduino, which has a built in radio. The Candle project uses the RF-Nano.


/* END OF SETTINGS
 *  
 *  
 * LYCAMOBILE
 * APN: data.lycamobile.nl
 * Username: lmnl
 * Password: plus
 * CUSD command to get phone number: *102#
 * 
 * LEBARA
 * APN: internet
 * No username or password are required, so they can be empty.
 *  
*/

#define DEBUG                                     // Display debug information in the serial output.
//#define MY_DEBUG                                  // Enable MySensors debug output to the serial monitor, so you can check if the radio is working ok.
//#define JESSE                                     // Enables the special features for Jesse Howard's Candle prototypes.


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
//#define DEBUG_SIGNING
#define MY_ENCRYPTION_SIMPLE_PASSWD "smarthome"      // Be aware, the length of the password has an effect on memory use.
//#define MY_SECURITY_SIMPLE_PASSWD "changeme"      // Be aware, the length of the password has an effect on memory use.
//#define MY_SIGNING_SOFT_RANDOMSEED_PIN A7         // Setting a pin to pickup random electromagnetic noise helps make encryption more secure.

// Mysensors advanced settings
#define MY_TRANSPORT_WAIT_READY_MS 10000            // Try connecting for 10 seconds. Otherwise just continue.
//#define MY_RF24_CHANNEL 100                       // In EU the default channel 76 overlaps with wifi, so you could try using channel 100. But you will have to set this up on every device, and also on the controller.
#define MY_RF24_DATARATE RF24_1MBPS                 // Slower datarate makes the network more stable
//#define MY_RF24_DATARATE RF24_250KBPS             // Slower datarate increases wireless range
//#define MY_NODE_ID 10                             // Giving a node a manual ID can in rare cases fix connection issues.
//#define MY_PARENT_NODE_ID 0                       // Fixating the ID of the gatewaynode can in rare cases fix connection issues.
//#define MY_PARENT_NODE_IS_STATIC                  // Used together with setting the parent node ID. Daking the controller ID static can in rare cases fix connection issues.
#define MY_SPLASH_SCREEN_DISABLED                   // Saves a little memory.
//#define MY_DISABLE_RAM_ROUTING_TABLE_FEATURE      // Saves a little memory.


// MySensors devices form a mesh network by passing along messages for each other. Do you want this node to also be a repeater?
#define MY_REPEATER_FEATURE                         // Add or remove the two slashes at the beginning of this line to select if you want this sensor to act as a repeater for other sensors. If this node is on battery power, you probably shouldn't enable this.

// Define Node ID
//#define MY_NODE_ID 15
//#define MY_PARENT_NODE_ID 0
//#define MY_PARENT_NODE_IS_STATIC


// PINS
#define GSM_RECEIVE_PIN 3                           // Connect to U_TXD on the GSM module
#define GSM_TRANSMIT_PIN 4                          // Connect to U_RXD on the GSM module
#define RELAY1_PIN 5                                // Relay 1 pin number (use this on a door that needs a short pulse to open).
#define RELAY2_PIN 6                                // Relay 2 pin number (use this on a door that can be set to locked or unlocked mode, for example with a solenoid.).


#ifdef JESSE                                        // For the Candle prototype by Jesse Howard
#define TOGGLE_FAKE_DATA_PIN A3
#define TOP_MOTOR_SWITCH_PIN A1
#define BOTTOM_MOTOR_SWITCH_PIN A2
#define MOTOR_FORWARD_PIN 7
#define MOTOR_BACKWARD_PIN 8
#else
#define BUTTON1_PIN A0                              // Manual momentary button for door 1.
#define BUTTON2_PIN A1                              // Manual momentary button for door 2.
#endif

#ifdef RF_NANO
// If you are using an RF-Nano, you have to switch CE and CS pins.
#define MY_RF24_CS_PIN 9                            // Used by the MySensors library.
#define MY_RF24_CE_PIN 10                           // Used by the MySensors library.
#endif


// THESE VALUES CAN BE CHANGED

#define LOOP_DURATION 1000                          // The main loop runs every x milliseconds. This main loop starts the modem, and from then on periodically requests the password.
#define SECONDS_BETWEEN_HEARTBEAT_TRANSMISSION 120  // The smart lock might not send any data for a very long time if it isn't used. Sending a heartbeat tells the controller: I'm still there.
#define MAXIMUMTIMEOUTS 30                          // How often the network may fail before we conclude there is a connection problem.

#define LOCKED 1                                    // This makes it easier to understand what the code is doing. It allows the code to say "LOCKED" instead of "1".
#define UNLOCKED 0                                  // This makes it easier to understand what the code is doing. It allows the code to say "UNLOCKED" instead of "0".

#define RELAY_LOCKED 1                              // Some relays work in reverse. This allows you to choose if a 'lock' signal should send a HIGH or LOW signal to your relay
#define RELAY_UNLOCKED 0                            // Some relays work in reverse. This allows you to choose if an 'unlock' signal should send a HIGH or LOW signal to your relay.

#define EEPROM_STORAGE_START 512                    // From which position the eeprom can safely be used as storage without overwriting MySensors encryption data. Another option would be to use EEPROM_LOCAL_CONFIG_ADDRESS, a MySensors feature.


// Libraries
#include <MySensors.h>                              // The MySensors library that allows devices to communicate and form networks.
#include <avr/pgmspace.h>
#include <EEPROM.h>                                 // Allows for storing data on the Arduino itself, like a mini hard-drive.

// Software serial
#define _SS_MAX_RX_BUFF 255                         // The size of the software serial buffer
#define SERIAL_LINE_LENGTH 128                      // The maximum length that a line from the GSM modem can be.
#include <SoftwareSerial.h>                         // A software library for serial communication. In this case we use it to talk to the GSM modem. Currently not used because of a strange issue.
SoftwareSerial modem(GSM_RECEIVE_PIN,GSM_TRANSMIT_PIN); // Receive pin (RX), transmit pin (TX)



char serial_line[SERIAL_LINE_LENGTH];                // Holds the line received from the modem.

// MySensors children
#define DEVICE_STATUS_ID 0                          // The first 'child' of this device is a text field that contains status updates.
#define SMS_CHILD_ID 1                              // This field is used to tell the controller what text was in the sms. Useful for automation of other things that you want to trigger using an SMS.
#define SENDSMS_CHILD_ID 2                          // If this string is modified at the controller, a test sms will be sent. Useful to keep a simcard alive (must send at least one SMS every X months usually).
#define BUTTON_CHILD_ID 3                           // Set button ID number on this node.
#define TRANSMISSION_STATE_CHILD_ID 4               // The child ID of the data transmission switch.
#define SMS_CONTROL_ID 5                            // Toggle whether SMS is allowed to control the lock.

#define ROTATING_PASSWORD1_ID 10                    // Set password ID number on this node that can be set from outside.
#define ROTATING_PASSWORD2_ID 36                    // Set password ID number on this node that can be set from outside.
#define PHONENUMBER1_ID 120                         // If (part of) a phonenumber is given, only phonenumbers that have that part will be allowed to open the lock. E.g. "+31" only allows Dutch numbers. "+3161482375" would only allow that one number.
#define PHONENUMBER2_ID 135                         // If (part of) a phonenumber is given, only phonenumbers that have that part will be allowed to open the lock. E.g. "+31" only allows Dutch numbers. "+3161482375" would only allow that one number.

#define RELAY1_CHILD_ID 210                         // Set switch ID number on this node. Door 1.
#define RELAY2_CHILD_ID 211                         // Set switch ID number on this node. Door 2. This needs to be 1 higher than relay 1.

MyMessage text_message(DEVICE_STATUS_ID, V_TEXT);   // Sets up the message format that we'll be sending to the MySensors gateway later. The first part is the ID of the specific sensor module on this node. The second part tells the gateway what kind of data to expect.
MyMessage relay_message(TRANSMISSION_STATE_CHILD_ID, V_STATUS); // A generic boolean state message.
MyMessage lock_message(RELAY1_CHILD_ID, V_STATUS); // A message to send the state of locks. Was 'V_LOCK_STATUS', but this is a currently a read-only property.


// General variables
#define DOOR_COUNT 2                                // How many electric locks are attached.
#define RADIO_DELAY 100                             // Milliseconds delay betweeen radio signals. This gives the radio some breathing room.
boolean incomingSMS = 0;                            // Used in flow control when processing an SMS.
boolean waitingForResponse = false;                 // Used to check if the connection to the controller is ok. If we are still waiting by the next time a password is requested, something is fishy.
boolean send_all_values = 1;
boolean processing_line = 0;

boolean desired_door_states[DOOR_COUNT];            // An array that holds the desired door states. These are turned into:
boolean actualDoorStates[DOOR_COUNT];               // An array that holds the actual door states.

const char unlockedMessage[] PROGMEM = { "  unlocked" }; // This construction saves some memory. Only the first character has to be changed to create a message to send via SMS, such as "1 locked" or "2 unlocked".s
const char lockedMessage[] PROGMEM = { "  locked" }; // This construction saves some memory.
const char passwordMessage[] PROGMEM = { "Door   password" }; // This construction saves some memory.

#define WORK_STRING_SIZE 26                         // Length of an array that holds various strings.
char work_string[WORK_STRING_SIZE];                 // Creates an array that will hold various strings.


// MODEM STATES
#define MODEM_BUFFER_EMPTY 0                        // These three states are used to avoid sending the modem commands while it's still sending them to the Arduino.
#define WAITING_FOR_RESPONSE_FROM_MODEM 1
#define MODEM_PROCESSING_RESPONSE 2



byte modem_state = MODEM_BUFFER_EMPTY;
boolean modem_booted = false;


// DATA TRANSMISSION TOGGLE
boolean transmission_state = true;
boolean desired_transmission_state = true;

boolean sms_control_state = true;
boolean desired_sms_control_state = true;

boolean store_to_eeprom = false;
boolean send_sms = false;

#ifdef DEBUG
// A small funtion that can tell you how much ram is left.
int freeRam () {
  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}
// via https://playground.arduino.cc/Code/AvailableMemory
#endif




// The before function is run very early on. This makes it great for quickly changing pin settings.
void before()
{
  Serial.begin(115200);
  Serial.println(F("Hello world, I am a door lock."));

  memset(desired_door_states,0,sizeof(desired_door_states));
  memset(actualDoorStates,0,sizeof(actualDoorStates));

  for( byte checkingDoor = 0; checkingDoor < DOOR_COUNT; checkingDoor++ ){
    //Serial.print(F("BUTTON1_PIN + checkingDoor = ")); Serial.println(BUTTON1_PIN + checkingDoor);
    pinMode(RELAY1_PIN + checkingDoor, OUTPUT);     // Set all relay pins to be output.
     // Set the button pins as input.
    //pinMode(BUTTON1_PIN + checkingDoor, INPUT);   // Set the button pins as input.
    byte possibleDesiredState = EEPROM.read(EEPROM_STORAGE_START + RELAY1_CHILD_ID + checkingDoor);
    
    if( possibleDesiredState == 0 || possibleDesiredState == 1 ){ // If the eeprom has the number '255' here, then it clearly hasn't been set to hold a desired value before, and we should not use it.
      //Serial.println("Found useable state in eeprom");
      desired_door_states[checkingDoor] = possibleDesiredState;
      
      if( possibleDesiredState == LOCKED){          // This may seem complicated. The numerical state that represents 'locked' in the interface could be different from what the relay conciders locked. For example, 1 could be locked in the interface, but unlocked for the relay. And vice-versa. This construction deals with that possibility.
        digitalWrite(RELAY1_PIN + checkingDoor, RELAY_LOCKED);
      }
      if( possibleDesiredState == UNLOCKED){
        digitalWrite(RELAY1_PIN + checkingDoor, RELAY_UNLOCKED);
      }
      actualDoorStates[checkingDoor] = desired_door_states[checkingDoor];
    }
    else {
#ifdef DEBUG
      Serial.println(F("No useable state found in eeprom"));
#endif
      digitalWrite(RELAY1_PIN + checkingDoor, RELAY_UNLOCKED); // If no value was stored in the eeprom, start unlocked, for safety.
    }
  }


  if( EEPROM.read(EEPROM_STORAGE_START + 255) == 1 ){ // This position indicates whether any passwords and phone numbers have been stored in eeprom.
    Serial.println(F("Loading phone numbers and passwords from internal memory"));
    loadEverythingFromEeprom();
  }
  else{
    Serial.println(F("No preferences stored in internal memory yet"));
  }
}



#ifdef ALLOW_CONNECTING_TO_NETWORK
void presentation()
{
  sendSketchInfo(F("Smart lock"), F("2.0")); wait(RADIO_DELAY);

  present(DEVICE_STATUS_ID, S_INFO, F("Status")); wait(RADIO_DELAY);
  present(SMS_CHILD_ID, S_INFO, F("Received SMS")); wait(RADIO_DELAY);
  present(SENDSMS_CHILD_ID, S_INFO, F("Send SMS")); wait(RADIO_DELAY);
  present(PHONENUMBER1_ID, S_INFO, F("Phone number 1")); wait(RADIO_DELAY);
  present(PHONENUMBER2_ID, S_INFO, F("Phone number 2")); wait(RADIO_DELAY);

  // Presenting the password fields. This is currently over coplicated, but was an experiment in making the code more flexible so it could handle a lot of locks.
  
  for( byte i = 0; i < DOOR_COUNT; i++ ){           // We loop over all the doorlocks in the system, and check if they are in the right position.
    memset(work_string,0,sizeof(work_string));
    strcpy_P(work_string, passwordMessage);          // This copies the password message that we stored in progmem (flash memory) into real memory.
    
    work_string[5] = i + 49;                         // This will create "Door 1 password", "Door 2 password", etc. In that string the number is the 5th character (starting form 0). Ascii character 49 = "1", character 50 = "2", etc.
    //Serial.println(work_string);
    present(ROTATING_PASSWORD1_ID + (i*26), S_INFO, work_string); wait(RADIO_DELAY);
    
    work_string[6] = '\0'; // Shortens the string to just be "Door 1", "Door 2", etc.
    //Serial.println(work_string);
    present(RELAY1_CHILD_ID + i, S_BINARY, work_string, true); wait(RADIO_DELAY);
  }

  present(TRANSMISSION_STATE_CHILD_ID, S_BINARY, F("Data transmission")); wait(RADIO_DELAY);
  present(SMS_CONTROL_ID, S_BINARY, F("Mobile control")); wait(RADIO_DELAY);

  send_all_values = true;                           // Whenever a new presentation is requested, we should also send the current values of the children.
}


void send_values()
{
#ifdef DEBUG
  Serial.println(F("Sending button states"));
#endif
  send(text_message.setSensor(DEVICE_STATUS_ID).set( F("OK")),1); wait(RADIO_DELAY);  
  send(text_message.setSensor(SMS_CHILD_ID).set( F(""))); wait(RADIO_DELAY);
  send(text_message.setSensor(SENDSMS_CHILD_ID).set( F("Change me to send SMS"))); wait(RADIO_DELAY);
  send(text_message.setSensor(PHONENUMBER1_ID).set( phone1 ),1); wait(RADIO_DELAY);
  send(text_message.setSensor(PHONENUMBER2_ID).set( phone2 ),1); wait(RADIO_DELAY);

#ifdef DEBUG
  Serial.print(F("Sending phone1: ")); Serial.println( phone1 );
#endif

  //for( byte i = 0; i < DOOR_COUNT; i++ ){
    //send(text_message.setSensor(ROTATING_PASSWORD1_ID + i).set( F("change_me") )); wait(RADIO_DELAY);
  //}
  send(text_message.setSensor(ROTATING_PASSWORD1_ID).set( rotatingPassword1 )); wait(RADIO_DELAY);
  send(text_message.setSensor(ROTATING_PASSWORD2_ID).set( rotatingPassword2 )); wait(RADIO_DELAY);

  send(relay_message.setSensor(TRANSMISSION_STATE_CHILD_ID).set(transmission_state),1); wait(RADIO_DELAY);
  send(relay_message.setSensor(SMS_CONTROL_ID).set(sms_control_state),1); wait(RADIO_DELAY);

  if( transmission_state ){                           // Send the state of the locks (if data transmission is allowed)
    for( byte i = 0; i < DOOR_COUNT; i++ ){
      send(lock_message.setSensor(RELAY1_CHILD_ID + i).set( actualDoorStates[i] )); wait(RADIO_DELAY);
    }
  }
}
#endif // connecting to network

void setup()
{
  wait(1000);

#ifndef JESSE
  pinMode(BUTTON1_PIN, INPUT_PULLUP);
  pinMode(BUTTON2_PIN, INPUT_PULLUP);
#endif

#ifdef ALLOW_CONNECTING_TO_NETWORK

#ifdef JESSE
  pinMode(MOTOR_FORWARD_PIN, OUTPUT);
  pinMode(MOTOR_BACKWARD_PIN, OUTPUT);
  pinMode(TOP_MOTOR_SWITCH_PIN, INPUT_PULLUP);
  pinMode(BOTTOM_MOTOR_SWITCH_PIN, INPUT_PULLUP);
  transmission_state = digitalRead(TOP_MOTOR_SWITCH_PIN); // Sets the intial state of the motorized toggle switch.
#else
  transmission_state = loadState(TRANSMISSION_STATE_CHILD_ID);
#endif
  Serial.print(F("Initial transmission state: ")); Serial.println(transmission_state);

  
  // Has a connection to the controller been established?
  if( isTransportReady() ){
    Serial.println(F("Connected to gateway"));
    send(text_message.setSensor(DEVICE_STATUS_ID).set( F("STARTING GSM"))); wait(RADIO_DELAY); 
  }else{
    Serial.println(F("Not connected to gateway"));
    //timeOutCount = MAXIMUMTIMEOUTS;                 // Start the system in 'no connection' mode, meaning the backup passwords may be used.
  }

#endif // allow connecting to network


#ifdef DEBUG
  for ( int i = EEPROM_STORAGE_START; i < EEPROM_STORAGE_START + 512; i++ ){
    //char k = EEPROM.read(i);
    //Serial.print(i); Serial.print(F("=")); Serial.write(k); Serial.print(F(" (")); Serial.println( (byte) k);
  }
#endif


  Serial.println(F("Connecting to GSM.."));
  modem.begin(57600);  //57600                      // The modem automatically adapts to work at the same baud rate when we send a few commands.
  modem.println(F("AT"));
  wait(300);
  modem.println(F("AT"));
  wait(300);
  modem.println(F("AT"));
  wait(300);
  
  modem.println(F("AT&F"));                         // Reset the GSM modem.
  wait(15000);                                      // After a reset, the GSM modem needs some time to settle.

  while( modem.available() > 0 ){                   // Empty the buffer before we start the actual bootup procedure
    modem.read();
  }

  wdt_enable(WDTO_8S);                              // Starts the watchdog timer. If it is not reset once every few seconds, then the entire device will automatically restart.                                
}


void loop()
{
  static unsigned long last_loop_time = 0;          // Holds the last time the main loop ran.
  static bool buttonBeingPressed = 0;               // Used to debounce the push buttons.
  static byte number_of_bytes_received = 0;         // How many characters we've stored in the serial_line character array.
  static byte loop_counter = 0;                     // Count how many loops have passed (reset to 0 after at most 254 loops).
  static byte modem_counter = 0;

  //wdt_reset();                                      // Reset the watchdog timer


#ifdef ALLOW_CONNECTING_TO_NETWORK
  // If a presentation is requested, we also send the values of the children.
  if( send_all_values ){
#ifdef DEBUG
    Serial.println(F("RESENDING VALUES"));
#endif
    send_all_values = 0;
    send_values();
  }

  // User initiated a transmission state change remotely
  if( desired_transmission_state != transmission_state ){
    Serial.print(F("Desired transmission state has changed to ")); Serial.println(desired_transmission_state);
#ifdef JESSE
    transmission_state = motor_switch(desired_transmission_state);
#else
    transmission_state = desired_transmission_state;
#endif
    EEPROM.update(EEPROM_STORAGE_START + TRANSMISSION_STATE_CHILD_ID,transmission_state); // Remember the prefered state in the eeprom memory. This will make sure it is remembered after a reboot.

    Serial.print(F("Sending new transmission state: ")); Serial.println(transmission_state);
    //controller_got_transmission_state = false;
    if( transmission_state ){
      send_values();                                // If data transmissin is re-enabled, all current values (including the real state of the locks) are sent to the controller.
    }
    else {
      send(relay_message.setSensor(TRANSMISSION_STATE_CHILD_ID).set(transmission_state),1); wait(RADIO_DELAY); // We acknowledge to the controller that we are now in the new state.
    }
    
  }

  // User initiated a SMS control change remotely
  if( desired_sms_control_state != sms_control_state ){
    Serial.print(F("Desired sms control state has changed to ")); Serial.println(desired_sms_control_state);
    sms_control_state = desired_sms_control_state;
    
    Serial.print(F("Sending new sms control state: ")); Serial.println(sms_control_state);
    //controller_got_transmission_state = false;
    send(relay_message.setSensor(SMS_CONTROL_ID).set(sms_control_state),1);  wait(RADIO_DELAY);// We acknowledge to the controller that we are now in the new state.
    wait(RADIO_DELAY);
  }

  if( send_sms ){
    //Serial.print(F("controller: send sms. Work_string: ")); Serial.println(work_string);
    send_sms = false;                               // Indicates the sms has been sent.
    sendSMS();
  }

  // If any of the passwords or phone numbers have been updated, they should be stored/updated in eeprom.
  if( store_to_eeprom ){
    store_to_eeprom = false;                        // Indicates the storage cycle is done.
    storeEverythingToEeprom();
    send(text_message.setSensor(DEVICE_STATUS_ID).set( F("New value stored"))); wait(RADIO_DELAY);
  }
  
#endif // connecting to network

  


  //
  //  KEEP SIM CARD ALIVE
  //
  
#ifdef SEND_SMS_EVERY_49_DAYS
  // This is an optional feature.
  // Every 49,7 days (using the millis() rollover) the system sends out an SMS. This helps keep the simcard active and registered on the GSM network. 
  // Use at your own risk: if the system experiences a power loss, the timer starts at 0 again. If your experience frequent powerlosses, then the simcard might be-deregistered anyway, since the keep-alive SMS's won't get sent. 
  // A slight delay is built in: the first sms is sent a week after the smart lock becomes active. This avoid sending a lot of SMS's if you are still playing with setting up the device, and powerlosses may be frequent.
  // This smart lock also offers another option. You can let the controller trigger the sending of the sms using the 'send test sms' button. Of course, your controller could also be down when you scheduled to trigger the button.
  static bool keepAliveSMSsent = false;             // Used to make sure an SMS is only sent as the milliseconds starts, and not during every loop in the millisecond.
  if(millis() < 5){
    keepAliveSMSsent  = false;                      // When the clock rolls over, set the variable back so that a new SMS can be sent.
  }
  if (millis() > 604800000 && millis() < 604800010 && keepAliveSMSsent == false){ // 604800000 = 1 week in milliseconds. Sending the first keep-alive SMS after a week avoids sending a lot of SMS-es while testing the system (which may involve a lot of reboots).
    keepAliveSMSsent  = true;
    strcpy(work_string, phone1); 
    sendSMS();                                      // If the work string only contains a phone number - as is the case here - a generic "Hello from Candle" SMS will be sent.
  }
#endif



  //
  //  GSM BOOTUP SEQUENCE
  //

  if( modem_state == MODEM_BUFFER_EMPTY && modem_booted == false ){
    // The receive buffer has been processed. Time to send a new command in the list, or just go into "ready" state.
    modem_counter++;
    modem_state = WAITING_FOR_RESPONSE_FROM_MODEM;
    Serial.println();
    Serial.print(F("__GSM boot step ")); Serial.println(modem_counter);
#ifdef ALLOW_CONNECTING_TO_NETWORK
    send(text_message.setSensor(DEVICE_STATUS_ID).set( (char) modem_counter )); wait(RADIO_DELAY);
#endif
    
    switch( modem_counter ){

      case 0:
        modem.println(F("AT"));                     // Never called, since the modem_counter jumps from 0 to 1 just before this.
        break;
      case 1:
        modem.println(F("AT+CCID"));                // Initiating the GSM modem
        //Serial.println(F("<<AT"));
        break;
      case 2:
        modem.println(F("ATE0"));                   // Don't echo the AT commands back to the Arduino. ATE1 turns it on instead (and it's the default too).
        //Serial.println(F("<<ATE0"));
        break;
      case 3:
        modem.println(F("AT+EGMR=?"));              // Gets IMEI code.
        break;
      case 4:
        modem.println(F("AT+EGMR=2,7"));            // Set IMEI write state
        wait(3000);
        break;
      case 5:
        modem.print(F("AT+CSTT=\""));               // Send the GPRS settings
        modem.print(APN_URL);
        modem.print(F("\",\""));
        modem.print(APN_USERNAME);
        modem.print(F("\",\""));
        modem.print(APN_PASSWORD);
        modem.println(F("\""));
        wait(3000);
        break;
      case 6:
        modem.println(F("AT+COPS=0"));              // Connect to the GSM network
        wait(3000);
        break;
      case 7:
        modem.println(F("AT+CGATT=1"));             // Enable GPRS connection
        wait(4000);
      case 8:
        modem.println(F("AT+CGATT?"));              // Did we manager to connect to GPRS?
        break;
      case 9:
        modem.println(F("AT+CREG?"));               // Is the modem succesfully registered on the network?
        break;
      case 10:
        modem.println(F("AT+CMGF=1"));              // SMS mode. Get SMS as plain text. 
        break;
      case 11:
        modem.println(F("AT+CSDH=0"));              // CSDH=0 asks the modem to not send full SMS headers.
        break;
      case 12:
        //Serial.println(F("10_delete all sms"));
        modem.println(F("AT+CNMI=2,2,0,0,0"));      // Listen to incoming messages.
        break;
      case 13:
        //Serial.println(F("8_sms_live_mode"));            
        modem.println(F("AT+CMGD=1,4"));            // Delete all SMS messages stored on the modem.
        wait(4000);
        break;
      case 14:
        modem.println(F("AT+COPS?"));               // Outputs which GSM network the modem is connected to
        break;
      /*
      case 12:
        //modem.println(F("AT+CGATT?"));
        break;
      case 13:
        //modem.println(F("AT+CSCS=?"));
        break;
      case 14:
        //modem.println(F("AT+COPS?"));
        break;
      case 15:
        //modem.println(F("AT^STF=1"));
        //modem.println(F("AT+CSCS=\"GSM\""));
        //modem.println(F("AT+CSCS=\"8859-1\""));
        //modem.println(F("AT+CSCS=\"UCS2\""));
        //modem.println(F("AT^STF=1"));
        //modem.println(F("AT^STA=?"));
        //modem.println(F("AT+CSSN=?"));
        break;
      */
      
      default:
        Serial.println();
        Serial.println(F("MODEM BOOT COMPLETE"));
        Serial.println();
        modem_state = MODEM_BUFFER_EMPTY;
        modem_booted = true;
#ifdef ALLOW_CONNECTING_TO_NETWORK
        send(text_message.setSensor(DEVICE_STATUS_ID).set( F("GSM READY"))); wait(RADIO_DELAY);
#endif
        break;
    }
    wdt_reset();
    wait(2000);
  }
  


  //
  //  MAIN LOOP
  //

  if( millis() - last_loop_time > LOOP_DURATION ){ // Runs every second
    last_loop_time = millis();

    wdt_reset();                                  // Reset the watchdog timer. If this doesn't happen, the device must have crashed, and it will be automatically rebooted by the watchdog.

    if( loop_counter >= SECONDS_BETWEEN_HEARTBEAT_TRANSMISSION ){ // If a couple of minutes have passed, tell the controller we're still here
      loop_counter = 0;
      Serial.println(F("Sending heartbeat"));
      sendHeartbeat();                        
      if( modem_booted == false && modem.available() == 0 && modem_state == WAITING_FOR_RESPONSE_FROM_MODEM ){ // If the boot process never (fully) ran, then this allows for a retry.
        modem_counter = 0;
        modem_state = MODEM_BUFFER_EMPTY;
#ifdef ALLOW_CONNECTING_TO_NETWORK
        send(text_message.setSensor(DEVICE_STATUS_ID).set( F("Modem not responding") )); wait(RADIO_DELAY);
#endif   
#ifdef DEBUG
        Serial.println(F("Resetting modem state"));
#endif
      }
    }
    else{
      loop_counter++;
    }
  }



  //
  //  Manage power to the locks
  //

  for( byte checkingDoor = 0; checkingDoor < DOOR_COUNT; checkingDoor++ ){ // We loop over all the doorlocks in the system, and check if they are in the right position.
    //Serial.print(F("checkingDoor ")); Serial.println(checkingDoor);
    if( desired_door_states[checkingDoor] != actualDoorStates[checkingDoor] ){
      Serial.println(F("Changing door status"));
      Serial.println(checkingDoor);
      char updateMessage[11];
      if( desired_door_states[checkingDoor] == LOCKED ){
        digitalWrite(RELAY1_PIN + checkingDoor, RELAY_LOCKED);
        strcpy_P(updateMessage, lockedMessage);   // Copy the string "  locked" from PROGMEM / FLASH into RAM
      }
      else if( desired_door_states[checkingDoor] == UNLOCKED ){
        digitalWrite(RELAY1_PIN + checkingDoor, RELAY_UNLOCKED);
        strcpy_P(updateMessage, unlockedMessage); // Copy the string "  unlocked" from PROGMEM / FLASH into RAM
      }
      updateMessage[0] = checkingDoor + 49;       // Here the first character of the string is changed to create a useful message, such as "1 locked" or "2 unlocked". Ascii character '1' has number 49. 2 has number 50, etc.
#ifdef DEBUG
      Serial.print(F("updateMessage: "));Serial.println(updateMessage);
      Serial.print(F("relay ID: ")); Serial.println(RELAY1_CHILD_ID + checkingDoor);
      Serial.print(F("new desired state: ")); Serial.println(desired_door_states[checkingDoor]);
#endif
#ifdef ALLOW_CONNECTING_TO_NETWORK
      if( transmission_state ){
        send(lock_message.setSensor(RELAY1_CHILD_ID + checkingDoor).set( desired_door_states[checkingDoor] )); wait(RADIO_DELAY); // Tell the controller in what state the lock is.
        send(text_message.setSensor(DEVICE_STATUS_ID).set( updateMessage )); wait(RADIO_DELAY);
      }
#endif
      actualDoorStates[checkingDoor] = desired_door_states[checkingDoor];
      EEPROM.update(EEPROM_STORAGE_START + RELAY1_CHILD_ID + checkingDoor, desired_door_states[checkingDoor]); // Store the new position in eeprom. After a reboot it will automatically jump back to this state.
    }
  }


#ifdef DOOR1_SELF_LOCKING
  if( actualDoorStates[0] == UNLOCKED ){
    Serial.println(F("Self locking in a few seconds"));
    wdt_disable();
    wait(SELF_LOCKING_DELAY * 1000);
    wdt_enable(WDTO_8S);
    desired_door_states[0] = LOCKED;
  }
#endif


#ifdef DOOR2_SELF_LOCKING
  if( actualDoorStates[1] == UNLOCKED ){
    Serial.println(F("Self locking in a few seconds"));
    wdt_disable();
    wait(SELF_LOCKING_DELAY * 1000);
    wdt_enable(WDTO_8S);
    desired_door_states[1] = LOCKED;
  }
#endif


#ifdef HAS_BUTTONS
  // Check button to toggle the state
  if (digitalRead(BUTTON1_PIN) == HIGH){
    //Serial.println(F("1"));
    wait(100);
    if(buttonBeingPressed == 0){
      desired_door_states[0] = !desired_door_states[0]; // On press of the button, toggle the door 1 desired status. e.g. locked -> unlocked.
#ifdef ALLOW_CONNECTING_TO_NETWORK
      if( transmission_state ){
        send(text_message.setSensor(DEVICE_STATUS_ID).set( F("Button 1 pressed") )); wait(RADIO_DELAY);
      }
#endif
      Serial.print(F("Button1->")); Serial.println(desired_door_states[0]);
    }
    buttonBeingPressed = 1;
  }
  else if (digitalRead(BUTTON2_PIN) == HIGH){
    //Serial.println(F("2"));
    wait(100);
    if(buttonBeingPressed == 0){
      desired_door_states[1] = !desired_door_states[1]; // On press of the button, toggle the door 2 desired status. e.g. locked -> unlocked.
#ifdef ALLOW_CONNECTING_TO_NETWORK
      if( transmission_state ){
        send(text_message.setSensor(DEVICE_STATUS_ID).set( F("Button 2 pressed") )); wait(RADIO_DELAY);
      }
#endif
      Serial.print(F("Button2->")); Serial.println(desired_door_states[1]);
    }
    buttonBeingPressed = 1;  
  }
  else{
    buttonBeingPressed = 0; 
  }
#endif



  //
  // CHECK FOR INCOMING DATA FROM MODEM
  //
  
  if( modem.available() > 0 ){
    
    char singleChar = modem.read();
#ifdef DEBUG
    //Serial.print(number_of_bytes_received); Serial.print(F("=")); Serial.write(singleChar); Serial.print(F(" (")); Serial.print( (byte) singleChar);
    //Serial.print(F("), to go = ")); Serial.println( modem.available() );
#endif
    if( singleChar != 0 ){ // Ignore Null character. This gets sent by the modem once in a while.
      serial_line[number_of_bytes_received] = singleChar;
      number_of_bytes_received++;

      if( modem_state == WAITING_FOR_RESPONSE_FROM_MODEM ){ // We got a proper response from the modem.
        Serial.print(F(">>Got response from modem >")); Serial.println( modem.available() );
        modem_state = MODEM_PROCESSING_RESPONSE;
      }
    }
    
    char peek_char = 49; //  ascii 1. Doesn't matter what it is really.
    if( modem.available() > 0 ){
      peek_char = modem.peek();
      //Serial.print(F("-peek = ")); Serial.println(peek_char);
      if( peek_char == 10 ){
        //Serial.println(F("removing peek"));
        modem.read();             // Clear the CR and LF from the buffer.
      }
    }
    
    
    if( (singleChar == 13 && peek_char == 10) || number_of_bytes_received == SERIAL_LINE_LENGTH ){ // Each line of text the modem sends ends with two special characters, the line feed (LF) and Carriage return (CR). We don't need those anymore.
      //Serial.print(F("Received a line with a length of ")); Serial.println(number_of_bytes_received);
      serial_line[number_of_bytes_received] = '\0';
      number_of_bytes_received = 0;
      //Serial.println(serial_line);
      //Serial.print(F("  -to go: ")); Serial.println( modem.available() ); // After this line, how many more characters are there left in the buffer.
      processLine();
    }


    if( modem.available() == 0 ){
      if( modem_state == MODEM_PROCESSING_RESPONSE && number_of_bytes_received == 0 ){
        //Serial.println(F("Response buffer processed"));
        modem_state = MODEM_BUFFER_EMPTY;
      }
#ifdef DEBUG
      if( number_of_bytes_received != 0 ){
        Serial.println(F("Yikes, buffer is empty but serial_line is not complete. Buffer overflow happened?"));
        number_of_bytes_received = 0;
      }
#endif
    }
  }



} // End of main loop







// PROCESS LINE
//
// This parses the serial data from the GSM modem, and for example filters out the actual SMS text.
void processLine() 
{
#ifdef DEBUG
  Serial.print(F("processing: ")); Serial.println(serial_line);
  //send(text_message.setSensor(SMS_CHILD_ID).set(serial_line)); wait(RADIO_DELAY);
#endif

  // If the incomingSMS has been set, that means we already found the phonenumber part of the sms, and are now at the actual sms content part.
  if ( incomingSMS == 1 ){
    incomingSMS = 0;                              // Reset for when the next sms needs to be dissected.
#ifdef DEBUG
    Serial.println(F("-At the actual text"));
#endif
    // Does the SMS content begin with a correct password?
    byte password1length = strlen(rotatingPassword1);
    byte password2length = strlen(rotatingPassword2);
    bool password1found = strncmp(serial_line, rotatingPassword1, password1length);
    bool password2found = strncmp(serial_line, rotatingPassword2, password2length);
    password1found = !password1found;
    password2found = !password2found;

#ifdef DEBUG
    Serial.print(F("pass 1 found?: "));Serial.println(password1found);
    Serial.print(F("pass 2 found?: "));Serial.println(password2found);
#endif
    
    if( password1found || password2found ){ // If password matches, and using SMS to control the lock is allowed
      Serial.println(F("Correct password received"));

      if( sms_control_state ){
        char * command;
        command = strstr(serial_line, " ");          // Search for the index of the space
        if (command != NULL)                        // If successful then command now points to the second word in the SMS
        {
          // Serial.print(F("Command = ")); Serial.println(command);
          // Check what the command is, and act on it.
          bool commandLock = strncmp (command, " lock", 5);
          bool commandUnlock = strncmp (command, " unlock", 7);
          bool commandStatus = strncmp (command, " status", 7);
          if(commandLock == 0){
            Serial.println(F("->lock"));
            if(password1found){ desired_door_states[0] = LOCKED; }
            if(password2found){ desired_door_states[1] = LOCKED; }
          }
          else if(commandUnlock == 0){
            Serial.println(F("->unlock"));
            if(password1found){ desired_door_states[0] = UNLOCKED; }
            if(password2found){ desired_door_states[1] = UNLOCKED; }
          }
          else if(commandStatus == 0){
            Serial.println(F("command: send status sms"));
            //char updateMessage[11];
            if( password1found ){ // If it's zero, it means it was found.
              if( actualDoorStates[0] == LOCKED ){ strcpy_P(work_string, lockedMessage); }
              else { strcpy_P(work_string, unlockedMessage); }
              work_string[0] = 49; // ascii 1
            }
            if( password2found ){ 
              if( actualDoorStates[1] == LOCKED ){ strcpy_P(work_string, lockedMessage); }
              else { strcpy_P(work_string, unlockedMessage); }
              work_string[0] = 50; // ascii 2
            }
            Serial.println(F("Status SMS: ")); Serial.println(work_string);
            sendSMS(); // Sends the work_string array, which now contains a message about the door's lock status
          }
        }
      }
    }
    else {                                        // If the SMS did not contain a good password, we will send it to the controller.
      Serial.println(F("Incorrect password"));
      //char tempLine[25];
      //byte i = 25;                              // How many characters we will copy into the new variable
      //while ( i-- ) *( tempLine + i ) = *( serial_line + i ); // Copying characters from serial_line variable into a new one.
      //send(text_message.setSensor(SMS_CHILD_ID).set(tempLine)); wait(RADIO_DELAY);
#ifdef ALLOW_CONNECTING_TO_NETWORK
      if( transmission_state ){
        serial_line[24] = '\0';
        send(text_message.setSensor(SMS_CHILD_ID).set(serial_line)); wait(RADIO_DELAY);
      }
#endif
    }
    Serial.println(F("Deleting SMS message"));
    modem.println(F("AT+CMGD=1,4"));        // Delete all received SMS messages (for A6 based GSM modem).
  }
  else {
    
    //bool connectedToNetwork = strncmp(serial_line, "+CREG: 5", 8);
    //if (connectedToNetwork == 0){
    if( !strncmp(serial_line, "+CREG: 5", 8) ){    // Connected to the GSM network
      Serial.println(F("GSM CONNECTED"));
#ifdef ALLOW_CONNECTING_TO_NETWORK
      send(text_message.setSensor(DEVICE_STATUS_ID).set( F("GSM OK"))); wait(RADIO_DELAY);
#endif
    }

    else if( !strncmp(serial_line, "+CMS ERR", 8) ){    // Counter intuitively, if the string matches, the result is 0. In this case, the modem tells us there is an error. After an attempt to send an SMS, this could indicate a lack of network connection or lack of funds.
      Serial.println(F("GSM ERROR (no network or funds?)"));
#ifdef ALLOW_CONNECTING_TO_NETWORK
        if( transmission_state ){
        serial_line[24] = '\0';
        Serial.println(serial_line);
        send(text_message.setSensor(SMS_CHILD_ID).set(serial_line)); wait(RADIO_DELAY);
        wait(1000);
        send(text_message.setSensor(DEVICE_STATUS_ID).set( F("GSM network error (CMS)") )); wait(RADIO_DELAY);
        wait(2000);
        send(text_message.setSensor(DEVICE_STATUS_ID).set( F("Check funds or phone #1") )); wait(RADIO_DELAY);
      }
#endif
    }

    else if( !strncmp(serial_line, "+CME ERR", 8) ){    // This error could occus if a user sends an incorrect code to their mobile provider
      Serial.println(F("GSM ERROR (incorrect code sent?)"));
#ifdef ALLOW_CONNECTING_TO_NETWORK
      if( transmission_state ){
        serial_line[24] = '\0';
        send(text_message.setSensor(SMS_CHILD_ID).set(serial_line)); wait(RADIO_DELAY);
        //send(text_message.setSensor(DEVICE_STATUS_ID).set( F("GSM error (CME)") )); wait(RADIO_DELAY);
        //wait(1000);
      }
#endif
    }

    // We can send special commands to the modem. The response is encoded in a 7 bit instead of 8 bit way, and needs to be decoded.
    else if( !strncmp(serial_line, "+CUSD:", 6) ){ // We received the response from the *# command.

      Serial.println(F("Decoding")); // Serial.println(serial_line);
      
      byte commandPos = 11;                       // 11 is the position in the character array where the returned command actually starts.
      byte savePos = 0;                           // The position in the array where we are storing the new value.
      //static byte shift = 0;                      // How many of the bits must be shifted over to the next byte. Since are are handling 8 bits but only need 7, we keep having one bit left over. Once there is a need to shift 7 bits, that means we have the extra 'bonus' character.
      byte shift = 0;                      // How many of the bits must be shifted over to the next byte. Since are are handling 8 bits but only need 7, we keep having one bit left over. Once there is a need to shift 7 bits, that means we have the extra 'bonus' character.

      //Serial.print(F("shift = ")); Serial.println(shift);
      byte nextLeftOver = 0;                      // The leftover bits from the current translation
      byte prevLeftOver = 0;                      // The leftover bits from the previous translation.


      //char value = 49
      while ( commandPos < SERIAL_LINE_LENGTH ){ //sizeof(serial_line) ){ // was 255 // Alternatively, the reserved size of the serial_line array is simply SERIAL_LINE_LENGTH
        
        prevLeftOver = nextLeftOver;
        nextLeftOver = 0;

        boolean should_send_line = false;
        char value = serial_line[commandPos]; // Get a character from the serial data.
        if( value == 0 ){
          //Serial.println(F("found NULL")); // Shouldn't really happen?
          //should_send_line = true;
          break;
        }
        
        //Serial.print(commandPos); Serial.print(F("=")); Serial.write(value); Serial.println(F(" "));

        if( savePos == 24 || value == 34 || value == 0 ){
        //if( should_send_line ){
          work_string[savePos] = '\0';
          savePos = 0;                                // Set the postion of the write head back to 0.
#ifdef DEBUG
          Serial.println(work_string);
#endif
#ifdef ALLOW_CONNECTING_TO_NETWORK
          if( transmission_state ){
            send(text_message.setSensor(DEVICE_STATUS_ID).set( work_string )); wait(RADIO_DELAY);
          }
#endif
          wait(2000);                                 // Allows the user to read the response more easily if it has multiple lines.
        }
        if( value == 34 ){       // We're at the end of the serial_line.
          //Serial.println(F("found closing \""));
          break;
        }

        // Here we safeguard the bits we will need in the next round by copying them into a new variable.
        for( byte j = 0; j <= shift; j++ ){
          bitWrite( nextLeftOver, j, bitRead(value,(7-shift) + j) ); 
        }
        
        value = value << shift;                   // Scoot everything over. The bits that get destroyed have already been safeguarded.    
        value = value | prevLeftOver;             // overlay the previous left over bits. This will create the final character.
        //Serial.print(">"); Serial.println(value);
        bitWrite( value, 7, 0 );                  // Force it to be a 7-bit character by turning the 8th bit to 0. 
        //Serial.print(">"); Serial.print( (char) value);
        //Serial.print(">"); Serial.print( (char) value); Serial.print(">"); Serial.println(value);
        work_string[savePos] = value;              // Store the final decoded value back in the original serial_line array (to save memory).
        savePos++;
        
        shift++;
        if( shift == 7 ){
          //Serial.print( (char) nextLeftOver);   // Once every 7 loops the left-over part will itself be 7 bits long. We can just print this as the 7th character.
          work_string[savePos] = (char) nextLeftOver; //value;              // Store the final decoded value back in the original array.
          savePos++;
          nextLeftOver = 0;
          shift = 0;
        }
        commandPos++;
      }
      Serial.println();
    }

    else if( !strncmp(serial_line, "+CMT:", 4) ){  // Are we at the part of the SMS that holds the sender's phonenumber?
#ifdef DEBUG
      Serial.println(F("+CMT found"));
      // Can we find a phonenumber in the serial line?
      Serial.print(F("phone#1 to check against = ")); Serial.println(phone1);
      Serial.print(F("phone#2 to check against = ")); Serial.println(phone2);
#endif
      
      char * p;
      char * q;
      //byte p;
      //byte q;      
      p = strstr (serial_line, phone1);            // At what position in the serial_line have we spotted the phone number? If it is 0 this means it has not been found.
      q = strstr (serial_line, phone2);
      if (p || q) {                               // If one of the phone numbers was found in the string, then one of these will be bigger than 0.
        Serial.println(F("Phone number is allowed"));
        incomingSMS = 1;                          // We may now proces the sms content, which should be on the next serial line.
#ifdef DEBUG
        Serial.print(F("Buffer still available: ")); Serial.println( modem.available() );
#endif
        return;

        //char foundNumber[15];
        //memcpy( foundNumber, &serial_line[8], 10 ); // Starts at position 10 in the serial_line and then copies 12 characters
      }
      else {
        Serial.println(F("Permission denied for: ")); // Neither allowed phone number was detected. Fun fact: if the two phone number arrays to compare against are "" (empty), the above should also trigger, which means any phone number may control the device if no phone numbers are set.
        char foundNumber[14];                     // Holds the phone number that sent the SMS
        for (byte i = 8; i < 20; i++) {
          char character = serial_line[i];
          if( isDigit(character) ){
            Serial.write(character);
            foundNumber[i - 8] = character;
            foundNumber[i - 7] = '\n';
          }
        }
        Serial.println();
        // Send the unexpected phone number:
#ifdef ALLOW_CONNECTING_TO_NETWORK
        send(text_message.setSensor(DEVICE_STATUS_ID).set(F("Unknown phone number!"))); wait(RADIO_DELAY);
        send(text_message.setSensor(SMS_CHILD_ID).set( foundNumber )); wait(RADIO_DELAY); // Tell the controller which phone number just sent us an SMS.
#endif
      }
    }
  }
}

#ifdef ALLOW_CONNECTING_TO_NETWORK
// This function is called when messages from the home controller arrive.
void receive(const MyMessage &message)
{
  //timeOutCount = 0;
#ifdef DEBUG
  Serial.print(F("->receiving message for child ")); Serial.println(message.sensor);
  Serial.print(F("->message type: ")); Serial.println(message.type);
#endif

  if( message.isAck() ){
#ifdef DEBUG
    Serial.println(F("- Echo"));
#endif
  }
  else if( message.type == V_LOCK_STATUS ){       // Change lock state  
    desired_door_states[message.sensor - RELAY1_CHILD_ID] = message.getBool()?RELAY_LOCKED:RELAY_UNLOCKED;
    Serial.print(F("Controller -> door ")); Serial.print(message.sensor - RELAY1_CHILD_ID); Serial.print(F(" -> ")); Serial.println(desired_door_states[message.sensor - RELAY1_CHILD_ID]);

    if( !transmission_state ){ // We only echo this state back to the controller straight from the receive function if the main loop won't update the controller because the data transmission is disabled.
      send(lock_message.setSensor(message.sensor).set( message.getBool() )); // Tell the controller that the value was received.
    }
  }
  else if( message.type == V_STATUS ){
    
    send(relay_message.setSensor(message.sensor).set( message.getBool() ));
    
    if( message.sensor == TRANSMISSION_STATE_CHILD_ID ){  // Data transmission toggle  
      desired_transmission_state = message.getBool(); //?RELAY_ON:RELAY_OFF;
      //send(relay_message.setSensor(TRANSMISSION_STATE_CHILD_ID).set(desired_transmission_state));
      Serial.print(F("-New desired transmission state: ")); Serial.println(desired_transmission_state);
    }
    else if( message.sensor == SMS_CONTROL_ID ){    // SMS control toggle
      desired_sms_control_state = message.getBool(); //?RELAY_ON:RELAY_OFF;
      //send(relay_message.setSensor(SMS_CONTROL_ID).set(desired_sms_control_state));
      Serial.print(F("-New desired sms control state: ")); Serial.println(desired_sms_control_state);
    }
    else{
      desired_door_states[message.sensor - RELAY1_CHILD_ID] = message.getBool()?RELAY_LOCKED:RELAY_UNLOCKED;
      Serial.print(F("Controller -> door ")); Serial.print(message.sensor - RELAY1_CHILD_ID); Serial.print(F(" -> ")); Serial.println(desired_door_states[message.sensor - RELAY1_CHILD_ID]);
      if( !transmission_state ){ // We only echo this state back to the controller straight from the receive function if the main loop won't update the controller because the data transmission is disabled.
        send(lock_message.setSensor(message.sensor).set( message.getBool() )); // Tell the controller that the value was received.
      }
    }
  }
  else if( message.type == V_TEXT ){

    Serial.print(F("- incoming string: "));Serial.println(message.getString());
    send(text_message.setSensor(message.sensor).set( message.getString() )); // Tell the controller that the value was received.


    // If the controller asks us to send an SMS.
    if( message.sensor == SENDSMS_CHILD_ID ){
      //strcpy(smsToSend, message.getString());
      strcpy(work_string, message.getString());
      send_sms = true;
      //send(text_message.setSensor(DEVICE_STATUS_ID).set( F("Sending SMS...")));
    }

    if( message.sensor == ROTATING_PASSWORD1_ID ){
      if( rotatingPassword1 != message.getString() ){
        strcpy(rotatingPassword1, message.getString());
        Serial.print(F("Rotating password 1 is now: ")); Serial.println(rotatingPassword1);
        store_to_eeprom = true; 
        //send(text_message.setSensor(DEVICE_STATUS_ID).set( F("received latest password 1"))); wait(RADIO_DELAY);
      }
    }
    else if( message.sensor == ROTATING_PASSWORD2_ID ){
      if( rotatingPassword2 != message.getString() ){
        strcpy( rotatingPassword2, message.getString());
        Serial.print(F("Rotating password 2 is now: ")); Serial.println(rotatingPassword2);
        store_to_eeprom = true; 
        //send(text_message.setSensor(DEVICE_STATUS_ID).set( F("received latest password 2"))); wait(RADIO_DELAY);
      }
    }
    
    // Receiving the (parts of) phonenumbers that are allowed to operate the smart lock.
    else if( message.sensor == PHONENUMBER1_ID ){
      strcpy(phone1, message.getString());
      store_to_eeprom = true;
      Serial.println(F("received phone #1 ")); 
#ifdef DEBUG
      Serial.print(F("phone1: ")); Serial.println(phone1);
#endif
    }
    else if( message.sensor == PHONENUMBER2_ID ){
      //if( sizeof(message.getString()) > 2 ){
      strcpy(phone2, message.getString());
      store_to_eeprom = true; 
      //}
      Serial.println(F("received phone #2 "));
#ifdef DEBUG
      Serial.print(F("phone2: ")); Serial.println(phone2);
#endif
    }
    else{
      Serial.println(F("Incoming message fell through"));  
    }
    // If could also be that the Status and Received SMS string are sent back. Those should just be ignored.
  }
}
#endif // connecting to network




// Function to send an SMS to the main phone number.
void sendSMS()
{

  boolean placeholder = strstr(work_string, "Change me to send SMS");
  
  if( modem_booted && modem_state == MODEM_BUFFER_EMPTY && placeholder == NULL){
#ifdef DEBUG
    Serial.print(F("In sendSMS. string = ")); Serial.println(work_string);
    Serial.println(placeholder);
#endif

    //boolean actual_new_SMS_message = strstr(work_string, "Change me to send SMS");          // Search for the index
    //if (actual_new_SMS_message == NULL){                        // If successful then command now points to the second word in the SMS
  
    byte i = 0;
    char c = 49;
    
    if( work_string[0] == 42 ){       // Detect is the SMS to send starts with a *. If so, it will be parsed as command to the telephony provider. For example, with LycaMobile the code *101# will tell you your balance, and *102# will tell you your phone number.
      Serial.println(F("starts with *"));

      boolean contains_hash_character = false;
      while( c != 35 && i < WORK_STRING_SIZE ){
        if( work_string[i] == 35 ){                    // ascii character 35 is the #
          //Serial.println(F("#"));
          contains_hash_character = true;             // We only send the command if the string contains a closing # too.
        }
        i++;
      }
      
      if( contains_hash_character ){
        modem_state = WAITING_FOR_RESPONSE_FROM_MODEM;
#ifdef ALLOW_CONNECTING_TO_NETWORK
        send(text_message.setSensor(DEVICE_STATUS_ID).set( F("...") )); wait(RADIO_DELAY);
#endif
        //while( modem.available() ){ // The response may be large, so in this case we clear the existing buffer.
        //  modem.read();
        //}
        modem.print(F("AT+CUSD=1,\""));
        i = 0;
        c = 49;
        while( c != 35 && i < WORK_STRING_SIZE ){     // While we're not at the closing # in the command yet, we print the characters of the command
          c = work_string[i];                          // Could this step be skipped, and just print work_string[i] directly?
          //Serial.println(c);
          modem.write(c);
          i++;
        }
        modem.println(F("\",15"));              // Send a request for data to your provider, such as your prepaid balance or your phone number.
        modem_state = WAITING_FOR_RESPONSE_FROM_MODEM;
      }
    }
    
    else if( (work_string[0] == 43 || work_string[0] == 48) && isDigit(work_string[1]) && isDigit(work_string[2]) ){ //&& work_string[1] > 47 && work_string[1] < 58){ // The numbers 43 and 48 are the ascii codes for + and 0 respectively
      Serial.println(F("String started with 0 or +")); // We can only send an SMS is the SMS starts with a phone number.
      modem_state = WAITING_FOR_RESPONSE_FROM_MODEM;
      modem.print(F("AT+CMGS=\""));
      i = 0;
      c = 49;
      boolean at_actual_content = false;
      while( c != 0 && i < WORK_STRING_SIZE ){
        c = work_string[i];
        /*
        if( at_actual_content == false && c > 57){    // If we detect an alphabetical letter, perhaps the user has forgotten to use a space after the phone number. Here we add it manually.
          Serial.println(F("User forgot space?"));
          c = " ";
          i--;
        }
        */
        if( c == 32 && at_actual_content == false ){ // 32 is the ascii code for a space
          at_actual_content = true;
          modem.print(F("\"\r"));
          Serial.print(F("]Space["));
          wait(500);
        }
        
        modem.write(c);
        Serial.write(c);
        i++;
      }
      if( at_actual_content == false ){ // The string never contained a space / sms text, so we use a default one.
        modem.print(F("\"\r"));
        wait(500);
        modem.print(F("Smart lock says hello"));
      }
      //modem.write(0x1A); // The command to actually send away the SMS.
      modem.println( char(26) );
      modem_state = WAITING_FOR_RESPONSE_FROM_MODEM;
      Serial.println(F("]SMS sent"));
#ifdef ALLOW_CONNECTING_TO_NETWORK
      send(text_message.setSensor(DEVICE_STATUS_ID).set( F("SMS passed to modem") )); wait(RADIO_DELAY);
#endif
    }
    
    else if( phone1[0] == 43 || phone1[0] == 48 ){ // The numbers 43 and 48 are the ascii codes for + and 0 respectively
      Serial.println(F("Using phone #1"));

      modem.print(F("AT+CMGS=\"")); modem.print(phone1); modem.println(F("\"\r"));
      wait(200);
      //modem.print("test message from A6");

      i = 0;
      c = 49;
      while( c != 0 && i < WORK_STRING_SIZE ){
        c = work_string[i];
        modem.write(c);
        Serial.write(c);
        i++;
      }
      
      modem.println (char(26)); // ctrl-z
      /*
      modem.print(F("AT+CMGS=\""));
      i = 0;
      c = 49;
      boolean at_actual_content = false;
      while( c != 0 && i < sizeof(phone1) ){
        c = phone1[i];
        modem.write(c);
        Serial.write(c);
        i++;
      }
      modem.println(F("\"\r"));
      Serial.print(F("]Space["));
      */
      /*
      wait(200);
      i = 0;
      c = 49;
      while( c != 0 && i < WORK_STRING_SIZE ){
        c = work_string[i];
        modem.write(c);
        Serial.write(c);
        i++;
      }
      modem.write(0x1A); // The command to actually send away the SMS.
      */
      Serial.println(F("]SMS sent to phone1"));
      modem_state = WAITING_FOR_RESPONSE_FROM_MODEM;
#ifdef ALLOW_CONNECTING_TO_NETWORK
      send(text_message.setSensor(DEVICE_STATUS_ID).set( F("SMS sent to phone 1"))); wait(RADIO_DELAY);
#endif
    }
    else{
      Serial.println(F("No good phone number set, so cannot send SMS"));
#ifdef ALLOW_CONNECTING_TO_NETWORK
      if( transmission_state ){
        send(text_message.setSensor(DEVICE_STATUS_ID).set( F("No phone number 1"))); wait(RADIO_DELAY);
      }
#endif
    }

#ifdef ALLOW_CONNECTING_TO_NETWORK
      send(text_message.setSensor(SENDSMS_CHILD_ID).set( F("Change me to send SMS"))); //wait(RADIO_DELAY);
#endif

  wait(1000);
    
    
  }
#ifdef DEBUG
  else{
    Serial.println(F("Will/can not send this SMS"));
  }
#endif
}









#ifdef JESSE
boolean motor_switch(boolean destination) {
  Serial.print(F("Motor switching to ")); Serial.println(destination);
  //Serial.println(F("This list of measurements should start with 1 and end with 0"));

  digitalWrite(MOTOR_FORWARD_PIN, !destination);
  digitalWrite(MOTOR_BACKWARD_PIN, destination);

  boolean error = false;
  byte counter = 0;
  if(destination == false){                       // Moving to on state, in which the bototm switch is 1 and the top switch is 0.
    while( digitalRead(BOTTOM_MOTOR_SWITCH_PIN) ){ // While the bottom switch reads 1 it is not being pressed. When it is pressed, it will go to 0, and exit this loop.
      wait(3);
      Serial.println(digitalRead(BOTTOM_MOTOR_SWITCH_PIN));
      counter++;
      if(counter > 100){                          // This is a safety feature to exit the loop if something strange is going on.
        Serial.print(F("break"));
        error = true;
        break;
      }
    }
  }
  else{
    while( digitalRead(TOP_MOTOR_SWITCH_PIN) ){
      wait(3);
      Serial.println(digitalRead(TOP_MOTOR_SWITCH_PIN));
      counter++;
      if(counter > 100){
        error = true;
        Serial.print(F("break"));
        break;
      }
    }
  }
  digitalWrite(MOTOR_FORWARD_PIN, LOW);
  digitalWrite(MOTOR_BACKWARD_PIN, LOW);
  
  if( error == false ){
    //EEPROM.update(EEPROM_STORAGE_START + TRANSMISSION_STATE_CHILD_ID, transmission_state);   // We remember the current state of the toggle switch.
    Serial.println(F("WENT OK"));
    return destination;
    Serial.print(F("-Motor switched. New transmission state: ")); Serial.println(transmission_state);
  }
  else{
    Serial.println(F("ERROR motor swiching to the new position got stuck in an endless loop"));
    wait(400); // Give the motor some time to cool down ever so slightly
    Serial.println(F("ERROR motor swiching to the new position got stuck in an endless loop cooler done"));
    return !digitalRead(TOP_MOTOR_SWITCH_PIN);    // If the top switch (which is the main one that determines the transmission state) is in the on state, the top switch will read as 0, so we invert it.
  }
}
#endif // Jesse




void storeToEeprom(char TheArray[], int n, int eeprom_starting_position)       // Currently not used, but could be used to improve the code
{
  //Serial.println(F("storing to eeprom: "));
  char c = 49; // "1", a random non-null character
  for ( int i = 0; i < n; i++ ){
    if( c != '\0'){           // After we find the zero that terminiates the string, we keep writing zeros to the rest of the eeprom storage location.
      c = TheArray[i];
    }
    //Serial.print(i); Serial.print(F("=")); Serial.println(c);
    EEPROM.update(eeprom_starting_position + i, c);
  }
}
      
      
void loadFromEeprom(char TheArray[], int n, int eeprom_starting_position)
{
  //Serial.println(F("loading from eeprom: "));
  for ( int i = 0; i < n; i++ ){
    TheArray[i] = EEPROM.read(eeprom_starting_position + i);
    //Serial.print(i); Serial.print(F("=")); Serial.println(TheArray[i]);
  }
}

void storeEverythingToEeprom(){
#ifdef DEBUG
  Serial.println(F("{{ Storing everything to eeprom"));
#endif
  EEPROM.update(EEPROM_STORAGE_START + 255,1);           // Indicates that the eeprom now contains user data.
  storeToEeprom(rotatingPassword1, 26, EEPROM_STORAGE_START + ROTATING_PASSWORD1_ID);
  storeToEeprom(rotatingPassword2, 26, EEPROM_STORAGE_START + ROTATING_PASSWORD2_ID);
  storeToEeprom(phone1, 15, EEPROM_STORAGE_START + PHONENUMBER1_ID); // To be more future proof, we store the phone numbers a little higher than the passwords so that we could store 4 passwords (at positions 10, 36, 52 87), and then 4 phone numbers (120, 135, 150, 165)
  storeToEeprom(phone2, 15, EEPROM_STORAGE_START + PHONENUMBER2_ID);  
}


void loadEverythingFromEeprom(){
#ifdef DEBUG
  Serial.println(F("}} Loading everything from eeprom"));
#endif
  loadFromEeprom(rotatingPassword1, 26, EEPROM_STORAGE_START + ROTATING_PASSWORD1_ID);
  loadFromEeprom(rotatingPassword2, 26, EEPROM_STORAGE_START + ROTATING_PASSWORD2_ID);
  loadFromEeprom(phone1, 15, EEPROM_STORAGE_START + PHONENUMBER1_ID);
  loadFromEeprom(phone2, 15, EEPROM_STORAGE_START + PHONENUMBER2_ID);
}












/*
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
