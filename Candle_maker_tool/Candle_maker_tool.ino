/*
 * 
 * The Candle Maker Tool allows you to connect a wide range of electronic parts to your smart home. It tries to be as universal as possible, and makes it easy to connect:
 * 
 * Input:
 * - Two binary inputs. For example: a simple on/off switch.
 * - Two analog inputs. For example: rotating dials or sensors that have an analog output.
 * 
 * Output:
 * - Two servos.
 * - Two on/off parts, such as a relay or an LED.
 * - Two analog outputs.
 * - An OLED display. Use it to display any value you like.
 * 
 * The fun starts when you incorporate these parts with your home automations. For example:
 * - Let a servo rotate depending on the temperature in your home.
 * - Turn on an LED on your desk when the dishwasher is done.
 * - Get the value of any sensor with an analog output. There is a wide range available, including gas sensors, light sensors, bending sensors, noise sensors, and so much more.
 * - Show the latest SMS your smart lock received on the OLED screen.
 * 
 * Use your imagination, and have fun!
 * 
 *
 * SETTINGS */ 

// You can enable and disable the settings below by adding or removing double slashes ( // ) in front of a line.

//#define BINARY_SENSOR1_CONNECTED                  // Binary sensor 1. Did you connect an on/off input on pin 2? For example, this could be a button or a motion sensor. As long as it only output "on" and "off", you can connect it.
//#define BINARY_SENSOR2_CONNECTED                  // Binary sensor 2. Did you connect an on/off input on pin 3? For example, this could be a button or a motion sensor. As long as it only output "on" and "off", you can connect it.

//#define ANALOG_SENSOR1_CONNECTED                  // Analog sensor 1. Did you connect an analog sensor on A0?
//#define ANALOG_SENSOR2_CONNECTED                  // Analog sensor 2. Did you connect an analog sensor on A1?

//#define BINARY_ACTUATOR1_CONNECTED                // Digital output 1. Did you connect an LED, relay or other binary 'actuator' part to pin 4?
//#define BINARY_ACTUATOR1_SELF_LOCKING             // Turn digital output 1 off automatically? If you have connected something on the first digital output, should that part be set back to 'off' after a little while?

//#define BINARY_ACTUATOR2_CONNECTED                // Digital output 1. Did you connect an LED, relay or other binary 'actuator' part to pin 4?
//#define BINARY_ACTUATOR2_SELF_LOCKING             // Turn digital output 2 off automatically? Should digital output 2 turn itself off after a little while?

#define SELF_LOCKING_DELAY 3                        // Turn off delay. If you want a digital output to turn itself off again after a short amount of time, how many seconds should pass before this happens?

//#define PWM_ACTUATOR1_CONNECTED                   // Servo 1. Did you connect a servo on pin 6?
//#define PWM_ACTUATOR2_CONNECTED                   // Servo 2. Did you connect a servo on pin 7?

//#define ANALOG_ACTUATOR1_CONNECTED                // Analog output 1. Did you connect an analog actuator on pin A2?
//#define ANALOG_ACTUATOR2_CONNECTED                // Analog output 2. Did you connect an analog actuator on pin A3?


//#define HAS_DISPLAY                               // Did you connect an OLED display? If you connect a display you can send text messages to display on it.
//#define TWO_LINES                                 // Two lines on display. Adds an extra output for the display, so you can send two strings to it instead of one.

#define MY_REPEATER_FEATURE                         // Act as signal repeater. Should this sensor act as a repeater for your other devices? This can help the signal spread further.

#define RF_NANO                                     // RF-Nano. Check this box if you are using the RF-Nano Arduino, which has a built in radio. The Candle project uses the RF-Nano.


 /* END OF SETTINGS
 *
 *
 *  TODO / IDEAS
 *  - Store and load strings using eeprom.
 *  - Add 1 Basic digital PWM output?
 *  - Add stepper motor?
 *  - Add rotational encoder?
 *
 */


//#define DEBUG // General debug option, give extra information via the serial output when enabled.
//#define MY_DEBUG                                  // Enable MySensors debug output to the serial monitor, so you can check if the radio is working ok.


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

#define RADIO_DELAY 50                              // Gives the radio some space to catch a breath


