/*
 * 
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

#define INTERVALSBETWEENSENDING 60                  // Sleep time between reads for the BME sensor (in seconds). Keep this value at 60 if you have enabled the forecast feature, as the forecast algorithm needs a sample every minute. MAximum is 255 (because the value is stored as a byte, instead of the bigger 'int')

#define HAS_DISPLAY                                 // Did you connect a display?

//#define FAHRENHEIT                                // Do you want temperature measurements to be in Fahrenheit?

#define MY_ENCRYPTION_SIMPLE_PASSWD "changeme"        // Be aware, the length of the password has an effect on memory use.

#define RF_NANO                                     // RF-Nano. Check this box if you are using the RF-Nano Arduino, which has a built in radio. The Candle project uses the RF-Nano.


 /* END OF SETTINGS
 *
 *
 *
 */

//#define DEBUG // General debug option, give extra information via the serial output when enabled.
//#define MY_DEBUG // Enable MySensors debug output to the serial monitor, so you can check if the radio is working ok.


// Enable and select the attached radio type
#define MY_RADIO_RF24                               // This is a common and simple radio used with MySensors. Downside is that it uses the same frequency space as WiFi.
//#define MY_RADIO_NRF5_ESB                         // This is a new type of device that is arduino and radio all in one. Currently not suitable for beginners yet.
//#define MY_RADIO_RFM69                            // This is an open source radio on the 433mhz frequency. Great range and built-in encryption, but more expensive and little more difficult to connect.
//#define MY_RADIO_RFM95                            // This is a LoRaWan radio, which can have a range of 10km.

// MySensors: Choose your desired radio power level. High power can cause issues on cheap Chinese NRF24 radio's.
//#define MY_RF24_PA_LEVEL RF24_PA_MIN
#define MY_RF24_PA_LEVEL RF24_PA_LOW
//#define MY_RF24_PA_LEVEL RF24_PA_HIGH
//#define MY_RF24_PA_LEVEL RF24_PA_MAX

// Mysensors security
//#define MY_SECURITY_SIMPLE_PASSWD "changeme"        // Be aware, the length of the password has an effect on memory use.
//#define MY_SIGNING_SOFT_RANDOMSEED_PIN A7           // Setting a pin to pickup random electromagnetic noise helps make encryption more secure.


// Mysensors advanced settings
#define MY_TRANSPORT_WAIT_READY_MS 10000            // Try connecting for 10 seconds. Otherwise just continue.
//#define MY_RF24_CHANNEL 100                       // In EU the default channel 76 overlaps with wifi, so you could try using channel 100. But you will have to set this up on every device, and also on the controller.
#define MY_RF24_DATARATE RF24_1MBPS                 // The datarate influences range. 1MBPS is the most widely supported. 250KBPS will give you more range.
//#define MY_NODE_ID 10                             // Giving a node a manual ID can in rare cases fix connection issues.
//#define MY_PARENT_NODE_ID 0                       // Fixating the ID of the gatewaynode can in rare cases fix connection issues.
//#define MY_PARENT_NODE_IS_STATIC                  // Used together with setting the parent node ID. Daking the controller ID static can in rare cases fix connection issues.
#define MY_SPLASH_SCREEN_DISABLED                   // Saves a little memory.
//#define MY_DISABLE_RAM_ROUTING_TABLE_FEATURE      // Saves a little memory.

// MySensors devices form a mesh network by passing along messages for each other. Do you want this node to also be a repeater?
#define MY_REPEATER_FEATURE                       // Add or remove the two slashes at the beginning of this line to select if you want this sensor to act as a repeater for other sensors. If this node is on battery power, you probably shouldn't enable this.

// Only send data when there is a big enough difference with the previous measurement? Because the radio will to be used less this way, this saves battery.
//#define COMPARE_MEASUREMENTS

// Are you using this sensor on battery power?
//#define BATTERY_POWERED                           // Just remove the two slashes at the beginning of this line if your node is battery powered. It will then go into deep sleep as much as possible. While it's sleeping it can't work as a repeater!


