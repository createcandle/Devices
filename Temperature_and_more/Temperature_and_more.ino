/* 
 * Temperature and more sensor
 * 
 * This device can measure:
 * - Temperature
 * - Humidity 
 * - Air pressure (barometer). 
 * 
 * By looking at changes in air pressure it can predict the weather. 
 * - unknown    (it's still calculating)
 * - stable     (expect more of the same)
 * - unstable   (expect strong changes in weather)
 * - sunny      (expect improving weather)
 * - cloudy     (clouds and possibly rain)
 * - thunderstorm (strong shift to low pressure)
 * 
 * You can use it indoor or outdoor. It does not have to be outside to be able to predict the weather.
 * 
 * The main sensor used is the Bosch BME280.
 *
 *
 * SETTINGS */ 

// You can enable and disable the settings below by adding or removing double slashes ( // ) in front of a line.

#define SECONDS_BETWEEN_SENDING 60                  // Sleep time between reads for the BME sensor (in seconds). Keep this value at 60 if you have enabled the forecast feature, as the forecast algorithm needs a sample every minute. MAximum is 255 (because the value is stored as a byte, instead of the bigger 'int')

#define HAS_TOUCH_SCREEN                            // Did you connect a touch screen?

//#define MY_REPEATER_FEATURE                       // Act as a repeater? The devices can pass along messages to each other to increase the range of your network.

#define RF_NANO                                     // RF-Nano. Check this box if you are using the RF-Nano Arduino, which has a built in radio. The Candle project uses the RF-Nano.


 /* END OF SETTINGS
 *
 *
 *
 */

// PINS
#define TOUCH_SCREEN_RX_PIN 5                       // The receive (RX) pin for the touchscreen. This connects to the transmit (TX) pin of the touchscreen.
#define TOUCH_SCREEN_TX_PIN 6                       // The receive (TX) pin for the touchscreen. This connects to the transmit (RX) pin of the touchscreen.

// The BME280 uses I2C, so its SDA (data) pin should be connected to pin A4 on the Arduino, and the sensor's SCL (clock) pin should be connected to pin A5 on the Arduino.

#ifdef RF_NANO
// If you are using an RF-Nano, you have to switch CE and CS pins.
#define MY_RF24_CS_PIN 9                            // Used by the MySensors library.
#define MY_RF24_CE_PIN 10                           // Used by the MySensors library.
#endif

//#define DEBUG                                     // Do you want to see extra debugging information in the serial output?
//#define DEBUG_SCREEN                              // Do you want to see extra debugging information about the touch screen in the serial output?
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

// Mysensors security
#define MY_ENCRYPTION_SIMPLE_PASSWD "changeme"      // Be aware, the length of the password has an effect on memory use.
//#define MY_SECURITY_SIMPLE_PASSWD "changeme"      // Be aware, the length of the password has an effect on memory use.
//#define MY_SIGNING_SOFT_RANDOMSEED_PIN A7         // Setting a pin to pickup random electromagnetic noise helps make encryption more secure.


// Mysensors advanced settings
#define MY_TRANSPORT_WAIT_READY_MS 10000            // Try connecting for 10 seconds. Otherwise just continue.
//#define MY_RF24_CHANNEL 100                       // In EU the default channel 76 overlaps with wifi, so you could try using channel 100. But you will have to set this up on every device, and also on the controller.
#define MY_RF24_DATARATE RF24_1MBPS                 // The datarate influences range. 1MBPS is the most widely supported. 250KBPS will give you more range.
//#define MY_NODE_ID 10                             // Giving a node a manual ID can in rare cases fix connection issues.
//#define MY_PARENT_NODE_ID 0                       // Fixating the ID of the gatewaynode can in rare cases fix connection issues.
//#define MY_PARENT_NODE_IS_STATIC                  // Used together with setting the parent node ID. Daking the controller ID static can in rare cases fix connection issues.
#define MY_SPLASH_SCREEN_DISABLED                   // Saves a little memory.
//#define MY_DISABLE_RAM_ROUTING_TABLE_FEATURE      // Saves a little memory.







// LIBRARIES
#include <MySensors.h>                              // The MySensors library. Hurray!
#include "Seeed_BME280.h"                           // "Grove - Barometer Sensor BME280". A relatively new library (as of 2018), works well with cheap BME280 sensors from China.
#include <avr/wdt.h>                                // The watchdog timer - if the device becomes unresponsive and doesn't periodically reset the timer, then it will automatically reset once the timer reaches 0.

BME280 bme280;                                      // Create the BME sensor object

#ifdef HAS_TOUCH_SCREEN

#include <SoftwareSerial.h>
SoftwareSerial touch_screen_serial(TOUCH_SCREEN_RX_PIN,TOUCH_SCREEN_TX_PIN);                       // RX (receive) pin, TX (transmit) pin

