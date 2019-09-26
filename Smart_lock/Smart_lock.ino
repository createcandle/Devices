 /*
 * DESCRIPTION
 * 
 * This device can toggle two relays. These relays can be controlled via the Candle Controller, but also via SMS.
 * The SMS function is password protected: you have to send a password to switch the relay.
 * 
 * If you send an SMS without the password, it will be shown as the Incoming SMS. This way you can get some insight, and use this to automate other things in your home. For example, you could create a rule that when the system receices an SMS with "purifier on" in it, the air purifier will turn on.
 * 
 * This way you can control smart devices in your home even if it/Candle is not connected to the internet.
 * 
 * SETTINGS */ 
//#define HAS_BUTTONS                               // Have you attached buttons? You can toggle the state of the lock with a push button if you want. If you want this, even if you only want to toggle one door, you have to attach two buttons, one for each door.

// It's important to change these to your own phone number(s). When the device starts up it will always start with these values.
// You will also be able to change this via your controller later, but in case no connection to the controller can be established it's a good idea to enter your phonenumber here.
char phone1[15] = "310612345678";                   // Phone number of user #1. This is the main number to which status updates will be sent.
char phone2[15] = "310612345678";                   // Phone number of user #2 (optional). If there is no second user, set it the same number as user #1.

char rotatingPassword1[26] = "door1";               // Door 1 password. If the device is powered up and can't reconnect to the home network, then this will be the fallback password. Maximum length is 25 characters.
#define DOOR1_SELF_LOCKING                          // Self locking? Should door number one automatically re-lock itself after a short amount of time?
char rotatingPassword2[26] = "door2";               // Door 2 password. If the device is powered up and can't reconnect to the home network, then this will be the fallback password. Maximum length is 25 characters.
//#define DOOR2_SELF_LOCKING                        // Self locking? Should door number two automatically re-lock itself after a short amount of time?

#define SELF_LOCKING_DELAY 5                        // Self locking delay. If a door is self-locking, how many seconds before it should re-lock?


#define APN_URL "data.lycamobile.nl"                // The APN URL from your mobile provider.
#define APN_USERNAME "lmnl"                         // The APN username from your mobile provider.
#define APN_PASSWORD "plus"                         // The APN password from your moble provider.
#define CUSD_COMMAND "*102#"                        // Extra command. This is optional. For example, you can send a code to your provider asking what your prepaid balance is, or asking what your phone number is.

//#define SEND_SMS_EVERY_49_DAYS                      // Send SMS every 49 days. If you are using a prepaid simcard, you may have to send an SMS once in a while to stop your simcard from getting disabled. This function tries sending an SMS to you once every 49 days.

#define RF_NANO                                     // RF-Nano. Check this box if you are using the RF-Nano Arduino, which has a built in radio. The Candle project uses the RF-Nano.


/* END OF SETTINGS
 *  
 *  
 *  
 * _Possible improvements_
 * - for increased safety you could limit which phonenumbers are even allowed to send commands. This could also be a comma-separated text value on the controller.
 * - currently both relays share the password, and the second relay just has a '2' added to the password. This could easily be separated out. // Is this still true?
 * - Allow only one button to be attached, despite having two locks.
 * - Store password in eeprom? Less secure, but could make it more useable.
 * 
 * 
 * Lots of useful commands: https://github.com/stephaneAG/SIM800L/blob/master/README.md
 * 
 * Usefel A6 GSM modem page: http://makerangst.com/a6-gsm-gprs-module-send-an-sms
 *  
 *  
 *  TO DO:
 *  - deal with
 *  
 *  MODEM SAYS: AT
 *
 * MODEM SAYS: AT
 *
 * MODEM SAYS: AT
 *
 * MODEM SAYS: AT&F
 *
 * MODEM SAYS: 
 * 
 * MODEM SAYS: COMMAND NO RESPONSE!
 * 
 * --> reset?
 *  
*/

//#define DEBUG                                       // Display debug information in the serial output.
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

// Mysensors security
//#define DEBUG_SIGNING
#define MY_ENCRYPTION_SIMPLE_PASSWD "changeme"        // Be aware, the length of the password has an effect on memory use.
//#define MY_SECURITY_SIMPLE_PASSWD "changeme"        // Be aware, the length of the password has an effect on memory use.
//#define MY_SIGNING_SOFT_RANDOMSEED_PIN A7           // Setting a pin to pickup random electromagnetic noise helps make encryption more secure.

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
//#define MY_REPEATER_FEATURE                       // Add or remove the two slashes at the beginning of this line to select if you want this sensor to act as a repeater for other sensors. If this node is on battery power, you probably shouldn't enable this.

// Define Node ID
//#define MY_NODE_ID 15
//#define MY_PARENT_NODE_ID 0
//#define MY_PARENT_NODE_IS_STATIC


// PINS
#define GSM_RECEIVE_PIN 3                           // Connect to U_TXD on the GSM module
#define GSM_TRANSMIT_PIN 4                          // Connect to U_RXD on the GSM module
#define RELAY1_PIN 5                                // Relay 1 pin number (use this on a door that needs a short pulse to open).
#define RELAY2_PIN 6                                // Relay 2 pin number (use this on a door that can be set to locked or unlocked mode, for example with a solenoid.).
#define BUTTON1_PIN A0                              // Manual button for door 1. Also known as A0
#define BUTTON2_PIN A1                              // Manual button for door 2. Also known as A1

#ifdef RF_NANO
// If you are using an RF-Nano, you have to switch CE and CS pins.
#define MY_RF24_CS_PIN 9                            // Used by the MySensors library.
#define MY_RF24_CE_PIN 10                           // Used by the MySensors library.
#endif