// LIBRARIES
#include <MySensors.h>                              // The MySensors library. Hurray!
//#include <Wire.h>                                 // Enables the Wire communication protocol, used by the BME280.
#include "Seeed_BME280.h"                           // "Grove - Barometer Sensor BME280". A relatively new library (as of 2018), works well with cheap BME280 sensors from China.
#include <avr/wdt.h>                                // The watchdog timer - if the device becomes unresponsive and doesn't periodically reset the timer, then it will automatically reset once the timer reaches 0.

BME280 bme280;                                      // Create the BME sensor object

#ifdef HAS_DISPLAY
#include <SoftwareSerial.h>
SoftwareSerial mySerial(8,7);                       // RX, TX
#endif


// VARIABLES YOU CAN CHANGE
#define INTERVAL 1000                               // When active, the main check occurs every 1000 milliseconds, which is a second.
#define COMPARETEMPERATURE 1                        // Send temperature only if it changed? 1 = Yes 0 = No. Can save battery.
#define COMPAREHUMIDITY 1                           // Send temperature only if changed? 1 = Yes 0 = No. Can save battery.
#define COMPAREPRESSURE 1                           // Send temperature only if changed? 1 = Yes 0 = No. Can save battery.

#ifdef COMPARE_MEASUREMENTS
float tempThreshold = 0.1;                          // How big a temperature difference has to minimally be before an update is sent. Makes the sensor less precise, but also less jittery, and can save battery.
byte presThreshold = 10;                            // How big a barometric difference (in Pascal) has to minimally be before an update is sent. Makes the sensor less precise, but also less jittery, and can save battery.
#endif

#ifdef FAHRENHEIT
bool fahrenheit = true;                             // can be set from the settings above. Forces temperature to be calculaten in Fahrenheit.
#else
bool fahrenheit = false;
#endif

// VARIABLES YOU PROBABLY SHOULDN'T CHANGE
#define RADIO_DELAY 100                             // milliseconds betweeen radio signals during the presentation phase.
#define TEMP_CHILD_ID 0                             // For MySensors. Within this node each sensortype should have its own ID number.
#define HUM_CHILD_ID 1
#define BARO_CHILD_ID 2 
#define FORECAST_CHILD_ID 3

// Forecast variables
#define CONVERSION_FACTOR (1.0/10.0)                // Used by forecast algorithm to convert from Pa to kPa, by dividing hPa by 10.
#define STABLE 0                                    // "Stable Weather Pattern"
#define SUNNY 1                                     // "Slowly rising Good Weather", "Clear/Sunny "
#define CLOUDY 2                                    // "Slowly falling L-Pressure ", "Cloudy/Rain "
#define UNSTABLE 3                                  // "Quickly rising H-Press",     "Not Stable"
#define THUNDERSTORM 4                              // "Quickly falling L-Press",    "Thunderstorm"
#define UNKNOWNN 5                                  // "Unknown (More Time needed)
byte lastForecast = 5;                              // Stores the previous forecast. Icons are only redrawn if necessary.
const byte LAST_SAMPLES_COUNT = 5;
float lastPressureSamples[LAST_SAMPLES_COUNT];
byte minuteCount = 0;                               // The forecast algorithm keeps track of time
bool firstRound = true;                             // The forecast algorithm needs to know if it is creating the first forecast.
float pressureAvg;                                  // Average value after 2 hours is used as reference value for the next iteration.
float pressureAvg2;


// Mysensors messages
MyMessage temperatureMsg(TEMP_CHILD_ID, V_TEMP);
MyMessage humidityMsg(HUM_CHILD_ID, V_HUM);
MyMessage pressureMsg(BARO_CHILD_ID, V_PRESSURE);
MyMessage forecastMsg(BARO_CHILD_ID, V_FORECAST);

/*
// A function to measure how much ram is used.
int freeRam () {
  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}
// via https://playground.arduino.cc/Code/AvailableMemory
*/