#define MAX_BASIC_COMMAND_LENGTH 16                 // How many bytes are in the longest basic command?
#define TOUCHSCREEN_WIDTH 240                       // 
#define TOUCHSCREEN_HEIGHT 320                      // 
#define SCREEN_PADDING 10                           // Used to space things, e.g. the distance of content to the edge of the screen.
#define ITEM_HEIGHT 80                              // How many pixels tall a single item on the screen is.
#define LABEL_HEIGHT 25                             // How many pixels in height labels like 'temperature' are.

boolean touched = false;                            // Was the touchscreen just touched?

signed int touchX = 0;                              // Touch screen position X
signed int touchY = 0;                              // Touch screen position Y


// Basic commands for the touch screen. All commands for the TFT should start with 0x7E, but to save storage space this is taken care of in the basicCommand function.
PROGMEM const byte play[] =             {0x07, 0x11, ' ', 'P', 'L', 'A', 'Y', 0xEF,}; // Places the word ' play' on the screen.
PROGMEM const byte on[] =               {0x07, 0x11, ' ', 'O','N', ' ', ' ', 0xEF,}; // Places the word ' on  ' on the screen.
PROGMEM const byte off[] =              {0x07, 0x11, ' ', 'O', 'F', 'F', ' ', 0xEF,}; // Places the word ' off ' on the screen.
PROGMEM const byte w[] =                {0x07, 0x11, 'w', 0x00, ' ', 0x00, 0x00, 0xEF,}; // Places the word 'w    ' on the screen.
PROGMEM const byte menu[] =             {0x07, 0x11, 'M', 'E', 'N', 'U', ' ', 0xEF,}; // Places the word 'menu ' on the screen.
PROGMEM const byte more[] =             {0x07, 0x11, 'M', 'O', 'R', 'E', '>', 0xEF,}; // Places the word 'menu ' on the screen.

PROGMEM const byte set_vertical[] =     {0x03, 0x04, 0x00, 0xEF,}; // To set rotation of the screen to vertical. Try 0x01 or 0x03 instead of the 0x02.
PROGMEM const byte fill_black[] =       {0x04, 0x20, 0x00, 0x00, 0xEF,}; // Fill screen with one color
//PROGMEM const byte fill_blue[] =        {0x04, 0x20, 0x00, 0xFF, 0xEF,}; // Fill screen with one color

PROGMEM const byte text_color_white[] = {0x04, 0x02, 0xFF, 0xFF, 0xEF,}; // white text color. fill screen with one color
//PROGMEM const byte text_color_black[] = {0x04, 0x02, 0x00, 0x00, 0xEF,}; // Dark text color. fill screen with one color
//PROGMEM const byte text_color_red[] =   {0x04, 0x02, 0xF8, 0x00, 0xEF,}; // .. text color. fill screen with one color

//PROGMEM const byte resetTFT[] =         {0x02, 0x05, 0xEF,}; // Resets the TFT. But has no real effect.
//PROGMEM const byte testTFT[] =          {0x02, 0x00, 0xEF}; // Test the TFT, should respond with "OK".
PROGMEM const byte backlight_on[] =     {0x03, 0x06, 0xFF, 0xEF}; // Backlight intensity to half-full
PROGMEM const byte backlight_off[] =    {0x03, 0x06, 0x00, 0xEF}; // Backlight intensity to zero
//PROGMEM const byte serialSpeedUp[] =    {0x03, 0x40, 0x03, 0xEF,}; // Sets communication speed to 57600 (from 9600)
//PROGMEM const byte serialSlowDown[] =   {0x03, 0x40, 0x00, 0xEF,}; // Sets communication speed to 9600 again. Oddly enough, it seems it works fastest at this speed..

// Colors
#define BLACK 0
#define GREY111111 4226
#define GREY333333 12678
#define GREY666666 25388
#define GREYCCCCCC 52857
#define YELLOW 65504
#define LIGHT_YELLOW 65510
#define LIGHT_ORANGE 65126
#define RED 63488
#define GREEN 2016
#define WHITE 65535

// A list of strings stored in program memory in order to save ram.
#define SCREEN_TEMPERATURE 0
#define SCREEN_HUMIDITY 1
#define SCREEN_FORECAST 2
#define SCREEN_STABLE 3
#define SCREEN_SUNNY 4
#define SCREEN_CLOUDY 5
#define SCREEN_UNSTABLE 6
#define SCREEN_THUNDERSTORM 7
#define SCREEN_UNKNOWN 8

typedef byte MessageID;                             // TODO Is this used?  Not really. Simplifying the message table could save some storage space.

struct MessageDef {
  MessageID ID;
  char Description[20];
};

const MessageDef MessageTable[] PROGMEM = {
  {SCREEN_TEMPERATURE,"Temperature"},
  {SCREEN_HUMIDITY,"Humidity"},
  {SCREEN_FORECAST,"Barometer"},
  {SCREEN_STABLE,"STABLE"},
  {SCREEN_SUNNY,"SUNNY"},
  {SCREEN_CLOUDY,"Cloudy"},
  {SCREEN_UNSTABLE,"Unstable"},
  {SCREEN_THUNDERSTORM,"Thunderstorm"},
  {SCREEN_UNKNOWN,"Unknown"},
  };