// Allow the smart lock to send you an SMS once every 49 days? It's better to let your controller trigger this, but it's a nice option to have.
// #define SEND_SMS_EVERY_49_DAYS                   //  This can help keep the simcard 'alive'. The first sms will be sent a week after the smart lock powers on.

// THESE VALUES CAN BE CHANGED

#define LOOPDURATION 4000                           // The main loop runs every x milliseconds. This main loop starts the modem, and from then on periodically requests the password.
#define LOOPSBETWEENPASSWORDREQUESTS 45             // 45 * 4seconds = 3 minutes. So every 3 minute the node asks if the password is still the same.
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
#define _SS_MAX_RX_BUFF 256                         // The size of the software serial buffer
#define SERIAL_LINE_LENGTH 48                       // The maximum length that a line from the GSM modem can be.
#include <SoftwareSerial.h>                         // A software library for serial communication. In this case we use it so talk to the SIM800L GSM modem.
SoftwareSerial softSerial1(GSM_RECEIVE_PIN,GSM_TRANSMIT_PIN); // Receive pin (RX), transmit pin (TX)
char serialLine[SERIAL_LINE_LENGTH];                // Holds the line received from the modem.

// MySensors children
#define DEVICE_STATUS_ID 0                          // The first 'child' of this device is a text field that contains status updates.
#define SMS_CHILD_ID 1                              // This field is used to tell the controller what text was in the sms. Useful for automation of other things that you want to trigger using an SMS.
#define SENDSMS_CHILD_ID 2                          // If this button is pressed at the controller, a test sms will be sent. Useful to keep a simcard alive (must send at least one SMS every X months usually).
#define BUTTON_CHILD_ID 3                           // Set button ID number on this node.

#define RELAY1_CHILD_ID 10                          // Set switch ID number on this node.
#define RELAY2_CHILD_ID 11                          // Set switch ID number on this node.

#define ROTATING_PASSWORD1_ID 20                    // Set password ID number on this node that can be set from outside.
#define ROTATING_PASSWORD2_ID 21                    // Set password ID number on this node that can be set from outside.

#define PHONENUMBER1_ID 30                          // If (part of) a phonenumber is given, only phonenumbers that have that part will be allowed to open the lock. E.g. "+31" only allows Dutch numbers. "+3161482375" would only allow that one number.
#define PHONENUMBER2_ID 31                          // If (part of) a phonenumber is given, only phonenumbers that have that part will be allowed to open the lock. E.g. "+31" only allows Dutch numbers. "+3161482375" would only allow that one number.

MyMessage charmsg(DEVICE_STATUS_ID, V_TEXT);        // Sets up the message format that we'll be sending to the MySensors gateway later. The first part is the ID of the specific sensor module on this node. The second part tells the gateway what kind of data to expect.
MyMessage relaymsg(RELAY1_CHILD_ID, V_LOCK_STATUS); // EXPERIMENT, try V_TRIPPED or V_STATUS or V_LOCK_STATUS  // is used by the relays and the toggle to send their status to Domoticz.

// General variables
#define DOOR_COUNT 2                                // How many electric locks are attached.
#define RF_DELAY 100                                // Milliseconds delay betweeen radio signals. This gives the radio some breathing room.
byte timeOutCount = 0;                              // How often did we fail to the password after we requested it? If it's too often, then the connection is down.
boolean incomingSMS = 0;                            // Used in flow control when processing an SMS.
boolean waitingForResponse = false;                 // Used to check if the connection to the controller is ok. If we are still waiting by the next time a password is requested, something is fishy.
boolean send_all_values = 1;
boolean processing_line = 0;

boolean desiredDoorStates[DOOR_COUNT];              // An array that holds the desired door states. These are turned into:
boolean actualDoorStates[DOOR_COUNT];               // An array that holds the actual door states.

const char unlockedMessage[] PROGMEM = { "  unlocked" }; // This construction saves some memory. Only the first character has to be changed to create a message to send via SMS, such as "1 locked" or "2 unlocked".s
const char lockedMessage[] PROGMEM = { "  locked" }; // This construction saves some memory.
const char passwordMessage[] PROGMEM = { "Door   password" }; // This construction saves some memory.

#define WORK_STRING_SIZE 16                         // Length of an array that holds various strings.

// A small funtion that can tell you how much ram is left.
int freeRam () {
  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}
// via https://playground.arduino.cc/Code/AvailableMemory


// Function to send an SMS to the main phone number.
void sendSMS(char smsToSend[26])
{
  //Serial.print(F("ABOUT TO SEND: ")); Serial.println(smsToSend);
  if(sizeof(phone1) > 9){                           // Check if we have received a real phone number to send the SMS to.
    Serial.println(F("Sending sms"));
    softSerial1.print(F("AT+CMGS=\""));
    softSerial1.print(phone1);                      
    softSerial1.print(F("\"\r"));
    wait(100);
    softSerial1.print(smsToSend);
    softSerial1.write(0x1A);
    //send(charmsg.setSensor(DEVICE_STATUS_ID).set( F("Sent an SMS"))); wait(RF_DELAY);  
  }
  else {
    Serial.println(F("Cannot send SMS: no phone number set"));
    send(charmsg.setSensor(DEVICE_STATUS_ID).set( F("No phone number set yet!"))); wait(RF_DELAY);
  }
}


