/*
 * 
 * GENTLE ALARM CLOCK
 * 
 * This is a smart alarm clock: 
 * - It takes into account your sleep cycle and tries to wake you up at an opportune moment in that cycle.
 *   It does this in the 30 minutes before the alarm time you set. So if you set 8AM, it will find the best moment in the 7:30AM till 8AM window.
 * - Additionally, it has wake-up light functinality to slowly wake you up. If that doesn't do it, it will sound an audio alarm.
 * - Wake-up light intensity is mirrored to the controller, so that you could set the brightness of other lights in your room to be controller by the alarm clock.
 * 
 * Hardware:
 * - Arduino Nano
 * - Arduino Nano wireless expansion board
 * - NRF24 radio (to connect to MySensors controller)
 * - OLED screen
 * - Rotary encoder knob (KY-040)
 * - Motion sensor (radar type works well, but you can also use PIR)
 * - Bright LED. It should support setting varying brightness levels.
 * - Buzzer (could also trigger an MP3 player, or a voice recorder - have fun!)
 * 
 * 
 * How it works:
 * - It gets the clock time from the controller.
 * - You can set the alarm time (and enable or disable the alarm) with the rotary knob.
 * 
 * - Each minute it detects and sends the total seconds of movement during the previous minute.
 *   You can use a motion sensor of your choice for this. Point it at your sleepingplace in the bed.
 * 
 * 
 * SETTINGS */ 

byte motionThreshold = 8;                   // How many seconds of movement per 5 minutes should there be to start the wake-up procedure. Recommend to stay between 10 and 30.

#define MY_REPEATER_FEATURE                         // Act as signal repeater. Should this sensor act as a repeater for your other devices? This can help the signal spread further.

#define RF_NANO                                     // RF-Nano. Check this box if you are using the RF-Nano Arduino, which has a built in radio. The Candle project uses the RF-Nano.


 /* END OF SETTINGS
 * 
 * 
 * 
 * 
 * 
 * TO-DO
 * - Decide on a measurement value to send to the controller. "custom" would make sense, but may not be as widely supported by controllers.
 * - Toggle the alarm on-off status from the controller. This allows the workday logic to move to the controller.
 * - Turn off the screen (or show less data on it) during the night. Lower screen brightness somehow.
 * - Fix bug where audio alarm can stay on despite rotating the knob to turn it off.
 * - Check if internal clock is somewhat accurate.
 * - Make rotating knob work better. Maybe use a library..
 * - Add snooze?
 * - allow alarm to be triggered from outside the device. Useful in case of fire.
 * - MP3 player pin that stays high during the ringing phase.
 * 
 * NICE-TO-HAVE
 * - Calculate the work days (mon-fri) on-device, and offer a toggle to only sound the alarm on those days.
 * - Maybe add another separate button for enabling/disabling the alarm.
 * - make the minute loop counter work by adding up 60 loops of a second. But might make the clock very imprecise.
 * - Sensitivity dial in controller
 * - Set alarm time on the controller
 */


#ifdef RF_NANO
// If you are using an RF-Nano, you have to switch CE and CS pins.
#define MY_RF24_CS_PIN 9                            // Used by the MySensors library.
#define MY_RF24_CE_PIN 10                           // Used by the MySensors library.
#endif



// Enable MySensors debug output to the serial monitor, so you can check if the radio is working ok.
//#define MY_DEBUG 
//#define MY_DEBUG_VERBOSE_SIGNING


// Enable and select the attached radio type
#define MY_RADIO_RF24                               // This is a common and simple radio used with MySensors. Downside is that it uses the same frequency space as WiFi.
//#define MY_RADIO_NRF5_ESB                         // This is a new type of device that is arduino and radio all in one. Currently not suitable for beginners yet.
//#define MY_RADIO_RFM69                            // This is an open source radio on the 433mhz frequency. Great range and built-in encryption, but more expensive and little more difficult to connect.
//#define MY_RADIO_RFM95                            // This is a LoRaWan radio, which can have a range of 10km.

// MySensors: Choose your desired radio power level. High and max power can cause issues on cheap Chinese NRF24 radios.
//#define MY_RF24_PA_LEVEL RF24_PA_MIN
//#define MY_RF24_PA_LEVEL RF24_PA_LOW
//#define MY_RF24_PA_LEVEL RF24_PA_HIGH
#define MY_RF24_PA_LEVEL RF24_PA_MAX