const byte metadataArraySize = 8;                   // The size of the array to store the serial data we're decoding in.
byte metaData[metadataArraySize];                   // Holds metadata for a replayable signal.
byte screen_brightness = 255;                         // When this is 0 the screen is turned off.


#endif // End of has_touch_screen


#define INTERVAL 1000                               // When active, the internal clock runs every 1000 milliseconds, which is a second.


float previous_temperature = 0;                     // Contains the latest temperature value.
float temperature = 0;                              // Contains the latest temperature value.
byte previous_humidity = 0;                         // Contains the latest humidity value.
byte current_humidity = 0;                          // Contains the latest humidity value.
unsigned int previous_pressure = 0;                 // Contains the previously detected air pressure value.
unsigned int pressure = 0;                          // Contains the latest air pressure value.
byte forecast = 5;                                  // A number that represents a type of weather forecast


// VARIABLES YOU PROBABLY SHOULDN'T CHANGE
#define RADIO_DELAY 100                             // milliseconds betweeen radio signals during the presentation phase.
#define TEMP_CHILD_ID 0                             // For MySensors. Within this node each sensortype should have its own ID number.
#define HUM_CHILD_ID 1
#define BARO_CHILD_ID 2 
#define FORECAST_CHILD_ID 3
#define SCREEN_BUTTON_CHILD_ID 4

// Forecast variables
#define CONVERSION_FACTOR (1.0/10.0)                // Used by forecast algorithm to convert from Pa to kPa, by dividing hPa by 10.
#define STABLE 0                                    // "Stable Weather Pattern"
#define SUNNY 1                                     // "Slowly rising Good Weather", "Clear/Sunny "
#define CLOUDY 2                                    // "Slowly falling L-Pressure ", "Cloudy/Rain "
#define UNSTABLE 3                                  // "Quickly rising H-Press",     "Not Stable"
#define THUNDERSTORM 4                              // "Quickly falling L-Press",    "Thunderstorm"
#define UNKNOWNN 5                                  // "Unknown (More Time needed)
byte lastForecast = 10;                             // Stores the previous forecast. Icons are only redrawn if necessary.
const byte LAST_SAMPLES_COUNT = 5;
float lastPressureSamples[LAST_SAMPLES_COUNT];
byte minuteCount = 0;                               // The forecast algorithm keeps track of time
bool firstRound = true;                             // The forecast algorithm needs to know if it is creating the first forecast.
float pressureAvg;                                  // Average value after 2 hours is used as reference value for the next iteration.
float pressureAvg2;
float dP_dt;                                        // Used by the forecast algorithm.


// MySensors variables
MyMessage temperature_message(TEMP_CHILD_ID, V_TEMP);
MyMessage humidity_message(HUM_CHILD_ID, V_HUM);
MyMessage pressure_message(BARO_CHILD_ID, V_PRESSURE);
MyMessage string_message(FORECAST_CHILD_ID, V_TEXT);
MyMessage button_message(SCREEN_BUTTON_CHILD_ID, V_STATUS); // Allows the controller to turn the screen on and off.


boolean connectedToNetwork = false;                 // Are we connected to the local MySensors network? Used to display the 'w' connection icon.
boolean send_all_values = 1;                        // Sends the state of the toggle to the controller on startup or when requested by the controller.
boolean metric = true;                              // Should the device show metric or Fahrenheit?



/*
// A function to measure how much ram is used.
int freeRam () {
  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}
// via https://playground.arduino.cc/Code/AvailableMemory
*/


void presentation()  {
  // Send the sketch version information to the gateway and Controller
  sendSketchInfo(F("Weather station"), F("1.0")); wait(RADIO_DELAY);

  // Tell the MySensors gateway what kind of sensors this node has, and what their ID's on the node are, as defined in the code above.
  present(TEMP_CHILD_ID, S_TEMP, F("Temperature")); wait(RADIO_DELAY);
  present(HUM_CHILD_ID, S_HUM, F("Humidity")); wait(RADIO_DELAY);
  present(BARO_CHILD_ID, S_BARO, F("Air pressure")); wait(RADIO_DELAY);
  present(FORECAST_CHILD_ID, S_INFO, F("Forecast")); wait(RADIO_DELAY);
#ifdef HAS_TOUCH_SCREEN
  present(SCREEN_BUTTON_CHILD_ID, S_BINARY, F("Screen")); wait(RADIO_DELAY);
  send_all_values = 1;
#endif
}


#ifdef HAS_TOUCH_SCREEN
void send_values(){
#ifdef DEBUG
  Serial.println(F("Sending button states"));
#endif

  send(button_message.setSensor(SCREEN_BUTTON_CHILD_ID).set(1));
  wait(RADIO_DELAY);

}
#endif  