// The before function is run very early on. This makes it great for quickly changing pin settings.
void before()
{

  memset(desiredDoorStates,0,sizeof(desiredDoorStates));
  memset(actualDoorStates,0,sizeof(actualDoorStates));

  pinMode(BUTTON1_PIN, INPUT_PULLUP);
  pinMode(BUTTON2_PIN, INPUT_PULLUP);

  for( byte checkingDoor = 0; checkingDoor < DOOR_COUNT; checkingDoor++ ){
    //Serial.print(F("BUTTON1_PIN + checkingDoor = ")); Serial.println(BUTTON1_PIN + checkingDoor);
    pinMode(RELAY1_PIN + checkingDoor, OUTPUT);     // Set all relay pins to be output.
     // Set the button pins as input.
    //pinMode(BUTTON1_PIN + checkingDoor, INPUT); // Set the button pins as input.
    char possibleDesiredState = EEPROM.read(EEPROM_STORAGE_START + checkingDoor);
    
    if( possibleDesiredState == 0 || possibleDesiredState == 1 ){ // If the eeprom has the number '255' here, then it clearly hasn't been set to hold a desired value before, and we should not use it.
      //Serial.println("Found useable state in eeprom");
      desiredDoorStates[checkingDoor] = (boolean) possibleDesiredState;
      
      if( possibleDesiredState == LOCKED){          // This may seem complicated. The numerical state that represents 'locked' in the interface could be different from what the relay conciders locked. For example, 1 could be locked in the interface, but unlocked for the relay. And vice-versa. This construction deals with that possibility.
        digitalWrite(RELAY1_PIN + checkingDoor, RELAY_LOCKED);
      }
      if( possibleDesiredState == UNLOCKED){
        digitalWrite(RELAY1_PIN + checkingDoor, RELAY_UNLOCKED);
      }
      
      actualDoorStates[checkingDoor] = desiredDoorStates[checkingDoor];

    }
    else {
#ifdef DEBUG
      Serial.println("No useable state found in eeprom");
#endif
      digitalWrite(RELAY1_PIN + checkingDoor, RELAY_UNLOCKED); // If no value was stored in the eeprom, start unlocked, for safety.
    }
  }
  //digitalWrite(LED_PIN, HIGH);  
}


void presentation()
{
  sendSketchInfo(F("Smart lock"), F("1.7")); wait(RF_DELAY);

  present(DEVICE_STATUS_ID, S_INFO, F("Status")); wait(RF_DELAY);
  present(SMS_CHILD_ID, S_INFO, F("Received SMS")); wait(RF_DELAY);
  present(SENDSMS_CHILD_ID, S_INFO, F("Change to send SMS")); wait(RF_DELAY);
  present(PHONENUMBER1_ID, S_INFO, F("Phone number 1")); wait(RF_DELAY);
  present(PHONENUMBER2_ID, S_INFO, F("Phone number 2")); wait(RF_DELAY);

  // Presenting the password fields
  char workString[WORK_STRING_SIZE];                // Creates an array that will hold various strings.

  for( byte i = 0; i < DOOR_COUNT; i++ ){           // We loop over all the doorlocks in the system, and check if they are in the right position.
    memset(workString,0,sizeof(workString));
    strcpy_P(workString, passwordMessage);            // This copies the password message that we stored in progmem (flash memory) into real memory.
    
    workString[5] = i + 49;                         // This will create "Door 1 password", "Door 2 password", etc. In that string the number is the 5th character (starting form 0). Ascii character 49 = "1', character 50 = "2", etc.
    //Serial.println(workString);
    present(ROTATING_PASSWORD1_ID + i, S_INFO, workString); wait(RF_DELAY);
    
    workString[6] = '\0'; // Shortens the string to just be "Door 1", "Door 2", etc.
    //Serial.println(workString);
    present(RELAY1_CHILD_ID + i, S_LOCK, workString, true); wait(RF_DELAY);
  }

  send_all_values = 1;                              // Whenever a new presentation is requested, we should also send the current values of the children.
}

void send_values()
{
#ifdef DEBUG
  Serial.println(F("Sending button states"));
#endif
  send(charmsg.setSensor(DEVICE_STATUS_ID).set( F("Hello")),1); wait(RF_DELAY);  
  send(charmsg.setSensor(SMS_CHILD_ID).set( F("")),1); wait(RF_DELAY);
  send(charmsg.setSensor(SENDSMS_CHILD_ID).set( F("Change me to send SMS")),1); wait(RF_DELAY);
  send(charmsg.setSensor(PHONENUMBER1_ID).set( phone1 ),1); wait(RF_DELAY);
  send(charmsg.setSensor(PHONENUMBER2_ID).set( phone2 ),1); wait(RF_DELAY);

  for( byte i = 0; i < DOOR_COUNT; i++ ){
    send(charmsg.setSensor(ROTATING_PASSWORD1_ID + i).set( F("change_me") )); wait(RF_DELAY);
    send(relaymsg.setSensor(RELAY1_CHILD_ID + i).set( actualDoorStates[i] )); wait(RF_DELAY);
  }
}

void setup()
{
  wait(1000);
  Serial.begin(115200);
  Serial.println(F("Hello world, I am a SMS door lock."));
  
  // Has a connection to the controller been established?
  if(isTransportReady()){
    Serial.println(F("Connected to gateway"));    
  }else{
    Serial.println(F("Not connected to gateway"));
    timeOutCount = MAXIMUMTIMEOUTS;                 // Start the system in 'no connection' mode, meaning the backup passwords may be used.
  }
  
  Serial.println(F("Connecting to GSM.."));
  softSerial1.begin(57600);                         // The modem automatically adapts to work at the same baud rate when we send a few commands.
  softSerial1.println(F("AT"));
  wait(300);
  softSerial1.println(F("AT"));
  wait(300);
  softSerial1.println(F("AT"));
  wait(300);
  
  softSerial1.println(F("AT&F"));                   // Reset the GSM modem.
  wait(15000);                                      // The GSM modem needs some time to settle.

  wdt_enable(WDTO_8S);                              // Starts the watchdog timer. If it is not reset once every few seconds, then the entire device will automatically restart.                                
}