// PINS
#define BINARY_SENSOR1_PIN 2                        // A push button or switch, for example.
#define BINARY_SENSOR2_PIN 3                        // A switch that can be in an of or off state, for example.
#define BINARY_ACTUATOR1_PIN 4                      // A LED or relay, for example.
#define BINARY_ACTUATOR2_PIN 5                        
#define PWM_ACTUATOR1_PIN 6                         // A servo, for example.
#define PWM_ACTUATOR2_PIN 7
#define ANALOG_SENSOR1_PIN A0
#define ANALOG_SENSOR2_PIN A1
#define ANALOG_ACTUATOR1_PIN A2
#define ANALOG_ACTUATOR2_PIN A3

#ifdef RF_NANO
// If you are using an RF-Nano, you have to switch CE and CS pins.
#define MY_RF24_CS_PIN 9                            // Used by the MySensors library.
#define MY_RF24_CE_PIN 10                           // Used by the MySensors library.
#endif






// REQUIRED LIBRARIES
#include <MySensors.h>                              // The MySensors library. Hurray!
#include <avr/wdt.h>                                // The watch dog timer resets the device if it becomes unresponsive.

#if defined(PWM_ACTUATOR1_CONNECTED) || defined(PWM_ACTUATOR2_CONNECTED)
#include <Servo.h>                                  // Used to connect with servos. Make sure you only attach very small servos - an Arduino can't handle big ones.
#endif



// CHILDREN IDs

#define BINARY_SENSOR1_CHILD_ID 1
#define BINARY_SENSOR2_CHILD_ID 2
#define ANALOG_SENSOR1_CHILD_ID 3
#define ANALOG_SENSOR2_CHILD_ID 4
#define BINARY_ACTUATOR1_CHILD_ID 5
#define BINARY_ACTUATOR2_CHILD_ID 6
#define PWM_ACTUATOR1_CHILD_ID 7
#define PWM_ACTUATOR2_CHILD_ID 8
#define ANALOG_ACTUATOR1_CHILD_ID 9
#define ANALOG_ACTUATOR2_CHILD_ID 10

#define TEXT_STRING1_CHILD_ID 25
#define TEXT_STRING2_CHILD_ID 26

// MySensors messages
#ifdef BINARY_SENSOR1_CONNECTED
MyMessage binary_sensor_message(BINARY_SENSOR1_CHILD_ID, V_TRIPPED);  
#elif defined(BINARY_SENSOR2_CONNECTED) and !defined BINARY_SENSOR1_CONNECTED
MyMessage binary_sensor_message(BINARY_SENSOR2_CHILD_ID, V_TRIPPED); 
#endif

#ifdef BINARY_ACTUATOR1_CONNECTED
MyMessage binary_actuator_message(BINARY_ACTUATOR1_CHILD_ID, V_STATUS);
#elif defined(BINARY_ACTUATOR2_CONNECTED) and !defined BINARY_ACTUATOR1_CONNECTED
MyMessage binary_actuator_message(BINARY_ACTUATOR2_CHILD_ID, V_STATUS);
#endif

#ifdef PWM_ACTUATOR1_CONNECTED
MyMessage percentage_message(PWM_ACTUATOR1_CHILD_ID, V_PERCENTAGE);
#elif defined(PWM_ACTUATOR2_CONNECTED) and !defined PWM_ACTUATOR1_CONNECTED
MyMessage percentage_message(PWM_ACTUATOR2_CHILD_ID, V_PERCENTAGE);
#endif

#ifdef ANALOG_SENSOR1_CONNECTED
MyMessage analog_sensor_message(ANALOG_SENSOR1_CHILD_ID, V_CUSTOM);
#elif defined(ANALOG_SENSOR2_CONNECTED) and !defined ANALOG_SENSOR1_CONNECTED
MyMessage analog_sensor_message(ANALOG_SENSOR2_CHILD_ID, V_CUSTOM);
#endif

#if defined ANALOG_ACTUATOR1_CONNECTED and !defined PWM_ACTUATOR1_CONNECTED and !defined PWM_ACTUATOR2_CONNECTED
MyMessage percentage_message(ANALOG_ACTUATOR1_CHILD_ID, V_PERCENTAGE);
#elif defined(ANALOG_ACTUATOR2_CONNECTED) and !defined ANALOG_ACTUATOR1_CONNECTED and !defined PWM_ACTUATOR1_CONNECTED and !defined PWM_ACTUATOR2_CONNECTED
MyMessage percentage_message(ANALOG_ACTUATOR2_CHILD_ID, V_PERCENTAGE);
#endif

#ifdef HAS_DISPLAY
MyMessage text_message(TEXT_STRING1_CHILD_ID, V_TEXT);             // Sets up the message format that we'll be sending to the MySensors gateway later. The first part is the ID of the specific sensor module on this node. The second part tells the gateway what kind of data to expect.
#endif



