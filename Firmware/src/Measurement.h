#ifndef MEASUREMENT_H
#define MEASUREMENT_H

#include <ClosedCube_HDC1080.h>
#include <json.hpp>
#include <time.h>
#include <string.h>

class Measurement {
public:
    Measurement(ClosedCube_HDC1080& hdc1080, int device_id);
    void SetDeviceID(int id);
    void RecordTime();
    nlohmann::json Serialize() const;
private:
    // Functions
    std::string SerializeDate(const struct tm& timeInfo) const;

    // Member Variables
    int mDevice_id;
    struct tm mTime;
    double mTemperature_hdc;
    double mHumidity_hdc;
};

#endif