void loop()
{
  static unsigned long lastLoopTime = 0;            // Holds the last time the main loop ran.
  static bool buttonBeingPressed = 0;               // Used to debounce the push buttons.
  static byte number_of_bytes_received = 0;         // How many characters we've stored in the serialLine array.

  // If a presentation is requested, we also send the values of the children.
  if( send_all_values ){
#ifdef DEBUG
    Serial.println(F("RESENDING VALUES"));
#endif
    send_all_values = 0;
    send_values();
  }

  if( softSerial1.available() > 0 ){
    char singleChar;
    //Serial.println(F("."));
    //processing_line = 1;
    
    
    //while( processing_line && softSerial1.available() > 0 ){
    //int maxLengthPossible = softSerial1.available();
    //Serial.print(F("maxLengthPossible:")); Serial.println(maxLengthPossible);
    //while( processing_line ){
    
    while( softSerial1.available() > 0 ){
      singleChar = softSerial1.read();
      Serial.print(singleChar); Serial.print(F("(")); Serial.print(number_of_bytes_received); Serial.print(F("/")); Serial.print(softSerial1.available()); Serial.print(F(")_"));
      if( singleChar == '\n' || softSerial1.available() == 0 ){ //number_of_bytes_received == maxLengthPossible ){
        //serialLine[number_of_bytes_received] = '\0';
        Serial.print(F("MODEM SAYS: ")); Serial.println(serialLine);
        number_of_bytes_received = 0;
        processLine();
        memset(serialLine,0,sizeof(serialLine)); // Set the serialLine back to empty
        if( softSerial1.available() == 0 ){
          incomingSMS = 0; // Just in case
        }

          
        //processing_line = false;
      }
      else if( number_of_bytes_received < SERIAL_LINE_LENGTH ){
        serialLine[number_of_bytes_received] = singleChar;
        serialLine[number_of_bytes_received + 1] = '\0';
        number_of_bytes_received++;
      }
    }
  }



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
    sendSMS("Candle lock says hi");
  }