// VARIABLES
boolean desired_binary_actuator1_state = 0;         // 0 or 1
boolean desired_binary_actuator2_state = 0;         // 0 or 1
boolean binary_actuator1_state = 0;                 // 0 or 1
boolean binary_actuator2_state = 0;                 // 0 or 1
int desired_binary_actuator1_delay = SELF_LOCKING_DELAY;
int desired_binary_actuator2_delay = SELF_LOCKING_DELAY;

int desired_pwm_actuator1_state = 0;                // Between 0 and 100
int desired_pwm_actuator2_state = 0;                // Between 0 and 100
int pwm_actuator1_state = 0;                        // Between 0 and 100
int pwm_actuator2_state = 0;                        // Between 0 and 100

int desired_analog_actuator1_state = 0;             // Between 0 and 100
int desired_analog_actuator2_state = 0;             // Between 0 and 100
int analog_actuator1_state = 0;                     // Between 0 and 100
int analog_actuator2_state = 0;                     // Between 0 and 100

boolean new_binary_sensor1_state = 0;               // 0 or 1
boolean new_binary_sensor2_state = 0;               // 0 or 1
boolean binary_sensor1_state = 0;                   // 0 or 1
boolean binary_sensor2_state = 0;                   // 0 or 1

int new_analog_sensor1_state = 0;                   // Between 0 and 1000
int new_analog_sensor2_state = 0;                   // Between 0 and 1000
int analog_sensor1_state = 0;                       // Between 0 and 1000
int analog_sensor2_state = 0;                       // Between 0 and 1000


// SERVOS
#if defined(PWM_ACTUATOR1_CONNECTED) || defined(PWM_ACTUATOR2_CONNECTED)
Servo servo1;                                       // Create servo object to control a servo
Servo servo2;                                       // Create servo object to control a servo
byte servo1_maximum_degrees = 180;                  // Servo type. Does your first servo go to 180 or to 270 degrees?
byte servo2_maximum_degrees = 180;                  // Servo type. Does your second servo go to 180 or to 270 degrees?
int servo_slow_step = 1024;
#endif

// OLED DISPLAY

#ifdef HAS_DISPLAY
#define INCLUDE_SCROLLING 0                         // Text scrolling for the OLED screen is not necessary
#define TEXT_STRING_LENGTH 26                       // How long the strings are that store the text to display on the screen
#define OLED_I2C_ADDRESS 0x3C                       // A technical requirement. All I2C OLED screens have an 'address'
#include <SSD1306Ascii.h>                           // Simple drivers for the screen.
#include <SSD1306AsciiAvrI2c.h>                     // "SSD1306Ascii"
SSD1306AsciiAvrI2c oled;                            // Creating the display object
byte screen_vertical_position = 3;                  // Used to always show both CO and CO2 levels at the top of the screen.
char string1[26] = "HELLO WORLD";                   // Initial text to show on display.
#endif

#if defined HAS_DISPLAY && defined TWO_LINES
char string2[26] = "";                              // Initial text line 2 to show on display.
#endif


// Timer
#define MAIN_LOOP_DURATION 1000                     // Main loop delay in milliseconds. 1000 milliseconds = 1 second.
static unsigned long lastLoopTime = 0;              // Holds the last time the main loop ran.


// Other
boolean send_all_values = true;                     // When this is true, all current values will be (re)-sent to the controller.


