#ifndef TEMPERATURE_H
#define TEMPERATURE_H

#include <Arduino.h>
#include "VFD_config.h"


#define TEMP_ARRAY_SIZE 10  // Size of rolling average array
#define NUM_READINGS 5      // Number of readings to average for each temperature measurement

class Temperature {
private:

    float tempArray[TEMP_ARRAY_SIZE];
    uint8_t currentIndex;
    uint32_t lastTempRead;
    uint8_t count;

    float lastTemp;

    VFDConfig& vfd;  // Reference to VFDConfig object
    int fanSpeed;
public:
    Temperature(VFDConfig& vfdConfig);
    void begin();
    float readTemperature();
    void updateFanSpeed();
    
    int getFanSpeed()    { return fanSpeed; }
};

#endif