void setup() {
  Serial.begin(115200); // for serial debugging over USB.
  Serial.println(F("Hello, I am a climate sensor"));

#ifdef HAS_TOUCH_SCREEN
  wait(2000);
  touch_screen_serial.begin(9600);
  wait(2000);

#ifdef DEBUG_SCREEN
  Serial.println(F("BC: backlight_on"));
#endif
  basicCommand(backlight_on);                       // Turn on the backlight (if it isn't already).

#ifdef DEBUG_SCREEN
 Serial.println(F("BC: vertical"));
#endif
  basicCommand(set_vertical);                       // Set the screen to vertical mode.

#ifdef DEBUG_SCREEN
  Serial.println(F("BC: white text"));
#endif
  basicCommand(text_color_white);                   // Set text color to white.

#ifdef DEBUG_SCREEN
  Serial.println(F("BC: fill black"));
#endif
  basicCommand(fill_black);                         // Set the background color of the screen to black.

  fontSize(1);                                      // Set the font size of the touch screen to its smallest size.
  
  setCur(SCREEN_PADDING,SCREEN_PADDING);
  writeText(SCREEN_TEMPERATURE);
  
  roundedRectangle(0,ITEM_HEIGHT, TOUCHSCREEN_WIDTH, ITEM_HEIGHT, 0, GREY111111);
  //simpleHorizontal(ITEM_HEIGHT, GREYCCCCCC);

  setCur(SCREEN_PADDING,SCREEN_PADDING + ITEM_HEIGHT);
  writeText(SCREEN_HUMIDITY); 

  //simpleHorizontal(ITEM_HEIGHT * 2, GREYCCCCCC);
  setCur(SCREEN_PADDING,SCREEN_PADDING + (ITEM_HEIGHT * 2));
  writeText(SCREEN_FORECAST);   
  

#endif // end of has_touch_screen


  if(isTransportReady()){                           // If we are connected to the MySensors network.
    Serial.println(F("Connected to gateway!"));
    connectedToNetwork = true;
    metric = getControllerConfig().isMetric;        // Ask if Celcius or Fahrenheit is prefered.
  }else{
    Serial.println(F("! NOT CONNECTED TO GATEWAY"));  
  }


  if(!bme280.init()){
    Serial.println(F("! Sensor error"));
#ifdef HAS_TOUCH_SCREEN 
    writeString("Sensor error",12);
#endif
  }

  wdt_enable(WDTO_2S);                              // Starts the watchdog timer. If it is not reset once every 2 seconds, then the entire device will automatically restart.                                
}


