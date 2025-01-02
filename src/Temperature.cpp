#include "Temperature.h"
#include "config.h"

Temperature::Temperature(VFDConfig& vfdConfig) : 
    tempArray{0}, 
    currentIndex(0), 
    lastTempRead(0), 
    count(0), 
    lastTemp(0.0f), 
    vfd(vfdConfig) {}

void Temperature::begin() {
    pinMode(FAN_PIN, OUTPUT);
    analogWrite(FAN_PIN, 0);
}

float Temperature::readTemperature() {
    float instantAverage = 0;

    for(int i = 0; i < 5; i++) {
        int rawADC = analogRead(NTC_PIN);
        float resistance = SERIES_RESISTANCE * (1023.0 / rawADC - 1.0);
        float steinhart = resistance / REFERENCE_RESISTANCE;
        steinhart = log(steinhart);
        steinhart /= B_VALUE;
        steinhart += 1.0 / NOMINAL_TEMPERATURE;
        steinhart = 1.0 / steinhart;
        steinhart -= 273.15; // Convert to Celsius
        instantAverage += steinhart;
        //delay(2);
    }
    // Calculate average of current readings
    float currentTemp = instantAverage / NUM_READINGS;
    
    // Add to rolling average array
    tempArray[currentIndex] = currentTemp;
    
    // Update index and count
    currentIndex = (currentIndex + 1) % TEMP_ARRAY_SIZE;
    if (count < TEMP_ARRAY_SIZE) {
        count++;
    }
    
    // Calculate rolling average
    float rollingAverage = 0;
    for (uint8_t i = 0; i < count; i++) {
        rollingAverage += tempArray[i];
    }
    rollingAverage /= count;
    
    // Update VFD with rolling average
    vfd.SetTemp(rollingAverage);
    return rollingAverage;
}

void Temperature::updateFanSpeed() {
    if (millis() - lastTempRead >= 1000) {
        float temp = readTemperature();
        
        if (temp >= TEMP_PANIC) {
            vfd.disable();                                      // Disable VFD if temperature exceeds panic threshold
            fanSpeed = FAN_MAX_SPEED;                           // Run fan at maximum speed
        } else if (temp <= TEMP_MIN) {
            fanSpeed = vfd.isEnabled() ? FAN_MIN_SPEED : 0;     // Turn on fan to minimum speed if VFD is enabled
        } else if (temp >= TEMP_MAX) {
            fanSpeed = FAN_MAX_SPEED;
        } else {                                                // Update fan speed based on temperature
            fanSpeed = map(temp * 10, TEMP_MIN * 10, TEMP_MAX * 10,                         
                         FAN_MIN_SPEED, FAN_MAX_SPEED);
        }
        
        analogWrite(FAN_PIN, fanSpeed);
       
        lastTemp = temp;
        lastTempRead = millis();
    }
}