#endif

  /*
   * // test functions
   * 
    if( (millis() % 4000) == 0 ) { // test function. for easy testing, also comment out the sms-delete line further down.
    //Serial.println("_____");
    while (softSerial1.available())
    softSerial1.read();
    msg = "";
    softSerial1.write("AT+CMGR=1\r\n"); // get first SMS on the phone
    wait(1);
    }

    useful for quick debugging and playing with the GSM modem.
    if(Serial.available()){    
      softSerial1.write(Serial.read());
    }
  */

  //
  // MAIN LOOP
  // runs every few seconds. By counting how often this loop has run (and resetting that counter back to zero after 250 loops), it becomes possible to schedule all kinds of things without using a lot of memory.
  // maximum time that can be scheduled is 4s * 250 loops = 1000 seconds. So the entire things runs approximately every 16 minutes.
  //

  static byte loopCounter = 0;                            // Count how many loops have passed (reset to 0 after at most 254 loops).
  //static boolean loopDone = false;                        // used to make sure the 'once every millisecond' things only run once every millisecond (or 2.. sometimes the millis() function skips a millisecond.);

  // allow the next loop to only run once. This entire construction saves memory by not using a long to store the last time the loop ran.
  //if( (millis() % LOOPDURATION) > LOOPDURATION - 4 && loopDone == true ) {
  //  loopDone = false;  
  //}

  // Main loop to time actions.
  //if( (millis() % LOOPDURATION) < 4 && loopDone == false ) { // this module's approach to measuring the passage of time saves a tiny bit of memory.
    //loopDone = true;
  if (millis() - lastLoopTime > LOOPDURATION) {
    lastLoopTime = millis();
    loopCounter++;

    wdt_reset();                                      // Reset the watchdog timer

    // schedule
    switch (loopCounter) {
      
      case 1:    
        softSerial1.println(F("AT"));               // Initiating the GSM modem
        //request(PHONENUMBER1_ID, V_TEXT); wait(RF_DELAY); // Periodically check which phone numbers are allowed. By requesting this very early we can later on avoid overwriting any values that the user may have set.
        softSerial1.println(F("AT"));
        //request(PHONENUMBER2_ID, V_TEXT); wait(RF_DELAY); // Periodically check which phone numbers are allowed.
        softSerial1.println(F("AT+EGMR=2,7"));
        Serial.println(F("Sending door states"));
        request(RELAY1_CHILD_ID, V_LOCK_STATUS); wait(RF_DELAY);           // Periodically check which phone numbers are allowed. By requesting this very early we can later on avoid overwriting any values that the user may have set.
        request(RELAY2_CHILD_ID, V_LOCK_STATUS); wait(RF_DELAY);           // Periodically check which phone numbers are allowed.
        break;
        
      case 2:
        //Serial.println(F("2"));
        softSerial1.print(F("AT+CSTT=\""));
        softSerial1.print(APN_URL);
        softSerial1.print(F("\",\""));
        softSerial1.print(APN_USERNAME);
        softSerial1.print(F("\",\""));
        softSerial1.print(APN_PASSWORD);
        softSerial1.println(F("\""));
        softSerial1.println(F("AT+COPS=0"));
        //send(relaymsg.setSensor(RELAY1_CHILD_ID).set( actualDoorStates[0] )); wait(RF_DELAY); // Tell the controller in what state the lock is.
        //send(relaymsg.setSensor(RELAY2_CHILD_ID).set( actualDoorStates[1] )); wait(RF_DELAY); // Tell the controller in what state the lock is.
        //request(ROTATING_PASSWORD1_ID, V_TEXT); wait(RF_DELAY);     // Periodically check if there is a new password set.
        //request(ROTATING_PASSWORD2_ID, V_TEXT); wait(RF_DELAY);     // Periodically check if there is a new password set.
        break;
        
      case 3:    
        //Serial.println(F("3"));
        softSerial1.println(F("AT+CGATT=1"));       // AT+CGATT=1 is a GPRS attach command.
        if(strcmp(rotatingPassword1, "door1") == 0 && strcmp(rotatingPassword2, "door2") == 0 ){ // Here we assume the controller has no password set yet (or doesn't support this), so we send it ours.
          Serial.println(F("sending default passwords"));
          send(charmsg.setSensor(ROTATING_PASSWORD1_ID).set(rotatingPassword1)); wait(RF_DELAY);
          send(charmsg.setSensor(ROTATING_PASSWORD2_ID).set(rotatingPassword2)); wait(RF_DELAY);
        }else{
          Serial.println(F("not sending default passwords to controller"));
        }
        break;
        
      case 5:
        softSerial1.println(F("AT+CGATT?"));
        break;
        
      case 7:
        //Serial.println(F("7"));
        softSerial1.println(F("AT+CMGF=1")); // AT+CMGF=1 = sms mode. Get SMS as plain text. 
        wait(300);
        softSerial1.println(F("AT+CSDH=0"));        // CSDH=0 asks the modem to not send full SMS headers.
        break;
        
      case 8:
        //Serial.println(F("8_sms_live_mode"));            
        softSerial1.println(F("AT+CNMI=2,2,0,0,0")); // Listen to incoming messages. 
        break;
        
      case 10:
        //Serial.println(F("10_delete all sms"));
        softSerial1.println(F("AT+CMGD=1,4"));      // Delete all SMS messages stored on the modem.
        break;
        
      case 12:
        //Serial.println(F("12_AT+COPS?"));
        softSerial1.println(F("AT+COPS?"));
        break;
        
      case 13:
        //softSerial1.println(F("AT^STF=1"));
        break;
        
      case 14:
        //Serial.println(F("14_CUSD"));
        softSerial1.print(F("AT+CUSD=1,\""));
        softSerial1.print(CUSD_COMMAND);
        softSerial1.println(F("\",15"));            // Send a request for data to your provider, such as your prepaid balance or your phone number.
        break;
        
      // Cases 15 and above happen every few minutes
      case 16:    
        //Serial.println(F("(16) Requesting phone numbers"));
        //request(PHONENUMBER1_ID, V_TEXT); wait(RF_DELAY);                 // periodically check which phone numbers are allowed to control the smart lock.
        //request(PHONENUMBER2_ID, V_TEXT); wait(RF_DELAY);                 // periodically check which phone numbers are allowed to control the smart lock.
        break;
        
      case 17:    
        //Serial.println(F("(17) Requesting the current passwords"));
        //request(ROTATING_PASSWORD1_ID, V_TEXT); wait(RF_DELAY);           // periodically check if there is a new password set.
        //request(ROTATING_PASSWORD2_ID, V_TEXT); wait(RF_DELAY);           // periodically check if there is a new password set.
        waitingForResponse = true;                  // used to check if the connection with the controller is ok.
        break;
        
      case 18:  
        //Serial.println(F("(18) Free RAM = ")); //F function does the same and is now a built in library, in IDE > 1.0.0
        //Serial.println(freeRam()); // this is a smaller, alternative function to check free memory.
        sendHeartbeat();
        break;
    }

    // after booting the system we skip sending the AT commands again, and go straight to re-requesting the latest passwords.
    if(loopCounter > 15 + LOOPSBETWEENPASSWORDREQUESTS){
      loopCounter = 15;
    }

    //Serial.print(F("loopcounter = ")); + Serial.println(loopCounter);
    
    if(waitingForResponse == true){
      if(timeOutCount < MAXIMUMTIMEOUTS){                 
        timeOutCount++;                             // Server failed to give the password in time. We're still waiting.. so connection problems?
      }
      else{
        //Serial.println(F("! CONNECTION LOST?"));  // Server failed to give us the current password a few times. The server must be down.
      }
    }
  }


  for( byte checkingDoor = 0; checkingDoor < DOOR_COUNT; checkingDoor++ ){ // We loop over all the doorlocks in the system, and check if they are in the right position.
    //Serial.print(F("checkingDoor ")); Serial.println(checkingDoor);
    if( desiredDoorStates[checkingDoor] != actualDoorStates[checkingDoor] ){
      Serial.println(F("Changing door status"));
      char updateMessage[11];
      if( desiredDoorStates[checkingDoor] == LOCKED ){
        digitalWrite(RELAY1_PIN + checkingDoor, RELAY_LOCKED);
        strcpy_P(updateMessage, lockedMessage);
      }
      else if( desiredDoorStates[checkingDoor] == UNLOCKED ){
        digitalWrite(RELAY1_PIN + checkingDoor, RELAY_UNLOCKED);
        strcpy_P(updateMessage, unlockedMessage);  // Copy the string "  unlocked" from PROGMEM / FLASH into RAM
      }
      updateMessage[0] = checkingDoor + 49; // Here the first character of the string is changed to create a useful message, such as "1 locked" or "2 unlocked". Ascii character '1' has number 49. 2 has number 50, etc.
      Serial.print(F("updateMessage: "));Serial.println(updateMessage);
      Serial.print(F("relay ID: ")); Serial.println(RELAY1_CHILD_ID + checkingDoor);
      Serial.print(F("new desired state: ")); Serial.println(desiredDoorStates[checkingDoor]);
      
      send(relaymsg.setSensor(RELAY1_CHILD_ID + checkingDoor).set( desiredDoorStates[checkingDoor] )); wait(RF_DELAY); // Tell the controller in what state the lock is.
      send(charmsg.setSensor(DEVICE_STATUS_ID).set( updateMessage )); wait(RF_DELAY);
      actualDoorStates[checkingDoor] = desiredDoorStates[checkingDoor];
      EEPROM.update(EEPROM_STORAGE_START + checkingDoor, desiredDoorStates[checkingDoor]); // Store the new position in eeprom. After a reboot it will automatically jump back to this state.
    }
  }