void presentation()
{
  sendSketchInfo(F("Candle maker tool"), F("1.0"));    // Send the name and version to the controller
#ifdef BINARY_SENSOR1_CONNECTED
  present(BINARY_SENSOR1_CHILD_ID, S_MOTION, F("On-off sensor 1"));
#endif
#ifdef BINARY_SENSOR2_CONNECTED
  present(BINARY_SENSOR2_CHILD_ID, S_MOTION, F("On-off sensor 2"));  
#endif

#ifdef ANALOG_SENSOR1_CONNECTED
  present(ANALOG_SENSOR1_CHILD_ID, S_CUSTOM, F("Analog sensor 1"));
#endif
#ifdef ANALOG_SENSOR2_CONNECTED
  present(ANALOG_SENSOR2_CHILD_ID, S_CUSTOM, F("Analog sensor 2")); 
#endif

#ifdef BINARY_ACTUATOR1_CONNECTED
  present(BINARY_ACTUATOR1_CHILD_ID, S_BINARY, F("Button 1"));
#endif
#ifdef BINARY_ACTUATOR2_CONNECTED
  present(BINARY_ACTUATOR2_CHILD_ID, S_BINARY, F("Button 2")); 
#endif

#ifdef PWM_ACTUATOR1_CONNECTED
  present(PWM_ACTUATOR1_CHILD_ID, S_DIMMER, F("Servo control 1"));
#endif
#ifdef PWM_ACTUATOR2_CONNECTED
  present(PWM_ACTUATOR2_CHILD_ID, S_DIMMER, F("Servo control 2")); 
#endif

#ifdef ANALOG_ACTUATOR1_CONNECTED
  present(ANALOG_ACTUATOR1_CHILD_ID, S_DIMMER, F("Analog control 1"));
#endif
#ifdef ANALOG_ACTUATOR2_CONNECTED
  present(ANALOG_ACTUATOR2_CHILD_ID, S_DIMMER, F("Analog control 2")); 
#endif

#ifdef HAS_DISPLAY
  present(TEXT_STRING1_CHILD_ID, S_INFO, F("Display text 1"));
#endif

#if defined HAS_DISPLAY && defined TWO_LINES
  present(TEXT_STRING2_CHILD_ID, S_INFO, F("Display text 2"));
#endif

  send_all_values = true;    
}

void setup()
{
  Serial.begin(115200);
  Serial.println(F("Hello"));

#ifdef RF_NANO
  Serial.println(F("I am using RF_NANO settings"));
#endif


#ifdef HAS_DISPLAY
  // Initiate the display
  oled.begin(&Adafruit128x64, OLED_I2C_ADDRESS);
  oled.setFont(Adafruit5x7);
  oled.ssd1306WriteCmd(SSD1306_DISPLAYON);
  oled.setScroll(false);
  oled.setCursor(0,0);
  oled.set2X();
  oled.print(string1);
#endif


  if(isTransportReady()){
    Serial.println(F("Connected to gateway"));
  }else{
    Serial.println(F("! NOT CONNECTED TO GATEWAY"));  
  }


  // Set initial pin states

#ifdef BINARY_SENSOR1_CONNECTED
  pinMode(BINARY_SENSOR1_PIN, INPUT_PULLUP);
  Serial.println(F("I have a binary sensor 1"));
#endif
#ifdef BINARY_SENSOR2_CONNECTED
  pinMode(BINARY_SENSOR2_PIN, INPUT_PULLUP);
  Serial.println(F("I have a binary sensor 2"));
#endif

#ifdef ANALOG_SENSOR1_CONNECTED
  pinMode(ANALOG_SENSOR1_PIN, INPUT_PULLUP);
  Serial.println(F("I have an analog sensor 1"));
#endif
#ifdef ANALOG_SENSOR2_CONNECTED
  pinMode(ANALOG_SENSOR2_PIN, INPUT_PULLUP);
  Serial.println(F("I have an analog sensor 2"));
#endif


  // Load previous states from EEPROM.

  // Set the initial position of the binary output (relay/LED).
#ifdef BINARY_ACTUATOR1_CONNECTED
  if( loadState(BINARY_ACTUATOR1_CHILD_ID) != 255 ){
    boolean original_state = loadState(BINARY_ACTUATOR1_CHILD_ID);
    desired_binary_actuator1_state = original_state;
    binary_actuator1_state = original_state;
  }
  pinMode(BINARY_ACTUATOR1_PIN, OUTPUT);
  digitalWrite(BINARY_ACTUATOR1_PIN, binary_actuator1_state);
  Serial.println(F("I have a binary actuator 1"));
#endif

#ifdef BINARY_ACTUATOR2_CONNECTED
  if( loadState(BINARY_ACTUATOR2_CHILD_ID) != 255 ){
    boolean original_state = loadState(BINARY_ACTUATOR2_CHILD_ID);
    desired_binary_actuator2_state = original_state;
    binary_actuator2_state = original_state;
  }
  pinMode(BINARY_ACTUATOR2_PIN, OUTPUT); 
  digitalWrite(BINARY_ACTUATOR2_PIN, binary_actuator2_state);
  Serial.println(F("I have a binary actuator 2"));
#endif


  // Set the initial position of the PWM output (servo)


#ifdef PWM_ACTUATOR1_CONNECTED
  if( loadState(PWM_ACTUATOR1_CHILD_ID) < 101 ){ 
    desired_pwm_actuator1_state = int(loadState(PWM_ACTUATOR1_CHILD_ID));
    pwm_actuator1_state = int(loadState(PWM_ACTUATOR1_CHILD_ID));
#ifdef DEBUG
    Serial.println("loaded pwm_actuator1 state from eeprom");
#endif
  }
  pinMode(PWM_ACTUATOR1_PIN, OUTPUT); 
  servo1.attach(PWM_ACTUATOR1_PIN);
  servo1.write(percentage_to_servo(pwm_actuator1_state, servo1_maximum_degrees));
  Serial.println(F("I have a servo 1"));
#endif

#ifdef PWM_ACTUATOR2_CONNECTED
  if( loadState(PWM_ACTUATOR2_CHILD_ID) < 101 ){
    desired_pwm_actuator2_state = int(loadState(PWM_ACTUATOR2_CHILD_ID));
    pwm_actuator2_state = int(loadState(PWM_ACTUATOR2_CHILD_ID));
#ifdef DEBUG
    Serial.println("loaded pwm_actuator2 state from eeprom");
#endif
  }
  pinMode(PWM_ACTUATOR2_PIN, OUTPUT); 
  servo2.attach(PWM_ACTUATOR2_PIN);
  servo2.write(percentage_to_servo(pwm_actuator2_state, servo2_maximum_degrees));
  Serial.println(F("I have a servo 2"));
#endif



  // Set the initial position of the binary output (relay/LED).
#ifdef ANALOG_ACTUATOR1_CONNECTED
  if( loadState(BINARY_ACTUATOR1_CHILD_ID) != 255 ){
    int original_state = loadState(ANALOG_ACTUATOR1_CHILD_ID);
    desired_analog_actuator1_state = original_state;
    analog_actuator1_state = original_state;
  }
  pinMode(ANALOG_ACTUATOR1_PIN, OUTPUT);
  digitalWrite(ANALOG_ACTUATOR1_PIN, percentage_to_servo(analog_actuator1_state,255)); //map(analogRead(ANALOG_SENSOR2_PIN), 0, 100, 0, 255)); //int(analog_actuator1_state * 2.55));
  Serial.println(F("I have an analog actuator 1"));
#endif

#ifdef ANALOG_ACTUATOR2_CONNECTED
  if( loadState(ANALOG_ACTUATOR2_CHILD_ID) != 255 ){
    int original_state = loadState(ANALOG_ACTUATOR2_CHILD_ID);
    desired_analog_actuator2_state = original_state;
    analog_actuator2_state = original_state;
  }
  pinMode(ANALOG_ACTUATOR2_PIN, OUTPUT); 
  digitalWrite(ANALOG_ACTUATOR2_PIN, percentage_to_servo(analog_actuator2_state,255)); //map(analogRead(ANALOG_SENSOR2_PIN), 0, 100, 0, 255)); //int(analog_actuator2_state * 2.55));
  Serial.println(F("I have an analog actuator 2"));
#endif


#ifdef HAS_DISPLAY
  Serial.println(F("I have an OLED screen"));
#endif

#if defined HAS_DISPLAY && defined TWO_LINES
  Serial.println(F("My screen has two lines"));
#endif

  wdt_enable(WDTO_8S);                              // Starts the watchdog timer. If it is not reset once every 2 seconds, then the entire device will automatically restart.                                
}

