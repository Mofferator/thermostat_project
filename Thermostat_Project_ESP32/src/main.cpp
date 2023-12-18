#include <Arduino.h>
#include <iostream>
#include <secrets.h>
#include <ServerManager.h>

#define INTERVAL 60 // interval between readings in seconds
#define DEBUG false

const char* ssid = WIFI_SSID;
const char* password = WIFI_PASS;

String hostname = HOST; // hostname of web server:

void setup() {
  if (DEBUG)
    Serial.begin(115200);
  ServerManager manager(ssid, password, hostname);
  manager.Run(INTERVAL);
}

void loop() {
  // nothing here
}


