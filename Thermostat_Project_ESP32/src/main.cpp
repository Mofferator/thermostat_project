/*
TODO: 
  Add date tracking from time servers
  Add actual temperature measurement
    - fill out read_temperature() function
*/

#include <Arduino.h>
#include <iostream>
#include <ROMTransactions.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <time.h>
#include <ClosedCube_HDC1080.h>
#include <secrets.h>
#include <Measurement.h>
#include <json.hpp>
#include <stdexcept>


////////////////// ROM /////////////////
#define EEPROM_SIZE 2
ROMTransactions ROM(EEPROM_SIZE);

////////////////// TEMP SENSOR /////////////////
#define TEMP_SENSOR_ADDR 0x40 // I2C Address of the HDC1080
#define HDC1080_SDA 13 // SDA line for the HDC1080
#define HDC1080_SCL 14 // SCL line for the HDC1080
ClosedCube_HDC1080 hdc1080;
////////////////////////////////////////////////

////////////////// WIFI DETAILS/////////////////
const char* ssid = WIFI_SSID;
const char* password = WIFI_PASS;

int status = WL_IDLE_STATUS;

// Initialize the WiFi Client
WiFiClient client;
///////////////////////////////////////////////


/////////////// TIME SERVER DETAILS ///////////
const char* ntpServer = "time-b-g.nist.gov";
const long  gmtOffset_sec = 0;
const int   daylightOffset_sec = 0;
///////////////////////////////////////////////


////////////////////// DEVICE ID CONFIG ////////////////////////
int device_id_addr = 0; // location of device ID in onboard flash
int device_id;
////////////////////////////////////////////////////////////////


////////////////// HARDWARE DEFINITIONS ///////////////////////
const int ERROR_PIN = 2; // An LED to represent an error occurring
///////////////////////////////////////////////////////////////


//////////////////////// HTTP DETAILS ////////////////////////////
int    HTTP_PORT   = 55555;
String HTTP_METHOD = "GET"; // or "POST"
String HOST_NAME = HOST; // hostname of web server:
String PATH_NAME   = "/";
//////////////////////////////////////////////////////////////////

int negotiate_device_id() {
  HTTPClient http;
  String mac = WiFi.macAddress();
  http.begin(client, "http://" + HOST_NAME + "/new_device/" + mac + "/");
  int resp = http.GET();
  Serial.print("Response Code: ");
  Serial.println(resp);
  if (resp == 200) {
    String payload = http.getString();
    Serial.print(payload);
    auto response = nlohmann::json::parse(payload);
    if (response.contains("error")){
      String err = response["error"].get<std::string>().c_str();
      Serial.println("ERROR:");
      Serial.print(err);
      digitalWrite(ERROR_PIN, HIGH);
      return 0;
    }
    else {
      int device_id = response["device_id"].get<int>();
      
      Serial.print("Recieved new device ID: ");
      Serial.println(device_id);
      return device_id;
    }
  }
  else {
    return 0;
  }
}

void send_data(nlohmann::json data) {
  
  const char* payload = to_string(data).c_str();
  int content_len = std::strlen(payload);

  HTTPClient http;
  http.begin(client, "http://" + HOST_NAME + "/");
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Content-Length", String(content_len));

  int resp = http.POST(payload);

  Serial.print("Response Code: ");
  Serial.println(resp);
  payload = http.getString().c_str();
  nlohmann::json response(payload);

  if (response.contains("error")) {
    Serial.print("Error: ");
    String err = response["error"].get<std::string>().c_str();
    if (err == "UNKNOWN_DEVICE_ID")
    {
      Serial.println("Unrecognized device ID, fetching new ID...");
      device_id = negotiate_device_id();
      ROM.WriteInt(device_id, device_id_addr);
    }
  }
  else if (response.contains("msg"))
  {
    String msg = response["msg"].get<std::string>().c_str();
    Serial.println(msg);
  }
  
}

void setup() {
  Serial.begin(9600);
  Serial.println("Attempting to connect to WPA network...");
  Serial.print("SSID: ");
  Serial.println(ssid);

  Serial.println("Initializing HDC1080");
  hdc1080.begin(TEMP_SENSOR_ADDR, HDC1080_SDA, HDC1080_SCL);

  status = WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
  }
  Serial.print("\nConnected to ");
  Serial.println(ssid);
  int start = millis();
  while (millis() < start + 1500){
    // wait 1.5s for wifi to connect
  }

  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  device_id = ROM.ReadInt(device_id_addr);

  // Check for default device_id
  // If found, negotiate a new id from server
  if (device_id == 0) {
    Serial.println("No device ID found, negotiating with server");
    device_id = negotiate_device_id();
    if (device_id == 0) {
      Serial.println("Unable to negotiate device id...");
      while (true){} // do nothing 
    }
    else {
      ROM.WriteInt(device_id, device_id_addr);
      Serial.print("Wrote ");
      Serial.print(ROM.ReadInt(device_id_addr));
      Serial.print(" at address 0x");
      Serial.println(device_id_addr);
    }
    
  }
  else {
    Serial.print("Loaded device ID from flash: ");
    Serial.println(device_id);
  }
}

void loop() {
  nlohmann::json j = Measurement(hdc1080, device_id).Serialize();
  send_data(j);
  delay(1000);
}