void loop()
{
  // If a presentation is requested, we also send the values of the children.
  if( send_all_values ){
#ifdef DEBUG
    Serial.println(F("RESENDING VALUES"));
#endif
    send_all_values = 0;
    send_values();
  }

  static unsigned long previousMillis = 0;          // Used to keep track of time.
  static byte intervalCounter = 255;                // How may intervals have passed.
  
#ifdef HAS_TOUCH_SCREEN    
  // Check if the screen is being touched.
  readResponse();
#endif



  //
  // MAIN LOOP
  // Runs every few seconds. By counting how often this loop has run (and resetting that counter back to zero after 250 loops), it becomes possible to schedule all kinds of things without using a lot of memory.
  // Maximum time that can be scheduled is 4s * 250 loops = 1000 seconds. So the maximum time between sending data can be 16 minutes.
  //


  if(millis() - previousMillis >= INTERVAL){        // Main loop, runs every second.
    previousMillis = millis();                      // Store the current time as the previous measurement start time.
    
    if(intervalCounter >= SECONDS_BETWEEN_SENDING){
      intervalCounter = 0;
    }else{
      intervalCounter++;
    }
    //Serial.println(intervalCounter);
    
    wdt_reset(); // Reset the watchdog timer


    // Clock schedule
    switch (intervalCounter) {

      //
      // TEMPERATURE
      //
      case 0:                                       // If we are in the first second of the clock

        temperature = bme280.getTemperature();
  
        if(temperature != -127.00 && temperature != 85.00 && temperature != previous_temperature) { // Avoids working with measurement errors.
          previous_temperature = temperature;
          
          if(!metric){
            temperature = (temperature * 9.0)/5.0 + 32.0;
          }
  
          Serial.print(F("Sending temp: ")); Serial.println(temperature);
          connectedToNetwork = false;               // If we receive an echo then this will be set back to true.
          send(temperature_message.set(temperature,1),1); // Ask for an echo from the controller
  
  #ifdef HAS_TOUCH_SCREEN
          displayNetworkStatus();
  
          // Clear the background for this part of the screen.
          roundedRectangle(0,LABEL_HEIGHT, TOUCHSCREEN_WIDTH, ITEM_HEIGHT - LABEL_HEIGHT, 0, BLACK);
          
          setCur(SCREEN_PADDING,SCREEN_PADDING + LABEL_HEIGHT);
          fontSize(4);
          
          char value_string[7];                       // We create a character array to hold a 'string' representation of the temperature value.
          dtostrf(temperature, 0, 1, value_string);   // Here the floating point value is turned into a 'string'.
  
          byte string_length = 4;
          if( abs(temperature) < 10 ){ string_length = 3; }
          if( abs(temperature) > 99 ){ string_length = 5; }
          if( temperature < 0 ){ string_length++; }    // Accounting for the minus character
          
          writeString(value_string,string_length);
  
          fontSize(2);
          if(metric){
            writeString("C",1);
          }
          else{
            writeString("F",1);
          }
        
#endif
        }
      break;


    //
    // HUMIDITY
    //

    case 1:
      
      current_humidity = bme280.getHumidity();

      if( current_humidity != previous_humidity ){
        previous_humidity = current_humidity;
  
  
        Serial.print(F("Sending humidity ")); Serial.println(current_humidity);
        send(humidity_message.set(current_humidity));
              
  #ifdef HAS_TOUCH_SCREEN

        displayNetworkStatus();                       // Show or remove the W icon in the top-right of the screen.
        
        // Clear the background for this part of the screen.
        roundedRectangle(0,ITEM_HEIGHT + LABEL_HEIGHT, TOUCHSCREEN_WIDTH, ITEM_HEIGHT - LABEL_HEIGHT, 0, GREY111111);
  
        setCur(SCREEN_PADDING,SCREEN_PADDING + ITEM_HEIGHT + LABEL_HEIGHT);
        fontSize(4);
        
        // Show humidity value on the screen  
        displayNumber(current_humidity);
        fontSize(2);
        writeString("%",1);

        // Show quality opinion on the screen.
        fontSize(4);
        if (current_humidity > 0 && current_humidity < 30){
          writeString(" DRY",4);
        }
        else if (current_humidity < 65){  
          writeString(" GOOD",5);
        }
        else {
          writeString(" MOIST",6);
        }
        
#endif

      }
      break;



    //
    // PRESSURE
    //
    case 2:
      
      pressure = round(bme280.getPressure() / 100);
      Serial.print(F("Sending pressure ")); Serial.println(pressure);
      send(pressure_message.set((pressure),1));

      forecast = sample(pressure/100);
      //forecast = random(5);                         // Show a random icon. This is used in exhibits, to make it a bit more interesting.

      if (pressure != previous_pressure || forecast != lastForecast) {
        previous_pressure = pressure;
        lastForecast = forecast;
        
#ifdef HAS_TOUCH_SCREEN
        // Show pressure value on the screen
  
        // Clear the background for this part of the screen.
        roundedRectangle(0,(ITEM_HEIGHT * 2) + LABEL_HEIGHT, TOUCHSCREEN_WIDTH, (ITEM_HEIGHT * 2) - LABEL_HEIGHT, 0, BLACK);
  
        setCur(SCREEN_PADDING,SCREEN_PADDING + (ITEM_HEIGHT * 2) + LABEL_HEIGHT);
        fontSize(2);
        
        char pressure_value_string[6];                       // We create a character array to hold a 'string' representation of the temperature value.
        memset(pressure_value_string,0,sizeof(pressure_value_string));// We fill the character array with zeros
        dtostrf(pressure, 0, 0, pressure_value_string);   // Here the floating point value is turned into a 'string'.
  
        writeString(pressure_value_string,4);

        byte circle_size = 40;
        unsigned int cloud_color = WHITE;

        setCur(SCREEN_PADDING, SCREEN_PADDING + (ITEM_HEIGHT * 2) + (LABEL_HEIGHT * 2));
#endif
        
        // UNKNOWN
        if(forecast == 5){
          Serial.println(F("UNKNOWN"));
          send(string_message.set(F("Unknown")));       // Sending the latest forecast to the controller.
#ifdef HAS_TOUCH_SCREEN
          //fontSize(4);
          writeText(SCREEN_UNKNOWN);
#endif
        }
        
        // STABLE WEATHER
        if(forecast == STABLE){
          Serial.println(F("STABLE"));
          send(string_message.set(F("Stable")));       // Sending the latest forecast to the controller.
          cloud_color = WHITE;
#ifdef HAS_TOUCH_SCREEN
          fontSize(4);
          writeText(SCREEN_STABLE);
#endif
        }

        // UNSTABLE WEATHER
        else if(forecast == UNSTABLE){
          Serial.println(F("UNSTABLE"));
          send(string_message.set(F("Unstable")));
#ifdef HAS_TOUCH_SCREEN
          fontSize(4);
          writeText(SCREEN_UNSTABLE);
#endif
        }

        // SUNNY WEATHER
        else if(forecast == SUNNY){
          Serial.println(F("SUNNY"));
          send(string_message.set(F("Sunny")));
          //circle_size = 50;
#ifdef HAS_TOUCH_SCREEN
          //circle(TOUCHSCREEN_WIDTH / 2, ITEM_HEIGHT * 3, circle_size, LIGHT_YELLOW); // x, y, radius, color
          circle(TOUCHSCREEN_WIDTH / 2, ITEM_HEIGHT * 3, circle_size, YELLOW);
          circle(TOUCHSCREEN_WIDTH / 2, ITEM_HEIGHT * 3, circle_size - 10, LIGHT_ORANGE);
          fontSize(1);
          writeText(SCREEN_SUNNY);
#endif
        }

        // CLOUDY WEATHER OR THUNDERSTORM
        else if(forecast == CLOUDY || forecast == THUNDERSTORM){
          if(forecast == THUNDERSTORM){ cloud_color = RED; }

#ifdef HAS_TOUCH_SCREEN
          circle(TOUCHSCREEN_WIDTH / 2, ITEM_HEIGHT * 3, circle_size, cloud_color); // x, y, radius, color
          circle((TOUCHSCREEN_WIDTH / 2) - circle_size, ITEM_HEIGHT * 3 + 10, circle_size - 10, cloud_color);
          circle(int((TOUCHSCREEN_WIDTH / 2) + circle_size), ITEM_HEIGHT * 3 + 20, circle_size - 20, cloud_color);
          fontSize(1);
#endif
          if(forecast == THUNDERSTORM){
            Serial.println(F("THUNDERSTORM"));
            send(string_message.set(F("Thunderstorm")));
#ifdef HAS_TOUCH_SCREEN
            writeText(SCREEN_THUNDERSTORM);
#endif
          }
          else {
            Serial.println(F("CLOUDY"));
            send(string_message.set(F("Cloudy")));
#ifdef HAS_TOUCH_SCREEN
            writeText(SCREEN_CLOUDY);
#endif
          }
        }
      }
      break;
    }
  }
}



