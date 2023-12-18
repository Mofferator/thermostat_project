#ifndef SERVERMANAGER_H
#define SERVERMANAGER_H

#include <string>
#include <WiFi.h>
#include <ClosedCube_HDC1080.h>
#include <HTTPClient.h>
#include <json.hpp>
#include <Measurement.h>

#define TEMP_SENSOR_ADDR 0x40 // I2C Address of the HDC1080
#define HDC1080_SDA 13 // SDA line for the HDC1080
#define HDC1080_SCL 14 // SCL line for the HDC1080

class ServerManager {
public:
    ServerManager(String ssid, String password, String hostname) : mWiFiSSID(ssid), mWiFiPassword(password), mHostName(hostname) { 
        mDeviceID = 0;
        Serial.println("Initializing HDC1080");
        hdc1080.begin(TEMP_SENSOR_ADDR, HDC1080_SDA, HDC1080_SCL);
    }
    void Run(int interval);
private:
    // Functions
    void Connect();
    void Disconnect();
    void SendData();
    int NegotiateDeviceID();
    void SendHTTP(const nlohmann::json& json);


    ClosedCube_HDC1080 hdc1080;
    WiFiClient client;
    const int status = WL_IDLE_STATUS;
    const char* ntpServer = "time-b-g.nist.gov";
    const long  gmtOffset_sec = 0;
    const int   daylightOffset_sec = 0;
    const int device_id_addr = 0;
    const int ERROR_PIN = 2;
    const String HTTP_METHOD = "GET"; // or "POST"
    const String PATH_NAME   = "/";
    String mWiFiSSID;
    String mWiFiPassword;
    String mHostName;
    int mDeviceID;
};

#endif