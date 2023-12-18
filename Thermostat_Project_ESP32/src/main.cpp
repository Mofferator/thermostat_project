#include <Arduino.h>
#include <iostream>
#include <secrets.h>
#include <ServerManager.h>

const char* ssid = WIFI_SSID;
const char* password = WIFI_PASS;

String hostname = HOST; // hostname of web server:

void setup() {
  Serial.begin(115200);
  ServerManager manager(ssid, password, hostname);
  manager.Run(5);
}

void loop() {
  // nothing here
}