float getLastPressureSamplesAverage()
{
  float lastPressureSamplesAverage = 0;
  for (byte i = 0; i < LAST_SAMPLES_COUNT; i++) {
    lastPressureSamplesAverage += lastPressureSamples[i];
  }
  lastPressureSamplesAverage /= LAST_SAMPLES_COUNT;

  return lastPressureSamplesAverage;
}


// Forecast algorithm found here
// http://www.freescale.com/files/sensors/doc/app_note/AN3914.pdf
// Pressure in hPa -->  forecast done by calculating kPa/h
byte sample(float pressure) 
{
  // Calculate the average of the last n minutes.
  int index = minuteCount % LAST_SAMPLES_COUNT;
  lastPressureSamples[index] = pressure;
  
  minuteCount++;
  if (minuteCount > 185) {
    minuteCount = 6;
  }

  if (minuteCount == 5) {
    pressureAvg = getLastPressureSamplesAverage();
  }
  else if (minuteCount == 35) {
    float lastPressureAvg = getLastPressureSamplesAverage();
    float change = (lastPressureAvg - pressureAvg) * CONVERSION_FACTOR;
    if (firstRound) { // first time initial 3 hour 
      dP_dt = change * 2; // note this is for t = 0.5hour
    }
    else {
      dP_dt = change / 1.5; // divide by 1.5 as this is the difference in time from 0 value.
    }
  }
  else if (minuteCount == 65) {
    float lastPressureAvg = getLastPressureSamplesAverage();
    float change = (lastPressureAvg - pressureAvg) * CONVERSION_FACTOR;
    if (firstRound) {                               // First time initial 3 hour
      dP_dt = change;                               // Note this is for t = 1 hour
    }
    else {
      dP_dt = change / 2;                           // Divide by 2 as this is the difference in time from 0 value
    }
  }
  else if (minuteCount == 95) {
    float lastPressureAvg = getLastPressureSamplesAverage();
    float change = (lastPressureAvg - pressureAvg) * CONVERSION_FACTOR;
    if (firstRound) {                               // First time initial 3 hour
      dP_dt = change / 1.5;                         // Note this is for t = 1.5 hour
    }
    else {
      dP_dt = change / 2.5;                         // Divide by 2.5 as this is the difference in time from 0 value
    }
  }
  else if (minuteCount == 125) {
    float lastPressureAvg = getLastPressureSamplesAverage();
    pressureAvg2 = lastPressureAvg; // store for later use.
    float change = (lastPressureAvg - pressureAvg) * CONVERSION_FACTOR;
    if (firstRound) { // first time initial 3 hour
      dP_dt = change / 2;                           // Note this is for t = 2 hour
    }
    else {
      dP_dt = change / 3;                           // Divide by 3 as this is the difference in time from 0 value
    }
  }
  else if (minuteCount == 155) {
    float lastPressureAvg = getLastPressureSamplesAverage();
    float change = (lastPressureAvg - pressureAvg) * CONVERSION_FACTOR;
    if (firstRound) {                               // First time initial 3 hour
      dP_dt = change / 2.5;                         // Note this is for t = 2.5 hour
    } 
    else {
      dP_dt = change / 3.5;                         // divide by 3.5 as this is the difference in time from 0 value
    }
  }
  else if (minuteCount == 185) {
    float lastPressureAvg = getLastPressureSamplesAverage();
    float change = (lastPressureAvg - pressureAvg) * CONVERSION_FACTOR;
    if (firstRound) { // first time initial 3 hour
      dP_dt = change / 3;                           // Note this is for t = 3 hour
    } 
    else {
      dP_dt = change / 4;                           // Divide by 4 as this is the difference in time from 0 value
    }
    pressureAvg = pressureAvg2;                     // Equating the pressure at 0 to the pressure at 2 hour after 3 hours have past.
    firstRound = false;                             // Flag to let you know that this is on the past 3 hour mark. Initialized to 0 outside main loop.
  }

  byte forecast = UNKNOWNN;
  if (minuteCount < 35 && firstRound) {             // If time is less than 35 min on the first 3 hour interval.
    forecast = UNKNOWNN;
  }
  else if (dP_dt < (-0.25)) {
    forecast = THUNDERSTORM;
  }
  else if (dP_dt > 0.25) {
    forecast = UNSTABLE;
  }
  else if ((dP_dt > (-0.25)) && (dP_dt < (-0.05))) {
    forecast = CLOUDY;
  }
  else if ((dP_dt > 0.05) && (dP_dt < 0.25))
  {
    forecast = SUNNY;
  }
  else if ((dP_dt >(-0.05)) && (dP_dt < 0.05)) {
    forecast = STABLE;
  }
  else {
    forecast = UNKNOWNN;
  }

  // Useful for debugging
  //Serial.print(F("BME280 - Forecast at minute "));
  //Serial.print(minuteCount);
  //Serial.print(F(" dP/dt = "));
  //Serial.print(dP_dt);
  //Serial.print(F("kPa/h --> "));
  //Serial.println(weather[forecast]);
  //Serial.println(freeRam());
  
  return forecast;
}


    //for (float i=0; i<=6 ; i=i+.4) {
      // Calculate sun rays end points
      //int resultX = sunPositionX + (int)round( sunSize * cos( i ) );
      //int resultY = sunPositionY + (int)round( sunSize * sin( i ) );
    //}
      