// Mysensors security
// Do you want encrypted communication? If you do, then all devices in your network need to use the same password.
#define MY_ENCRYPTION_SIMPLE_PASSWD "changeme"
//#define MY_SECURITY_SIMPLE_PASSWD "changeme"        // Be aware, the length of the password has an effect on memory use.
//#define MY_SIGNING_SOFT_RANDOMSEED_PIN A7         // Setting a pin to pickup random electromagnetic noise helps make encryption more secure.

// Mysensors advanced settings
#define MY_TRANSPORT_WAIT_READY_MS 10000            // Try connecting for 10 seconds. Otherwise just continue.
//#define MY_RF24_CHANNEL 100                       // In EU the default channel 76 overlaps with wifi, so you could try using channel 100. But you will have to set this up on every device, and also on the controller.
#define MY_RF24_DATARATE RF24_1MBPS               // Slower datarate makes the network more stable?
//#define MY_NODE_ID 10                             // Giving a node a manual ID can in rare cases fix connection issues.
//#define MY_PARENT_NODE_ID 0                       // Fixating the ID of the gatewaynode can in rare cases fix connection issues.
//#define MY_PARENT_NODE_IS_STATIC                  // Used together with setting the parent node ID. Daking the controller ID static can in rare cases fix connection issues.
#define MY_SPLASH_SCREEN_DISABLED                   // Saves a little memory.
//#define MY_DISABLE_RAM_ROUTING_TABLE_FEATURE      // Saves a little memory.


#define ONE_SECOND 1000                             // How many milliseconds does a second last?
#define LOOPDURATION 60000                          // The main loop runs every x milliseconds. It's like a minute counter on a clock.
//#define MEASUREMENT_INTERVAL 5                    // After a number of loops we start again.


// LIBRARIES (in the Arduino IDE go to Sketch -> Include Library -> Manage Libraries to add these if you don't have them installed yet.)
#include <MySensors.h>                              // MySensors library                  

#define INCLUDE_SCROLLING 0
#define OLED_I2C_ADDRESS 0x3C
#include <SSD1306Ascii.h>                         // Simple drivers for the OLED screen.
#include <SSD1306AsciiAvrI2c.h>
SSD1306AsciiAvrI2c oled;


// Clock variables
byte hours = 12;
byte minutes = 0;
uint32_t unixTime = 0;

// The lines below may be useful for a future feature.
// leap year calulator expects year argument as years offset from 1970
//#define LEAP_YEAR(Y)     ( ((1970+(Y))>0) && !((1970+(Y))%4) && ( ((1970+(Y))%100) || !((1970+(Y))%400) ) )
//static  const uint8_t monthDays[]={31,28,31,30,31,30,31,31,30,31,30,31}; // API starts months from 1, this array starts from 0


// Alarm variables
#define SADEH_MOTION_THRESHOLD 2                    // How many movements a minute will we consider as enough to officially count as 'light sleep'
boolean alarmSet = false;                           // Has the user enabled the alarm?
byte alarmLevel = 0;                                // has multiple levels to indicate current alarm state.
#define ALARM_DISABLED 0                            // The alarm should not go off
#define ALARM_SET 1                                 // The alarm should go off
#define ALARM_SEARCHING 2                           // Now looking for the opportune moment to wake the user
#define ALARM_WAKING 3                              // Now turning up the lights.
#define ALARM_RINGING 4                             // Now also making noise.
#define ALARM_DEADLINE 5                            // The user's ultimate alarm time has been reached, and there was no opportune moment earlier.
#define ALARM_SNOOZING 6                            // The user wants to snooze.
#define ALARM_DONE 7                                // The user is awake. The alarm shouldn't go off until tomorrow.
#define ALARM_OUTSIDE_SOURCE 8                      // The alarm was triggered from the controller. For example, in case of fire or carbon monoxide detection.
boolean beep = false;                               // Is the speaker currently in a beep? Used to making the beep ---- beep ---- beep noise pattern when the alarm is in the ringin phase.

byte alarmHours = 0;
byte alarmMinutes = 0;
byte displayHours = 0;
byte displayMinutes = 0;

byte snoozeMinutes = 5;

byte send_all_values = true;                        // When this is set to true, al current states will be sent to the controller. This will initialise them there.

// Sadeh algorithm variables
byte consecutiveSleepMinutesRadar = 0;
boolean detectedREM = false;
byte minutesSinceREM = 0;