void setup() {
  //Wire.begin(); // Wire.begin(sda, scl) // starts the wire communication protocol, used to chat with the BME280 sensor.
  Serial.begin(115200); // for serial debugging over USB.

  if(isTransportReady()){
    Serial.println(F("Connected to gateway!"));
  }else{
    Serial.println(F("! NOT CONNECTED TO GATEWAY"));  
  }

#ifdef HAS_DISPLAY
  mySerial.begin(115200);
  wait(1500);
  mySerial.print(F("CLR(0);"));
  mySerial.print(F("SBC(0);"));
  
  if(isTransportReady()){
    mySerial.print(F("DCV16(115,1,w,0);"));
  }

  // Labels
  mySerial.print(F("DCV16(10,155,Barometer,5);"));  // Barometer label

  // Wait icon
  mySerial.print(F("CIRF(120,239,35,3);"));       // Circle as a background
  //mySerial.print(F("SBC(3);"));
  //mySerial.print(F("DCV16(105,230,wait,15);"));     // Show while the forecast is not available yet.
  //mySerial.println(F("SBC(0);"));

  wait(400);
#endif

  if(!bme280.init()){
    Serial.println(F("! Sensor error"));
#ifdef HAS_DISPLAY
    mySerial.println(F("DCV16(10,250,Sensor error,1);"));
#endif
  }

  wdt_enable(WDTO_2S);                              // Starts the watchdog timer. If it is not reset once every 2 seconds, then the entire device will automatically restart.                                
}



void presentation()  {
  // Send the sketch version information to the gateway and Controller
  sendSketchInfo(F("Temperature and more"), F("1.3")); wait(RADIO_DELAY);

  // Tell the MySensors gateway what kind of sensors this node has, and what their ID's on the node are, as defined in the code above.
  present(TEMP_CHILD_ID, S_TEMP, F("Temperature")); wait(RADIO_DELAY);
  present(HUM_CHILD_ID, S_HUM, F("Humidity")); wait(RADIO_DELAY);
  present(BARO_CHILD_ID, S_BARO, F("Barometer")); wait(RADIO_DELAY);
}