void send_values()
{

  // Binary sensor
#ifdef BINARY_SENSOR1_CONNECTED
  send(binary_sensor_message.setSensor(BINARY_SENSOR1_CHILD_ID).set(binary_sensor1_state)); wait(RADIO_DELAY);
#endif
#ifdef BINARY_SENSOR2_CONNECTED
  send(binary_sensor_message.setSensor(BINARY_SENSOR2_CHILD_ID).set(binary_sensor2_state)); wait(RADIO_DELAY);
#endif

  // Analog sensor
#ifdef ANALOG_SENSOR1_CONNECTED
  send(analog_sensor_message.setSensor(ANALOG_SENSOR1_CHILD_ID).set(analog_sensor1_state)); wait(RADIO_DELAY);
#endif
#ifdef ANALOG_SENSOR2_CONNECTED
  send(analog_sensor_message.setSensor(ANALOG_SENSOR2_CHILD_ID).set(analog_sensor2_state)); wait(RADIO_DELAY);
#endif

  // Binary actuator
#ifdef BINARY_ACTUATOR1_CONNECTED
  send(binary_actuator_message.setSensor(BINARY_ACTUATOR1_CHILD_ID).set(binary_actuator1_state)); wait(RADIO_DELAY);
#endif
#ifdef BINARY_ACTUATOR2_CONNECTED
  send(binary_actuator_message.setSensor(BINARY_ACTUATOR2_CHILD_ID).set(binary_actuator2_state)); wait(RADIO_DELAY);
#endif

  // PWM actuator (servo)
#ifdef PWM_ACTUATOR1_CONNECTED
  send(percentage_message.setSensor(PWM_ACTUATOR1_CHILD_ID).set(pwm_actuator1_state)); wait(RADIO_DELAY);
#endif
#ifdef PWM_ACTUATOR2_CONNECTED
  send(percentage_message.setSensor(PWM_ACTUATOR2_CHILD_ID).set(pwm_actuator2_state)); wait(RADIO_DELAY);
#endif

  // Analog actuator
#ifdef ANALOG_ACTUATOR1_CONNECTED
  send(percentage_message.setSensor(ANALOG_ACTUATOR1_CHILD_ID).set(analog_actuator1_state)); wait(RADIO_DELAY);
#endif
#ifdef ANALOG_ACTUATOR2_CONNECTED
  send(percentage_message.setSensor(ANALOG_ACTUATOR2_CHILD_ID).set(analog_actuator2_state)); wait(RADIO_DELAY);
#endif


#ifdef HAS_DISPLAY
  send(text_message.setSensor(TEXT_STRING1_CHILD_ID).set(F("Change me"))); wait(RADIO_DELAY);
#endif

#if defined HAS_DISPLAY && defined TWO_LINES
  send(text_message.setSensor(TEXT_STRING2_CHILD_ID).set(F("Change me too"))); wait(RADIO_DELAY);
#endif

}