// Rotary encoder knob
#define ROTARY_CLK_PIN A0                           // Connected to CLK on the KY-040 rotary encoder
#define ROTARY_DT_PIN A1                            // Connected to DT on the KY-040 rotary encoder
#define ROTARY_SWITCH_PIN A2                        // Connected to SW on the KY-040 rotary encoder
int previousRotaryValue;                            // The previous value read from the rotary encoder, to compare against.
boolean rotarySwitchPressed = 0;                    // The state of the push button on the rotary encoder.
boolean lastKnobDirection = 1;                      // 0 = Counter clockwise, 1 = clockwise.

// Motion sensor details
#define MOTION_SENSOR_PIN 3                         // On what pin is the radar sensor connected?

// LED details
#define LED_PIN 4                                   // The pin that is connected to the LED on/off control.
#define LED_PWM_LENGTH 20                           // MICROseconds that each PWN up-and-down phase lasts. You may have too fine-tune this for your LED.
int brightness = 0;
byte brightnessPercentage = 0;

// Audio player
#define AUDIO_PLAYER_PIN 6                          // The pin where an audio player can be triggered. This turns on the entire time the alarm is ringing. (optional)

// Speaker
#define SPEAKER_PIN 7                               // The pin where a speaker is connected (optional).


// Mysensors settings.
#define RADIO_DELAY 100                             // Milliseconds between sending radio signals. This keeps the radio happy.
#define CHILD_ID_STATUS 0                           // Child ID of the sensor
#define CHILD_ID_MOTION_SENSOR 1                    // Child ID of the sensor
#define CHILD_ID_SET_ALARM 2                        // Allows the alarm to be turned on or off from the controller.
#define CHILD_ID_RINGING 3                          // Allows the controller to set other devices in the room to turn on when the alarm clock is ringing.
#define CHILD_ID_SENSITIVITY 4                      // Set the movement threshold from the controller interface.


MyMessage statusMessage(CHILD_ID_STATUS,V_TEXT);    // Sets up the message format that we'll be sending to the MySensors gateway later. The first part is the ID of the specific sensor module on this node. The second part tells the gateway what kind of data to expect.
MyMessage motionMessage(CHILD_ID_MOTION_SENSOR, V_TEMP);  // Sets up the message format that we'll be sending to the MySensors gateway later.
MyMessage dimmerMessage(CHILD_ID_RINGING, V_PERCENTAGE);// Create a dimmer that can be used on the controller to set the value of, for example. another lamp.
MyMessage relayMessage(CHILD_ID_SET_ALARM, V_STATUS); // Allow the controller to enable or disable the alarm


void presentation()
{
  // send the sketch version information to the gateway and Controller
  sendSketchInfo(F("Gentle alarm clock"), F("1.6")); wait(RADIO_DELAY);
    
  // Register all child sensors with the gateway
  present(CHILD_ID_STATUS, S_INFO, F("Status")); wait(RADIO_DELAY);          // General status of the device, as well as the sleep phase
  present(CHILD_ID_MOTION_SENSOR, S_TEMP, F("Motion detected level")); wait(RADIO_DELAY);  // Total motion count for the past five minutes
  present(CHILD_ID_RINGING, S_DIMMER, F("Dimmer level")); wait(RADIO_DELAY);       // The level of the built-in wake-up LED is also mirrored to the controller. That way you could perhaps use some automation to set another lamp in the room to also slowly rise in brightness.
  present(CHILD_ID_SET_ALARM, S_BINARY, F("Alarm set")); wait(RADIO_DELAY);      // Allow the controller to turn the alarm on or off.
  present(CHILD_ID_SENSITIVITY, S_DIMMER, F("Motion sensitivity")); wait(RADIO_DELAY);      // Set the motion count threshold that will trigger alarm.

  send_all_values = true;
}


