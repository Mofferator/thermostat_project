#include <ServerManager.h>

void ServerManager::Run(int interval) {
    while(true) {
        SendData();
        esp_sleep_enable_timer_wakeup(interval * 1000000);
        Serial.println("Going to sleep...");
        esp_deep_sleep_start();
    }
}

void ServerManager::Connect() {
    WiFi.begin(mWiFiSSID.c_str(), mWiFiPassword.c_str());
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.print("\nConnected to ");
    Serial.println(mWiFiSSID.c_str());
    int start = millis();
    while (millis() < start + 1500){
        // wait 1.5s for wifi to connect
    }
}

void ServerManager::Disconnect() {
    WiFi.disconnect();
    Serial.printf("Disconnected from %s\n", mWiFiSSID.c_str());
}

void ServerManager::SendData() {
    auto measurement = Measurement(hdc1080, mDeviceID);
    Connect();
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    measurement.RecordTime();
    mDeviceID = NegotiateDeviceID();
    measurement.SetDeviceID(mDeviceID);
    Serial.println(to_string(measurement.Serialize()).c_str());
    SendHTTP(measurement.Serialize());
    Disconnect();
}

int ServerManager::NegotiateDeviceID() {
    HTTPClient http;
    String mac = WiFi.macAddress();
    http.begin(client, "http://" + mHostName + "/new_device/" + mac + "/");
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

void ServerManager::SendHTTP(const nlohmann::json& json) {
    const char* payload = to_string(json).c_str();
    int content_len = std::strlen(payload);

    HTTPClient http;
    http.begin(client, "http://" + mHostName + "/");
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
        Serial.print(err);
    } else if (response.contains("msg")) {
        String msg = response["msg"].get<std::string>().c_str();
        Serial.println(msg);
    }
}