#ifdef DOOR1_SELF_LOCKING
  if( actualDoorStates[0] == UNLOCKED ){
    Serial.println("Self locking in a few seconds");
    wdt_disable();
    wait(SELF_LOCKING_DELAY * 1000);
    wdt_enable(WDTO_8S);
    desiredDoorStates[0] = LOCKED;
  }
#endif

#ifdef DOOR2_SELF_LOCKING
  if( actualDoorStates[1] == UNLOCKED ){
    Serial.println("Self locking in a few seconds");
    wdt_disable();
    wait(SELF_LOCKING_DELAY * 1000);
    wdt_enable(WDTO_8S);
    desiredDoorStates[1] = LOCKED;
  }
#endif

#ifdef HAS_BUTTONS
  // Check button to toggle the state
  if (digitalRead(BUTTON1_PIN) == HIGH){
    //Serial.println(F("1"));
    wait(100);
    if(buttonBeingPressed == 0){
      desiredDoorStates[0] = !desiredDoorStates[0]; // On press of the button, toggle the door 1 desired status. e.g. locked -> unlocked.
      send(charmsg.setSensor(DEVICE_STATUS_ID).set( F("Button 1 pressed") ));
      Serial.print(F("Button1->")); Serial.println(desiredDoorStates[0]);
    }
    buttonBeingPressed = 1;
  }
  else if (digitalRead(BUTTON2_PIN) == HIGH){
    //Serial.println(F("2"));
    wait(100);
    if(buttonBeingPressed == 0){
      desiredDoorStates[1] = !desiredDoorStates[1]; // On press of the button, toggle the door 2 desired status. e.g. locked -> unlocked.
      send(charmsg.setSensor(DEVICE_STATUS_ID).set( F("Button 2 pressed") ));
      Serial.print(F("Button2->")); Serial.println(desiredDoorStates[1]);
    }
    buttonBeingPressed = 1;  
  }
  else{
    buttonBeingPressed = 0; 
  }
#endif

} // End of main loop