void loop()
{
  // You should not change these variables:
  static unsigned long previousMillis = 0;          // Used to keep track of time.
  static byte intervalCounter = 255;                // How may intervals have passed.
  static float temperature = 0;                     // Contains the latest temperature value.
  static byte humidity = 0;                         // Contains the latest humidity value.
  static float pressure = 0;                        // Contains the latest airpressure value. // could this be made into a 16bit int to save memory?
  static byte forecast = 5;                         // A number that represents a type of weather forecast
#ifdef COMPARE_MEASUREMENTS
  static float lastTemperature = -1;                // Contains the previous temperature value.
  static byte lastHumidity = -1;                    // Contains the previous humidity value.
  static int lastPressure = -1;                     // Contains the previous pressure value.
#endif


  //
  // MAIN LOOP
  // Runs every few seconds. By counting how often this loop has run (and resetting that counter back to zero after 250 loops), it becomes possible to schedule all kinds of things without using a lot of memory.
  // Maximum time that can be scheduled is 4s * 250 loops = 1000 seconds. So the maximum time between sending data can be 16 minutes.
  //


  if(millis() - previousMillis >= INTERVAL){        // Main loop, runs every second.
    previousMillis = millis();                      // Store the current time as the previous measurement start time.
    
    if(intervalCounter >= INTERVALSBETWEENSENDING){
      intervalCounter = 0;
    }else{
      intervalCounter++;
    }
    
    wdt_reset(); // Reset the watchdog timer


/*
#ifdef HAS_DISPLAY
    // progress bar integrated in a horizontal separation line
    mySerial.print(F("PL(0,70,"));
      mySerial.print(String(intervalCounter * 4)); 
      mySerial.print(F(",70,"));
      mySerial.print(String(7 + lineColor)); 
      mySerial.println(F(");"));
#endif
*/


    //
    // TEMPERATURE
    //
 
    if (intervalCounter == 0){
      
      //Serial.print(bme280.getTemperature());
      temperature = bme280.getTemperature();
      if ( !getControllerConfig().isMetric || fahrenheit == true) {      // Convert temperature to Fahrenheit if prefered by the controller.
        temperature = temperature * 9.0 / 5.0 + 32.0;
      }

#ifdef COMPARE_MEASUREMENTS
      if(temperature != -127.00 && temperature != 85.00) { // Avoids working with measurement errors.
        if (COMPARETEMPERATURE == 1 && abs(temperature - lastTemperature) < tempThreshold) { // is the temperature difference bigger than the threshold?
          //Serial.print(temperature - lastTemperature);
          Serial.println(F("Temperature difference too small"));
        } else {
#endif

          Serial.print(F("Sending temp: ")); Serial.println(temperature);
          send(temperatureMsg.set(temperature,1));


          if(send(temperatureMsg.set(temperature,1),1)){ // Ask for a receipt from the controller
#ifdef HAS_DISPLAY
            // add W icon
            mySerial.print(F("DCV16(230,1,w,10);"));
          }else {
            // remove W icon
            mySerial.print(F("DCV16(230,1, ,10);"));
#endif        
          }

#ifdef HAS_DISPLAY
          //temperature = -5.67;
          //temperature = random(-40,40);             // + random();
          //Serial.print(F("random temp: "));Serial.println(temperature);
          //Serial.print(F("rounded test temp: ")); Serial.println( (int)round(temperature) );

          // Display a graph
          //Serial.println(minuteCount);
          byte tempX = 70 - (int)round(temperature/2);
          //tempX = 70 - (tempX/2);
          //Serial.println(tempX);
          
          if(minuteCount > 5){
            mySerial.print(F("BOXF("));             // A black box to erase previous graph.
              mySerial.print(String(minuteCount+4));
              mySerial.print(F(",50,"));
              mySerial.print(String(minuteCount+14));
              mySerial.print(F(",89,0);"));
            mySerial.print(F("PS("));               // Display temperature value as a point
              mySerial.print(String(minuteCount+4)); 
              mySerial.print(F(","));
              //mySerial.print(String(70 - (int)round(temperature/2))); 
              mySerial.print(String(tempX)); 
              mySerial.print(F(",7);"));            // Color 8 is dark grey    
          }
          
          mySerial.print(F("DCV16(10,20,Temperature,5);")); // Temperature label
          
          mySerial.print(F("DCV32(120,20,"));       // Temperature value
            mySerial.print( String(temperature) ); 
            mySerial.print(F("  ,15);"));
          
          mySerial.println(F("PL(0,70,240,70,15);"));// Horizontal line
          
#endif
          

#ifdef COMPARE_MEASUREMENTS
          lastTemperature = temperature;            // Save new value to be able to compare in the next loop.
        }
#endif

    }


    //
    // HUMIDITY
    //
  
    else if (intervalCounter == 1){
      
      humidity = bme280.getHumidity();
#ifdef COMPARE_MEASUREMENTS
      if (COMPAREHUMIDITY == 1 && humidity == lastHumidity) {
        Serial.println(F("Humidity difference too small"));
      } else {
#endif
        Serial.print(F("Sending humidity ")); Serial.println(humidity);
        send(humidityMsg.set(humidity));
            
#ifdef COMPARE_MEASUREMENTS
         lastHumidity = humidity;                   // Save new value to be able to compare in the next round.
      }
#endif


#ifdef HAS_DISPLAY
      // Display a graph
      if(minuteCount > 5){
          mySerial.print(F("BOXF("));               // A 10 pixel wide black box to erase the previous graph (if it exists).
            mySerial.print(String(minuteCount+4));
            mySerial.print(F(",119,"));
            mySerial.print(String(minuteCount+14));
            mySerial.print(F(",139,0);"));
            
          mySerial.print(F("PS("));                 // Display humidity value as a point.
            mySerial.print(String(minuteCount+4));  // Minutes are kept by the barometer, and range between 6 and 180; By adding 4 it starts at the same X position as the label.
            mySerial.print(F(","));
            mySerial.print(String(139 - (humidity/5))); // Humidity ranges between 0 and 100%, so the Y value of the dot ranges between 119 and 139.
            mySerial.print(F(",7);"));              // Color 8 is dark grey   
      }

      mySerial.print(F("DCV16(10,90,Humidity,5);"));// Humidity label

      mySerial.print(F("DCV32(120,90,"));           // Display the humidity value.
        mySerial.print(String(humidity)); 
        mySerial.print(F("%  ,15);"));
  
      mySerial.println(F("PL(0,140,240,140,15);"));  // Horizontal line
      
#endif
    }


    //
    // PRESSURE
    //
  
    else if (intervalCounter == 2){
      
      float pressure = bme280.getPressure() / 100;
#ifdef COMPARE_MEASUREMENTS
      if (COMPAREPRESSURE == 1 && abs(pressure - lastPressure) < presThreshold) { // is the pressure difference bigger than the threshold?
        Serial.println(F("Pressure difference too small"));
      } else {
#endif
        Serial.print(F("Sending pressure ")); Serial.println(pressure);
        send(pressureMsg.set((pressure),1));

#ifdef COMPARE_MEASUREMENTS
         lastPressure = pressure;                   // Save new value to be able to compare in the next round.
      }
#endif


#ifdef HAS_DISPLAY
      mySerial.print(F("DCV16(120,155,"));          // Display barometer value
        mySerial.print( String((int)round(pressure)) ); 
        if(forecast == SUNNY || forecast == UNSTABLE){
          mySerial.print(F(" Rising "));
        }else if(forecast == CLOUDY || forecast == THUNDERSTORM){
          mySerial.print(F(" Falling"));
        }else if(forecast == STABLE){
          mySerial.print(F(" Stable "));
        }
        mySerial.println(F("  ,15"));  
#endif
      
    }           


    //
    // FORECAST
    //

    else if (intervalCounter == 3){
      
      forecast = sample(pressure/100);
      //forecast = random(5); // Useful for testing the animations.

      Serial.print(F("Forecast: ")); Serial.println(forecast);
      
      //if (forecast != lastForecast) {
        //Serial.println(F("New forecast, redrawing icons."));
        lastForecast = forecast;

#ifdef HAS_DISPLAY
        // Icon starting variables
        byte sunPositionX = 120;                    // Setting the initial position of the sun icon.
        int  sunPositionY = 240;                    // Setting the initial position of the sun icon.
        byte sunSize = 30;                          // Setting the initial size of the sun icon.
        byte cloudColor = 7;                        // Original cloud color (0 = black, 7 = grey, 8 - dark grey, 9 = brown, 15 = white)
#endif

        // STABLE WEATHER
        if(forecast == STABLE){
          send(forecastMsg.set(F("Stable")));       // Sending the latest forecast to the controller.
#ifdef HAS_DISPLAY
          cloudColor = 15;                          // Color 15 is white
#endif  
        }

        // SUNNY WEATHER
        if(forecast == SUNNY){
          send(forecastMsg.set(F("Sunny")));
#ifdef HAS_DISPLAY
          sunSize = 50;
#endif
        }

        // CLOUDY WEATHER
        if(forecast == CLOUDY){
          send(forecastMsg.set(F("Cloudy")));
        }

        // UNSTABLE WEATHER
        if(forecast == UNSTABLE){
          send(forecastMsg.set(F("Unstable")));
#ifdef HAS_DISPLAY
          sunPositionX = 90;                        // Moving the sun a little up and to the left
          sunPositionY = 220;                           
#endif
        }

        // THUNDERSTORM
        if(forecast == THUNDERSTORM){
          send(forecastMsg.set(F("Thunderstorm")));
#ifdef HAS_DISPLAY
          cloudColor = 1;                           // Color 1 is red
#endif
        }


          
#ifdef HAS_DISPLAY

        //
        // Here we make the weather icons
        //

        // Reset the background color.
        mySerial.print(F("BOXF(10,180,230,300,0);"));

        // Display a sun icon
        if(forecast == SUNNY || forecast == STABLE || forecast == UNSTABLE){
          
          for (float i=0; i<=6 ; i=i+.4) {

            // Calculate sun rays end points
            int resultX = sunPositionX + (int)round( sunSize * cos( i ) );
            int resultY = sunPositionY + (int)round( sunSize * sin( i ) );
            
            mySerial.print("PL(");                  // Draw a sun ray
              mySerial.print( String(sunPositionX) ); 
              mySerial.print(F(",")); 
              mySerial.print( String(sunPositionY) ); 
              mySerial.print(F(",")); 
              mySerial.print( String(resultX)); 
              mySerial.print(F(",")); 
              mySerial.print( String(resultY)); 
              mySerial.print(F(",4));")); 
          }
          mySerial.print(F("CIRF("));               // Sun the sun's central circle
            mySerial.print( String(sunPositionX) ); 
            mySerial.print(F(",")); 
            mySerial.print( String(sunPositionY) ); 
            mySerial.print(F(","));
            mySerial.print(String(sunSize - 10));
            mySerial.print(F(",4);"));           
        }

        // Display a cloud icon
        if(forecast == CLOUDY || forecast == UNSTABLE || forecast == THUNDERSTORM){
          mySerial.print(F("CIRF(90,240,20,"));     // Draw a cloud circle
            mySerial.print(String(cloudColor)); 
            mySerial.print(F(");"));
          mySerial.print(F("CIRF(120,235,25,"));    // Draw a cloud circle 
            mySerial.print(String(cloudColor)); 
            mySerial.print(F(");")); 
          mySerial.print(F("CIRF(150,245,15,"));    // Draw a cloud circle 
            mySerial.print(String(cloudColor)); 
            mySerial.print(F(");")); 
          mySerial.print(F("BOXF(90,255,150,260,"));// Draw the cloud bottom line
            mySerial.print(String(cloudColor)); 
            mySerial.print(F(");"));
        }

        // Display rain
        if(forecast == CLOUDY || forecast == THUNDERSTORM){
          byte rainDrops = 10; // Changes both the amount of raindrops and their angle.
          if(forecast == THUNDERSTORM){ rainDrops = 5; }

          // Rain lines
          for (byte i=90; i<151; i=i+(5 + rainDrops)) {
          
            mySerial.print(F("PL(")); 
              mySerial.print(String(i)); 
              mySerial.print(F(",270,")); 
              mySerial.print(String(i - (20 - (rainDrops * 2)))); // Makes the rain more slanted if it's stormy.
              mySerial.print(F(",290,15);"));
          }
        }
        mySerial.println(); // A newline tells the screen to create the graphics from the commands it has received.

#endif
      //} // End of forecast-was-not-previous-forecast check
    }


/*    
#ifdef BATTERY_POWERED
  // This code will only be included in the sketch if the BATTERY POWERED feature is enabled.
  //if(BME280shouldAsk == true && measuring == false) { // Both parts are done, so we can let the sensor sleep again.
    //unsigned long quicktimecheck = millis(); // To check how much time has passed since the beginning of being awake, and then calculate from that how long to sleep until the next intended measuring time, we need to know how many milliseconds have passed.
    //unsigned long sleeptime = BME280measurementSleepTime - (quicktimecheck - previousMillis); // How much time has passed already during the calculating? Subtract that from the intended interval time.
    //
  if (intervalCounter < SENDCOUNT){             // We're still in the active phase.
    int sleeptime = millis() - previousMillis;  // how many milliseconds should we sleep to reach the end of the second?
    previousMillis = previousMillis - INTERVAL; // playing with the clock. This way we are sure that when the node wakes up, it will seem like a second has passed.
    Serial.println(F("powernapping.."));
    sleep (sleeptime);                          // sleeping until the second is over.
    
  }else{
    int sleeptime = (INTERVAL * INTERVALSBETWEENMEASURING) - (SENDCOUNT * INTERVAL); // how many seconds remain in the minute
    intervalsCounter = INTERVALSBETWEENMEASURING;  // Setting the clock forward, so that when the node wakes up, it seems like the time between measurements has passed.
    Serial.println(F("hibernating.."));
    sleep (sleeptime);
  }
#endif
*/

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
  float dP_dt; // should be a root variable perhaps?
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

/*
to-do: create funtion that writes to the screen to save space
void pri(String truc) {
  if (debug==1) {
    Serial.print(truc);
  }
}
*/

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
