/*
 *
 * Signal Hub
 * 
 * This device can copy signals from wireless remote controls that use the 433 frequency, and then rebroadcast them. It can optionally also copy Infra red (IR) signals.
 * 
 * It can do this in three ways:
 * - Copy and replay ON and OFF signals. For example, from cheap wireless power switches. It basically copies remote controls.
 * - Copy and then replay a single signal. For example, to emulate a window sensor.
 * - Recognise signals without replaying them. For example, After learning the signal once, it can detect when a window sensor is triggered again. Or when a button on a remote control is pressed.
 * 
 * This allows you to:
 * - Create a smart home security solution using cheap window and movement sensors.
 * - Automatically turn on lights and other devices when you get home, or when the sun goes down etc, using wireless power sockets.
 * - Control automations using wireless buttons or remote controls.
 * 
 * An Arduino Nano can store 50 "recognise only" signals, or about 20 on/off signals. You can store any combination of these. If you need to store more signals you could look into using an Arduino Mega.
 * 
 * Are there any limits?
 * - This does not work on things like garage door openers or keyless entry systems for cars. 
 * These devices have a very basic protection: the code changes everytime you use it, so replaying signals will not open the door again.
 * 
 * Security?
 * - Many cheap 433Mhz devices do not use encryption. This allows us to copy the signal in the first place. 
 * This also means that your neighbour can in theory do the same thing you can: copy and replay signals picked up through the walls.
 *
 * 
 * 
 * SETTINGS */


#define HAS_TOUCH_SCREEN                            // Have you connected a touch screen? Connecting a touch screen is recommended.  

#define MY_ENCRYPTION_SIMPLE_PASSWD "changeme"      // If you are using the Candle Manager, the password will be changed to what you chose in the interface automatically. Be aware, the length of the password has an effect on memory use.

#define RF_NANO                                     // RF-Nano. Check this box if you are using the RF-Nano Arduino, which has a built in radio. The Candle project uses the RF-Nano.

 /* END OF SETTINGS
  *  
  * 
  * ABOUT THE CODE
  * 
  * The code has a number of states it can be in.
  * LISTENING MODE. Here The main loop continuously listens for signals. If it detects it calls three successive funtions:
  * 1. Check if signal is a signal (SignalViabilityCheck function)
  * 2. Clean up the signal (signalCleaner function)
  * 3. Analyse the signal to find the binary code it represents (signalAnalysis function).
  * 
  * If a valid binary code is found, the next action depends on which 'state' the system is in.
  * - If in LISTENING_SIMPLE state, then the signal is compared to all the stored signals. It lets you know if there is a match.
  * - If in LISTENING_ON state, then the signal is compared to all the stored signals. It lets you know if there is a match.
  * - If in COPYING_SIMPLE state, the code is stored as a 'simple' signal. This can then be replayed later.
  * - If in COPYING_ON state, the code is stored, after which the system asks for the OFF code (COPYING_OFF state), and then stores it with the same data. 
  * - If in LEARNING_SIMPLE state, only the binary code is stored, and not the meta-data required to fully recreate the signal.
  * 
  * The final states the system can be in are:
  * - IN_MENU. This is when the system is displaying a menu on the screen.
  * - REPLAYING. This is the state while a signal is being replayed.
  * 
  * Depending on the current state the various functions can work in slightly different ways. 
  * take for example the scanEeprom function:
  * - When in LISTENING state it compares the latest found signal to existing signals stored in the EEPROM memory.
  * - When in REPLAYING state it returns data required to rebuild the original signal.
  * - If called with a 0, then it does not try to recognise or rebuild anything. This is used during setup, when we only need to know how many signals are stored.
  * 
  * __SIGNAL ANALYSIS DETAILS__
  * When it detects a signal, the code tries to find the part of the signal that repeats. 
  * In normal operation the signalCleaner function cleans up the signal and simultaneously tries to find the 'betweenSpace' variable. 
  * Often, 433Mhz signals have a repeating binary code that is interrupted by a short burst of different signals (called the 'anomaly' in this code).
  * If no anomaly can be detected, then the repeating part is probably 'back to back', without a separator signal. 
  * In this case there is a 'backup' function, the 'pattern finder'. This uses brute force to find the signal.
  * If both methods fail, then the signal cannot be copied.
  *  
  *  
  *  
  *  TODO
  *  - Check if signal is already stored before storing it. Then again, there can be good reasons to store a signal twice. Perhaps only check for doubles with recognise-only signals?
  *  - Another bit could be used to store if an on/off signal should also be recognisable. That way the remote could be used twice somehow.. Or: 
  *  - Request current status of on/off toggles from the controller. Though it might be jarring or even dangerous if all devices suddenly toggled to their new positions.
  *  - Turn off the display after a while.
  *  - Send new children as they are created.
  *  - Allow multiple quick succession touch screen events to add play commands to the playlist.
  */


//#define DEBUG                                     // Do you want to see extra debugging information in the serial output?
//#define DEBUG_SCREEN                              // Do you want to see extra debugging information about the touch screen in the serial output?
//#define MY_DEBUG                                  // Enable MySensors debug output to the serial monitor, so you can check if the radio is working ok.

// Receiver and transmitter pins
#define RECEIVER 3                                  // The pin where the receiver is connected.
#define TRANSMITTER 4                               // The pin where the transmitter is connected.

#define TOUCH_SCREEN_RX_PIN 7                       // The receive (RX) pin for the touchscreen. This connects to the transmit (TX) pin of the touchscreen.
#define TOUCH_SCREEN_TX_PIN 8                       // The receive (TX) pin for the touchscreen. This connects to the transmit (RX) pin of the touchscreen.

#ifdef RF_NANO
// If you are using an RF-Nano, you have to switch CE and CS pins.
#define MY_RF24_CS_PIN 9                            // Used by the MySensors library.
#define MY_RF24_CE_PIN 10                           // Used by the MySensors library.
#endif


// This code has an extra pattern finding trick. Using brute force it will try to find a pattern in the data. The downside is it takes a lot of time to analyse signals this way. 
// This means the system might not detect a signal because it is busy analysing a bad signal. It's up to you if you want to use it.
//#define PATTERN_FINDER

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



// REQUIRED LIBRARIES

#include <MySensors.h>                              // The library that helps form the wireless network.
#include <EEPROM.h>                                 // Allows for storing data on the Arduino itself, like a mini hard-drive.


//#define HAS_BASIC_OLED_SCREEN                     // Have you connected a simple OLED screen? Connecting a screen is recommend. 

// Basic OLED screen
#ifdef HAS_BASIC_OLED_SCREEN
#define INCLUDE_SCROLLING 0                         
#define OLED_I2C_ADDRESS 0x3C
#include <SSD1306Ascii.h>                           // Driver for the simple OLED screen.
#include <SSD1306AsciiAvrI2c.h>                     // "SSD1306Ascii"
SSD1306AsciiAvrI2c oled;
#endif


// Touch screen
#ifdef HAS_TOUCH_SCREEN

#include <SoftwareSerial.h>
SoftwareSerial touch_screen_serial(TOUCH_SCREEN_RX_PIN,TOUCH_SCREEN_TX_PIN);                       // RX (receive) pin, TX (transmit) pin

#define MAX_BASIC_COMMAND_LENGTH 16                 // How many bytes are in the longest basic command?
#define TOUCHSCREEN_WIDTH 240
#define TOUCHSCREEN_HEIGHT 320
#define BUTTON_HEIGHT 53                            // How many pixels tall are the touch screen buttons?
#define BUTTON_PADDING (BUTTON_HEIGHT/2) - 7        // The font is 14 pixels high, so this calculation places it in the middle of the buttons.

const byte howManyReplayButtonsWillFitOnScreen = (byte)floor( (TOUCHSCREEN_HEIGHT / BUTTON_HEIGHT) - 2 ); // Minus one for the menu button, and one for the 'more' button.

boolean touched = false;                            // Was the touchscreen just touched?
byte visibleReplayButtonsPage = 0;                  // The user can scroll through all the replay buttons.

signed int touchX = 0;                              // Touch screen position X
signed int touchY = 0;                              // Touch screen position Y

// Basic commands for the touch screen. All commands for the TFT should start with 0x7E, but to save storage space this is taken care of in the basicCommand function.
PROGMEM const byte play[] =             {0x07, 0x11, ' ', 'P', 'L', 'A', 'Y', 0xEF,}; // Places the word ' play' on the screen.
PROGMEM const byte on[] =               {0x07, 0x11, ' ', 'O','N', ' ', ' ', 0xEF,}; // Places the word ' on  ' on the screen.
PROGMEM const byte off[] =              {0x07, 0x11, ' ', 'O', 'F', 'F', ' ', 0xEF,}; // Places the word ' off ' on the screen.
PROGMEM const byte w[] =                {0x07, 0x11, 'w', 0x00, ' ', 0x00, 0x00, 0xEF,}; // Places the word 'w    ' on the screen.
PROGMEM const byte menu[] =             {0x07, 0x11, 'M', 'E', 'N', 'U', ' ', 0xEF,}; // Places the word 'menu ' on the screen.
PROGMEM const byte more[] =             {0x07, 0x11, 'M', 'O', 'R', 'E', '>', 0xEF,}; // Places the word 'menu ' on the screen.

PROGMEM const byte set_vertical[] =     {0x03, 0x04, 0x02, 0xEF,}; // To set rotation of the screen to vertical. Try 0x01 or 0x03 instead of the 0x02.
PROGMEM const byte fill_black[] =       {0x04, 0x20, 0x00, 0x00, 0xEF,}; // Fill screen with one color
PROGMEM const byte fill_blue[] =        {0x04, 0x20, 0x00, 0xFF, 0xEF,}; // Fill screen with one color

PROGMEM const byte text_color_white[] = {0x04, 0x02, 0xFF, 0xFF, 0xEF,}; // white text color. fill screen with one color
//PROGMEM const byte text_color_black[] = {0x04, 0x02, 0x00, 0x00, 0xEF,}; // Dark text color. fill screen with one color
//PROGMEM const byte text_color_red[] =   {0x04, 0x02, 0xF8, 0x00, 0xEF,}; // .. text color. fill screen with one color

PROGMEM const byte resetTFT[] =         {0x02, 0x05, 0xEF,}; // Resets the TFT. But has no real effect.
PROGMEM const byte testTFT[] =          {0x02, 0x00, 0xEF}; // Test the TFT, should respond with "OK".
PROGMEM const byte backlight_on[] =     {0x03, 0x06, 0xFF, 0xEF}; // Backlight intensity to half-full
PROGMEM const byte backlight_off[] =    {0x03, 0x06, 0x00, 0xEF}; // Backlight intensity to zero
//PROGMEM const byte serialSpeedUp[] =    {0x03, 0x40, 0x03, 0xEF,}; // Sets communication speed to 57600 (from 9600)
PROGMEM const byte serialSlowDown[] =   {0x03, 0x40, 0x00, 0xEF,}; // Sets communication speed to 9600 again. Oddly enough, it seems it works fastest at this speed..

PROGMEM const byte test[] =   {0x02, 0x00, 0xEF,}; // Test message


#endif

PROGMEM const char detectedMessage[] = { "Detected  " }; // This construction saves some memory.
PROGMEM const char replayMessage[]   = { "Replay    " }; // This construction saves some memory.

/* 
The colors use this RGB565 format:
http://www.barth-dev.de/online/rgb565-color-picker/

white: FF FF = 65535;
red: F8 00 = 63488
purple: F0 1F = 61471
blue: 00 1F = 31
light blue: 07 FF = 2047
bright green: 07 E0 = 2016
yellow: FF 20 = 65312
orange: FC 80 = 64640
half grey: 7BEF = 31727
*/




// Keypad
#define KEYPAD_PIN A0                               // The pin where the analog keypad is connected. These keypads vary their resistance according to which button is pressed.
byte buttonPressed = 100;                           // The last button that was pressed by the user.
byte prevButtonState = 100; 
#define KEYPAD_BUTTON_COUNT 0                       // How many buttons does your keypad have? Leave at 0 if you are using a touch screen.

#if KEYPAD_BUTTON_COUNT > 0
boolean buttonsToggleStatus[KEYPAD_BUTTON_COUNT + 1]; // Array to hold the buttons' toggle status (if the button has an on/off signal). For simple signals buttonsToggleStatus[x] is always 0. For on/off signals this actually switches between 0 and 1. For simplicity, the zero position of the array is ignored.
#endif


// SIGNAL CLONING

/* What length of signal are we looking for? This determines how many edges we need to be able to store. Here's a chart:
 * 
 * 1 byte   = 8 bits  = 16 timings
 * 2 bytes  = 16 bits = 32 timings
 * 3 bytes  = 24 bits = 48 timings <- default for MINIMAL_SIGNAL_LENGTH. Cheap window and door sensors use this.
 * 4 bytes  = 32 bits = 64 timings
 * 5 bytes  = 40 bits = 80 timings
 * 6 bytes  = 48 bits = 96 timings
 * 8 bytes  = 64 bits = 128 timings <- Most wireless sockets use 8 byte signals. I have never come across a longer signal.
 * 10 bytes = 80 bits = 160 timings
 * 12 bytes = 96 bits = 192 timings
 * 
 * 16 bytes = 128bits = 256 timings
 * 
 * As you can see, getting more than 16 bytes in practice requires more memory than the Arduino Nano has.
 * Ideally, your MAXEDGES setting should be 3x the size of the repeating signal you are looking for.
 * 
 */
#define MAXEDGES 300                                // Maximum samples, limited by RAM. The findPattern() function requires more signals to work than the default analyser.
#define GRANULARITY 50                              // Sensitivity. Lower can grab faster signals, but might be less dependable. Set somewhere between 50 and 100.
#define EDGES_TO_SKIP 0                             // How much of the beginning of the incoming signal to skip. This is a memory saving measure. Sometimes the first part of the signal is the 'pre-amble': simple timings to help the receiver synchronise its clock. Set somewhere between 0 and 100. Default is 60. 
#define MINIMAL_SIGNAL_LENGTH 48                    // How many edges should a signal have at the least? ( 1 byte = 2 nibbles = 8 bits = 16 edges).

#define MINIMUMDURATION 100                         // Minimum low or high signal duration for a valid signal in microseconds. Set somewhere between 50 and 100. Helps to avoid noise.
#define MAXIMUMDURATION 20000                       // Maximum low or high signal duration for a valid signal.
#define MINIMUMSILENCE 30000                        // Minimum low period after a complete signal in microseconds. If there is no input for this long, we can conclude that the transmission must have finished.
#define EEPROM_STORAGE_START 512                    // From which position the eeprom can safely be used as storage without overwriting MySensors encryption data. Another option woudl be to use EEPROM_LOCAL_CONFIG_ADDRESS.
#define MAXI 249 * GRANULARITY                      // The maximum length in microseconds that any timing may have to be precisely stored. If a timing is longer, it will be truncated.

byte timings[MAXEDGES];                             // Creates the array we use to store the timings, but that is also re-used for other things in order to save memory.
const byte metadataArraySize = 8;                   // The size of the array to store the serial data we're decoding in.
byte metaData[metadataArraySize];                   // Holds metadata for a replayable signal.
long signalstart = 0;                               // Used to calculate the length of each received timing.
long signalend;                                     // Used to calculate the length of each received timing.
unsigned int interval;                              // The length of a timing in milliseconds.
signed int edges = 0 - EDGES_TO_SKIP;               // By starting below zero we can try to avoid the beginning of signals, which often contains a 'pre-amble': a boring pattern to help the receiver get up to speed.
byte bucketsTotal = 0;                              // This describes how many different timings there are in the signal. To clean up the signal it's wise to lump neighbouring timings together.
byte repeatingPatternLength = 255;                  // This describes how many timings the repeating part of the signal is made of.
byte amountOfStoredSignals = 0;                     // How many signals are curently stored on the EEPROM storage. This counts detect-only as well as replayable signals.
byte amountOfStoredReplayableSignals = 0;           // How many replayable signals are stored in the EEPROM?
byte repeatingSignalByteLength = 3;                 // How many bytes does the repeating part of the signal take up in EEPROM?
boolean lastByteIsSplit = false;                    // Is the last byte of a new signal only half-filled? Signals are multiples of 4 bits, but we can only store them in multiples of 8. So a signal with 20 bits (5 'nibbles'), would still be stored in 3*8 = 24 bits. We need to know that when reconstructing the signal.
boolean anomalyInside = false;                      // Is the anomaly part of the byte/nibble, or is it outside of it?
byte betweenSpace = 250;                            // Which timing denotes the space ("the anomaly" as it's called in this code) between repeated signals? If it's still set at 250 after the analysis, the the analysis must have failed to find it.
int startPosition = 1;                              // The start position of a clean signal (this cuts of the first one)
int endPosition = MAXEDGES - 1;                     // The end position of a clean signal (this cuts of the last one, which is often truncated).
int positionOfLastSignalEnd = EEPROM_STORAGE_START; // Denotes the EEPROM position right before the position where a new signal can safely be stored.
boolean validSignal = true;                         // Used by the signal detection loop If the 'high' part of a timing was bad, then this is set to false. Looking for a valid 'low' is then skipped.
boolean captureFinished = false;                    // If the entire edges array is full of freshly received timings, this is set to true. Analysis the signal is then set in motion.
byte bucketCount = 0;                               // How many different types of timings are there in the signal. This is used to tighten up the signal.
boolean connectedToNetwork = false;                 // Are we connected to the local MySensors network? Used to display the 'w' connection icon.
byte lengthOfSignalWeAreWaitingFor = 3;             // Used when recording on-off signals. The off-signal should have the same byte length as the on signal.     
byte brightnessTimer = 0;                           // When this reaches 0 the screen is turned off.

