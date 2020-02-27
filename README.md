# Devices
This is the code for the Candle smart home devices. See:

https://www.candlesmarthome.com/devices for more details.

All Candle devices allow you to easily toggle whether they are allowed to send sensitive data to the central controller.




### Smart lock

The smart lock allows you to control two electric locks.

Candle can work without an internet connection. But how can you uplock your front door when you're not home, a popular feature, when there's no internet? The Candle smart lock solves this by adding a GSM modem. There are multiple safety features to make this work well, such as:
- A password is required, one for each door.
- Only phone numbers you provide can toggle the lock
- You can decide when the lock can be remote-togged in the first place. Only enable it when you're on holiday, for example.


### CO2 sensor

Measuring CO2 can be useful, as high levels can influence how well you sleep and how energized you feel. But CO2 levels can also reveal how many people are in a room. That's why the Candle CO2 sensor has a unique privacy feature: you can tell it to generate fake data for a while. This allows you to pretend you're alone in a room when you're not, for example.


### Dust sensor

Measure the fine dust levels in a room.


### Temperature and more sensor

This sensor measures temperature (in Celcius or Fahrenheit), air humidity and air pressure. 

It has an optional TFT screen on which it displays:
- Temperature, also as a simple graph
- humidity, also as a simple graph
- Air pressure
- Weather predition icon


### Energy use meter

This devices detects light pulses from your electricity meter, and uses this to determine how much power your home uses. It allows you to see watts your home is using at the moment, as well as see longer trends as Kwh in the last hours and/or days.

If you attach a touch screen you can get simple bar graphs and see live data about the power your home is using.


### Smart alarm clock

This alarm clock uses a radar sensor to detect when you move in your sleep, and will try to wake you when you're already at a light sleep point in your sleep cycle. If you set it to wake you at 8am, it will start looking for an opportune moment from 7:30.

It can wake you up slowly with its own LED, but the intended use is to one by one switch on other lights in the room. If all else fails, it can sound a classic buzzer.


### Plant health sensor

Measure the moisture level of up to 5 plants. You can also set the minimal moisture level that each plant requires. When this level is reached, a "thirsty" switch (each plant has one) will be toggled. You can then create your own automatations to handle this, such as notifying you, or even watering them automatically.

The code already has some hidden functionality to toggle 5 relays directly.


### Anemone

This device allows you to control your internet connection. It is designed to disconnect the internet cable going into your router (although you could also limit connectivity to a specific device).

Candle doesn't require an internet connection to work, and this device enforces that idea. It's a device that producers of cloud dependent smart devices could and would never produce.


### Signal Hub

This is a device that can learn to recognise and also replay almost any signal from 433Mhz devices. Many people own these type of devices, such as power sockets, because they are cheap and simple.

It works by recognizing patterns in transmissions. Once recognized, it stores the minimal pattern required in a very space efficient way, while preserving the timings. It can store up to 20 patterns in just 512 bytes.

It is highly recommended to attach an Open Smart touch screen. This allows you to replay signals by pressing buttons on the screen. It also guides you through the proces of copying the signals, such as when to press the buttons on a remote control that you are copying.
- Signals are stored in the Arduino memory, and it can hold between 10 and 20 signals.

With different settings it can copy and replay infrared signals instead.