// PROCESS LINE
//
// This parses the serial data from the GSM modem, and for example filters out the actual SMS text.
void processLine() 
{
#ifdef DEBUG
  //Serial.print(F("processing: ")); Serial.println(serialLine);
#endif
  wdt_reset();                                    // Reset the watchdog timer

  //serialLine[24] = '\0';
  send(charmsg.setSensor(SMS_CHILD_ID).set(serialLine));
      
  // If the incomingSMS has been set, that means we already found the phonenumber part of the sms, and are now at the actual sms content part.
  if ( incomingSMS == 1 ){
    incomingSMS = 0;                                // Reset for when the next sms needs to be dissected.
#ifdef DEBUG
    Serial.println(F("-At the actual text"));
#endif
    // Does the SMS content begin with a correct password?
    byte password1length = strlen(rotatingPassword1);
    byte password2length = strlen(rotatingPassword2);
    bool password1found = strncmp(serialLine, rotatingPassword1, password1length);
    bool password2found = strncmp(serialLine, rotatingPassword2, password2length);
    password1found = !password1found;
    password2found = !password2found;

#ifdef DEBUG
    Serial.print(F("pass 1 found?: "));Serial.println(password1found);
    Serial.print(F("pass 2 found?: "));Serial.println(password2found);
#endif
    
    if( password1found || password2found ){
      Serial.println(F("Correct password received"));

      char * command;
      command = strstr(serialLine, " ");            // Search for the index of the space
      if (command != NULL)                          // If successful then command now points to the second word in the SMS
      {
        // Serial.print(F("Command = ")); Serial.println(command);
        // Check what the command is, and act on it.
        bool commandLock = strncmp (command, " lock", 5);
        bool commandUnlock = strncmp (command, " unlock", 7);
        bool commandStatus = strncmp (command, " status", 7);
        if(commandLock == 0){
          Serial.println(F("->lock"));
          if(password1found){ desiredDoorStates[0] = LOCKED; }
          if(password2found){ desiredDoorStates[1] = LOCKED; }
        }
        else if(commandUnlock == 0){
          Serial.println(F("->unlock"));
          if(password1found){ desiredDoorStates[0] = UNLOCKED; }
          if(password2found){ desiredDoorStates[1] = UNLOCKED; }
        }
        else if(commandStatus == 0){
          Serial.println(F("command: send status sms"));
          char updateMessage[11];
          if(password1found == 0){ 
            if( actualDoorStates[0] == LOCKED ){ strcpy_P(updateMessage, lockedMessage); }
            else { strcpy_P(updateMessage, unlockedMessage); }
            updateMessage[0] = '1';
          }
          if(password2found == 0){ 
            if( actualDoorStates[1] == LOCKED ){ strcpy_P(updateMessage, lockedMessage); }
            else { strcpy_P(updateMessage, unlockedMessage); }
            updateMessage[0] = '2';
          }
          sendSMS(updateMessage);
        }
      }
    }
    else {                                          // If the SMS did not contain a good password, we will send it to the controller.
      Serial.println(F("Incorrect password"));
      //char tempLine[25];
      //byte i = 25;                                // How many characters we will copy into the new variable
      //while ( i-- ) *( tempLine + i ) = *( serialLine + i ); // Copying characters from serialLine variable into a new one.
      //send(charmsg.setSensor(SMS_CHILD_ID).set(tempLine)); wait(RF_DELAY);
      serialLine[24] = '\0';
      send(charmsg.setSensor(SMS_CHILD_ID).set(serialLine));
    }
    softSerial1.println(F("AT+CMGD=1,4"));          // Delete all received SMS messages (for A6 based GSM modem).
  }
  else {
    
    //bool connectedToNetwork = strncmp(serialLine, "+CREG: 5", 8);
    //if (connectedToNetwork == 0){
    if( !strncmp(serialLine, "+CREG: 5", 8) ){      // Connected to the GSM network
      Serial.println(F("GSM CONNECTED"));
      send(charmsg.setSensor(DEVICE_STATUS_ID).set( F("GSM CONNECTED"))); wait(RF_DELAY);
    }


    else if( !strncmp(serialLine, "+CUSD:", 6) ){        // We received the response from the command.
      byte commandPos = 11;                         // 11 is the position in the character array where the returned command actually starts.
      byte savePos = 0;                             // The position in the array where we are storing the new value.
      static byte shift = 0;                        // How many of the bits must be shifted over to the next byte. Since are are handling 8 bits but only need 7, we keep having one bit left over. Once there is a need to shift 7 bits, that means we have the extra 'bonus' character.
      byte nextLeftOver = 0;                        // The leftover bits from the current translation
      byte prevLeftOver = 0;                        // The leftover bits from the previous translation.

      while ( commandPos < 255 ){
        prevLeftOver = nextLeftOver;
        nextLeftOver = 0;
        
        unsigned char value = serialLine[commandPos]; // Get a character from the serial data.

        if( savePos == 24 || value == "\"" || value == 34){ // Each time we've decoded 25 characters, or if we have arrived at the end of the serialLine, then we should send the retrieved information to the controller
          serialLine[savePos] = '\0';
          savePos = 0;
#ifdef DEBUG
          Serial.print(F(" sending: ")); Serial.println(serialLine);
#endif
          send(charmsg.setSensor(DEVICE_STATUS_ID).set( serialLine )); wait(RF_DELAY);
        }
        if( value == "\"" || value == 34 ){         // We're at the end of the serialLine.
          //Serial.println(F("--end-of-serial-line--"));
          break;
        }

        // Here we safeguard the bits we will need in the next round by copying them into a new variable.
        for( byte j = 0; j <= shift; j++ ){
          bitWrite( nextLeftOver, j, bitRead(value,(7-shift) + j) ); 
        }
        
        value = value << shift;                     // Scoot everything over. The bits that get destroyed have already been safeguarded.    
        value = value | prevLeftOver;               // overlay the previous left over bits. This will create the final character.
        bitWrite( value, 7, 0 );                    // Force it to be a 7-bit character by turning the 8th bit to 0. 
        //Serial.print( (char) value);
        serialLine[savePos] = value;                // Store the final decoded value back in the original serialLine array (to save memory).
        savePos++;
        
        shift++;
        if( shift == 7 ){
          //Serial.print( (char) nextLeftOver);       // Once every 7 loops the left-over part will itself be 7 bits long. We can just print this as the 7th character.
          serialLine[savePos] = (char) nextLeftOver; //value;              // Store the final decoded value back in the original array.
          savePos++;
          nextLeftOver = 0;
          shift = 0;
        }
        commandPos++;
      }
      Serial.println();
    }


    //bool atPhoneNumberLine = strncmp(serialLine, "+CMT", 4); // Are we at the part of the SMS that holds the sender's phonenumber?
    //if (atPhoneNumberLine == 0){
    else if( !strncmp(serialLine, "+CMT:", 4) ){         // Are we at the part of the SMS that holds the sender's phonenumber?
#ifdef DEBUG
      Serial.println(F("+CMT found"));
      // Can we find a phonenumber in the serial line?
      Serial.print(F("phone#1 to check against = ")); Serial.println(phone1);
      Serial.print(F("phone#2 to check against = ")); Serial.println(phone2);
#endif
      
      //char * p;
      //char * q;
      byte p;
      byte q;      
      p = strstr (serialLine, phone1);              // At what position in the serialLine have we spotted the phone number? If it is 0 this means it has not been found.
      q = strstr (serialLine, phone2);
      if (p || q) { // If one of the phone numbers was found in the string, then one of these will be bigger than 0.
        Serial.println(F("Phone number is allowed"));
        incomingSMS = 1;                            // We may now proces the sms content, which should be on the next serial line.
        Serial.print(F("Soft Serial available: ")); Serial.println( softSerial1.available() );
        return;
        /*
        if (p){
          send(charmsg.setSensor(DEVICE_STATUS_ID).set( phone1 )); wait(RF_DELAY); // Tell the controller which phone number just sent us an SMS.
        }
        else{
          send(charmsg.setSensor(DEVICE_STATUS_ID).set( phone2 )); wait(RF_DELAY); // Tell the controller which phone number just sent us an SMS.
        }
        */
        //Serial.println(F("DONE"));
        //memcpy( foundNumber, &serialLine[8], 10 ); // Starts at position 10 in the serialLine and then copies 12 characters
      }
      else {
        send(charmsg.setSensor(DEVICE_STATUS_ID).set(F("Unknown phonenumber!")));
        Serial.println(F("Permission denied for "));    // Neither allowed phone number was detected. Fun fact: if the two phone number arrays to compare against are "" (empty), the above should also trigger, which means any phone number may control the device.
        char foundNumber[14];                       // Holds the phone number that sent the SMS
        for (byte i = 8; i < 20; i++) {
          unsigned char character = serialLine[i];
          if( isDigit(character) ){
            Serial.write(character);
            foundNumber[i - 8] = character;
            foundNumber[i - 7] = '\n';
          }
        }
        Serial.println();
        // Send the unexpected phone number:
        send(charmsg.setSensor(DEVICE_STATUS_ID).set( foundNumber )); wait(RF_DELAY); // Tell the controller which phone number just sent us an SMS.
      }
    }
  }
}