#define PLAYLIST_SIZE 6
byte playlist[PLAYLIST_SIZE];                       // Sometimes multiple demands to play a signal come in. This holds all the signals we should replay one after the other
byte playlist_position = 0;                         // Signals that should be replayed are placed in a playlist, so they can be played one after the other of multiple should be played.

byte response_position = 0; // EXPERIMENT TO DEAL WITH SERIAL BETTER


// DESCRIPTION BIT STATES
#define DESCRIPTION_HALFBYTE 0
#define DESCRIPTION_DUO_RECONSTRUCTION_TYPE 1
#define DESCRIPTION_ANOMALY_INSIDE 2
#define DESCRIPTION_ON_OFF 3
#define DESCRIPTION_REPLAYABLE 4
#define DESCRIPTION_UNRECONSTRUCTABLE_DUO 5
#define DESCRIPTION_ANOMALY 6
#define DESCRIPTION_LARGE_ANOMALY 7


// DISPLAY & STATES
// The list is shared between the display function as well as to desribe what the device is currently doing.

// Menu states
#define MENU_MAIN 200
#define MENU_NEW 30
 #define LEARNING_SIMPLE 31
 #define LEARNING_ON 32
 #define COPYING_SIMPLE 33
 #define COPYING_ON 34

#define MENU_DELETE_LAST 40
 #define DELETE_LAST 41

#define MENU_DELETE_ALL 50
 #define DELETE_ALL 51

#define LEARNING_OFF 38
#define COPYING_OFF 39



// General display codes
#define PROCESSING 1                                // Not a state, only used for display.
#define MATCH 2                                     // Not a state, only used for display.
#define REPLAYING 3
#define SIGNAL_STORED 4                             // Not a state, only used for display.

// Display codes for errors
#define BAD_SIGNAL 5                                // Not a state, only used for display.
#define OUT_OF_SPACE 6                              // Not a state, only used for display.
#define PLAY_SIMPLE_SIGNAL 7
#define PLAY_ON_SIGNAL 8                            // Not a state, only used for display.
#define PLAY_OFF_SIGNAL 9                           // Not a state, only used for display.
#define DELETED_LAST 10                             // Not a state, only used for display.
#define DELETED_ALL 11                              // Not a state, only used for display.

#define NO_MORE_FREE_BUTTONS  100                   // Not a state, only used for display.
#define NO_SIGNAL_STORED_YET 101                    // Not a state, only used for display.

#define STARTUP 254                                 // This state is only used while booting up the device.
#define LISTENING 255                               // This is the default state which it always returns to.

byte state = STARTUP;                               // This variable stores what the device is currently doing. It shares some states with the display output.

#ifdef HAS_TOUCH_SCREEN
#define DISPLAY_REPLAY_BUTTONS 253                  // State used with touchscreen only.


// The MessageDef code below comes from https://arduino.stackexchange.com/questions/49236/printing-char-array-from-array-of-structs-in-progmem-to-serial

typedef byte MessageID;                             // TODO Is this used?  Not really. Simplifying the message table could save some storage space.

struct MessageDef {
  MessageID ID;
  char Description[20];
};

const MessageDef MessageTable[] PROGMEM = {
  {LISTENING,       "CANCEL"},
  {MENU_NEW,        "New signal"},
  {MENU_DELETE_LAST,"Delete last"},
  {MENU_DELETE_ALL, "Delete all"},
  
  {LEARNING_SIMPLE, "Detect single"},
  {LEARNING_ON,     "Detect on + off"},  
  {COPYING_SIMPLE,  "Replay single"},
  {COPYING_ON,      "Replay on + off"},

  {LEARNING_SIMPLE, "Really delete last"},
  {LEARNING_SIMPLE, "Really delete ALL"},
  
  {PROCESSING,      "Processing"}, // 1
  {MATCH,           "Signal matched!"}, // 2
  {REPLAYING,       "Replaying "}, // 3
  {SIGNAL_STORED,   "Signal was stored"}, // 4
  {BAD_SIGNAL,      "Bad signal try again"}, // 5
  {OUT_OF_SPACE,    "Out of storage space"}, // 6
  {PLAY_SIMPLE_SIGNAL,"Play the signal"}, // 7
  {PLAY_ON_SIGNAL,  "Play ON signal"}, // 8
  {PLAY_OFF_SIGNAL, "Play OFF signal"}, // 9
  {DELETED_LAST,    "Deleted last"}, // 10
  {DELETED_ALL,     "Deleted all"} // 11
  };

#define START_OF_MENU_NEW 4
#define START_OF_MENU_DELETE_LAST 8
#define START_OF_MENU_DELETE_ALL 9
#define START_OF_MENU_STATUS_MESSAGES 10
#endif


// MYSENSORS

#define RADIO_DELAY 400                             // Milliseconds between sending radio signals. This keeps the radio happy.
#define DEVICE_STATUS_ID 1                          // The first 'child' of this device is a text field that contains status updates.
#define LISTENER_OUTPUT_ID 2                        // The first 'child' of this device is a text field that contains status updates.

// The device creates 4 virtual buttons that allow signals to be recorded even without an attached screen and keypad.
#define LEARN_SIMPLE_BTN_ID 3                       // Learn to detect a single signal.
#define LEARN_ON_OFF_BTN_ID 4                       // Learn to detect an ON and an OFF signal that belong together.
#define COPYING_SIMPLE_BTN_ID 5                     // Learn to replay a single signal.
#define COPYING_ON_OFF_BTN_ID 6                     // Learn to replay an ON and an OFF signal that belong together.

MyMessage textmsg(DEVICE_STATUS_ID, V_TEXT);    // Sets up the message format that we'll be sending to the MySensors gateway later. In this case it's a text variable. The first part is the ID of the specific sensor module on this node. The second part tells the gateway what kind of data to expect.
MyMessage buttonmsg(LEARN_SIMPLE_BTN_ID, V_STATUS); // The message for replayable signals' buttons. This is an on/off message.
MyMessage detectmsg(10, V_TRIPPED);                 // The message for detect-only signals. This is an on/off message.


static unsigned long lastLoopTime = 0;              // Holds the last time the main loop ran.
boolean resend_button_states = 1;

void before()
{
  Serial.begin(115200);
  Serial.println(F("Hello, I am a Signal Hub."));
}


void presentation()
{
  scanEeprom();                                    // Find out how many signals are stored in memory.
  
  sendSketchInfo(F("Signal Hub"), F("1.1")); wait(RADIO_DELAY); // Child 0. Sends the sketch version information to the gateway and Controller
  present(DEVICE_STATUS_ID, S_INFO, F("Device status")); wait(RADIO_DELAY); // Child 1. This outputs general status details.

  //present(LISTENER_OUTPUT_ID, S_INFO, F("Detected codes")); wait(RADIO_DELAY); // Child 2. This outputs the ID of detected signals that were matched to signals in eeprom.
#if !(defined(HAS_TOUCH_SCREEN))
  Serial.println(F("NO TOUCHSCREEN"));
  present(LEARN_SIMPLE_BTN_ID, S_BINARY, F("Recognize a single code")); wait(RADIO_DELAY);// Child 3
  present(LEARN_ON_OFF_BTN_ID, S_BINARY, F("Recognize an ON+OFF code")); wait(RADIO_DELAY); // Child 4
  present(COPYING_SIMPLE_BTN_ID, S_BINARY, F("Copy a single code")); wait(RADIO_DELAY); // Child 5
  present(COPYING_ON_OFF_BTN_ID, S_BINARY, F("Copy an ON/OFF code")); wait(RADIO_DELAY); // Child 6
#endif

  char childNameMessage[11];
  
  strcpy_P(childNameMessage, replayMessage);
  
  // We loop over all the replayable signals, and present them to the controller.
  for( byte replayableID=10; replayableID < 10 + amountOfStoredReplayableSignals; replayableID++ ){
#ifdef DEBUG
    Serial.print(F("Replayable child ID ")); Serial.println(replayableID);
#endif
    childNameMessage[7] = (replayableID - 10) + 49;
    present(replayableID, S_BINARY, childNameMessage); wait(RADIO_DELAY);
  }

  strcpy_P(childNameMessage, detectedMessage);
  
  // We loop over all the detect-only signals, and present them to the controller. 
  for( byte recognisedID=100; recognisedID < 100 + (amountOfStoredSignals - amountOfStoredReplayableSignals); recognisedID++ ){
#ifdef DEBUG
    Serial.print(F("Detectable child ID ")); Serial.println(recognisedID);
#endif
    childNameMessage[9] = (recognisedID - 100) + 49; // ASCII character '1' has number 49.
    present(recognisedID, S_DOOR, childNameMessage); wait(RADIO_DELAY);
  }

  resend_button_states = 1;
}



void send_values(){
#ifdef DEBUG
  Serial.println(F("Sending button states"));
#endif

#if !(defined(HAS_TOUCH_SCREEN))
  send(buttonmsg.setSensor(LEARN_SIMPLE_BTN_ID).set(0));
  send(buttonmsg.setSensor(LEARN_ON_OFF_BTN_ID).set(0));
  send(buttonmsg.setSensor(COPYING_SIMPLE_BTN_ID).set(0));
  send(buttonmsg.setSensor(COPYING_ON_OFF_BTN_ID).set(0));
#endif

  // We loop over all the replayable signals, and send their values.
  for( byte replayableID=10; replayableID < 10 + amountOfStoredReplayableSignals; replayableID++ ){
    //Serial.print(F("replay loadState at presentation: ")); Serial.println(loadState(replayableID));
    if( loadState(replayableID - 9) > 1 ){
#ifdef DEBUG
      Serial.println(F("LoadState had big value, setting to 0."));
#endif
      saveState(replayableID - 9, 0); // The -9 is to offset the ID back the the savestates in the eeprom. So child 10 has savestate 1, etc.
    }
    boolean saved_toggle_state = loadState(replayableID - 9);
    send(buttonmsg.setSensor(replayableID).set( saved_toggle_state)); wait(RADIO_DELAY); // Tell the controller in what state the child is. //?0:1
  }

  wait(RADIO_DELAY);

  // We loop over all the detect-only signals, and send their values.
  for( byte recognisedID=100; recognisedID < 100 + (amountOfStoredSignals - amountOfStoredReplayableSignals); recognisedID++ ){
    send(detectmsg.setSensor(recognisedID).set( 0 )); wait(RADIO_DELAY); // Tell the controller in what state the child is.
  }

}

