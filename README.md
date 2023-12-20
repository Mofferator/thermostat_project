# IoT Temperature Sensor Project
My thermostat is a compulsive liar, and I started this project to prove it!

## Hardware
- The hardware for this project consists of an ESP32 which reads the temperature and humidity from a HDC1080 air sensor
- The board also features a usb to uart (cp2102) converter, enabling easy uploading of code in the development process

## Firmware
- The basic operation of the device is as follows
  - Read the temperature and humidity
  - Connect to the specified wifi network if possible
  - Serialize the environment data into json and send to webserver via HTTP request
  - disconnect from wifi
  - Enter deep sleep for a configurable number of seconds

## Server
- The webserver containerized, consisting of a python flask web server and a MySQL database.
- The webserver also has several webpages where the temperature readings from the various devices and rooms can be viewed

