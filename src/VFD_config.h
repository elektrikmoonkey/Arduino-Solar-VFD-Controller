#ifndef VFD_CONFIG_H
#define VFD_CONFIG_H

#include <Arduino.h>
/*#include <cstring>
#include <cmath>*/

#include <string.h>
#include <Arduino.h>

#include <TimerOne.h>

#include "config.h"

class VFDConfig {
private:
    uint16_t sineTable[SINE_STEPS];
    bool enabled;
    bool relayState;
    float currentFreq;
    uint32_t voltageCheckTime;
    float lastVoltage;

    static const uint8_t VOLTAGE_SAMPLES = 5;//20;    // Number of samples for instant average
    static const uint8_t VOLTAGE_HISTORY = 20;//10;    // Size of rolling history
    float voltageArray[VOLTAGE_HISTORY];          // Array for rolling average
    uint8_t voltageIndex;                         // Current index in array
    uint8_t voltageCount;                         // Number of readings in array
   // unsigned long lastVoltageRead;                // Timing control

    float readVoltage(); // be sure is only once per loop < use getVoltage() to get last read

    uint32_t lastPotRead;   
    float targetFreq;
    // void updateLEDs();
    //void checkVoltage();
    void generateSineTable();
    uint32_t ledLastUpdate;
    //bool ledState;
   // void updateLEDs();
   // uint16_t getLEDBlinkRate();

  //  Temperature(float vfd);
    float temp;

    int buttonState;
    unsigned long lastDebounceTime = 0;
    unsigned long debounceDelay = 50;
    int lastButtonState = HIGH;

    unsigned long currentMillis;
    unsigned long lastBlinkGreen;
    unsigned long lastBlinkRed;
    unsigned long GreenTime;
    unsigned long RedTime;
    bool ledStateGreen;
    bool ledStateRed;

    
public:
    VFDConfig();
    void begin();
    void setupPWM();
   
    float readPot() { return analogRead(POT_PIN); }
    float readFreq()    { return targetFreq; }
    
    void checkVoltage();
    float getVoltage() { return lastVoltage; }

    void updatePWM();
    void enable();
    void disable();
    bool isEnabled()    { return enabled; }

    void updateFrequency();

    bool checkButton();

    void SetTemp(float temp);

     // void setRelay(bool state); // TODO if need to control relay from outside
    bool getRelayState() { return relayState; }

    void updateLEDs();
   // void disableVoltageCheck(); // Method to disable voltage check for testing << dont need it 
};

#endif