void setup() 
{
  // Output updates over the serial port
  Serial.begin(115200);
  while (!Serial) {}                                // Is this really necessary?
  Serial.println(F("Hello world!"));

  pinMode(MOTION_SENSOR_PIN, INPUT);                // Set motion sensor pin as input

  // Get last known preferences from onboard storage
  alarmHours = loadState(1);                        // To what hour was the alarm set?
  if(alarmHours > 24){alarmHours = 8;}
  
  alarmMinutes = loadState(2);                      // To which minute was the alarm set?
  if(alarmMinutes > 55){alarmMinutes = 0;}

  if(loadState(3) != 255){
    alarmSet = loadState(3);                          // Was the alarm set to on or off?
  }
  alarmLevel = alarmSet;                            // At startup set the alarm level to reflect if the alarm should be enabled or disabled.
  
  if(loadState(4) != 255){
    motionThreshold = loadState(4);                 // What is the desired sensitivity
  }
 
  // rotary encoder knob
  pinMode (ROTARY_CLK_PIN,INPUT);                   // Rotary encoder clock pin
  pinMode (ROTARY_DT_PIN,INPUT);                    // Rotary encoder data pin
  pinMode(ROTARY_SWITCH_PIN,INPUT_PULLUP);          // Rotary encoder switch pin
  previousRotaryValue = digitalRead(ROTARY_CLK_PIN); 
  rotarySwitchPressed = digitalRead(ROTARY_SWITCH_PIN);
  

  // Start the display (if there is one)
  oled.begin(&Adafruit128x64, OLED_I2C_ADDRESS);
  oled.setFont(Adafruit5x7);
  oled.ssd1306WriteCmd(SSD1306_DISPLAYON);
  oled.setScroll(false);
//  oled.set2X();
//  oled.setCursor(0,0);
//  oled.print(F("ZZ:ZZ"));
  updateClockDisplay();                             // First update of the display to set the alarm time.


  // LED
  pinMode(LED_PIN, OUTPUT);                         // Set the LED pin as output
  analogWrite(LED_PIN, 0);                          // Let's test the LED light
  wait(2000);
  analogWrite(LED_PIN, 255);
  wait(2000);
  analogWrite(LED_PIN, 0);


  // Check if there is a network connection
  if(isTransportReady()){
    Serial.println(F("Connected to gateway!"));
    
    //Serial.print(F("Time: ")); Serial.println(controllerTime);

    // Show connection icon on the display
    oled.set1X();
    oled.setCursor(115,0);
    oled.print(F("w"));

  }else{
    Serial.println(F("! NOCONNECTION"));
    
    oled.set1X();
    oled.setCursor(115,0);
    oled.print(F(" "));
    
  }
  
  wdt_enable(WDTO_8S);                              // Starts the watchdog timer. If it is not reset once every few seconds, then the entire device will automatically restart.                                
}


void send_values()
{
  send(statusMessage.setSensor(CHILD_ID_STATUS).set( F("Hello") )); wait(RADIO_DELAY);
  send(dimmerMessage.set(0)); wait(RADIO_DELAY);  // Reset the dimmer level to 0.
  send(relayMessage.setSensor(CHILD_ID_SET_ALARM).set(alarmSet)); wait(RADIO_DELAY);  // Send the current setting of the alarm (on or off)
  requestTime(); wait(RADIO_DELAY);               // Request the current time from the controller.
}


