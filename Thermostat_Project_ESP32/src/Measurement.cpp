#include <Measurement.h>
#include <string.h>

Measurement::Measurement(ClosedCube_HDC1080& hdc1080, int device_id) {
    while(!getLocalTime(&mTime)){
        Serial.println("Failed to get time data, retrying...");
    }
    mDevice_id = device_id;
    mTemperature_hdc = hdc1080.readT();
    mHumidity_hdc = hdc1080.readHumidity();
}

void Measurement::setDeviceID(int id) {
    mDevice_id = id;
}

nlohmann::json Measurement::Serialize() const {
    std::string dateString = SerializeDate(mTime);

    nlohmann::json output = {
        {"device_id", mDevice_id},
        {"date_info", dateString},
        {"temperature_C", mTemperature_hdc},
        {"r_humidity_%", mHumidity_hdc}
    };

    return output;
}

std::string Measurement::SerializeDate(const struct tm& timeInfo) const {
    char buffer[80];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &mTime);
    return buffer;
}