#ifdef HAS_TOUCH_SCREEN

void displayNetworkStatus()                         // Show connection icon on the display
{
  if( connectedToNetwork ){
#ifdef DEBUG_SCREEN
    Serial.println(F("BC: show W icon"));
#endif
    setCur(TOUCHSCREEN_WIDTH - 30, SCREEN_PADDING );
    fontSize(1);
    basicCommand(w);
  }
  else {
#ifdef DEBUG_SCREEN
    Serial.println(F("BC: hide W icon"));
#endif
    roundedRectangle(TOUCHSCREEN_WIDTH - 50,0, 50, LABEL_HEIGHT, 0, BLACK);  
  }
}



void simpleHorizontal(unsigned int y, unsigned int color)                        // Draw a horizontal line on the screen.
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



void circle(unsigned int x, unsigned int y, unsigned int r, unsigned int c) // Draw a pixel on the screen.
{
#ifdef DEBUG_SCREEN
  Serial.println(F("CIRCLE"));
  Serial.print(F("x:")); Serial.println(x);       // top-right x-position
  Serial.print(F("y:")); Serial.println(y);       // top-left y-position
  Serial.print(F("r:")); Serial.println(r);       // radius
  Serial.print(F("c:")); Serial.println(c);       // color
#endif
  byte command[12] = {0x7E, 0x0A, 0x28, highByte(x), lowByte(x), highByte(y), lowByte(y), highByte(r), lowByte(r), highByte(c), lowByte(c), 0xEF};
  
  for( int i=0; i < sizeof(command); i++ ){
    touch_screen_serial.write( command[i] );
    //Serial.print(command[i],HEX); Serial.print(F(" "));
  }
  waitForResponse();
}




// This function writes text to the screen. You can use the setCur function to place the cursor in the desired position first.

// This function writes text to the screen. You can use the setCur function to place the cursor in the desired position first.
void writeText(byte textID)
{
#ifdef DEBUG_SCREEN
  Serial.println(F("WRITETEXT"));
#endif
  byte string_length = strlen_P(MessageTable[textID].Description);

  touch_screen_serial.write( 0x7E );
  touch_screen_serial.write( string_length + 2);
  touch_screen_serial.write( 0x11 );
  for( byte i=0; i < string_length; i++ ){
    char char_byte = pgm_read_byte(MessageTable[textID].Description + i);
    //Serial.print("-"); Serial.print(char_byte);
    touch_screen_serial.write( char_byte );
  }
  touch_screen_serial.write( 0xEF );
  
  waitForResponse();
}


void writeString(char string_array[], byte string_length)
{
#ifdef DEBUG_SCREEN
  Serial.println(F("WRITESTRING"));
#endif
  //byte string_length = strlen_P(MessageTable[textID].Description) + 2;
  
  touch_screen_serial.write( 0x7E );
  touch_screen_serial.write( string_length + 2 );
  touch_screen_serial.write( 0x11 );
  for( byte i=0; i < string_length; i++ ){
    char char_byte = string_array[i];
    //Serial.print("-"); Serial.print(char_byte);
    touch_screen_serial.write( char_byte );
  }
  touch_screen_serial.write( 0xEF );
  
  waitForResponse();
}


#ifdef DEBUG_SCREEN
void drawPix(int x, int y, int c) // Draw a pixel on the screen.
{
  Serial.println(F("DRAWPIX:"));
  Serial.print(F("x: ")); Serial.println(x);
  Serial.print(F("y: ")); Serial.println(y);
  Serial.print(F("c: ")); Serial.println(c);
  byte command[10] = {0x7E, 0x08, 0x21, highByte(x), lowByte(x), highByte(y), lowByte(y), highByte(c), lowByte(c), 0xEF};
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
  byte command[8] = {0x7E, 0x06, 0x01, highByte(x), lowByte(x), highByte(y), lowByte(y), 0xEF};
  for( int i=0; i < sizeof(command); i++ ){
    touch_screen_serial.write( command[i] );
    //Serial.print(command[i],HEX); Serial.print(F(" "));
  }
  waitForResponse();
}