void loop()
{

/*  the main loop has four levels:
 *  - Continuously:
 *  - - Check if the rotary knob has been turned.
 *  - Flickering: this runs at the rate of 1000 times per second. 
 *  - - It is used to PWM the LED.
 *  - Flutter: this runs once a second. It does things like 
 *  - - Increase the LED brightness once the alarm is in ringing mode, and 
 *  - - Check if the motion sensor is in its active state.
 *  - Heartbeat: this runs once a minute. 
 *  - - It takes note of the total movement over the last minute, and sends along the data.
 *  - - It checks if it's time to wake up the user. 
 *  - - It also updates the minute counter on the display
 *  
 */

  // Send everything to the controller. This will initialise things there.
  if( send_all_values ){
    //Serial.println(F("Sending all values"));
    send_all_values = false;
    send_values();
  }




  // Main loop variables
  static unsigned long lastLoopTime = 0;            // Holds the last time the main loop ran.
  static int loopCounter = 0;                       // Count how many heartbeat loops (minutes) have passed.

  // Creating variables to track sleep.
  static byte motionCounter = 0;                    // The movement count for the last minute.
  static byte movementsList[5];                     // An arrray (list) that stores the last 5 motionCounter values.
  static int motionTotal = 0;                       // Total movement count for the past 5 minutes



  // Rotary knob
  static int rotaryValue;
  static boolean takeStep = 0;
  rotaryValue = digitalRead(ROTARY_CLK_PIN);
  if (rotaryValue != previousRotaryValue){          // Check if the rotary encoder knob is rotating
    if(alarmLevel == ALARM_WAKING || alarmLevel == ALARM_RINGING){                             // In case the alarm was ringing, pressing or rotating the knob should turn off the alarm until the next day.
        turnOffRinging();
        alarmLevel = ALARM_SNOOZING;
        if(snoozeMinutes == 0){snoozeMinutes = 5;}
      }
                                // In case the alarm was still making sound somehow, turn it off.
    takeStep = !takeStep;                           // On every loop though this gets changed into its opposite. So 0 -> 1 -> 0 -> 1 etc. This is used to remove some strange behaviour.
    if(takeStep){                                   // If we have already ignored a step, then go further.
      // Check in which direction it's rotating.
      if (digitalRead(ROTARY_DT_PIN) != rotaryValue) {
        Serial.println(F("Counterclockwise"));

        if(lastKnobDirection == 0){                 // Rotating left rapidly decreases the alarm time.

          if(alarmLevel == ALARM_SNOOZING){
            // if snoozing, then rotating the knob adjusts the snooze minutes.
            if(snoozeMinutes >= 6){snoozeMinutes = snoozeMinutes - 5;}
          }else{
            // If not snoozing, then rotating the knob adjusts the alarm time.
            if(alarmMinutes <= 30){
              alarmMinutes = 30;
              if(alarmHours == 0){alarmHours = 23;}else{alarmHours--;}
            }
            if(alarmMinutes >= 30){
              alarmMinutes = 30;
            }
            Serial.print(F("New alarm hours: ")); Serial.println(alarmHours); 
          }
          //lastKnobDirection = 1;
        }
        lastKnobDirection = 0;
      } else {                                      // Rotating right slowly increases the alarm time.
        if(lastKnobDirection == 1){
        Serial.println (F("Clockwise"));
          if(alarmLevel == ALARM_SNOOZING){
            // if snoozing, then rotating the knob adjusts the snooze minutes.
            if(snoozeMinutes < 15){snoozeMinutes = snoozeMinutes + 5;}
          }else{
            // If not snoozing, then rotating the knob adjusts the alarm time.
            alarmMinutes = alarmMinutes + 5;
            if(alarmMinutes > 55){
              alarmMinutes = 0; 
              alarmHours = alarmHours + 1;
              if(alarmHours > 23){alarmHours = 0;}
            }
            Serial.print(F("New alarm: ")); Serial.print(alarmHours); Serial.print(F(":")); Serial.println(alarmMinutes);
          }
        }
        lastKnobDirection = 1;
      }
      updateClockDisplay();
    }
  }
  previousRotaryValue = rotaryValue;


  // PRESS KNOB BUTTON
  
  if (digitalRead(ROTARY_SWITCH_PIN) == 1 && rotarySwitchPressed == 0){
    rotarySwitchPressed = 1;
    Serial.println(F("Button pressed"));
    Serial.print(F("Alarm level: "));Serial.println(alarmLevel);
    if(alarmLevel > 1){
      Serial.println(F("Resetting alarm"));
      turnOffRinging();
      alarmLevel = alarmSet;
    }else{
      alarmSet = !alarmSet;                         // If the alarm is not ringing, then pressing the button turns the alarm setting on or off completely.
      alarmLevel = alarmSet;
      Serial.print(F("Alarm set to: "));Serial.println(alarmSet);
      saveState(3, alarmSet);                       // Store the preference in local memory
      send(relayMessage.set(alarmSet));             // Tell the controller that the user has changed the alarm setting.
    }

    updateClockDisplay();

    wait(100);
  } else if (digitalRead(ROTARY_SWITCH_PIN) == 0){
    rotarySwitchPressed = 0;
    wait(100);
  }


  //
  // FLICKERING - EVERY MICROSECOND
  // Software pulse width modulation (PWM) for the high brightness LED, using modulo.
  //

  if(alarmLevel == ALARM_WAKING || alarmLevel == ALARM_RINGING){

    unsigned int pulsy = map(brightness,0,255,0,LED_PWM_LENGTH); // Depending on the intended brightness level, the LED should be on during a proportional period of the PWM time.

    if(micros() % LED_PWM_LENGTH < pulsy ){
      digitalWrite(LED_PIN, 1);
      //Serial.print(F("|")); // For debugging
    }else{
      digitalWrite(LED_PIN, 0);
      //Serial.print(F(".")); // For debugging
    }
  }else{
    digitalWrite(LED_PIN, 0);  
  }
   

  //
  // FLUTTER - EVERY SECOND
  //

  static boolean loopDone = false;                        // Tsed to make sure the 'once every millisecond' things only run once every millisecond (or 2.. sometimes the millis() function skips a millisecond.);

  // Allow the next loop to only run once. This entire construction saves memory by not using a long to store the last time the loop ran.
  if( (millis() % ONE_SECOND) > ONE_SECOND - 4 && loopDone == true ) {
    loopDone = false;  
  }

  // Main loop to time actions.
  if( (millis() % ONE_SECOND) < 4 && loopDone == false ) { // This approach to measuring the passage of time saves a tiny bit of memory.
    loopDone = true;
    
    
    //if (millis() - lastLoopTime > ONE_SECOND) {
    //lastLoopTime = millis(); // this variable is now already used by the minute counter.

    wdt_reset(); // Reset the watchdog timer every second. If the device crashes, then the watchdog won't be reset, and this will in turn cause it to reset the entire device.

    // Check if the movement sensor is seeing movement.
    boolean motionState = digitalRead(MOTION_SENSOR_PIN);
    if (motionState == HIGH) {
      motionCounter++;
      Serial.print(F("~"));
      Serial.print(motionCounter);
    }

   
    // Show that movement is being detected on the screen.
    oled.set1X();
    oled.setCursor(115,1);                            // In the top-right corner..
    if(motionCounter % 2){
      oled.print(F("_"));
    }else{
      oled.print(F("."));
    }
 


    // WAKE UP! Every second we make the LED a little brighter.
    if(alarmLevel == ALARM_WAKING || alarmLevel == ALARM_RINGING){
      brightness++;
      if(brightness <= 255){
        brightnessPercentage = map(brightness, 0, 255, 0, 100); 
        send(dimmerMessage.set(brightnessPercentage));
        //Serial.print(F("Sending dimmer brightness: *")); Serial.println(brightnessPercentage);
      }
    }

    // WAKE UP! Time to also make noise.
    if(alarmLevel == ALARM_WAKING && brightness > 255){
      alarmLevel = ALARM_RINGING;
    }

    // The alarm has been on for a while, and has not been turned off by the user. Perhaps the user is fast asleep or not home. We reset the alarm. This will create a second chance if there is still enough time and movement. Otherwise it will just be set for the next day.
    if(brightness == 765){
      turnOffRinging();
      alarmLevel = alarmSet;
    }

    // Play MP3
    if(alarmLevel == ALARM_RINGING){
      digitalWrite(AUDIO_PLAYER_PIN, 1);
    }else{
      digitalWrite(AUDIO_PLAYER_PIN, 0);
    }

    // Beep
    if(alarmLevel == ALARM_RINGING && beep == false){
      tone(SPEAKER_PIN, 1000);
      beep = true;
    }else{
      noTone(SPEAKER_PIN);
      beep = false;
    }

  }




  //
  // HEARTBEAT - EVERY MINUTE
  // Runs every minute. By counting how often this loop has run (and resetting that counter back to zero after a number of loops), it becomes possible to schedule all kinds of things without using a lot of memory.
  //


  if (millis() - lastLoopTime >= LOOPDURATION) {
    lastLoopTime = millis();
    loopCounter++;
    if(loopCounter > 5){
      Serial.print(F("loopCounter ")); Serial.println(loopCounter);  
      loopCounter = 1;
      requestTime();

      Serial.print(F("Minutes since REM phase: ")); Serial.println(minutesSinceREM);
    }

    // Fun but incomplete stuff to detect REM phases
    if(detectedREM && minutesSinceREM < 250){
      minutesSinceREM++;
    }

    // Update the time
    unixTime += 60;                                 // Add a minute to the clock. Maybe adding 59 is better?
    breakUpTime(unixTime);                          // Turn the unix time into human-readable time


    // Save the alarm details. (They will only be overwritten if they have changed).
    saveState(1, alarmHours);
    saveState(2, alarmMinutes);
    saveState(3, alarmSet);
    saveState(4, motionThreshold);


    // ALARM ACTIVE CHECK - Should we start the wake up procedure?
    if(alarmLevel == ALARM_SET && hours == alarmHours && minutes == alarmMinutes){
      Serial.println(F("Alarm now searching (waiting for movement)"));
      alarmLevel = ALARM_SEARCHING;  // Here we enable searching for the right moment to wake up.
      //send(statusMessage.setSensor(CHILD_ID_STATUS).set( F("Starting wake up") )); wait(RADIO_DELAY);
    }


    // If there was a lot of movement in the past 5 minutes, then the user is sleeping lightly, and it's now time to slowly turn on the lights.
    // if(alarmSet == true && alarmSearching == true && movementsList[1] >= SADEH_MOTION_THRESHOLD && movementsList[2] >= SADEH_MOTION_THRESHOLD && movementsList[3] >= SADEH_MOTION_THRESHOLD &&  movementsList[4] >= SADEH_MOTION_THRESHOLD && movementsList[5] >= SADEH_MOTION_THRESHOLD){
    if(alarmLevel == ALARM_SEARCHING && motionTotal >= motionThreshold){ // if there was a high movement count in the last 5 minutes, then the user is in a light sleep moment. 
      Serial.println(F("Entering waking phase "));
      alarmLevel = ALARM_WAKING;
    }

    // If we are in the alarm searching phase, but we didn't find a good moment to wake up the user, then at the end just sound the alarm at the defined alarm time. Like a normal alarmclock.
    if(alarmLevel == ALARM_SEARCHING && hours == displayHours && minutes == displayMinutes){
      alarmLevel = ALARM_RINGING;
    }

    // If the user is snoozing, this counts down back to the normal alarm ringing state.
    if(alarmLevel == ALARM_SNOOZING){
      
      if(snoozeMinutes == 0){
        alarmLevel = ALARM_RINGING;  
      } else if (snoozeMinutes > 0){
        snoozeMinutes--;
      }
    }

    if(alarmLevel == ALARM_RINGING){
      if(brightness < 254){brightness = 254;} // This is here for the snooze funtion.
    }

    // SADEH algorithm. All this is not really required.
    static byte consecutiveSleepMinutesMotion = 0;
    static byte consecutiveAwakeMinutesMotion = 0;
    if(motionCounter < SADEH_MOTION_THRESHOLD){
      consecutiveAwakeMinutesMotion = 0;
      if(consecutiveSleepMinutesMotion < 250){
        consecutiveSleepMinutesMotion++;
      }else if (consecutiveSleepMinutesMotion == 15){
        detectedREM = true;
        send(statusMessage.setSensor(CHILD_ID_STATUS).set( F("DEEP SLEEP") )); wait(RADIO_DELAY);
      }
    }else{
      consecutiveSleepMinutesMotion = 0;
      if(consecutiveAwakeMinutesMotion < 250){
        consecutiveAwakeMinutesMotion++;
      }
      if(consecutiveAwakeMinutesMotion == 5){
        
        if(detectedREM && minutesSinceREM > 60){
          Serial.println(F("Light sleep after REM"));
          send(statusMessage.setSensor(CHILD_ID_STATUS).set( F("Light sleep after rem") )); wait(RADIO_DELAY);
          minutesSinceREM = 0; //  we found the light sleep phase in a good time segment after the rem phase. So reset this counter.
        }else{
          Serial.println(F("Light sleep"));
          send(statusMessage.setSensor(CHILD_ID_STATUS).set( F("Light sleep") )); wait(RADIO_DELAY);
        }
        
        // Definitely moved away from the REM phase, so reset those variables:
        consecutiveSleepMinutesMotion = 0;
        detectedREM = false;
      }    
    }


    movementsList[loopCounter] = motionCounter;
    Serial.print(F("Loop number and motion sensor movements: ")); Serial.print(loopCounter); Serial.print(F(" -> ")); Serial.println(movementsList[loopCounter]);

    motionTotal = movementsList[1] + movementsList[2] + movementsList[3] + movementsList[4] + movementsList[5];

    Serial.print(F("Sending motion total: ")); Serial.println(motionTotal);
    send(motionMessage.set(motionTotal)); wait(RADIO_DELAY);
    
    // We ask the server to acknowledge that it has received the data. It it doesn't, remove the connection icon.
    if( send(motionMessage.set(motionTotal)) ){ // was ),1) ){
      Serial.println(F("Connection is ok"));

      // add W icon
      oled.set1X();
      oled.setCursor(115,0);
      oled.print(F("w"));

    }else {
      Serial.println(F("Connection lost"));

      // remove W icon
      oled.set1X();
      oled.setCursor(115,0);
      oled.print(F(" "));
          
    }

    // Every loop (minute) the movement counter is reset.
    motionCounter = 0;
    
    // Finally, update the display.
    updateClockDisplay();
  }
}