// This function is called when messages from the home controller arrive.
void receive(const MyMessage &message)
{
  wdt_reset();                                    // Reset the watchdog timer
  timeOutCount = 0;
#ifdef DEBUG
  Serial.print(F("->receiving message for child ")); Serial.println(message.sensor);
#endif

  if (message.isAck()) {
    Serial.println(F("-Ack"));
  }
  else if (message.type == V_LOCK_STATUS) {            // Change relay state  
    desiredDoorStates[message.sensor - RELAY1_CHILD_ID] = message.getBool()?RELAY_LOCKED:RELAY_UNLOCKED;
    Serial.print(F("Controller -> door ")); Serial.print(message.sensor - RELAY1_CHILD_ID); Serial.print(F(" -> ")); Serial.println(desiredDoorStates[message.sensor - RELAY1_CHILD_ID]);

    //for( byte checkingDoor = 0; checkingDoor < DOOR_COUNT; checkingDoor++ ){
    //  if( message.sensor == RELAY1_CHILD_ID + checkingDoor ){ // If we found the door the message corresponds with
    //    desiredDoorStates[checkingDoor] = message.getBool()?RELAY_LOCKED:RELAY_UNLOCKED; // Set the desired state of the lock. In the next loop this will be made actual.
    //    Serial.print(F("Controller -> door ")); Serial.print(checkingDoor); Serial.print(F(" -> ")); Serial.println(desiredDoorStates[checkingDoor]);
    //  }
    //}
  }
  else if (message.type == V_TEXT) {

    Serial.print(F("- incoming string: "));Serial.println(message.getString());
    
    // Receiving the rotating passwords.
    //waitingForResponse = false;

    if( message.sensor >= ROTATING_PASSWORD1_ID && message.sensor < ROTATING_PASSWORD1_ID + DOOR_COUNT ){
      send(charmsg.setSensor(DEVICE_STATUS_ID).set( F("received new password"))); wait(RF_DELAY);
    }

    if(message.sensor == ROTATING_PASSWORD1_ID){
      if( rotatingPassword1 != message.getString() ){
        strcpy(rotatingPassword1, message.getString());
        Serial.print(F("Rotating password 1 is now: ")); Serial.println(rotatingPassword1);
        //send(charmsg.setSensor(DEVICE_STATUS_ID).set( F("received latest password 1"))); wait(RF_DELAY);
      }
    }
    else if(message.sensor == ROTATING_PASSWORD2_ID){
      if( rotatingPassword2 != message.getString() ){
        strcpy( rotatingPassword2, message.getString());
        Serial.print(F("Rotating password 2 is now: ")); Serial.println(rotatingPassword2);
        //send(charmsg.setSensor(DEVICE_STATUS_ID).set( F("received latest password 2"))); wait(RF_DELAY);
      }
    }
      
      //for ( int i = 0; i < 25; i++ ){
      //  EEPROM.update (EEPROM_STORAGE_START + (message.sensor * 25) + i, rotatingPassword1 [ i ] );
      //}

    // Receiving the (parts of) phonenumbers that are allowed to operate the smart lock.
    else if(message.sensor == PHONENUMBER1_ID){
      //if( sizeof(message.getString()) > 2 ){
      strcpy(phone1, message.getString());
      //}
      Serial.println(F("received phone #1 ")); 
#ifdef DEBUG
      Serial.print(F("phone1: ")); Serial.println(phone1);
#endif
      //send(charmsg.setSensor(DEVICE_STATUS_ID).set( F("received phone 1"))); wait(RF_DELAY);
    }
    else if(message.sensor == PHONENUMBER2_ID){
      //if( sizeof(message.getString()) > 2 ){
      strcpy(phone2, message.getString());
      //}
      Serial.println(F("received phone #2 "));
#ifdef DEBUG
      Serial.print(F("phone2: ")); Serial.println(phone2);
#endif
      //send(charmsg.setSensor(DEVICE_STATUS_ID).set( F("received phone 2"))); wait(RF_DELAY);
    }
        
    // If the controller asks us to send an SMS.
    else if(message.sensor == SENDSMS_CHILD_ID){
      char smsToSend[26]; // Todo: shouldn't this be global so that the other function can handle it? And then set 'send-sms' var to true.
      strcpy(smsToSend, message.getString());
      sendSMS(smsToSend); // ToDo: make this change a variable and a state instead, so that sending can be handled in the main loop. It's best to keep interuptions short.
    }
  }
}