void setup() 
{
  pinMode(RECEIVER, INPUT_PULLUP);                  // 433 receiver
  //pinMode(RECEIVER, INPUT);                       // 433 receiver
  pinMode(TRANSMITTER, OUTPUT);                     // 433 transmitter
  digitalWrite(TRANSMITTER, LOW);                   // 433 transmitter set to off
#if KEYPAD_BUTTON_COUNT > 0
  pinMode(KEYPAD_PIN, INPUT);                       // Set keypad pin as input
#endif

#ifdef HAS_BASIC_OLED_SCREEN
  oled.begin(&Adafruit128x64, OLED_I2C_ADDRESS);    // Start the display (if there is one)
  oled.setFont(Adafruit5x7);
  oled.ssd1306WriteCmd(SSD1306_DISPLAYON);
  oled.setScroll(false);        
#endif

#ifdef HAS_TOUCH_SCREEN
  wait(2000);
  touch_screen_serial.begin(9600);
  wait(2000);

#ifdef DEBUG
  for( int t = EEPROM_STORAGE_START - 1; t < EEPROM.length(); t++ ){
    Serial.print(t); Serial.print(F(" > ")); Serial.println( EEPROM.read(t) );
  }
  Serial.print(F("replay button slots: ")); Serial.println( howManyReplayButtonsWillFitOnScreen );
#endif


#ifdef DEBUG_SCREEN
  Serial.println(F("BC: test"));
  basicCommand(test);
#endif

#ifdef DEBUG_SCREEN
  Serial.println(F("BC: serial_slow"));
#endif
  //basicCommand(serialSlowDown);  
  
#ifdef DEBUG_SCREEN
 Serial.println(F("BC: reset"));
#endif
  //basicCommand(resetTFT);                           // Reset the TFT.

#ifdef DEBUG_SCREEN
  Serial.println(F("BC: backlight_on"));
#endif
  basicCommand(backlight_on);

#ifdef DEBUG_SCREEN
 Serial.println(F("BC: vertical"));
#endif
  basicCommand(set_vertical);                       // Set the screen to vertical mode.

#ifdef DEBUG_SCREEN
  Serial.println(F("BC: white text"));
#endif
  basicCommand(text_color_white);                   // Set text color to white.

#endif

  // Check if there is a network connection
  if(isTransportReady()){
    Serial.println(F("Connected to gateway!"));
    connectedToNetwork = true;
    //send(relaymsg.setSensor(RELAY1_CHILD_ID).set( actualDoorStates[0] )); wait(RADIO_DELAY); // Tell the controller in what state the lock is.
  
    send(textmsg.setSensor(DEVICE_STATUS_ID).set( F("Hi") )); //wait(RADIO_DELAY);
  
#ifdef DEBUG    
    Serial.print(F("Stored signal count: "));
    Serial.println(amountOfStoredSignals);
    Serial.print(F("Replayable signal count: "));
    Serial.println(amountOfStoredReplayableSignals);
#endif


  }
  else{
    Serial.println(F("! NOCONNECTION"));
    connectedToNetwork = false;
  }
  
  state = LISTENING;
  updateDisplay(LISTENING);                         // Show "Listening" on the display.
  clearTimingsArray();                              // Reset everything, so that we are ready to listen for a signal.

#ifdef DEBUG
  Serial.print(F("Free RAM after setup = ")); Serial.println(freeRam());
#endif

}
 
 
void loop()
{
  //readResponse();
  //wait(200);
  //Serial.print(F("."));
  //readResponse();
  
  // This part of the code checks the radio receiver pins.
  if( !captureFinished ){
    //Serial.print(F("|"));
    if( signalstart == 0 ){
      signalstart = micros();
    }

    // Part 1 - HIGH
    while( bitRead(PIND, RECEIVER) == HIGH ){}
    signalend = micros();
    interval = signalend - signalstart;
    if (interval > MINIMUMDURATION && interval < MAXIMUMDURATION ){
      if( edges >= 0 ){                             // The edges variable can start at a negative number. This helps cut off the pre-amble that is often transmitted, and that we don't really need.
        if( interval > MAXI ){                      // Very long edges will be truncated.
          timings[edges] = 249;
        }
        else if( interval != 0 ){
          timings[edges] = (byte) constrain((interval / GRANULARITY), 0, 255); // this rounds the number to something simpler that should fit in a byte array.
        }
      }
      edges++;
    }
    else {
      edges = 0 - EDGES_TO_SKIP;
      validSignal = false;
      captureFinished = true;
      //Serial.println(F("bad timing HIGH"));
      signalstart = micros();
    }

    // Part 2 - LOW
    if( validSignal ){                              // We only try to find a low part if the high part of the signal before it was valid.
      while( bitRead(PIND, RECEIVER) == LOW && interval < MINIMUMSILENCE ){}
      signalstart = micros();
      interval = signalstart - signalend;
      
      if( interval > MINIMUMDURATION && interval < MAXIMUMDURATION ){ // Check if the timing falls within reasonable boundaries.
        if( edges >= 1 ){
          if(interval > MAXI){                      // MAXI = 254 * granularity
            timings[edges] = 249;                   // A timing longer than this will be truncated.
          }
          else if( interval != 0 ){
            timings[edges] = (byte) constrain((interval / GRANULARITY), 0, 255); // This rounds the number to something simpler that should fit in a byte array.
          }
        }
        edges++;
      }
      else if( interval >= MINIMUMSILENCE ){        // If the low part of the signal takes a very long time, it could be that the transmission is over.
        captureFinished = true;
        //Serial.println(F("minimum LOW silence reached"));
        if( edges < 0 ){ edges = 0 - EDGES_TO_SKIP; }
        signalstart = 0;
      }
      else {
        edges = 0 - EDGES_TO_SKIP;
        //validSignal = false;
        captureFinished = true;
        //Serial.println(F("bad timing LOW"));
      }
    }
    if( edges >= MAXEDGES - 2 ){                    // Our timings array is full of fresh data.
      captureFinished = true;
      //Serial.print(F("edges full "));
    }
  }
  
  else {                                            // SignalCapture is finished.
    captureFinished = false;                        // Reset for next round.
    boolean signalIsOk = false;
    //Serial.println(F("DONE CAPTURING"));
#ifdef HAS_TOUCH_SCREEN    
    readResponse();
#endif

    if( validSignal && edges > MINIMAL_SIGNAL_LENGTH * 2 ){ // The timings data is long enough.
      if( signalViabilityCheck() ){                 // A quick quality check on the signal.
#ifdef DEBUG
        Serial.println(F("PROCESSING"));
#endif
#ifdef HAS_BASIC_OLED_SCREEN
        updateDisplay(PROCESSING);
#endif
        betweenSpace = 250;                         // Reset the betweenSpace. Might be superfluous.
        if( signalCleaner() ){                      // Cleaning and tightening the received signal.
          if( signalAnalysis() ){
            Serial.println(); 
            signalIsOk = true;

            if( state == LISTENING ){               // System received a good signal while in the LISTENING state.
              Serial.println(F("Comparing"));
              //byte detectedSignalNumber = scanEeprom();
              //Serial.print(F("DetectedSignalNumber = ")); Serial.println(detectedSignalNumber);
              if( scanEeprom() ){                   // If the signal matches a signal in eeprom, then the scanEeprom function returns its number in eeprom. If there is no match, it returns 0.
                Serial.println(F("MATCH"));
                send(textmsg.setSensor(DEVICE_STATUS_ID).set( F("Detected a known signal") )); //wait(RADIO_DELAY);
              }
            }
            else if( state > MENU_NEW && state < MENU_DELETE_LAST ){                             // States in which a signal is supposed to be copied to the EEPROM memory.
                
              Serial.print(F("State ")); Serial.println(state);

              // If we are looking for an OFF signal, then we should check if this signal is just as long as the ON signal we heard earlier.
              if( (state == LEARNING_OFF || state == COPYING_OFF) && repeatingSignalByteLength != lengthOfSignalWeAreWaitingFor ){
                // Poor signal. Try again.
              }
              else {
                
                // Store the signal
                if( writeSignalToEeprom() ){        // We try to store the signal in the EEPROM (internal storage)
                  
                  // If we are doing an on/off copy, then we must move to the next step in the process.
                  if( state == COPYING_ON || state == LEARNING_ON ){
                    Serial.println(F(" Now play OFF signal"));
                    lengthOfSignalWeAreWaitingFor = repeatingSignalByteLength; // When we record the 'off' signal later, we want it to be the same length.
                    send(textmsg.setSensor(DEVICE_STATUS_ID).set( F("Now play the OFF signal") )); //wait(RADIO_DELAY);
                    if( state == COPYING_ON ){ state = COPYING_OFF; } // switch to the next part of the process.
                    if( state == LEARNING_ON ){ state = LEARNING_OFF; } // switch to the next part of the process.
                  }
                  else{
                    updateDisplay(SIGNAL_STORED);
                    presentation();                 // Re-present the node, which now has a new child.
                    Serial.println(F("Finished copying"));
                    send(textmsg.setSensor(DEVICE_STATUS_ID).set( F("OK") ));// wait(RADIO_DELAY);
                    state = LISTENING;
                  }
                }
                else{                               // Storing the signal in EEPROM failed.
                  Serial.println(F("Error storing signal"));
                  updateDisplay(OUT_OF_SPACE);
                  send(textmsg.setSensor(DEVICE_STATUS_ID).set( F("Error storing signal") )); //wait(RADIO_DELAY);
                  state = LISTENING;
                }
              }
              updateDisplay(state);
            }
          }                                         // End of 'signal is ok'.
          else {
#ifdef DEBUG
            Serial.println(F("signalAnalysis failed"));
#endif
          }
        }
        else{
          Serial.println(F("No repeating part found")); // The signal cleaner couldn't find a repeating part
        }
      }
      else {
        Serial.println(F("Low quality signal"));
      }
      clearTimingsArray();
    }
    else{                                           // The captured signal didn't have enough edges.
      captureFinished = false;
      validSignal = true;
    }


    // If a presentation has been requested, we send the new button states.
    if( resend_button_states ){
      resend_button_states = 0;
      send_values();
    }

#if defined (HAS_BASIC_OLED_SCREEN) || defined (HAS_TOUCH_SCREEN)

    //
    // MAIN MENU
    //

    // Once every 100 milliseconds check if the menu button is being pressed.
    if( millis() - lastLoopTime > 100 ){
      lastLoopTime = millis();

      // If there is a signal to be played in the playlist, play it.
      if( playlist_position > 0 ){
        Serial.print(F("Playing signal from playlist #")); Serial.println(playlist_position);
        if( playlist[playlist_position] > 100 ){ // Bigger than 100 means it should be turned on.
          replay( playlist[playlist_position] - 100, 1);
        }
        else{
          replay( playlist[playlist_position], 0);
        }
        playlist_position--;
        Serial.print(F("-new #")); Serial.println(playlist_position);
      }
      

      // Turn screen off after a while
      if( brightnessTimer > 1){
        brightnessTimer--;
        //Serial.println(brightnessTimer);
      }
      else if( brightnessTimer == 1 ){
        brightnessTimer = 0;
#ifdef DEBUG_SCREEN
          Serial.println(F("BC: backlight_off"));
#endif
        basicCommand(backlight_off);                // Set the screen to vertical mode.
      }
      
      
#ifdef HAS_TOUCH_SCREEN
      if( touched ){
        turnOnScreen();
        //Serial.println(F("TOUCH"));
        buttonPressed = touchScreenButtonPress();   // Check what part of the touchscreen was pressed.
#ifdef DEBUG_SCREEN
        drawPix(touchX,touchY, 0);                  // Draw a pixel where the user touched the screen.
#endif
        touched = false;

#elif KEYPAD_BUTTON_COUNT == 4
      buttonPressed = keypad4();
      if( buttonPressed != prevButtonState ){
#elif KEYPAD_BUTTON_COUNT == 12
      buttonPressed = keypad12();
      if( buttonPressed != prevButtonState ){ 
#endif
        
        if( buttonPressed == KEYPAD_BUTTON_COUNT ){ // Menu button is being pressed, which works as cancel if the user is already recording.         
          showMenu();
#ifdef DEBUG                                        
          Serial.print(F("New state: ")); Serial.println(state);
#endif
           
          // Sub menu for deleting the eeprom.
          if( state == DELETE_ALL ){
            Serial.println(F("ERASING STORED SIGNAL DATA"));
            visibleReplayButtonsPage = 0;
            updateDisplay(DELETED_ALL);
            for( int g = EEPROM_STORAGE_START; g <= EEPROM.length(); g++ ){  // Deleting the data stored in the eeprom.
              EEPROM.update(g, 255);
            }
            scanEeprom();                           // Re-index the EEPROM, which in this case sets all stored signal counters back to 0.
            state = LISTENING;
          }
          else if( state == DELETE_LAST ){
            visibleReplayButtonsPage = 0;
            scanEeprom();
            updateDisplay(DELETE_LAST);
            state = LISTENING;
          }
          updateDisplay(state);
        }
        
#ifdef HAS_TOUCH_SCREEN
        else if( buttonPressed <= howManyReplayButtonsWillFitOnScreen ){  // A send-signal button is being pressed, and we're not inside the menu, so we should replay a signal.
          if( buttonPressed <= amountOfStoredReplayableSignals - (visibleReplayButtonsPage * howManyReplayButtonsWillFitOnScreen) ){
            //boolean onOrOff = 0;
            //if( touchX > TOUCHSCREEN_WIDTH / 2 ){   // If the user pressed the right side of the virtual button.
            //  onOrOff = 1;
            //}
            //replay( buttonPressed + (visibleReplayButtonsPage * howManyReplayButtonsWillFitOnScreen), onOrOff ); // The replay function manages the required state change internally.

            if( playlist_position < PLAYLIST_SIZE){ // We only add to the playlist if there is space left in the playlist.
              
              Serial.print(F("-Adding to playlist:")); Serial.print(buttonPressed + (visibleReplayButtonsPage * howManyReplayButtonsWillFitOnScreen));
              playlist_position++;
              if( touchX > TOUCHSCREEN_WIDTH / 2 ){ // Right side of the screen was clicked, so send signal to turn on.
                Serial.println(F(", On"));
                playlist[playlist_position] = (buttonPressed + (visibleReplayButtonsPage * howManyReplayButtonsWillFitOnScreen)) + 100;
              }
              else {
                Serial.println(F(", Off"));
                playlist[playlist_position] = (buttonPressed + (visibleReplayButtonsPage * howManyReplayButtonsWillFitOnScreen));
              }
            }



          }
        }
        else {
          Serial.println(F("MORE>"));
          visibleReplayButtonsPage++;
          if( visibleReplayButtonsPage * howManyReplayButtonsWillFitOnScreen >= amountOfStoredReplayableSignals ){
            visibleReplayButtonsPage = 0;
          }
          updateDisplay(state);
        }
#else   // Basic oled with analog buttons, and a signal replay button has just been pressed.
        else{
          boolean onOrOff = buttonsToggleStatus[signalNumber];
          buttonsToggleStatus[signalNumber] = !buttonsToggleStatus[signalNumber]; // Remember that the button has been toggled.
          replay( buttonPressed, onOrOff );
        }
        prevButtonState = buttonPressed;
#endif
        clearTimingsArray();
      }
    }                                               // End of checking if the menu button is being pressed.
#endif
  }                                                 // End of checking the received signal.
}                                                   // End of main loop.



//
//  VIABILITY CHECK
//
//  The signal should have a minimal level of diversity. This function checks how many different types of timings there are, and how often they exist. 

boolean signalViabilityCheck()
{
#ifdef DEBUG
  Serial.println(F("__Viability check"));
  printRawSignal();
#endif

  while( timings[endPosition] == 0 ){ 
    endPosition--;                                  // If the signal has a lot of zero's at the end, remove them.
    if( endPosition < MINIMAL_SIGNAL_LENGTH ){ break; }
  }

  boolean diverseEnough = false;                    // At the beginning of this function we assume the signal is not diverse enough, until proven otherwise.
  int uniqueCounter = 0;                            // How often a certain timing exists in the timings data.
  bucketCount = 0;

  for( byte s = 0; s < 255; s++ ){                  // Check how often the different timings exist.
    uniqueCounter = 0;
    byte samenumber = 0;
    for( int j = startPosition; j <= endPosition; j++ ){
      if( timings[j] == s ){
        uniqueCounter++;
      }
    }

    if( s == 0 && uniqueCounter > 1 ){
      return false;
    }

    // If there was at least one occurence of this timing, then increase the bucketCount.
    if( s != 0 && uniqueCounter > 1 ){
      //Serial.print(F("- bucket: ")); Serial.print(s); Serial.print(F(" has ")); Serial.println(uniqueCounter);
      bucketCount++;
    }
    
    // If we find a timing that takes up more than 10% of all timings and less than 90%, then this indicates the signal is varied enough.
    if( s > 1 && uniqueCounter > round((endPosition-startPosition) / 10) && uniqueCounter < round((endPosition-startPosition) * .8) && diverseEnough == false ){
      diverseEnough = true;
#ifdef DEBUG
      //Serial.print(F("signal is diverse enough (")); Serial.println(s);
#endif
    }
  }

  
  if( diverseEnough && bucketCount >=2 && bucketCount < 20 ){ // If both indicators are positive, then the signal may be usable.
#ifdef DEBUG
    printRawSignal(); 
    Serial.print(F("Buckets @ quality check: ")); Serial.println(bucketCount);                          
#endif
    return true;
  }
  else{
    return false;                                   // Indicates that the signal was not useable.
  }
}


//
//  SIGNAL CLEANER
//
// This merges timings that are very similar into one. To do this the variable P is used to combine increasingly distant neighbouringing timings together. 
// In the first loop, when P is 1, only direct neighbours are merged (for example: 5 and 6). When P is 2 it could, for example, merge 4 and 6 together. And so forth.
// When P is 2, this is also the first round that we look for the repeating part of the timings. Most signals have some special timings in between the repeating bits. Here we look for it, and point to it with the 'betweenSpace' variable.

boolean signalCleaner()
{
#ifdef DEBUG  
  Serial.println(F("__signalCleaner"));
#endif
  
  for( byte i = 0; i < 8; i++ ){
    metaData[i] = 0;                                // Set all metadata values to 0. Don't want old data messing things up.
  }
  repeatingPatternLength = 255;                     // This is only allowed to become smaller. After all, we want the smallest repeating code.

  // Sorting timings into the most popular buckets.
  int previousCounter = 0;
  betweenSpace = 250;
  byte prevBucketCount = 0;
  byte highestFoundTiming = 0;

  byte p = 1;                                       
  while( p <= 4 ){
#ifdef DEBUG
    Serial.print(F("P")); Serial.println(p);
#endif
    bucketCount = 0;                                // Reset bucket count.
    byte lastOftenFoundTiming = 0;                  // Reset last found neighbouring timing.
    boolean foundBetweenSpaceThisRound = false;
    
    for( byte i = 0; i < betweenSpace + 5; i++ ){   // The "+5" is a failsafe. The idea is it will scan a bit more of the original signal, just in case we find a betweenSpacing that's even better nearby. In practise it's not that useful..
      int currentCounter = 0;
      int lastFoundJ = endPosition;                 // The position in the array of the last timing of this type that we found. We use this to check if there is enough distance between these timings, as the betweenSpaces we're looking for act like spacers between the repeating signals.
      
      for ( int j = endPosition; j >= startPosition; j-- ){ // count all occurrences of this timing in the data
        if( timings[j] == i ){
          currentCounter++;
          
          if( p > 1 && bucketCount > 1 && currentCounter < 10 && foundBetweenSpaceThisRound == false ){ // If this timing was found more than 10 times, it's probably not the between space. J has to be bigger than 5 to allow the code to grab the spacer data later.
            if ( lastFoundJ < endPosition - 3 && lastFoundJ - j > MINIMAL_SIGNAL_LENGTH && lastFoundJ - j < repeatingPatternLength ){  // endPosition - 3 is done so that we are sure we have enough space at the end of the array left over to copy the anomaly data from later.
              // We found it! Determining the betweenSpace is important. It means we're certain we've found the part of the timings array that is the repeating part.
              foundBetweenSpaceThisRound == true;   // No need to keep looking while processing the higher timings.
              betweenSpace = i;
              endPosition = lastFoundJ;             // We remember where the signal we found starter and ended. From now on we'll only be focussing on this small part of the timings array.
              startPosition = j;
              repeatingPatternLength = endPosition - startPosition;
#ifdef DEBUG
              Serial.print(F("repeatingPatternLength found = ")); Serial.println(repeatingPatternLength);
#endif
            }
          }
          lastFoundJ = j;
        }
      }
      if( currentCounter > 0 ){                     // We found at least one occurence of the timing we're currently looking at.
#ifdef DEBUG
        //Serial.print(F("Found ")); Serial.print(i); Serial.print(F(" this often: ")); Serial.println(currentCounter);
#endif
        bucketCount++;
        highestFoundTiming = i;
        if( previousCounter != 0 ){                 // If we also already found another timing on a previous loop though, then we can compare the two.
          if( lastOftenFoundTiming != 0 && lastOftenFoundTiming < i && i - lastOftenFoundTiming <= p && lastOftenFoundTiming != i ){ // If the neighbouring big timing isn't too far away from this one, then we may be able to gobble it up (or let it gobble up the current timing).
            if( currentCounter >= previousCounter ){ // The current timing is found more often in the timings array. Whichever is bigger wil 'win', and the other timing will be set to be the same as the most often occuring of the two.
#ifdef DEBUG
              //Serial.print(F("Shifting timing ")); Serial.print(lastOftenFoundTiming); Serial.print(F(" (found ")); Serial.print(previousCounter); Serial.print(F(" times) UP to ")); Serial.print(i); Serial.print(F(" (found ")); Serial.print(currentCounter); Serial.println(F(" times)."));
#endif
              for( int e = startPosition; e <= endPosition; e++ ){ // Loop over the dataset and change all occurences of the other timing into the current timing.
                if( timings[e] == lastOftenFoundTiming ){ 
                  timings[e] = i;
                }
              }
              //currentCounter = previousCounter + currentCounter; // Reflect that this is now a bigger bucket.
            }
            else{                                   // Previous counter was bigger
#ifdef DEBUG
              //Serial.print(F("Shifting timing ")); Serial.print(i); Serial.print(F(" (found ")); Serial.print(currentCounter); Serial.print(F(" times) DOWN to ")); Serial.print(lastOftenFoundTiming); Serial.print(F(" (found ")); Serial.print(previousCounter); Serial.println(F(" times)."));
#endif
              for( int e = startPosition; e <= endPosition; e++ ){ // loop over the timings and lift the previous ones up.
                if( timings[e] == i ){
                  timings[e] = lastOftenFoundTiming;
                }
              }
            }
            bucketCount--;                          // We just merged two timings together into a single bucket.
          }
        }
        lastOftenFoundTiming = i;                   // Remember what the last timing was..
        previousCounter = currentCounter;           // ..and how often we found it.
      }
    }                                               // End of loop that goes over all timings to count how often each timing exists.
    if( bucketCount != prevBucketCount ){           // If the bucketCount changed, then we may want to comb through the timings array again with the same P setting.
      prevBucketCount = bucketCount;
      if(betweenSpace != 250 && bucketCount < 5){   // We found the betweenSpace and have a cleaned up signal. So we have all that we need.
        p = 5;                                      // Breaks out of the while loop.
      }
    }
    else{                                           // If the bucketcount is unchanged, the current P setting is no longer having any effect, and we should try increasing it.
      //Serial.println(F("Unchanged bucketcount"));
      p++;                                          // By increasing P the next iteration will be allowed to merge timings that are further apart from each other.
    }
    lastOftenFoundTiming = 0;
    if( highestFoundTiming < 10 ){                  // After one round of cleaning the signal we can exit the process here in case the signal is very fast. Otherwise we may acutally damage the timings data by merging them too much.
#ifdef DEBUG
      Serial.println(F("Fast signal"));             // This is a very fast signal (probably without a betweenSpace anyway). Some more expensive modern devices use this. Wireless carkeys, for example.
#endif
      break;
    }
  }                                                 // End of while loop.
#ifdef DEBUG
  printRawSignal();
#endif
  if( betweenSpace == 250 ){                        // 250 is the default value. This means the previous function did not find a between-space indicator. Time to bring in the pattern finder.
    if( state > MENU_NEW && state < MENU_DELETE_LAST ){
      if( findPattern() ){                          // The pattern finder is our last hope. It uses the 'brute force method' to search for a repeating pattern in the signal. It can find repeating patterns when the signal does not have spaces between the repeating parts.
        return true;
      }
    }
    return false;                                   // Signal analysis is impossible.
  }
  else{
    return true;                                    // We found a betweenSpace.
  }
}


//
//  SIGNAL ANALYSIS
//
// Now that the signal is clean we can try to find the byte code it contains. We try to fill the metaData array, which describes the signal, and will be stored to eeprom if necessary.
//
// The medaData array is as follows;
// metaData[0] -> The first timing of the first repeating duo
// metaData[1] -> The second timing of the first repeating duo
// metaData[2] -> The first timing of the second repeating duo
// metaData[3] -> The second timing of the second repeating duo
// metaData[4] -> The first timing of the first anomaly
// metaData[5] -> The second timing of the first anomaly
// metaData[6] -> The first timing of the second anomaly
// metaData[7] -> The second timing of the second anomaly
//
// The code can only store two anomalous bits (which are sent in between the main repeating part of the code)

boolean signalAnalysis()
{
#ifdef DEBUG  
  Serial.println(F("__signalAnalysis"));
#endif
  
  //byte jump = 1;                                  // Used by the function that tries to determine the timing duo's that the repeating signal consists of.
  byte anomalyCount = 0;  
  byte minimumDuosNeeded = int(repeatingPatternLength / 2) - 2;
#ifdef DEBUG
  Serial.print(F("Duos needed: ")); Serial.print(minimumDuosNeeded);
  Serial.print(F(" (repeatingPatternLength: ")); Serial.println(repeatingPatternLength);
#endif
  byte totalDuosPossible = 0;
  for( int i = startPosition; i < startPosition + 5; i++ ){
#ifdef DEBUG
    Serial.print(F("_____i_")); Serial.println(i - startPosition);
#endif
    memset(metaData, 0, sizeof(metaData));          // Clear the metadata array.
    totalDuosPossible = 0;                          // Will check how many timing-duo's we can find in the signal if using the current base duos
    anomalyCount = 0;
    for ( int j = i; j < i + repeatingPatternLength; j = j + 2 ){
      if( timings[j] == metaData[0] && timings[j+1] == metaData[1] ){
        totalDuosPossible++;                        // The current duo was in the metadata, so increase the possible duos counter.
#ifdef DEBUG
        Serial.print( timings[j] ); Serial.print(F(",")); Serial.print( timings[j+1] ); Serial.println(F(" 0"));
#endif
      }
      else if( timings[j] == metaData[2] && timings[j+1] == metaData[3] ){
        totalDuosPossible++;                        // The current duo was in the metadata, so increase the possible duos counter.
#ifdef DEBUG
        Serial.print( timings[j] ); Serial.print(F(",")); Serial.print( timings[j+1] ); Serial.println(F(" 1"));
#endif
      }
      else if( metaData[0] == 0 ){                  // If it wasn't in the metadata, add it to the metadata, and register that we did in fact find a possible duo.
        metaData[0] = timings[j];
        metaData[1] = timings[j+1];
        totalDuosPossible++;
#ifdef DEBUG
        Serial.print( timings[j] ); Serial.print(F(",")); Serial.print( timings[j+1] ); Serial.println(F(" 0 <- Duo A")); 
#endif
      }
      else if( metaData[2] == 0 ){
        metaData[2] = timings[j];
        metaData[3] = timings[j+1];
        totalDuosPossible++;
#ifdef DEBUG
        Serial.print( timings[j] ); Serial.print(F(",")); Serial.print( timings[j+1] ); Serial.println(F(" 1 <- Duo B")); 
#endif
      }
      else if( metaData[4] == 0 ){
        metaData[4] = timings[j];
        metaData[5] = timings[j+1];
        anomalyCount++;
#ifdef DEBUG
        Serial.print( metaData[4] ); Serial.print(F(",")); Serial.print( metaData[5] ); Serial.println(F(" <- Anomaly A"));       
#endif
        
        if( totalDuosPossible < minimumDuosNeeded){
          j = 1000;                                 // Anomaly found too soon, should be at the end of the signal, after all the duos are found. So we can skip checking this.
        }
        else if( (totalDuosPossible * 2) + (anomalyCount * 2) < repeatingPatternLength ){
          metaData[6] = timings[j+2];
          metaData[7] = timings[j+3];
          anomalyCount++;
#ifdef DEBUG
        Serial.print( metaData[6] ); Serial.print(F(",")); Serial.print( metaData[7] ); Serial.println(F(" <- Anomaly B"));         
#endif
        }
      }
      else{
        //Serial.println(F("bad round"));
        j = 1000;                                   // Too many anomalies (the code can handle 2). Skip to the next round, maybe we will have better luck.
      }
#ifdef DEBUG
      if( totalDuosPossible % 8 == 0 ){ Serial.println(); }
#endif

    }
    if( totalDuosPossible >= minimumDuosNeeded ){
#ifdef DEBUG
      Serial.print(F("Enough duos found (")); Serial.println(totalDuosPossible);
      Serial.println(metaData[0]);
      Serial.println(metaData[1]);
      Serial.println(metaData[2]);
      Serial.println(metaData[3]);
#endif
      endPosition = endPosition + (i - startPosition); // Shifting the window a little bit, if necessary. 
      startPosition = startPosition + (i - startPosition); // Shifting the window a little bit, if necessary.


      Serial.println();
      Serial.println();
      for ( int s = startPosition; s <= endPosition; s++ ){

        Serial.print(timings[s]); Serial.print(F(","));
        if(s % 8 == 0){ Serial.println(); }
      }
      Serial.println(); Serial.println();
      
      break;
    }
    else {
#ifdef DEBUG
      Serial.println(F("Not enough possible duos")); Serial.println();
#endif
      if( i - startPosition == 4 ){
        return false;                               // What's this all about?
      }
    }
  }

  // Switch the duo's around in necessary, so that the one the has a difference in timings is the first one. This will save a byte in storage later.
  if( metaData[0] == metaData[1] == metaData[2] && metaData[1] < metaData[3]){
    metaData[1] = metaData[3];
    metaData[3] = metaData[0];
  }

  repeatingSignalByteLength = (byte)totalDuosPossible / 8;
  
  byte leftovers = totalDuosPossible % 8;

#ifdef DEBUG
  Serial.print(F("leftover:")); Serial.println( leftovers );
  Serial.print(F("anomalyCount:")); Serial.println( anomalyCount );
#endif

  lastByteIsSplit = false;
  anomalyInside = false;
  if( leftovers > 0 ){
    repeatingSignalByteLength++;                    // If there is a nibble (half a byte) left over, then increase the required array length by one.
    
    if( leftovers == 4 ){
      lastByteIsSplit = true;
      //Serial.println(F("Last byte is split"));
    }
    else if( leftovers + anomalyCount == 4 ){
      lastByteIsSplit = true;
      anomalyInside = true;
      //Serial.println(F("anomaly is inside nibble"));
    }
    else if( leftovers + anomalyCount == 8 ){
      anomalyInside = true;
      //Serial.println(F("anomaly is inside byte"));
    }   
    else {
      Serial.println(F("Cannot handle this signal"));
      return false;                                 // The system cannot currently deal with this signal, sorry.
    }
  }
  
  // Take 8 bits and encode them into a byte.
  for( byte i = 0; i < repeatingSignalByteLength; i++ ){
    byte eepromByte = 0;
    for( int j = 0; j < 8; j++ ){
      if( timings[startPosition] == metaData[0] && timings[startPosition + 1] == metaData[1] ){
        Serial.print(F("0"));
        bitWrite(eepromByte,j,0);
        startPosition = startPosition + 2;        
      }
      else if( timings[startPosition] == metaData[2] && timings[startPosition + 1] == metaData[3] ){
        Serial.print(F("1"));
        bitWrite(eepromByte,j,1);
        startPosition = startPosition + 2;
      }
      else{
        Serial.print(F("X"));
        bitWrite(eepromByte,j,0);
      }                                             // End of bit loop
    }                                               // End of byte loop
    timings[i] = eepromByte;
//#ifdef DEBUG
    Serial.print(F(" > HEX: ")); 
//#endif
    Serial.print(eepromByte, HEX);
//#ifdef DEBUG
    Serial.print(F("  DEC: ")); Serial.println(eepromByte); 
//#endif
  }
  Serial.println();
  return true;                                      // return if the signal analysis went ok.
}


//
//  PATTERN FINDER
//
//  Finds repeating patterns in the signal data. This is a backup function. Some signals don't have a betweenSpace. In those cases we use brute force to look for the longest repeating pattern we can find. This function takes longer.

boolean findPattern()
{
#ifdef DEBUG
  Serial.println(F("___pattern_finder___"));
#endif  
  byte maxPatternLength = (byte)constrain((endPosition-startPosition)/2, 31, 255); // what length is the pattern we're looking for allowed to be? It's most likely 8 bytes = 64 bits = 128 timings, but we can't be sure in this case. To find a repeating pattern, we need to be able to have it in the data twice, so the maximum length is the timings array length divided by two.
  byte searchLength = 32;                         
  while( searchLength + 4 <= maxPatternLength ){  // Check the maximum pattern length we can search for in the current memory.
    searchLength = searchLength + 4;
  }
  //Serial.print(F(" searchLength that we begin pattern matching with: ")); Serial.println(searchLength);
  for( searchLength; searchLength > MINIMAL_SIGNAL_LENGTH; searchLength = searchLength - 4 ){ // Find repeating patterns of a minimal length, starting with a long as possible signal, and then working down.
    int patternFirstPosition = 0;                 // Where we found an often occuring pattern for the first time. To keep things simple(low memory) we only search the first 255 positions of the array.
    for ( int i = startPosition; i <= endPosition-searchLength; i++ ){
      byte patternCount = 0;
      for ( int y = startPosition; y <= endPosition-searchLength; y++ ) {
        boolean oksofar = true;                   // Starts out assuming the pattern is found, and then starts comparing. As soon as one of the timings is not the same, it sets this to false.
        for (byte r = 0; r < searchLength; r++) { // We compare the selected patterns
          if( timings[i+r] != timings[y+r] ){
            oksofar = false;                      // If the pattern is not found at this position.
            break;                                // No need continuing the comparison.
          }
        }
        if( oksofar == true ){
          patternCount++;                         // We scanned over the position in the array, and nothing tripped up the recogniser, meaning it actuallly found the pattern.
          y = y + (searchLength-1);               // Minus one, because the for-loop will also add one. We skip ahead and see if we can find the same pattern again straigth after.
        }
      }
      if(patternCount > 1){                       // We found a repeating pattern!
        // Quick quality check. It makes sure the found code isn't just the same number in a row a lot.
        int sameNumber = 0;
        for( int j = startPosition; j <= endPosition; j++ ){
          if( timings[j] == timings[startPosition] ){
            sameNumber++;
          }
        }
        if(sameNumber > 15){                      // This many of the same timings in a row is a bad sign.
          return false;
        }
        Serial.print(F("Pattern: "));
        for (int t = 0; t < searchLength; t++) {
          Serial.print(timings[i + t]); Serial.print(F(",")); 
        }
        Serial.println();
        // Update the global variables:
        repeatingPatternLength = searchLength;    // Store the length of the repeating part.
        startPosition = i;                        // Set the start position of the repeating part of the signal.
        endPosition = i + searchLength;           // Set the end position of the repeating oart of the signal.
        return true;
      }
    }
  }
  //Serial.println(F("Pattern finder failed"));
  return false;                                     // We reached the end without finding any repeating pattern.
}


//
//  SCAN EEPROM
//
// This function has multiple uses, depending on the state. 
// When signalNumber is given as 0, then it acts as a simple scan of the eeprom, and can compare found signals to those in eeprom.
// If state is listening, then it returns the number of a signal mathed in eeprom.
byte scanEeprom()
{
  //Serial.println(F("SCANNING EEPROM"));

  byte whatWeCameHereFor = 0;
  amountOfStoredSignals = 0;                        // Reset these. Everytime this function runs they are updated.
  byte amountOfStoredReplayableSignalsScan = 0;
  positionOfLastSignalEnd = EEPROM_STORAGE_START - 1; // MySensors has another value we could grab to get even more space.
  boolean finishedScanningEeprom = false;
  while( finishedScanningEeprom == false ){
    //Serial.println(positionOfLastSignalEnd);
    byte storedSignalLength = EEPROM.read(positionOfLastSignalEnd + 1); // This get the first byte of the next stored signal (if it exists), and indicates how much data it takes on the eeprom.
#ifdef DEBUG
    Serial.print(F("#")); Serial.println(amountOfStoredSignals + 1);
    Serial.print(F("-storedlength:"));Serial.println(storedSignalLength);
#endif
    if( storedSignalLength == 0xff || storedSignalLength == 0x00 ){ // No more signals found in the EEPROM.
      finishedScanningEeprom = true;
    }

    // For each signal in eeprom:
    else {
      //if( positionOfLastSignalEnd + storedSignalLength > EEPROM.length() ){ break; } // This should theoretically never happen. But if it did it would lock up the device, so it can't hurt.
      // DELETE THE LAST RECORDED SIGNAL?
      if( state == DELETE_LAST && EEPROM.read(positionOfLastSignalEnd + 1 + storedSignalLength) == 0xFF ){ // If the next storage slot empty?
        Serial.println(F("Deleting last stored signal"));
        for( int j = positionOfLastSignalEnd + 1; j < positionOfLastSignalEnd + 1 + storedSignalLength; j++ ){ // Overwrite the last signal with 255's (the default memory state).
          EEPROM.update(j, 0xFF);
        }
        return 1;
      }
      if( storedSignalLength > 0 ){
        amountOfStoredSignals++;                    // Update how many signals are stored in eeprom.
      }
      byte descriptionData = EEPROM.read(positionOfLastSignalEnd + 2);
      if( bitRead(descriptionData, DESCRIPTION_REPLAYABLE) == 1 ){  // Is it a replayable signal?
        amountOfStoredReplayableSignalsScan++;
        if( state == REPLAYING && amountOfStoredReplayableSignalsScan == buttonPressed ){
#ifdef DEBUG
          Serial.println(F("Eeprom scan found the signal to replay"));
#endif
          whatWeCameHereFor = storedSignalLength;
          startPosition = positionOfLastSignalEnd + 1; // Recycling variables to save memory.
          endPosition = positionOfLastSignalEnd + storedSignalLength;
        }

#ifdef HAS_TOUCH_SCREEN
        if( state == DISPLAY_REPLAY_BUTTONS ){
          // The signal is replayable. If a touchscreen is attached, we show the button for it on the screen.
          byte pageStart = visibleReplayButtonsPage * howManyReplayButtonsWillFitOnScreen;
          byte itemNumber = (amountOfStoredReplayableSignalsScan - 1 ) % howManyReplayButtonsWillFitOnScreen;  

          //Serial.print(F("howManyReplayButtonsWillFitOnScreen: ")); Serial.println(howManyReplayButtonsWillFitOnScreen);
          //Serial.print(F("amountOfStoredReplayableSignals so far: ")); Serial.println(amountOfStoredReplayableSignalsScan);
          //Serial.print(F("page start: "));Serial.println( pageStart );
          //Serial.print(F("replayable. itemNumber: ")); Serial.println( itemNumber );
          
          if( amountOfStoredReplayableSignalsScan > pageStart && amountOfStoredReplayableSignalsScan <= pageStart + howManyReplayButtonsWillFitOnScreen ){
            if( bitRead(descriptionData, DESCRIPTION_ON_OFF) == 0 ){
#ifdef DEBUG_SCREEN
          Serial.println(F("BC: 4 commands for next page button:"));
#endif
              roundedRectangle( 0, BUTTON_HEIGHT + (itemNumber * BUTTON_HEIGHT), TOUCHSCREEN_WIDTH, BUTTON_HEIGHT, (int)BUTTON_HEIGHT/3, 31727 ); // dark grey rounded rectangle.
              setCur( BUTTON_PADDING, BUTTON_PADDING + BUTTON_HEIGHT + (itemNumber * BUTTON_HEIGHT) );
              displayNumber(itemNumber + (visibleReplayButtonsPage * howManyReplayButtonsWillFitOnScreen) + 1 );
              basicCommand(play);
            }
            else {
#ifdef DEBUG_SCREEN
          Serial.println(F("BC: roundedRect"));
#endif
              roundedRectangle( 0, BUTTON_HEIGHT + (itemNumber * BUTTON_HEIGHT), (int)TOUCHSCREEN_WIDTH/2, BUTTON_HEIGHT, (int)BUTTON_HEIGHT/3, 31727 ); // dark grey rounded rectangle.
#ifdef DEBUG_SCREEN
          Serial.println(F("BC: setCur"));
#endif
              setCur( BUTTON_PADDING, BUTTON_PADDING + BUTTON_HEIGHT + (itemNumber * BUTTON_HEIGHT) );
#ifdef DEBUG_SCREEN
          Serial.println(F("BC: display_number"));
#endif
              displayNumber(itemNumber + (visibleReplayButtonsPage * howManyReplayButtonsWillFitOnScreen) + 1 );
#ifdef DEBUG_SCREEN
          Serial.println(F("BC: off"));
#endif
              basicCommand(off);
#ifdef DEBUG_SCREEN
          Serial.println(F("BC: roundedRect"));
#endif
              roundedRectangle( (int)TOUCHSCREEN_WIDTH/2, BUTTON_HEIGHT + (itemNumber * BUTTON_HEIGHT), (int)TOUCHSCREEN_WIDTH/2, BUTTON_HEIGHT, (int)BUTTON_HEIGHT/3, 31727 ); // dark grey rounded rectangle.
#ifdef DEBUG_SCREEN
          Serial.println(F("BC: setcur"));
#endif
              setCur( BUTTON_PADDING + (TOUCHSCREEN_WIDTH / 2), BUTTON_PADDING + BUTTON_HEIGHT + (itemNumber * BUTTON_HEIGHT) );
#ifdef DEBUG_SCREEN
          Serial.println(F("BC: display_number"));
#endif
              displayNumber(itemNumber + (visibleReplayButtonsPage * howManyReplayButtonsWillFitOnScreen) + 1 );
#ifdef DEBUG_SCREEN
          Serial.println(F("BC: on"));
#endif
              basicCommand(on);
            }
          }
          //else {
            //Serial.println(F("(skip)"));
          //}
        }
#endif

      }
      else {
#ifdef DEBUG
        Serial.println(F("-detectable"));
#endif
      }
      // LISTENING
      positionOfLastSignalEnd = positionOfLastSignalEnd + storedSignalLength;
      if( state == LISTENING && whatWeCameHereFor == 0 && bitRead(descriptionData, DESCRIPTION_REPLAYABLE) == 0 ){ // We only check detect-only signals for now. This could be changed to include all signals by removing the last check in this if statement.
        // We should scan the stored binary data against the last signal we received.
        boolean areTheyTheSame = true;            // The code below continously tries to disprove that they are the same, and skips ahead at the first evidence that this is the case.
        for( byte i = 0; i < 1 + bitRead(descriptionData, DESCRIPTION_ON_OFF); i++ ){ // Check both the on and off signals.
          areTheyTheSame = true;
          for( byte j = 0; j < repeatingSignalByteLength; j++ ){
            if( timings[(repeatingSignalByteLength - j) - 1] != EEPROM.read((positionOfLastSignalEnd - (repeatingSignalByteLength*i)) - j ) ){
              areTheyTheSame = false;
              break;
            }
          }
          if( areTheyTheSame == true ){           // We've compared the entire signal, and.. they are the same!
            Serial.println(F("Match"));
            Serial.print(F("SEND: Toggle ")); Serial.print(99 + (amountOfStoredSignals - amountOfStoredReplayableSignalsScan)); Serial.print(F(" to ")); Serial.println(i);
            
            if( bitRead(descriptionData, DESCRIPTION_ON_OFF) ){ // This is an on-off type detection, so we just toggle it to the correct position.
              //Serial.println(F("-simple-on-off-toggle"));
              connectedToNetwork = send(detectmsg.setSensor(99 + (amountOfStoredSignals - amountOfStoredReplayableSignalsScan)).set(!i),1); wait(RADIO_DELAY); // This sends the found value to the server. If the signal is an on-off version, it sends the correct value (which needs to be inversed, hence the !i). It also asks for a receipt (the 1 at the end), so that it acts as a network status detection at the same time.

            }
            else{                                 // This is a simple single trigger-type detection.
              //Serial.println(F("-simple single trigger, will go back to off by itself."));
              connectedToNetwork = send(detectmsg.setSensor(99 + (amountOfStoredSignals - amountOfStoredReplayableSignalsScan)).set(1),1); wait(RADIO_DELAY); // This sends the found value to the server. If the signal is an on-off version, it sends the correct value (which needs to be inversed, hence the !i). It also asks for a receipt (the 1 at the end), so that it acts as a network status detection at the same time.
              wait(2000);
              send(detectmsg.setSensor(99 + (amountOfStoredSignals - amountOfStoredReplayableSignalsScan)).set(0),1); wait(RADIO_DELAY); // ... and turn if back off again at the controller.
            }
            updateDisplay(MATCH);
            whatWeCameHereFor = amountOfStoredSignals; // Sending back the index of this signal. 
            
            break;                                // Just in case that this is a double signal, then we don't want a second loop to overrule this result.
          }
        }
      } // end of detectable
    } // end of 'is a signal'                     // end of checking a stored signal.
  }                                                 // end of while loop, so the entire EEPROM has now been scanned.
  amountOfStoredReplayableSignals = amountOfStoredReplayableSignalsScan;
  return whatWeCameHereFor;
}


//
//  STORE SIGNAL
//

boolean writeSignalToEeprom()
{
  Serial.println(F("__Storing"));
  byte signalDescriber = 0;                         // This will hold all the setings for this signal. The 'Description byte' is basically a settings switchboard that describes the signal. It helps to keep the required eeprom storage low.
#if KEYPAD_BUTTON_COUNT > 0
  if( (state == COPYING_ON || state == COPYING_SIMPLE) && amountOfStoredReplayableSignals == KEYPAD_BUTTON_COUNT - 1 ){
    updateDisplay(NO_MORE_FREE_BUTTONS);            // Warn the user: there are no more physical buttons left.
  }
#endif
  // TODO?: If this is an on-off situation, maybe get the previous signal from storage and make sure that the meta data is similar? By doing a check we could make sure no rogue signals accidentally interfere with the recording proces. The odds of that happening are pretty slim though..
  byte spaceNeeded = 2;                             // We calculate how much space we need to store everything, and then check if it will still fit in memory. The minimal metadata length we can possibly start with is 2. One byte for signal length, and another for the signal describer

/*
 * THE COMPLETE DATA TO STORE
 * This is what a stored signal in the EEPROM looks like:
 * 
 * byte 0. How many of the eeprom's bytes are used to store the signal.
 * byte 1. Signal describer. Stores settings. See below for details.
 * 
 * The next part stores the metaData array. For 'recognise only' signals this is skipped.
 * byte 3. Most popular bit-duo part A. Together they are binary 0. (only stored if the signal should be replayed)
 * byte 4. Most popular bit-duo part B. Together they are binary 0. (only stored if the signal should be replayed)
 * byte 5. Second most popular bit-duo part A. Together they are binary 1. (optional, depending on signal and state)
 * byte 6. Second most popular bit-duo part B. Together they are binary 1. (optional, depending on signal and state)
 * byte 7. First anomaly duo. (optional, depending on signal and state)
 * byte 8. First anomaly duo. (optional, depending on signal and state)
 * byte 9. Second anomaly duo. (optional, depending on signal and state)
 * byte 10. Second anomaly duo. (optional, depending on signal and state)
 * 
 * byte 11 and onwards: stores the actual repeating part of the signal. Each byte has 8 bits, and these 8 bits can be expanded into the original signal.
 *
 * If this is an on+off signal, then it will be appended, but only it's repeating signal data.
 *
 * 
 * Signal describer byte:
 * bit 7. - Since there is a spacer, is it 2 bit (0) or 4 bit (1) spacer?
 * bit 6. - Is there a spacer anomaly? Usually there is. No (0)  or Yes (1). 
 * bit 5. - Can one of the duo's be reconstructed from the other one? No (1) or yes (0). If yes, then we can save 2 bytes, and must also store what type of reconstruction can be done (there are two options fo this).
 * bit 4. - Simple read-only signal (0), or are we planning to transmit it (1)? If it's read-only, then we only store the signal spacer, signal description (so we can now it's a simple signal), and the repeating signal array. A normal, simple 8 byte signal would thus take 10 bytes of storage on the EEPROM.
 * bit 3. - Is this a singular signal (0), or combined on/off signal (1). (The repeating signal payload is then doubled, and should later be split in half.)
 * bit 2. - Currently not used. Perhaps in the future this could be used to store if a replayable signal should also operate as a recognise-only signal
 * bit 1. - What type of duo reconstruction can be done? This depends on bit 5 being set to 0, indicating a duo reconstruction should be performed. If they are mirrored, then (0) is stored. Is the first duo just twice the first timing of the second duo (e.g. if it is 2&2 + 2&7), then a 1 is stored.
 * bit 0. - Set to 1 if the last byte of the repeating pattern is only half used.
 */
#ifdef DEBUG
  Serial.print(metaData[0]); Serial.println(F(" most popular bit-duo part A (0)"));
  Serial.print(metaData[1]); Serial.println(F(" most popular bit-duo part A (0)"));
  Serial.print(metaData[2]); Serial.println(F(" most popular bit-duo part B (1)"));
  Serial.print(metaData[3]); Serial.println(F(" most popular bit-duo part B (1)"));
  Serial.print(metaData[4]); Serial.println(F(" spacer anomaly 1"));
  Serial.print(metaData[5]); Serial.println(F(" spacer anomaly 2"));
  Serial.print(metaData[6]); Serial.println(F(" spacer anomaly 3"));
  Serial.print(metaData[7]); Serial.println(F(" spacer anomaly 4"));
#endif
  if( state != COPYING_OFF && state != LEARNING_OFF ){ // We only create the description bit on the ON part of an ON/OFF signal. The OFF part is just directly appended to the eeprom later.
    // IS THE LAST BYTE SPLIT? (Uneven number of tri-bits)
    if( lastByteIsSplit ){
      bitWrite(signalDescriber, DESCRIPTION_HALFBYTE , 1);   // We use this position to remember if there is an even or uneven amount of 'nibbles' (a nibble is half a byte).
    }
    // IS THE LAST ANOMALY "inside" the last nibble, or does it follow it?
    if( anomalyInside ){
      bitWrite(signalDescriber, DESCRIPTION_ANOMALY_INSIDE , 1);   // We use this position to remember if there is an even or uneven amount of 'nibbles' (a nibble is half a byte).
    }
    // ARE WE STORING AN ON/OFF SIGNAL?
    if( state == COPYING_ON || state == LEARNING_ON ){
      bitWrite(signalDescriber, DESCRIPTION_ON_OFF , 1);   // We use this position to remember if this is an ON/OFF signal.
      spaceNeeded = spaceNeeded + (repeatingSignalByteLength * 2); // If this is an on-off signal, then we will need to store the repeating part twice.
    } else {
      spaceNeeded = spaceNeeded + repeatingSignalByteLength;
    }
    // SIMPLE OR REPLAY? If it's simple, then all additional description bits can stay at 0.
    if( state == COPYING_ON || state == COPYING_SIMPLE ){
      bitWrite(signalDescriber, DESCRIPTION_REPLAYABLE , 1); // We use this position to remember if this is a recognise-only signal(0), or if it's a signal that can be replayed(1). 
      if( metaData[2] == metaData[1] && metaData[3] == metaData[0] ){
        //Serial.println(F("A duo can be reconstructed. Duo 2 is the mirror image of duo 1."));
        spaceNeeded = spaceNeeded + 2;
      }
      else if( metaData[0] == metaData[2] == metaData[3] ){
        //Serial.println(F("A duo can be reconstructed. Duo 2 is just twice the first timing of duo 1"));
        bitWrite(signalDescriber, DESCRIPTION_DUO_RECONSTRUCTION_TYPE , 1);
        spaceNeeded = spaceNeeded + 2;
      }
      else{
        // There is no clever way to store the duo's using less storage space.
        bitWrite(signalDescriber, DESCRIPTION_UNRECONSTRUCTABLE_DUO , 1); // We use this position to remember if a duo can be reconstructed (0) or not (1).
        spaceNeeded = spaceNeeded + 4;
      }
      // ANOMALY?
      if( metaData[4] != 0 && metaData[5] != 0 ){
        bitWrite(signalDescriber, DESCRIPTION_ANOMALY , 1); // We use this position to remember that there is at least one part anomaly.
        spaceNeeded = spaceNeeded + 2;
        if( metaData[6] != 0 && metaData[7] != 0 ){
          bitWrite(signalDescriber, DESCRIPTION_LARGE_ANOMALY , 1); // We use this position to remember that this is a double size anomaly.
          spaceNeeded = spaceNeeded + 2;
        }
      }
      
#ifdef DEBUG
      Serial.print(F("Signal description byte (")); Serial.println(signalDescriber);
      for (byte i = 0; i < 8; i++) {
        Serial.print(i); Serial.print(F(" = ")); Serial.println(bitRead(signalDescriber,i));
      }
      Serial.print(F("Storage space needed: ")); Serial.println(spaceNeeded);
      Serial.print(F("Storage space left: ")); Serial.println( EEPROM.length() - positionOfLastSignalEnd );
#endif
    }
  } // End of check if we're storing a single or double signal.
  if( spaceNeeded > 0 && positionOfLastSignalEnd + spaceNeeded < EEPROM.length() ){ // If there is enough space to store the new signal. E2END+1 would also work instead of EEPROM.length();
    Serial.println(F("Enough space left"));
    if( state != COPYING_OFF && state != LEARNING_OFF ){
      // Storing a new signal. Moving pointer to the next free space in the EEPROM.
      positionOfLastSignalEnd++;
      EEPROM.update(positionOfLastSignalEnd, spaceNeeded); // Here we store the length of all the data associated with this recording session.
      //Serial.print(positionOfLastSignalEnd); Serial.print(F(" ~~> "));Serial.print(spaceNeeded); Serial.println(F(" (length)"));
      positionOfLastSignalEnd++;
      EEPROM.update(positionOfLastSignalEnd, signalDescriber); // Here we store the signal describer which holds all the settings needed to recreate this signal.
      //Serial.print(positionOfLastSignalEnd); Serial.print(F(" ~~> "));Serial.print(signalDescriber); Serial.println(F(" (describer)"));
      /*
      Here we store the metaData array. This is how this loop works:
      0 -> bit4 (if replaying) -> metadata 1&2 must be stored
      1 -> bit5 (if a duo can not be cleverly reconstructed from the other) -> metadata 3&4 must be stored
      2 -> bit6 (if there is at least one part anomaly -> metadata 5&6 must be stored
      3 -> bit7 (if second part anomaly exists) -> metadata 7&8 must be stored.
      */
      if( bitRead(signalDescriber, DESCRIPTION_REPLAYABLE) == 1 ){ // If we want to be able to replay this signal, then we should store the meta data.
        for( byte i = 0; i < 4; i++ ){              // Looping over the signalDescriber and the metaData array at the same time.
          if( bitRead(signalDescriber, i+4) == 1 ){ // 4, 5, 6, 7
            positionOfLastSignalEnd++;
            EEPROM.update(positionOfLastSignalEnd, metaData[i*2]);
            //Serial.print(positionOfLastSignalEnd); Serial.print(F(" ~> ")); Serial.print(metaData[i*2]); Serial.println(F(" (meta)"));
            positionOfLastSignalEnd++;
            EEPROM.update(positionOfLastSignalEnd, metaData[i*2 + 1]);
            //Serial.print(positionOfLastSignalEnd); Serial.print(F(" ~> ")); Serial.print(metaData[i*2 + 1]); Serial.println(F(" (meta)"));
          }
        }
      }
    }                                               // End of check if this is part two of an on/off signal.    
    // Add the repeating part of the signal to the storage.
    for( byte i = 0; i < repeatingSignalByteLength; i++ ){
      positionOfLastSignalEnd++;
      Serial.print(positionOfLastSignalEnd); Serial.print(F(" ~~> "));Serial.println(timings[i]);
      EEPROM.update(positionOfLastSignalEnd, timings[i]);
    }
    if( state != COPYING_OFF && state != LEARNING_OFF ){
      amountOfStoredSignals++;                      // On/off signals only count as one because they would be attached to one button.
    }
  }
  else{
    return false;                                   // Unable to store the signal. It wasn't long enough or there was not enough space for it.
  }
  send(textmsg.setSensor(DEVICE_STATUS_ID).set( F("Signal stored OK") ));
  return true;                                      // Succesfully stored the signal
}



//
//  REPLAY
//

void replay(byte signalNumber, boolean onOrOff)
{
  Serial.print(F("__replaying_")); Serial.println(signalNumber);
  state = REPLAYING;
  //boolean onOrOff = 0;
  buttonPressed = signalNumber;
  repeatingPatternLength = 0;
  if( signalNumber == 0 ){ return; }                // We cannot play signal 0. Just a (superfluous) safeguard.
  byte storedSignalLength = scanEeprom();
  //Serial.print(F("storedSignalLength received from scanEeprom function: ")); Serial.println(storedSignalLength);
  if( storedSignalLength && amountOfStoredReplayableSignals > 0 ){

    saveState(signalNumber, onOrOff);             // We save the new current toggle state in the eeprom too.
    send(buttonmsg.setSensor(signalNumber + 9).set(onOrOff)); wait(RADIO_DELAY); // Tell the controller in what state the child is.
    send(textmsg.setSensor(DEVICE_STATUS_ID).set( F("Playing..") ));
    updateDisplay(REPLAYING);
    for( byte i = 0; i < 8; i++ ){
      metaData[i] = 0;                              // Reset all metadata values.
    }
    byte eepromReadPosition = 4;                    // Where in the eeprom signal we are reading back the data.
    byte restoredSignalLength = 0;                  // Pointer to where in the timings array the signal has been reconstructed.
    byte signalDescription = EEPROM.read(startPosition+1);
    lastByteIsSplit = bitRead(signalDescription, DESCRIPTION_HALFBYTE);
    anomalyInside = bitRead(signalDescription, DESCRIPTION_ANOMALY_INSIDE);

#ifdef DEBUG
    Serial.print(F("Data from EEPROM (length: ")); Serial.println(storedSignalLength);
    for( int i = startPosition; i <= endPosition; i++ ){              
      Serial.println( EEPROM.read(i) );
    }
#endif

#if defined HAS_BASIC_OLED_SCREEN && KEYPAD_BUTTON_COUNT > 0 && defined DEBUG
    // Show on the screen if the ON or OFF signal is being played.
    if( bitRead(signalDescription, DESCRIPTION_ON_OFF) ){
      oled.set1X();
      oled.setCursor(0,6);
      oled.print(F("part "));
      oled.print(buttonsToggleStatus[signalNumber] + 1);
    }
#endif

    repeatingPatternLength = storedSignalLength - 4;// This indicates how many of the bytes in the eeprom describe the actual repeating signal.
    metaData[0] = EEPROM.read( startPosition + 2 ); // This duo is always reconstructed from the eeprom data.
    metaData[1] = EEPROM.read( startPosition + 3 );
    if( bitRead(signalDescription,DESCRIPTION_UNRECONSTRUCTABLE_DUO) == 0 ){  // The second duo must be reconstructed because an eeprom space saving measure was used.
      if( bitRead(signalDescription,DESCRIPTION_UNRECONSTRUCTABLE_DUO) == 0 ){  // The second duo is a mirror image of the first duo. To reconstruct the second duo we just take the first duo and flip it.
        metaData[2] = metaData[1];
        metaData[3] = metaData[0];
      }
      else{                                         // The second duo is just twice the first timing of the first duo.
        metaData[2] = metaData[0];
        metaData[3] = metaData[0];
      }
    } 
    else {
      metaData[2] = EEPROM.read( startPosition + eepromReadPosition ); // If the second duo cannot be reconstructed from the first, then the first timing of the second duo can always be found at position 4 in the eeprom.
      eepromReadPosition++;
      metaData[3] = EEPROM.read( startPosition + eepromReadPosition ); // If the second duo cannot be reconstructed from the first, then the second timing of the second duo can always be found at position 5 in the eeprom.
      eepromReadPosition++;
      repeatingPatternLength = repeatingPatternLength - 2; // Since two of the bytes in eeprom were there to reconstruct the meta data, the actual repeating part must be a little shorter.
    }

#ifdef DEBUG
    Serial.println(F("Reconstructed duo metaData:"));
    for( byte w = 0; w < 4; w++ ){
      Serial.println(metaData[w]);
    }
#endif

    // The anomaly can be placed directly into the timings array.
    if( bitRead(signalDescription,DESCRIPTION_ANOMALY) == 1 ){ // Reconstruct part 1 of anomaly.
      repeatingPatternLength = repeatingPatternLength - 2;  // How many bytes in the eeprom actually describe the repeating signal is now reduced, since some of those bytes described the anomaly.
      timings[restoredSignalLength] = EEPROM.read( startPosition + eepromReadPosition ); // Eeprom position here is 4 or 6, depending on whether the second repeating duo was a reconstructable or not.
      restoredSignalLength++;                       
      eepromReadPosition++;
      timings[restoredSignalLength] = EEPROM.read( startPosition + eepromReadPosition ); // Eeprom position here is 5 or 7, depending on whether the second repeating duo was a reconstructable or not.
      restoredSignalLength++;
      eepromReadPosition++;
#ifdef DEBUG
      Serial.println(F("Reconstructed betweenSpace anomaly: "));
      Serial.print(F("0 => ")); Serial.println(timings[0]);
      Serial.print(F("1 => ")); Serial.println(timings[1]);    
#endif      
    }
    if( bitRead(signalDescription,DESCRIPTION_LARGE_ANOMALY) == 1 ){ // Reconstruct part 2 of anomaly.
      repeatingPatternLength = repeatingPatternLength - 2; // Since two of the bytes in eeprom were there to reconstruct the meta data, the actual repeating part must be a little shorter.
      timings[restoredSignalLength] = EEPROM.read( startPosition + eepromReadPosition );
      eepromReadPosition++;
      restoredSignalLength++;
      timings[restoredSignalLength] = EEPROM.read( startPosition + eepromReadPosition );
      eepromReadPosition++;
      restoredSignalLength++;
#ifdef DEBUG
      Serial.print(F("2 => ")); Serial.println(timings[2]);
      Serial.print(F("3 => ")); Serial.println(timings[3]);    
#endif
    }
#ifdef DEBUG
    Serial.println();
    Serial.print(F(">> restoredSignalLength ")); Serial.println(restoredSignalLength); Serial.println();
#endif

    if( bitRead(signalDescription, DESCRIPTION_ON_OFF) ){ // If it's an on+off signal then the repeating part should be cut in half to get the actual length, since they are stored back to back in the eeprom.
      repeatingPatternLength = repeatingPatternLength / 2;
    }
    // Looping over the EEPROM, with some special construction that switches which signal should be reconstructed based on the toggle state of the button.
    for( int i = (endPosition - repeatingPatternLength * (1+onOrOff)) + 1; i <= endPosition - (repeatingPatternLength * onOrOff); i++ ){              
      // Here we can reconstruct the original signal.
#ifdef DEBUG
      Serial.print(F(" byte ")); Serial.print(i); Serial.print(F(" = ")); Serial.println( EEPROM.read(i) );
#endif
      for( byte j = 0; j < 8; j++ ){
#ifdef DEBUG
        Serial.print(restoredSignalLength); Serial.print(F(" => ")); Serial.println( metaData[ bitRead(EEPROM.read(i),j) * 2 ] );
#endif
        timings[restoredSignalLength] = metaData[ bitRead(EEPROM.read(i),j) * 2 ];
        restoredSignalLength++;
#ifdef DEBUG
        Serial.print(restoredSignalLength); Serial.print(F(" => ")); Serial.println( metaData[ bitRead(EEPROM.read(i),j) * 2 + 1] );
#endif
        timings[restoredSignalLength] = metaData[ bitRead(EEPROM.read(i),j) * 2 +1];
        restoredSignalLength++;
      }
      //Serial.println();
    }
    // If the last byte is only half used, then we should not transmit the last half byte.
    if( lastByteIsSplit ){
#ifdef DEBUG
      Serial.println(F("Last byte is split"));
#endif
      restoredSignalLength = restoredSignalLength - 8; // Since the last 4 bits of the byte didn't actually contain valid data, we 'cut it off' from the part that we will transmit.
    }

    // If the anomaly is 'inside' the byte or nibble, then the anomaly length must be subtracted from the total restored length.
    if( anomalyInside ){
      Serial.println(F("Anomaly inside"));
      if( bitRead(signalDescription,DESCRIPTION_ANOMALY) == 1 ){
        restoredSignalLength = restoredSignalLength - 2;
      }
      if( bitRead(signalDescription,DESCRIPTION_LARGE_ANOMALY) == 1 ){
        restoredSignalLength = restoredSignalLength - 2;
      }
    }
#ifdef DEBUG
    Serial.print(F(">> restoredSignalLength ")); Serial.println(restoredSignalLength); Serial.println();
    for( int j = 0; j < restoredSignalLength; j++ ){
      Serial.print( timings[j] ); Serial.print(F(","));
      if( j % 4 == 3 ){ Serial.println(); }
      if( j % 32 == 31 ){ Serial.println(); }
    }
#endif

    // Finally, play the new timing a few times.
    Serial.println(F("REPLAYING"));
    for( byte i = 0; i < 8; i++ ){                  // Sending the pattern a few times.
      boolean high = false;
      for( int j = 0; j < restoredSignalLength; j++ ){
        if( high ){
          digitalWrite(TRANSMITTER, LOW);
          high = false;
        }
        else{
          digitalWrite(TRANSMITTER, HIGH);
          high = true;
        }
        interval = timings[j] * GRANULARITY;
        delayMicroseconds(interval);
      }
    }

    digitalWrite(TRANSMITTER, LOW);                 // Just to be safe
    wait(300);

#ifdef DEBUG    
    wait(300);
    Serial.println(F("NOW REPLAYING REVERSED"));
#endif

#ifdef DEBUG  
    for( byte i = 0; i < 8; i++ ){                  // Sending the pattern a few times.
      boolean high = false;
      for( int j = 0; j < restoredSignalLength; j++ ){
        if( high ){
          digitalWrite(TRANSMITTER, HIGH);
          high = false;
        }
        else{
          digitalWrite(TRANSMITTER, LOW);
          high = true;
        }
        interval = timings[j] * GRANULARITY;
        delayMicroseconds(interval);
      }
    }

    digitalWrite(TRANSMITTER, LOW);                 // Just to be safe
#endif
   
#ifdef DEBUG    
    wait(100);
    Serial.println(F("DONE REPLAYING"));
#endif
  }                                                 // End of check if at least one replayable signal is stored in the EEPROM.
#if KEYPAD_BUTTON_COUNT > 0
  else{
    //Serial.println(F("No signal to replay stored yet!"));
    updateDisplay(NO_SIGNAL_STORED_YET);            // Only useful to show this if there are physical buttons that don't have a replayable signal attached to them yet.
  }
#endif
  send(textmsg.setSensor(DEVICE_STATUS_ID).set( F("OK") ));
  state = LISTENING;
  //if( playlist_position > 0){
  //  playlist_position--;
  //}
}                                                   // End of replay function


//
//  UPDATE DISPLAY
//

void updateDisplay(byte currentStatus)              // Show info on the display
{
#ifdef HAS_TOUCH_SCREEN
#ifdef DEBUG  
  Serial.println(); Serial.print(F("UPDATE DISPLAY (state: ")); Serial.println(state);
#endif
  clearReceivedBuffer();
  turnOnScreen();                                   // If the screen is turned off, it will be turned on again.
  
  if( currentStatus == LISTENING ){
    showTouchButtons();
  }
  else {
    if( currentStatus == MATCH || currentStatus == REPLAYING ){
      wait(50);
      roundedRectangle( 0, ((howManyReplayButtonsWillFitOnScreen + 1) * BUTTON_HEIGHT), TOUCHSCREEN_WIDTH, TOUCHSCREEN_HEIGHT - ((howManyReplayButtonsWillFitOnScreen + 1) * BUTTON_HEIGHT), 0, 64640 ); // Orange rectangle.
      wait(50);
      setCur(BUTTON_PADDING,BUTTON_PADDING+((howManyReplayButtonsWillFitOnScreen + 1) * BUTTON_HEIGHT) );
      wait(50);
      if( currentStatus == MATCH ){
        writeText(START_OF_MENU_DELETE_ALL + MATCH);
        wait(2000);
      }
      else if( currentStatus == REPLAYING ){
        writeText(START_OF_MENU_DELETE_ALL + REPLAYING);
        displayNumber( buttonPressed );
        wait(2000);
      }
      roundedRectangle( 0, ((howManyReplayButtonsWillFitOnScreen + 1) * BUTTON_HEIGHT), TOUCHSCREEN_WIDTH, TOUCHSCREEN_HEIGHT - ((howManyReplayButtonsWillFitOnScreen + 1) * BUTTON_HEIGHT), 0, 0 ); // Black rectangle
      //Serial.print(F("amountOfStoredReplayableSignals:")); Serial.println(amountOfStoredReplayableSignals);
      //Serial.print(F("howManyReplayButtonsWillFitOnScreen:")); Serial.println(howManyReplayButtonsWillFitOnScreen);
      if( amountOfStoredReplayableSignals > howManyReplayButtonsWillFitOnScreen ){
        setCur(BUTTON_PADDING,BUTTON_PADDING+((howManyReplayButtonsWillFitOnScreen + 1) * BUTTON_HEIGHT));
        basicCommand(more);
      }
      return;
    }

    // If we are copying a signal, show the cancel button.
    if( state > MENU_NEW && state < MENU_DELETE_ALL + 5 ){
#ifdef DEBUG_SCREEN
          Serial.println(F("BC: fill_black"));
#endif
      basicCommand(fill_black);
      roundedRectangle( 0, 0, (int)TOUCHSCREEN_WIDTH / 2, BUTTON_HEIGHT, 10, 31 ); // Draw a blue (31) rounded rectangle, with a 10px radius.
      setCur(BUTTON_PADDING,BUTTON_PADDING);
      writeText(0);                                 // 0 is 'cancel'.
    }

    setCur(BUTTON_PADDING,BUTTON_PADDING+BUTTON_HEIGHT);

    // Copying
    if( currentStatus == COPYING_SIMPLE || currentStatus == LEARNING_SIMPLE ){
      writeText(START_OF_MENU_DELETE_ALL + PLAY_SIMPLE_SIGNAL); 
    }
    else if( currentStatus == COPYING_ON || currentStatus == LEARNING_ON ){
      writeText(START_OF_MENU_DELETE_ALL + PLAY_ON_SIGNAL);
    }
    else if( currentStatus == COPYING_OFF || currentStatus == LEARNING_OFF){
      writeText(START_OF_MENU_DELETE_ALL + PLAY_OFF_SIGNAL);
    }
    
    else if( currentStatus == SIGNAL_STORED ){
      writeText(START_OF_MENU_DELETE_ALL + SIGNAL_STORED);
      wait(3000);
    }

    // Errors
    else if( currentStatus == BAD_SIGNAL ){
      writeText(START_OF_MENU_DELETE_ALL + BAD_SIGNAL); 
    }
    else if( currentStatus == OUT_OF_SPACE ){
      writeText(START_OF_MENU_DELETE_ALL + OUT_OF_SPACE);
      wait(4000);
    }
  
    // Other
    else if( currentStatus == DELETE_LAST ){
      writeText(START_OF_MENU_DELETE_ALL + DELETED_LAST);
      wait(4000);
    }  
    else if( currentStatus == DELETED_ALL ){
      writeText(START_OF_MENU_DELETE_ALL + DELETED_ALL);
      wait(4000);
    }
  }

#endif

#ifdef HAS_BASIC_OLED_SCREEN
  oled.clear();                                     // Clear the display
  displayNetworkStatus();                           // Update the network connection icon.
  oled.set2X();                                     // Switch font size
  oled.setCursor(0,2);

  // Often used
  if( currentStatus == LISTENING ){
    oled.print(F("LISTENING.."));                   // Call this "Scanning" instead?
      oled.set1X(); 
      oled.setCursor(0,5);
      oled.print(F("Detect signals: ")); oled.print(amountOfStoredSignals - amountOfStoredReplayableSignals);
      oled.setCursor(0,6);
      oled.print(F("Replay signals: ")); oled.print(amountOfStoredReplayableSignals);
    
  }
  else if( currentStatus == PROCESSING ){
    oled.print(F("Processing"));
  }
  else if( currentStatus == MATCH ){
    oled.print(F("Found"));
    oled.setCursor(0,5);
    oled.print(F("match!"));
    wait(2000);
  }
  else if( currentStatus == REPLAYING ){
    oled.print(F("Playing ")); oled.print(buttonPressed); 
  }

  // Copying
  else if( currentStatus == COPYING_ON || currentStatus == LEARNING_ON ){
    oled.print(F("Play ON"));
    oled.set1X(); 
    oled.setCursor(0,6);
    oled.print(F(">cancel"));
  }
  else if( currentStatus == COPYING_OFF || currentStatus == LEARNING_OFF){
    oled.print(F("Play OFF"));
    oled.set1X(); 
    oled.setCursor(0,6);
    oled.print(F(">cancel"));    
  }  
  else if( currentStatus == COPYING_SIMPLE || currentStatus == LEARNING_SIMPLE ){
    oled.print(F("READY!"));
    oled.set1X();                                   
    oled.setCursor(0,4);
    oled.print(F("Start your signal"));
    oled.set1X(); 
    oled.setCursor(0,6);
    oled.print(F(">cancel"));    
  }
  else if( currentStatus == SIGNAL_STORED ){
    oled.print(F("SIGNAL"));
    oled.setCursor(0,4);
    oled.print(F("STORED"));
    if( state == COPYING_OFF || state == LEARNING_OFF ){
      oled.set1X(); 
      oled.setCursor(0,7);
      oled.print(F("Stored signals: ")); oled.print(amountOfStoredSignals);
    }
    wait(3000);
  }

  // Errors
  else if( currentStatus == BAD_SIGNAL ){
    oled.print(F("BAD SIGNAL"));
    oled.set1X();
    oled.setCursor(0,5);
    oled.print(F("TRY AGAIN"));    
  }
  else if( currentStatus == OUT_OF_SPACE ){
    oled.print(F("Out of space!"));
    wait(4000);
  }
  else if( currentStatus == NO_SIGNAL_STORED_YET ){
    oled.print(F("Empty"));  
    wait(2000);
  }
  else if( currentStatus == NO_MORE_FREE_BUTTONS ){
    oled.print(F("No buttons left"));
    oled.set1X();
    oled.setCursor(0,0);
    oled.print(F("NOTICE"));    
    oled.setCursor(0,5);
    oled.print(F("You can only replay"));
    oled.setCursor(0,6);
    oled.print(F("it via the app."));
    wait(4000);
  }

  // Other
  else if( currentStatus == DELETE_LAST ){
    oled.print(F("DELETED"));
    oled.setCursor(0,5);
    oled.print(F("ONE SIGNAL"));
    wait(4000);
  }  
  else if( currentStatus == DELETED_ALL ){
    oled.print(F("MEMORY"));
    oled.setCursor(0,5);
    oled.print(F("CLEAR"));
    wait(4000);
  }
#ifdef DEBUG
  else{
    oled.print(F("??? ")); oled.print(currentStatus);
  }
#endif
#endif
}                                                   // End of update display function



//
//  MENUS
//   


byte showMenu()                                     // Lets the user select which state to enter.
{
  Serial.println(F("__MENU"));
  //if( state == LISTENING ){
    state = MENU_MAIN;
  //}
  boolean stayInMenu = true;                        // We stay in the menu while this is true
  
#ifdef HAS_TOUCH_SCREEN
  byte menuOffset = 0;
  byte menuItems = 4;
  wait(100);
  clearReceivedBuffer();
  touchY = TOUCHSCREEN_HEIGHT;
  touched = true;
#ifdef DEBUG_SCREEN
  Serial.print(F("touch_screen_serial.available:")); Serial.println( touch_screen_serial.available() );
#endif

  while( stayInMenu ){
    menuOffset = 0;
    wait(50);
    readResponse(); // check for new touchscreen input.
    if( touched == true ){
      touched = false;
      buttonPressed = touchScreenButtonPress();
#ifdef DEBUG_SCREEN
      Serial.print(F("touched (")); Serial.println(buttonPressed);
#endif
      if( buttonPressed == 0 ){ // No matter where in the menu we are, when the cancel button is pressed the menu is closed.
#ifdef DEBUG_SCREEN
        Serial.println(F("Cancel button"));
#endif
        state = LISTENING;
        stayInMenu = false;                         // Exit menu
        break;
      }
      else {
#ifdef DEBUG_SCREEN
          Serial.println(F("BC: fill_blue"));
#endif
        basicCommand(fill_blue);                    // Set the background color to blue
        setCur(BUTTON_PADDING,BUTTON_PADDING);      // Set the text cursor at the top left of the screen
        writeText(0);                               // Display the Cancel item. All menu's have it.
        simpleHorizontal(BUTTON_HEIGHT);
        
        if( state == MENU_MAIN ){
          menuOffset = 1;
          if( amountOfStoredSignals > 0 ){
            menuItems = START_OF_MENU_NEW - 1;      // 3 items
          }
          else {
            menuItems = START_OF_MENU_NEW - 3;      // 1 item. No need to show the delete buttons if there are no stored signals.
          }
            
          if( buttonPressed == 1 ){
            Serial.println(F("entering menu_new"));
            state = MENU_NEW;
            menuOffset = START_OF_MENU_NEW;
            menuItems = START_OF_MENU_DELETE_LAST - START_OF_MENU_NEW; // 4 items         
          }
          if( buttonPressed == 2 ){
            Serial.println(F("entering delete last menu"));
            state = MENU_DELETE_LAST;
            menuOffset = START_OF_MENU_DELETE_LAST;
            menuItems = START_OF_MENU_DELETE_ALL - START_OF_MENU_DELETE_LAST; // 1 item
          }
          if( buttonPressed == 3 ){
            Serial.println(F("entering delete ALL menu"));
            state = MENU_DELETE_ALL;
            menuOffset = START_OF_MENU_DELETE_ALL;
            menuItems = START_OF_MENU_STATUS_MESSAGES - START_OF_MENU_DELETE_ALL; // 1 item
          }

          for( byte i = 0; i < menuItems; i++ ){
            Serial.print(F(">")); Serial.println(i);
            setCur(BUTTON_PADDING,BUTTON_PADDING + BUTTON_HEIGHT + (i * BUTTON_HEIGHT) );
            writeText(i + menuOffset);              // The offset helps to select the right number which corresponds to a menu item's string in the messages array.
            simpleHorizontal(BUTTON_HEIGHT + BUTTON_HEIGHT + (i * BUTTON_HEIGHT));
          }
        }
        else {                                      // When the user presses a button while in a sub menu.
          state = state + buttonPressed;
          stayInMenu = false;
          break;
        }
      }
      clearReceivedBuffer();
    }
  }

#elif defined (HAS_BASIC_OLED_SCREEN)
  byte menuItems = 4;                               // How many items are shown? Don't forget 'cancel' is always shown.
  byte menuPosition = 2;                            // Current position of the cursor.
  wait(100);

  while( stayInMenu ){
#if KEYPAD_BUTTON_COUNT == 4
    buttonPressed = keypad4();
#else if KEYPAD_BUTTON_COUNT == 12
    buttonPressed = keypad12();
#endif
    wait(100);
    if(buttonPressed != prevButtonState){
      Serial.print(F("[] button pressed: ")); Serial.println(buttonPressed);
      if( buttonPressed == KEYPAD_BUTTON_COUNT - 1 ){ // User has pressed the change button.
        menuPosition++;
        if( menuPosition > menuItems + 1 ){ menuPosition = 2; } // If necessary, loop around back to the top item in the menu.
        Serial.print(F("changed to ")); Serial.println(menuPosition);
      }
      else if( buttonPressed == KEYPAD_BUTTON_COUNT ){ // User has pressed the menu/ok button.
        Serial.print(F("select!")); Serial.println(menuPosition);
        if( menuPosition == 2 ){                    // User pressed cancel. This could move the user back to the main menu, but for simplicity, this currently just exits the menu.
          state = 255;
          stayInMenu = false;                       // We can now exit the menu.
        }
        else if( state == MENU_MAIN ){              // Going into sub-menu.
          state = menuPosition * 10;                // So the first submenu state is 30, the second submenu is 40, etc.
          menuPosition = 2;                         // Reset the cursor position to the top position.
        }
        else{                                       // User is in submenu, so they are making their final selection for a new state.
          state = state + menuPosition;             // If the submenu state was 30, the final states are 32, 33, etc.
          stayInMenu = false;                       // We can now exit the menu.
        }
      }
      else if( buttonPressed == 0 && prevButtonState == KEYPAD_BUTTON_COUNT ){
        // Here we display the actual menu items.
        oled.clear();
        displayNetworkStatus();
        oled.set2X(); 
        oled.setCursor(0,0);
        oled.print(F("MENU"));
        oled.set1X(); 
        oled.setCursor(8,2);
        oled.print(F("CANCEL"));
        oled.setCursor(8,3);
        if( state == MENU_MAIN ){
          oled.print(F("NEW"));
          oled.setCursor(8,4);
          oled.print(F("DELETE LAST"));
          oled.setCursor(8,5);
          oled.print(F("DELETE ALL"));
        }
        else if( state == MENU_NEW ){
          menuItems = 5;                            // Cancel + the items below.
          oled.print(F("Detect single"));
          oled.setCursor(8,4);
          oled.print(F("Detect On+Off"));
          oled.setCursor(8,5);
          oled.print(F("Replay single"));
          oled.setCursor(8,6);
          oled.print(F("Replay On+Off"));
        }
        else if( state == MENU_DELETE_LAST || state == MENU_DELETE_ALL){
          menuItems = 2;                            // Cancel + the item below.
          oled.print(F("I am sure"));
          oled.setCursor(8,4);
        }
      }
      // Add the pointer.
      for( byte w = 2; w < 7; w++ ){                // Update the position of the selection cursor on the screen.
        oled.setCursor(0,w);
        if( menuPosition == w ){
          oled.print(F(">"));
        }
        else{
          oled.print(F(" "));
        }
      }
      wait(1);
      prevButtonState = buttonPressed;
    }
  }
#endif  
}



//
//  RECEIVING DATA FROM THE NETWORK
//



void printRawSignal()                               // Prints the entire timings array to the serial output. Useful for debugging.
{
  Serial.print(F("__printing_signal_")); Serial.println(edges+1);
  for( int j = startPosition; j <= endPosition; j++ ){
    Serial.print(F(",")); Serial.print(timings[j]);
    if( j % 16 == 15 ){ Serial.println(); }
  }
  Serial.println();
}


void clearTimingsArray()
{
#ifdef DEBUG
  Serial.print(F("__Clearing_timings\n-state:")); Serial.println(state);
#endif
  memset(timings,0,sizeof(timings));                // Write zeros to timings array.
  startPosition = 0;
  endPosition = MAXEDGES - 1;
  repeatingPatternLength = 255;
  edges = 0 - EDGES_TO_SKIP;
  signalstart = 0;
  bucketsTotal = 0;
  captureFinished = false;
  validSignal = true;
  bucketCount = 0;
  signalstart = micros();
}


void displayNetworkStatus()                         // Show connection icon on the display
{
#ifdef HAS_TOUCH_SCREEN
  if( connectedToNetwork ){
    setCur(TOUCHSCREEN_WIDTH - 30, BUTTON_PADDING );
#ifdef DEBUG_SCREEN
          Serial.println(F("BC: show W icon"));
#endif
    basicCommand(w);
  }
#endif

#ifdef HAS_BASIC_OLED_SCREEN
  oled.set1X();
  oled.setCursor(122,0);
  if( connectedToNetwork ){
    oled.print(F("w"));
  }
  else{
    oled.print(F(" "));
  }
#endif
}


#ifdef DEBUG
int freeRam() {
  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}
// via https://playground.arduino.cc/Code/AvailableMemory
#endif

#ifdef HAS_BASIC_OLED_SCREEN && KEYPAD_BUTTON_COUNT != 0
byte keypad4()
{
  signed int buttonNumber = (analogRead(KEYPAD_PIN) - 200) * (4 - 0 + 1) / (1024 - 200 + 1) + 0;
  buttonNumber = constrain(buttonNumber, 0, 4);
  return (byte)buttonNumber;
}

byte keypad12()
{
  int analogValue = analogRead(KEYPAD_PIN);
  
  if (analogValue < 450) {
      return 0;
  } else if (analogValue < 500) {
      return 1;
  } else if (analogValue < 525) {
      return 2;
  } else if (analogValue < 555) {
      return 3;
  } else if (analogValue < 585) {
      return 4;
  } else if (analogValue < 620) {
      return 5;
  } else if (analogValue < 660) {
      return 6;
  } else if (analogValue < 705) {
      return 7;
  } else if (analogValue < 760) {
      return 8;
  } else if (analogValue < 820) {
      return 9;
  } else if (analogValue < 890) {
      return 10;
  } else if (analogValue < 976) {
      return 11;
  } else {
      return 12;
  }
}
#endif






//
//  BELOW HERE ARE THE TOUCHSCREEN FUNCTIONS
//

#ifdef HAS_TOUCH_SCREEN

byte touchScreenButtonPress()
{
  clearReceivedBuffer();
  return ceil(touchY / BUTTON_HEIGHT);
}

// Show the default interface, with numbered buttons that play signals when pressed.
void showTouchButtons()
{
#ifdef DEBUG_SCREEN
  Serial.println(F("__showTouchButtons "));

  Serial.println(F("BC: fill_black"));
#endif
  basicCommand(fill_black);
#ifdef DEBUG_SCREEN
          Serial.println(F("BC: roundedRect"));
#endif
  roundedRectangle( 0, 0, (int)TOUCHSCREEN_WIDTH / 2, BUTTON_HEIGHT, (int)BUTTON_HEIGHT / 2, 31 ); // blue rounded rectangle.
#ifdef DEBUG_SCREEN
          Serial.println(F("BC: set_cur"));
#endif
  setCur(BUTTON_PADDING,BUTTON_PADDING);
#ifdef DEBUG_SCREEN
          Serial.println(F("BC: menu"));
#endif
  basicCommand(menu);

  state = DISPLAY_REPLAY_BUTTONS;                   // This state is used to focus the scanEeprom function on displaying the replay buttons.
  scanEeprom();
  state = LISTENING;

  // Show 'MORE' button?
  if( amountOfStoredReplayableSignals > howManyReplayButtonsWillFitOnScreen ){
    setCur( BUTTON_PADDING, BUTTON_PADDING + BUTTON_HEIGHT + howManyReplayButtonsWillFitOnScreen * BUTTON_HEIGHT );
    basicCommand(more);                           
  }
  displayNetworkStatus();
}



void simpleHorizontal(int y)                        // Draw a horizontal line on the screen.
{

#ifdef DEBUG_SCREEN
  Serial.println(F("SIMPLEHORIZONTAL:"));
  Serial.print(F("y: ")); Serial.println(y);
#endif
  byte command[12] = {0x7E, 0x0A, 0x23, 0,0, highByte(y), lowByte(y), 0,240, 255, 255, 0xEF,};
  for( int i=0; i < sizeof(command); i++ ){
    touch_screen_serial.write( command[i] );
    //Serial.print(command[i],HEX); Serial.print(F(" "));
  }
  waitForResponse();
}


void roundedRectangle(int x, int y, int w, int h, int r, int c) // Draw a pixel on the screen.
{
  
#ifdef DEBUG_SCREEN
  Serial.println(F("ROUNDEDRECTANGLE"));
  //Serial.print(F("x:")); Serial.println(x);       // top-right x-position
  //Serial.print(F("y:")); Serial.println(y);       // top-left y-position
  //Serial.print(F("w:")); Serial.println(w);       // width
  //Serial.print(F("h:")); Serial.println(h);       // height
  //Serial.print(F("r:")); Serial.println(r);       // radius
  //Serial.print(F("c:")); Serial.println(c);       // color
#endif
  byte command[16] = {0x7E, 0x0E, 0x2C, highByte(x), lowByte(x), highByte(y), lowByte(y), highByte(w), lowByte(w), highByte(h), lowByte(h) - 2, highByte(r), lowByte(r), highByte(c), lowByte(c), 0xEF,};
  
  for( int i=0; i < sizeof(command); i++ ){
    touch_screen_serial.write( command[i] );
    //Serial.print(command[i],HEX); Serial.print(F(" "));
  }
  waitForResponse();
}



// This function writes text to the screen. You can use the setCur function to place the cursor in the desired position first.
void writeText(byte textID)
{
#ifdef DEBUG_SCREEN
  Serial.println(F("WRITETEXT"));
#endif
  byte j = 0;
  byte c = 0;
  byte stringLength = strlen_P(MessageTable[textID].Description);
  
  while ( j < stringLength ){
    byte command[9] = {0x7E, 0x07, 0x11, 0x00, 0x00, 0x20, 0x00, 0x00, 0xEF,}; // It took quite some testing to get this right. Just using 0x00 for the entire payload will not work.
    
    for( byte i=0; i < 5; i++ ){
      if(j+i < stringLength){
        command[3+i] = pgm_read_byte(MessageTable[textID].Description + j + i); // It works in batches of 5 characters..
      }
    }
    for( int i=0; i < sizeof(command); i++ ){
      touch_screen_serial.write( command[i] );
      //Serial.print(command[i],HEX); Serial.print(F(" "));
    }
    waitForResponse();
    //wait(20);
    j = j + 5;
  }
}

#ifdef DEBUG_SCREEN
void drawPix(int x, int y, int c) // Draw a pixel on the screen.
{
  Serial.println(F("DRAWPIX:"));
  Serial.print(F("x: ")); Serial.println(x);
  Serial.print(F("y: ")); Serial.println(y);
  Serial.print(F("c: ")); Serial.println(c);
  byte command[10] = {0x7E, 0x08, 0x21, highByte(x), lowByte(x), highByte(y), lowByte(y), highByte(c), lowByte(c), 0xEF,};
  for( int i=0; i < sizeof(command); i++ ){
    touch_screen_serial.write( command[i] );
    //Serial.print(command[i],HEX); Serial.print(F(" "));
  }
  waitForResponse();
}
#endif


// This function places the text cursor anywhere on the screen.
void setCur(int x, int y)
{
#ifdef DEBUG_SCREEN  
  Serial.println(F("SETCUR"));
  Serial.print(F("x: ")); Serial.println(x);
  Serial.print(F("y: ")); Serial.println(y);
#endif
  byte command[8] = {0x7E, 0x06, 0x01, highByte(x), lowByte(x), highByte(y), lowByte(y), 0xEF,};
  for( int i=0; i < sizeof(command); i++ ){
    touch_screen_serial.write( command[i] );
    //Serial.print(command[i],HEX); Serial.print(F(" "));
  }
  waitForResponse();
}


// This function outputs a variable number to the screen. It can show negative and positive numbers. It cannot show floats.
void displayNumber(signed int number)
{
#ifdef DEBUG_SCREEN
  Serial.print(F("DISPLAYNUMBER (")); Serial.println(number); 
#endif
  byte command[8] = {0x7E, 0x06, 0x13, 0x01, 0x0A, highByte(number), lowByte(number), 0xEF,};
  for( int i=0; i < sizeof(command); i++ ){
    touch_screen_serial.write( command[i] );
    //Serial.print(command[i],HEX); Serial.print(F(" "));
  }
  waitForResponse();
}


// This function reads the serial data (if available) from the screen. The implemented commands it can detect are 'ok', and the touch coordinates.
void readResponse()
{
  volatile int availableSerialCount = touch_screen_serial.available();
  //Serial.println(F("READRESPONSE"));
  if( availableSerialCount == 0 ){ 
    return;
  }

  boolean savingMessage = false;                    // When touch message, this starts recording data into an array for analysis.
  //byte metaData[metadataArraySize];               // An array to store the received serial data // TODO: use the metadata array instead. It's just as long, and their uses don't overlap.
  byte metaDataPosition = 0;                        // The metaData array is recycled: here is holds incoming serial data.
  byte startMarker = 0x7E;                          // Any response from the screen will start with this.
  byte endMarker = 0xEF;                            // Any response from the screen will end with this.
  byte rc;                                          // Hold the byte form the serial stream that we're examining.


  
  byte c = touch_screen_serial.peek();
  //Serial.print(F("SOME SERIAL DATA. Available:")); Serial.println(  availableSerialCount );
  //Serial.print(F("Peek:")); Serial.println(  c );
  if( c != startMarker ){
    rc = touch_screen_serial.read();
    Serial.print(F("throwing away left over touch_screen_serial byte:")); Serial.println(rc);
    return;
  }
  
  if( availableSerialCount < 5 ){                   // Any response should be at least 5 bytes.
#ifdef DEBUG_SCREEN
    Serial.print(F("Too short response from screen (")); Serial.println( touch_screen_serial.available() );
#endif
    return;
  }
#ifdef DEBUG_SCREEN
  Serial.println(F("GOT RESPONSE"));
#endif

  while( touch_screen_serial.available() ){                    //  By not checking for this the entire buffer is always cleared.
    rc = touch_screen_serial.read();
#ifdef DEBUG_SCREEN
    Serial.print(rc); Serial.print(F("-"));
#endif
    if( savingMessage == true ){                    // We are now in the part of the response that is the message we were looking for.
      if(rc != endMarker){
        metaData[metaDataPosition] = rc;
        metaDataPosition++;
        if( metaDataPosition >= metadataArraySize ){
          metaDataPosition = metadataArraySize - 1;
        }
      }
      else {                                        // We've arrived at the end marker.
        metaData[metaDataPosition] = '\0';          // Terminate the string.
        savingMessage = false;
        metaDataPosition = 0;
        if(metaData[metaDataPosition] == 0x06 && metaData[metaDataPosition + 1] == 0x07){
          touchX = touchToX( (metaData[metaDataPosition + 2] * 256) + metaData[metaDataPosition + 3] );
          touchY = touchToY( (metaData[metaDataPosition + 4] * 256) + metaData[metaDataPosition + 5] );
#ifdef DEBUG_SCREEN
          Serial.print(F("touchX=")); Serial.println(touchX);
          Serial.print(F("touchY=")); Serial.println(touchY);
          Serial.print(F("brightnessTimer=")); Serial.println(brightnessTimer);
#endif

          if( brightnessTimer == 0 ){
#ifdef DEBUG_SCREEN
          Serial.println(F("Turning on backlight"));
#endif
            basicCommand(backlight_on);                // If the screen was touched but the backlight was off, turn the backlight on.
          }
          else{
            touched = true;                           // To indicate that a touch event has just occured.
          }
          brightnessTimer = 255;
          clearReceivedBuffer();
        }
#ifdef DEBUG_SCREEN
        // OK message
        else if( metaData[metaDataPosition] == 0x03 && metaData[metaDataPosition + 1] == 0x6F && metaData[metaDataPosition + 2] == 0x6B ){
          Serial.println(F("-OK"));
        }
        else{                                       // Unimplemented response form the touch screen.
          Serial.println(F("-X-"));
          metaDataPosition++;
          if( metaDataPosition == metadataArraySize ){ return; }
        }
#endif
      }
    }
    if( rc == startMarker ){                        // Once a valid startMarker is found, we start saving the message into the array.
      savingMessage = true;
#ifdef DEBUG_SCREEN
      Serial.print(F("(startMarker) "));
#endif
    } 
  }
}

// This function can send basic string command that don't have any variable parts in them.
void basicCommand(const char* cmd)
{
#ifdef DEBUG_SCREEN
  Serial.println(F("BASIC COMMAND"));
#endif
  if( touch_screen_serial.available() ){                       // If necessary, clear old messages from the serial buffer.
    clearReceivedBuffer();
  } 

  touch_screen_serial.write(0x7E);                             // Starting byte, is always the same.
  byte b = 0;
  while( b < MAX_BASIC_COMMAND_LENGTH ){            // How many bytes are the basic commands at most?
    touch_screen_serial.write( pgm_read_byte(&cmd[b]) );
    //Serial.print( pgm_read_byte(&cmd[b]),HEX ); Serial.print(F(" "));
    if( pgm_read_byte(&cmd[b]) == 0xEF ){           // This breaks out of the loop.
      //waitForResponse(b);
      b = MAX_BASIC_COMMAND_LENGTH; 
    } 
    b++;
  }
  waitForResponse();
}


// This function can be activated after sending a command. It will wait until a response has arrived (or 100 milliseconds have passed), and then allow the Arduino to continue.
//void waitForResponse(byte expectedBytes) // From the touch screen

void waitForResponse() // From the touch screen
{
#ifdef DEBUG_SCREEN
  Serial.println(); Serial.println(F("WAITING FOR RESPONSE FROM SCREEN"));
  Serial.print(F("-available now: ")); Serial.println( touch_screen_serial.available() ); 
#endif
  byte b = 0;
  while( touch_screen_serial.available() == 0 && b < 250){
    b++;
    wait(1);
  }
#ifdef DEBUG_SCREEN
  Serial.print(F("wait time: ")); Serial.println(b);
#endif  
  if( touch_screen_serial.available() > 0 ){
    wait(10); // Perhaps some more bytes will show up.
    while( touch_screen_serial.available() > 0 ){                // Throwing away the response. All we care about is touch messages, and they are handled in the readResponse function.
      byte x = touch_screen_serial.read();
      //Serial.print(x); Serial.print(F("-"));
    }
    //Serial.println();
  }
  else if( b == 250 ){
    Serial.println(F("Touch screen did not respond to command"));
  }

}



void turnOnScreen()
{
#ifdef DEBUG_SCREEN
  Serial.print(F("__turnonscreen() (")); Serial.println(brightnessTimer);
#endif
  if( brightnessTimer == 0 ){
#ifdef DEBUG_SCREEN
    Serial.println(F("BC:backlight_on"));
#endif
    basicCommand(backlight_on);                     // Turn on the screen backlight again.
  }
  brightnessTimer = 255;
}


int touchToX(int x)
{
  return int constrain(((x - 80) / 3.7), 0, TOUCHSCREEN_WIDTH);
}
int touchToY(int y)
{
  return int constrain(((y - 100) / 2.8), 0, TOUCHSCREEN_HEIGHT);
}


void clearReceivedBuffer()
{
//Serial.print(F("cleaning:")); 
  while( touch_screen_serial.available() ){
    char x = touch_screen_serial.read();
    Serial.print(x);
  }
  Serial.println(); 
}

#endif // End of touch screen check.



void receive(const MyMessage &message)
{
  Serial.print(F("INCOMING MESSAGE for child #")); Serial.println(message.sensor);
  if (message.isAck()) {
    Serial.println(F("-Ack"));
  }
  else if( message.type==V_STATUS ){
    Serial.print(F("-Requested status: ")); Serial.println(message.getBool());

    // turnOnScreen(); // Handled by display function now.

#if !(defined(HAS_TOUCH_SCREEN))
    if( message.sensor < 10 && message.getBool() ){
      if( message.sensor == LEARN_SIMPLE_BTN_ID ){ // The user wants to the system to learn a new simple signal. This only starts when the button is toggled to on.
        send(textmsg.setSensor(DEVICE_STATUS_ID).set( F("Play the signal") )); //wait(RADIO_DELAY);
        state = LEARNING_SIMPLE;
      }
      else if( message.sensor == LEARN_ON_OFF_BTN_ID ){ // The user wants the system to learn a new on+off signal. This only starts when the button is toggled to on.
        send(textmsg.setSensor(DEVICE_STATUS_ID).set( F("Play the ON signal") )); //wait(RADIO_DELAY);
        state = LEARNING_ON;
      }
      if( message.sensor == COPYING_SIMPLE_BTN_ID ){ // The user wants to the system to learn a new simple signal. This only starts when the button is toggled to on.
        send(textmsg.setSensor(DEVICE_STATUS_ID).set( F("Play the signal") )); //wait(RADIO_DELAY);
        state = COPYING_SIMPLE;
      }
      else if( message.sensor == COPYING_ON_OFF_BTN_ID ){ // The user wants the system to learn a new on+off signal. This only starts when the button is toggled to on.
        send(textmsg.setSensor(DEVICE_STATUS_ID).set( F("Play the ON signal") )); //wait(RADIO_DELAY);
        state = COPYING_ON;
      }
      //send(buttonmsg.setSensor(message.sensor).set(0)); // The button will already be set back to 'off' after the entire signal is recorded and everything is re-presented.
    }
    else 
#endif

    if( message.sensor >= 10  && message.sensor < 100 ){ // If the user toggled a signal replay button.
      if( playlist_position < PLAYLIST_SIZE){ // We only add to the playlist if there is space left in the playlist.
        
        Serial.println(F("-Adding to playlist"));
        playlist_position++;
        if( message.getBool() == 0 ){
          playlist[playlist_position] = (message.sensor - 9);
        }
        else {
          playlist[playlist_position] = (message.sensor - 9) + 100;
        }
        //replay(message.sensor - 9, message.getBool()); // The old way of directly starting a replay. Now it uses a playlist.
      }
    }
  }
}




/**
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
 *
 */