void turnOffRinging()
{
  if(brightness != 0){
    Serial.println(F("Send: resetting dimmer to 0"));
    send(dimmerMessage.set(0));  
    // clear the bottom part of the screen
    oled.set2X();
    for (byte s=2; s<9; s++) {
      oled.setCursor(0,s);
      oled.print(F("             ")); 
    }
  }
  brightness = 0;
  brightnessPercentage = 0;
  //noTone(SPEAKER_PIN);
}


void receive(const MyMessage &message)
{
  Serial.print("__Incoming change for child: ");
  Serial.println(message.sensor);
  if (message.type==V_STATUS && message.sensor == CHILD_ID_SET_ALARM) { // Toggle of alarm on or off
    Serial.println(F("__RECEIVED ALARM TOGGLE"));
    // Change alarm state
    alarmSet = message.getBool()?1:0;
    saveState(3, alarmSet);                       // Store the preference in local memory

    if(alarmLevel > 1){
      turnOffRinging();
      alarmLevel = alarmSet;
    }
    
    updateClockDisplay();

    // Write some debug info
  }
  if (message.type == V_PERCENTAGE && message.sensor == 4) { // If it's the desired sensitivity level
    //  Retrieve the power or dim level from the incoming request message
    int receivedSensitivity = atoi( message.data );
    motionThreshold = byte(receivedSensitivity);
    saveState(4, motionThreshold);
    Serial.print(F("Requested motion threshold is "));
    Serial.println( motionThreshold );

  }  
}