void loop()
{
  // Send all the child states to the controller. This will initialise things there.
  if( send_all_values ){
#ifdef DEBUG
    Serial.println(F("Sending all values"));
#endif
    send_all_values = false;
    send_values();
  }


  // UPDATING EVERYTHING

#ifdef PWM_ACTUATOR1_CONNECTED
  if( pwm_actuator1_state != desired_pwm_actuator1_state ){
    signed int difference = desired_pwm_actuator1_state - pwm_actuator1_state;
    pwm_actuator1_state = desired_pwm_actuator1_state;
    send(percentage_message.setSensor(PWM_ACTUATOR1_CHILD_ID).set(pwm_actuator1_state)); wait(RADIO_DELAY);
    Serial.print(F("percentage to servo: ")); Serial.println(percentage_to_servo(pwm_actuator1_state, servo1_maximum_degrees));
    servo1.write(percentage_to_servo(pwm_actuator1_state, servo1_maximum_degrees));  
  }
#endif
#ifdef PWM_ACTUATOR2_CONNECTED
  if( pwm_actuator2_state != desired_pwm_actuator2_state ){
    signed int difference = desired_pwm_actuator2_state - pwm_actuator2_state;
    pwm_actuator2_state = desired_pwm_actuator2_state;
    send(percentage_message.setSensor(PWM_ACTUATOR2_CHILD_ID).set(pwm_actuator2_state)); wait(RADIO_DELAY);
    Serial.print(F("percentage to servo: ")); Serial.println(percentage_to_servo(pwm_actuator2_state, servo2_maximum_degrees));
    servo2.write(percentage_to_servo(pwm_actuator2_state, servo2_maximum_degrees));  
  }
#endif
  
  // Update binary actuator 1 if necessary
#ifdef BINARY_ACTUATOR1_CONNECTED
  if( desired_binary_actuator1_state != binary_actuator1_state ){
    Serial.println(F("BUTTON 1 ACTIVE"));
    binary_actuator1_state = desired_binary_actuator1_state;
    digitalWrite(BINARY_ACTUATOR1_PIN, binary_actuator1_state);
    desired_binary_actuator1_delay = SELF_LOCKING_DELAY;
    send(binary_sensor_message.setSensor(BINARY_SENSOR1_CHILD_ID).set(binary_sensor1_state)); wait(RADIO_DELAY);
  }
#endif
#ifdef BINARY_ACTUATOR2_CONNECTED
  // Update binary actuator 2 if necessary
  if( desired_binary_actuator1_state != binary_actuator1_state ){
    Serial.println(F("BUTTON 2 ACTIVE"));
    binary_actuator1_state = desired_binary_actuator1_state;
    digitalWrite(BINARY_ACTUATOR1_PIN, binary_actuator1_state);
    desired_binary_actuator2_delay = SELF_LOCKING_DELAY;
    send(binary_sensor_message.setSensor(BINARY_SENSOR2_CHILD_ID).set(binary_sensor2_state)); wait(RADIO_DELAY);
  }
#endif


#ifdef BINARY_SENSOR1_CONNECTED
  boolean new_binary_sensor1_state = digitalRead(BINARY_SENSOR1_PIN);
  if(new_binary_sensor1_state != binary_sensor1_state){
    binary_sensor1_state = new_binary_sensor1_state;
    send(binary_sensor_message.setSensor(BINARY_SENSOR1_CHILD_ID).set(binary_sensor1_state)); wait(RADIO_DELAY);
  }
#endif
#ifdef BINARY_SENSOR2_CONNECTED
  boolean new_binary_sensor2_state = digitalRead(BINARY_SENSOR2_PIN);
  if(new_binary_sensor2_state != binary_sensor2_state){
    binary_sensor2_state = new_binary_sensor2_state;
    send(binary_sensor_message.setSensor(BINARY_SENSOR2_CHILD_ID).set(binary_sensor2_state)); wait(RADIO_DELAY);
  }
#endif


  wait(20); // Give the system some calm moment before reading the analog value.
  
  // Analog sensor 1
#ifdef ANALOG_SENSOR1_CONNECTED
  new_analog_sensor1_state = map(analogRead(ANALOG_SENSOR1_PIN), 0, 1023, 0, 100);
  if( new_analog_sensor1_state != analog_sensor1_state ){
    analog_sensor1_state = new_analog_sensor1_state;
    Serial.print(F("Analog sensor 1 changed to: ")); Serial.println(analog_sensor1_state);
    send(analog_sensor_message.setSensor(ANALOG_SENSOR1_CHILD_ID).set(analog_sensor1_state)); wait(RADIO_DELAY);
  }
#endif

#ifdef ANALOG_SENSOR2_CONNECTED
  // Analog sensor 2
  new_analog_sensor2_state = map(analogRead(ANALOG_SENSOR2_PIN), 0, 1023, 0, 100);
  if( new_analog_sensor2_state != analog_sensor2_state ){
    analog_sensor2_state = new_analog_sensor2_state;
    Serial.println(F("Analog sensor 2 changed to: ")); Serial.println(analog_sensor2_state);
    send(analog_sensor_message.setSensor(ANALOG_SENSOR1_CHILD_ID).set(analog_sensor2_state)); wait(RADIO_DELAY);
  }
#endif

  // The watchdog makes sure that if the device freezes it will reboot itself. Here we tell it that everything is still running ok.
  if( millis() - lastLoopTime > MAIN_LOOP_DURATION ){
    lastLoopTime = millis();
    wdt_reset();

#if defined(BINARY_ACTUATOR1_CONNECTED) && defined(BINARY_ACTUATOR1_SELF_LOCKING)
    if( desired_binary_actuator1_delay > 0 ){
      if( desired_binary_actuator1_delay == 1 ){
        desired_binary_actuator1_state == 0;
      }
      desired_binary_actuator1_delay--;
    }
#endif
#if defined(BINARY_ACTUATOR2_CONNECTED) && defined(BINARY_ACTUATOR2_SELF_LOCKING)
    if( desired_binary_actuator2_delay > 0 ){
      if( desired_binary_actuator2_delay == 1 ){
        desired_binary_actuator2_state == 0;
      }
      desired_binary_actuator2_delay--;
    }
#endif
  }  
}

