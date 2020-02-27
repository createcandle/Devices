# Devices
This is the code for the Candle smart home devices.

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




### Signal Hub

This is a device that can learn to recognise and also replay almost any signal from 433Mhz devices. Many people own these type of devices, such as power sockets, because they are cheap and simple.

It works by recognizing patterns in transmissions. Once recognized, it stores the minimal pattern required in a very space efficient way, while preserving the timings. It can store up to 20 patterns in just 512 bytes.

It is highly recommended to attach an Open Smart touch screen. This allows you to replay signals by pressing buttons on the screen. It also guides you through the proces of copying the signals, such as when to press the buttons on a remote control that you are copying.
- Signals are stored in the Arduino memory, and it can hold between 10 and 20 signals.

With different settings it can copy and replay infrared signals instead.