void receiveTime(unsigned long controllerTime) {
  Serial.print(F("Received time: ")); Serial.println(controllerTime);
  unixTime = controllerTime;
  breakUpTime(unixTime);
  updateClockDisplay();                             // Update the hours on the display
}


void updateClockDisplay()                           // Update clock time display
{
  Serial.print(F("Updating display - level:")); Serial.println(alarmLevel);
  oled.set2X();                                     // Switch to large font size
  oled.setCursor(0,0);
  if(hours < 10){ oled.print(F(" ")); }
  oled.print(hours); 
  oled.print(F(":")); 
  if(minutes < 10){ oled.print(F("0")); }
  oled.print(minutes);

  // This is important: the display shows the alarm time as being 30 minutes later than the internal alarm time. This is done for more easy programming, since we mostly case about the moment 30 minutes before the alarm should go off.
  displayHours = alarmHours;
  if(alarmMinutes >= 30){displayHours++;}
  if(displayHours > 23){displayHours = 0;}

  displayMinutes = (alarmMinutes + 30) % 60;

  //update alarm time display    
  oled.set1X();
  oled.setCursor(70,0);
  
  if(displayHours < 10){oled.print(F("0"));}
  oled.print(displayHours);
  //oled.print(hours); // Used for debugging
  oled.print(F(":")); 
  if(displayMinutes < 10){ oled.print(F("0")); }
  oled.print(displayMinutes);
  //oled.print(minutes); // Used for debugging
  oled.setCursor(70,1);
  if(alarmLevel == ALARM_DISABLED){ oled.print(F("      ")); }
  else if(alarmLevel == ALARM_SET){ oled.print(F("SET   ")); } // Alarm set, but we're not in the 30 minuts before the deadline yet.
  else if(alarmLevel == ALARM_SEARCHING){ oled.print(F("WAIT. ")); } // We're in the 30 minutes before the user wants to wake up at the latest.
  else if(alarmLevel == ALARM_WAKING || alarmLevel == ALARM_RINGING){

    oled.print(F("WAKE  "));
  
    oled.set2X();
    for (byte s=2; s<7; s=s+2) {
      oled.setCursor(0,s);
      if(brightness == 0){
        oled.print(F("           ")); 
      }
      if(brightness <= 60){
        oled.print(F("| | | | | |")); 
      }else if(brightness <= 120){
        oled.print(F("|||||||||||")); 
      //}else if(brightness <= 180){
      //  oled.print(F("00000000000")); 
      }else if(brightness >= 200){
        oled.print(F("***********"));
      }
    }
  } else if(alarmLevel == ALARM_SNOOZING){
    oled.print(F("SNOOZE"));
    oled.set2X();
    oled.setCursor(12,3);
    oled.print(F("Snooze ")); oled.print(snoozeMinutes); oled.print(F("   "));
  }
}


void breakUpTime(uint32_t timeInput)
{
// Break the given time_t into time components.
// This is a more compact version of the C library localtime function
// Note that year is offset from 1970!

  uint32_t time;

  time = (uint32_t)timeInput;
  uint32_t second = time % 60;
  time /= 60; // now it is minutes
  minutes = time % 60;
  time /= 60; // now it is hours
  hours = time % 24;
  time /= 24; // now it is days
  //int Wday = ((time + 4) % 7) + 1;                  // Which day of the week is it. Sunday is day 1 
  
  Serial.print(F("Calculated time: "));
  Serial.print(hours);
  Serial.print(F(":"));
  Serial.println(minutes);
}


/**
 * The MySensors Arduino library handles the wireless radio link and protocol
 * between your home built sensors/actuators and HA controller of choice.
 * The sensors forms a self healing radio network with optional repeaters. Each
 * repeater and gateway builds a routing tables in EEPROM which keeps track of the
 * network topology allowing messages to be routed to nodes.
 *
 * Created by Hreenrik Ekblad <henrik.ekblad@mysensors.org>
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