// A small helper function which makes it easier to attach servos with different degrees of range.
int percentage_to_servo(byte percentage, byte maximum){
  return map(percentage, 0, 100, 0, maximum);
}


void receive(const MyMessage &message)
{
  Serial.println(F(">> receiving message"));
  
  if (message.isAck()) {
    Serial.println(F("-Got ACK"));
    return;
  }

  
  // Binary actuators
#ifdef BINARY_ACTUATOR1_CONNECTED
  if (message.type == V_STATUS && message.sensor == BINARY_ACTUATOR1_CHILD_ID ){
    desired_binary_actuator1_state = message.getBool(); //?RELAY_ON:RELAY_OFF;
#if !defined(BINARY_ACTUATOR1_SELF_LOCKING)
    saveState(BINARY_ACTUATOR1_CHILD_ID, desired_binary_actuator1_state);
#endif
    Serial.print(F("-New desired actuator 1 state: ")); Serial.println(desired_binary_actuator1_state);
  }
#endif
#ifdef BINARY_ACTUATOR2_CONNECTED
  if (message.type == V_STATUS && message.sensor == BINARY_ACTUATOR2_CHILD_ID ){
    desired_binary_actuator2_state = message.getBool(); //?RELAY_ON:RELAY_OFF;
#if !defined(BINARY_ACTUATOR2_SELF_LOCKING)
    saveState(BINARY_ACTUATOR2_CHILD_ID, desired_binary_actuator2_state);
#endif
    Serial.print(F("-New desired actuator 2 state: ")); Serial.println(desired_binary_actuator2_state);
  }
#endif


  // Servo motors
#ifdef PWM_ACTUATOR1_CONNECTED
  if (message.type == V_DIMMER && message.sensor == PWM_ACTUATOR1_CHILD_ID ){
    desired_pwm_actuator1_state = atoi( message.data );
    desired_pwm_actuator1_state = desired_pwm_actuator1_state > 100 ? 100 : desired_pwm_actuator1_state; // Clip incoming level to valid range of 0 to 100
    desired_pwm_actuator1_state = desired_pwm_actuator1_state < 0   ? 0   : desired_pwm_actuator1_state;
    saveState(PWM_ACTUATOR1_CHILD_ID, desired_pwm_actuator1_state);
    Serial.print(F("-New desired PWM output 1 state: ")); Serial.println(desired_pwm_actuator1_state);
  }
#endif
#ifdef PWM_ACTUATOR2_CONNECTED
  if (message.type == V_DIMMER && message.sensor == PWM_ACTUATOR2_CHILD_ID ){
    desired_pwm_actuator2_state = atoi( message.data );
    desired_pwm_actuator2_state = desired_pwm_actuator2_state > 100 ? 100 : desired_pwm_actuator2_state; // Clip incoming level to valid range of 0 to 100
    desired_pwm_actuator2_state = desired_pwm_actuator2_state < 0   ? 0   : desired_pwm_actuator2_state;
    saveState(PWM_ACTUATOR2_CHILD_ID, desired_pwm_actuator2_state);
    Serial.print(F("-New desired PWM output 2 state: ")); Serial.println(desired_pwm_actuator2_state);
  }
#endif


  // analog actuators
#ifdef ANALOG_ACTUATOR1_CONNECTED
  if (message.type == V_DIMMER && message.sensor == ANALOG_ACTUATOR1_CHILD_ID ){
    desired_analog_actuator1_state = atoi( message.data );
    desired_analog_actuator1_state = desired_analog_actuator1_state > 100 ? 100 : desired_analog_actuator1_state; // Clip incoming level to valid range of 0 to 100
    desired_analog_actuator1_state = desired_analog_actuator1_state < 0   ? 0   : desired_analog_actuator1_state;
    saveState(ANALOG_ACTUATOR1_CHILD_ID, desired_analog_actuator1_state);
    Serial.print(F("-New desired analog output 1 state: ")); Serial.println(desired_analog_actuator1_state);
  }
#endif
#ifdef ANALOG_ACTUATOR2_CONNECTED
  if (message.type == V_DIMMER && message.sensor == PWM_ACTUATOR2_CHILD_ID ){
    desired_analog_actuator2_state = atoi( message.data );
    desired_analog_actuator2_state = desired_analog_actuator2_state > 100 ? 100 : desired_analog_actuator2_state; // Clip incoming level to valid range of 0 to 100
    desired_analog_actuator2_state = desired_analog_actuator2_state < 0   ? 0   : desired_analog_actuator2_state;
    saveState(ANALOG_ACTUATOR2_CHILD_ID, desired_analog_actuator2_state);
    Serial.print(F("-New desired analog output 2 state: ")); Serial.println(desired_analog_actuator2_state);
  }
#endif


  // Text output to display
#ifdef HAS_DISPLAY
  if (message.type == V_TEXT && message.sensor == TEXT_STRING1_CHILD_ID ){
    memset(string1, 0, sizeof(string1));            // Clear the array that holds the string
    strcpy(string1, message.getString());
    Serial.print(F("-New string 1: ")); Serial.println(string1);
    oled.setCursor(0,0);
    oled.print(string1);
    oled.clearToEOL();
  }
#endif
#if defined HAS_DISPLAY && defined TWO_LINES
  if (message.type == V_TEXT && message.sensor == TEXT_STRING2_CHILD_ID ){
    memset(string2, 0, sizeof(string2));            // Clear the array that holds the string
    strcpy(string2, message.getString());
    Serial.print(F("-New string 2: ")); Serial.println(string2);
    oled.setCursor(0,2);
    oled.print(string2);
    oled.clearToEOL();
  }
#endif
}




/**
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
