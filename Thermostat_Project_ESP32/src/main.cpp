/*
TODO: 
  Add date tracking from time servers
  Add actual temperature measurement
    - fill out read_temperature() function
*/

#include <Arduino.h>
#include <EEPROM.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <time.h>
#include <ArduinoJson-v6.21.2.h>
#include <secrets.h>

#define EEPROM_SIZE 2

unsigned long previous_millis = 0;

////////////////// WIFI DETAILS/////////////////
const char* ssid = WIFI_SSID;
const char* password = WIFI_PASS;

int status = WL_IDLE_STATUS;

// Initialize the WiFi Client
WiFiClient client;
///////////////////////////////////////////////


/////////////// TIME SERVER DETAILS ///////////
const char* ntpServer = "pool.ntp.org";
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
int    HTTP_PORT   = 80;
String HTTP_METHOD = "GET"; // or "POST"
String HOST_NAME = "192.168.1.173"; // hostname of web server:
String PATH_NAME   = "/";
//////////////////////////////////////////////////////////////////


void write_int(int val, int addr){
  byte byte0 = val >> 8;
  
  EEPROM.write(addr, byte0);

  byte byte1 = val & 0xFF;
  EEPROM.write(addr + 1, byte1);
  EEPROM.commit();
  Serial.println(byte0);
  Serial.println(byte1);
}

int read_int(int addr)
{
  byte byte0 = EEPROM.read(addr);
  byte byte1 = EEPROM.read(addr + 1);
  return (byte0 << 8) + byte1;
}

int negotiate_device_id() {
  HTTPClient http;
  String mac = WiFi.macAddress();
  http.begin(client, "http://" + HOST_NAME + "/new_device/" + mac + "/");
  int resp = http.GET();
  Serial.print("Response Code: ");
  Serial.println(resp);
  if (resp == 200) {
    DynamicJsonDocument doc(1024);
    String payload = http.getString();
    Serial.print(payload);
    deserializeJson(doc, payload);
    if (doc.containsKey("error")){
      String err = doc["error"];
      Serial.println("ERROR:");
      Serial.print(err);
      digitalWrite(ERROR_PIN, HIGH);
      return 0;
    }
    else {
      int device_id = doc["device_id"];
      Serial.print("Recieved new device ID: ");
      Serial.println(device_id);
      return device_id;
    }
  }
  else {
    return 0;
  }
}

String FormatLocalTime(){
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return "";
  }
  int8_t H = timeinfo.tm_hour;
  int8_t M = timeinfo.tm_min;
  int8_t S = timeinfo.tm_sec;

  int8_t d = timeinfo.tm_mday;
  int8_t m = timeinfo.tm_mon + 1;
  int16_t y = timeinfo.tm_year +1900;
  
  String output = "";
  output += y;
  output += "-";
  output += m;
  output += "-";
  output += d;
  output += " ";
  output += H;
  output += ":";
  output += M;
  output += ":";
  output += S;
  return output;
}

String get_real_time() {
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  return FormatLocalTime();
}

float read_temperature() {
  return 20.0; // TODO: make this a real function
}

void send_data(float temperature) {
  DynamicJsonDocument doc(1024);
  doc["device_id"] = device_id;
  doc["date_info"] = get_real_time();
  doc["temperature"] = read_temperature();

  String payload;
  serializeJsonPretty(doc, payload);
  int content_len = measureJsonPretty(doc);

  HTTPClient http;
  http.begin(client, "http://" + HOST_NAME + "/");
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Content-Length", String(content_len));

  int resp = http.POST(payload);

  Serial.print("Response Code: ");
  Serial.println(resp);
  payload = http.getString();
  deserializeJson(doc, payload);
  if (doc.containsKey("error")) {
    Serial.print("Error: ");
    String err = doc["error"];
    if (err == "UNKNOWN_DEVICE_ID")
    {
      Serial.println("Unrecognized device ID, fetching new ID...");
      device_id = negotiate_device_id();
      write_int(device_id, device_id_addr);
    }
  }
  else if (doc.containsKey("msg"))
  {
    String msg = doc["msg"];
    Serial.println(msg);
  }
  
}

void setup() {
  Serial.begin(9600);
  EEPROM.begin(EEPROM_SIZE);
  Serial.println("Attempting to connect to WPA network...");
  Serial.print("SSID: ");
  Serial.println(ssid);

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
  String time = get_real_time();
  Serial.println(time);
  device_id = read_int(device_id_addr);

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
      write_int(device_id, device_id_addr);
      EEPROM.commit();
      Serial.print("Wrote ");
      Serial.print(read_int(device_id_addr));
      Serial.print(" at address 0x");
      Serial.println(device_id_addr);
    }
    
  }
  else {
    Serial.print("Loaded device ID from flash: ");
    Serial.println(device_id);
  }
  send_data(20);
}

void loop() {
  unsigned long current_time = millis();

  if (current_time > previous_millis + 10000)
  {
    previous_millis = current_time;
    send_data(20);
  }
}