// This function outputs a variable number to the screen. It can show negative and positive numbers. It cannot show floats.
void displayNumber(int number)
{
//#ifdef DEBUG_SCREEN
  Serial.print(F("DISPLAYNUMBER (")); Serial.println(number); 
  Serial.print("Number:"); Serial.println(number);
//#endif
  byte command[8] = {0x7E, 0x06, 0x13, 0x00, 0x0A, highByte(number), lowByte(number), 0xEF};
  for( int i=0; i < sizeof(command); i++ ){
    touch_screen_serial.write( command[i] );
    //Serial.print(command[i],HEX); Serial.print(F(" "));
  }
  waitForResponse();
}


// This function outputs a variable number to the screen. It can show negative and positive numbers. It cannot show floats.
void fontSize(byte font_size)
{
#ifdef DEBUG_SCREEN
  Serial.print(F("TEXTSIZE (")); Serial.println(font_size); 
#endif
  byte command[5] = {0x7E, 0x03, 0x03, lowByte(font_size), 0xEF};
  //byte command[5] = {0x7E, 0x03, 0x03, font_size, 0xEF,};
  for( int i=0; i < sizeof(command); i++ ){
    touch_screen_serial.write( command[i] );
    //Serial.print(command[i],HEX); Serial.print(F(" "));
  }
  waitForResponse();
}


// This function reads the serial data (if available) from the screen.
void readResponse()
{
  volatile int availableSerialCount = touch_screen_serial.available();
  
  if( availableSerialCount < 5 ){ 
    return;
  }
  //Serial.println(F("READRESPONSE"));

  boolean savingMessage = false;                    // When touch message, this starts recording data into an array for analysis.
  //byte metaData[metadataArraySize];               // An array to store the received serial data // TODO: use the metadata array instead. It's just as long, and their uses don't overlap.
  byte metaDataPosition = 0;                        // The metaData array is recycled: here is holds incoming serial data.
  byte startMarker = 0x7E;                          // Any response from the screen will start with this.
  byte endMarker = 0xEF;                            // Any response from the screen will end with this.
  byte rc;                                          // Hold the byte form the serial stream that we're examining.
  
  byte c = touch_screen_serial.peek();
  if( c != startMarker ){
    rc = touch_screen_serial.read();
    Serial.print(F("throwing away left over touch_screen_serial byte:")); Serial.println(rc);
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
#endif

          if( screen_brightness == 0 ){
#ifdef DEBUG_SCREEN
          Serial.println(F("Turning on backlight"));
#endif
            turnOnScreen(true);
          }
          else{
#ifdef DEBUG_SCREEN
          Serial.println(F("Turning off backlight"));
#endif
            turnOnScreen(false);
            //basicCommand(backlight_off);
            //screen_brightness = 0;                         // To indicate that a touch event has just occured.
          }
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
  if( touch_screen_serial.available() ){            // If necessary, clear old messages from the serial buffer.
    clearReceivedBuffer();
  } 

  touch_screen_serial.write(0x7E);                  // Starting byte, is always the same.
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
    wait(10);                                       // Perhaps some more bytes will show up.
    while( touch_screen_serial.available() > 0 ){   // Throwing away the response. All we care about is touch messages, and they are handled in the readResponse function.
      byte x = touch_screen_serial.read();
      //Serial.print(x); Serial.print(F("-"));
    }
    //Serial.println();
  }
  else if( b == 250 ){
    Serial.println(F("Touch screen did not respond to command"));
  }
}



void turnOnScreen(boolean desired_state)
{
//#ifdef DEBUG_SCREEN
  Serial.print(F("Setting screen backlicht to: ")); Serial.println(desired_state);
//#endif
  if( desired_state == true ){
#ifdef DEBUG_SCREEN
    Serial.println(F("BC: backlight_on"));
#endif
    screen_brightness = 255;
    basicCommand(backlight_on);                     // Turn on the touch screen backlight.
    send(button_message.setSensor(SCREEN_BUTTON_CHILD_ID).set(1));
  }
  else {
#ifdef DEBUG_SCREEN
    Serial.println(F("BC: backlight_off"));
#endif
    screen_brightness = 0;
    basicCommand(backlight_off);                    // Turn off the touch screen backlight
    send(button_message.setSensor(SCREEN_BUTTON_CHILD_ID).set(0));
  }
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
    connectedToNetwork = true;
  }
#ifdef HAS_TOUCH_SCREEN
  else if( message.type==V_STATUS && message.sensor == SCREEN_BUTTON_CHILD_ID ){
    boolean desired_state = message.getBool();
    Serial.print(F("-Requested status: ")); Serial.println(desired_state);
    turnOnScreen(desired_state);                // Set the touch screen brightness
  }
#endif
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
