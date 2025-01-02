#include "VFD_config.h"
#include <Arduino.h>



VFDConfig::VFDConfig() : 
    enabled(false), 
    relayState(false),
    currentFreq(0.0f), 
    lastVoltage(0.0f),
    voltageIndex(0),
    voltageCount(0),
    //lastVoltageRead(0),
    lastPotRead(0),
    targetFreq(MIN_FREQ),
    ledLastUpdate(0),
    ledStateGreen(false),
    ledStateRed(false)
    //lastBlinkGreen(0),
    //lastBlinkRed(0),
    //GreenTime(0),
    //RedTime(0)
   
  // ledState(false)
    {
        memset(voltageArray, 0, sizeof(voltageArray));
       
    }


void VFDConfig::begin() {
    generateSineTable();
    setupPWM();

    // Setup
    pinMode(VOLTAGE_PIN, INPUT);
    pinMode(POT_PIN, INPUT);
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    pinMode(RELAY_PIN, OUTPUT);
    //pinMode(PHASE_A_PIN, OUTPUT);   
    //pinMode(PHASE_B_PIN, OUTPUT);;  
    pinMode(LED_GREEN_PIN, OUTPUT);
    pinMode(LED_RED_PIN, OUTPUT);

    digitalWrite(RELAY_PIN, LOW);       // Start with relay off
    digitalWrite(LED_GREEN_PIN, LOW);
    digitalWrite(LED_RED_PIN, HIGH);  // Start with red ON (disabled state)
   

}

//Set VfdSetTemp
void VFDConfig::SetTemp(float vfdSetTemp)
{
    temp = vfdSetTemp;
}

void VFDConfig::updateLEDs() {   
     
    currentMillis = millis();
    
    float voltage = getVoltage(); // readVoltage(); // call readVoltage() only once per loop !!!
    //float temp = temperature.readTemperature();

   /* GreenTime = 0;
    RedTime = 0;*/
    
    //////////////////////////////////////////////////////////////////////////
    if (buttonState) {
                
        if(enabled){
            if(voltage <= TURN_OFF_VOLTAGE_HIGH) { // blink led 
                if(ledStateGreen) {
                    GreenTime = 1000;
                } else {
                    GreenTime = 500;
                }
                if (currentMillis - lastBlinkGreen >= GreenTime)
                {
                    lastBlinkGreen = currentMillis;
                    ledStateGreen = !ledStateGreen;
                    digitalWrite(LED_GREEN_PIN, ledStateGreen);
                }
            }
            else {
                 digitalWrite(LED_GREEN_PIN, HIGH);
            }            
        }  
        else
        {
            if(voltage >= TURN_ON_VOLTAGE) { // blink led                
                GreenTime = 500;
            } else {                
                GreenTime = 2000;
            // update led state
            }
            // update led state 
            if (currentMillis - lastBlinkGreen >= GreenTime) //500
            {
                lastBlinkGreen = currentMillis;
                ledStateGreen = !ledStateGreen;
                digitalWrite(LED_GREEN_PIN, ledStateGreen);
            }
        } 
///////////////////////////// Red LED ///////////////////////////////////////

        if(temp >= TEMP_MAX) {
                //digitalWrite(LED_GREEN_PIN, HIGH);
            RedTime = 1000;
            if (currentMillis - lastBlinkRed >= RedTime) {
                lastBlinkRed = currentMillis;
                ledStateRed = !ledStateRed;
                digitalWrite(LED_RED_PIN, ledStateRed);
            }
        }
        else if (temp >= TEMP_PANIC || voltage > CRITICAL_VOLTAGE_HIGH|| voltage < CRITICAL_VOLTAGE_LOW)
        {
            RedTime = 50;
            if (currentMillis - lastBlinkRed >= RedTime) {
                lastBlinkRed = currentMillis;
                ledStateRed = !ledStateRed;
                digitalWrite(LED_RED_PIN, ledStateRed);
            }
        } else {
             digitalWrite(LED_RED_PIN, LOW);
        }
    }
    // Button off 
    else {
        // 5. Red disabled during normal operation
        digitalWrite(LED_RED_PIN, HIGH);
        digitalWrite(LED_GREEN_PIN, LOW);
    }

}

bool VFDConfig::checkButton() {
    buttonState = digitalRead(BUTTON_PIN);
    
    /*Serial.print("Raw Button State: ");
    Serial.println(buttonState);*/
    
    // Add debouncing
    unsigned long currentMillis = millis();
    if ((currentMillis - lastDebounceTime) > debounceDelay) {
        if (buttonState != lastButtonState) {
            lastDebounceTime = currentMillis;
            lastButtonState = buttonState;
        }
    }
    
    return buttonState;
}


void VFDConfig::setupPWM() {
    // Arduino Timer1 setup
    Timer1.initialize(1000000/PWM_FREQ);
    Timer1.pwm(PHASE_A_PIN, 0);
    Timer1.pwm(PHASE_B_PIN, 0);
}


float VFDConfig::readVoltage() {
    const unsigned long maxSampleTime = 50; // Maximum time to spend sampling
    unsigned long startTime = millis();
    long sum = 0;
    int samples = 0;
    
    // Get samples while respecting time limit
    while (samples < VOLTAGE_SAMPLES && (millis() - startTime) < maxSampleTime) {
        sum += analogRead(VOLTAGE_PIN);
        samples++;
        yield(); // Allow other tasks to run
    }
    
    if (samples == 0) return lastVoltage; // Return last value if no samples taken
    
    float avgRawValue = (float)sum / samples;
    float instantVoltage = (avgRawValue * VOLTAGE_CALIBRATION_FACTOR) / 1023.0;
    
    // Add to rolling average array
    voltageArray[voltageIndex] = instantVoltage;
    
    // Update index and count for rolling average
    voltageIndex = (voltageIndex + 1) % VOLTAGE_HISTORY;
    if (voltageCount < VOLTAGE_HISTORY) {
        voltageCount++;
    }
    
    // Calculate rolling average
    float rollingSum = 0;
    for (uint8_t i = 0; i < voltageCount; i++) {
        rollingSum += voltageArray[i];
    }
    float averageVoltage = rollingSum / voltageCount;
    
    // Round to 2 decimal places
    averageVoltage = round(averageVoltage * 100) / 100.0;
    
    // Update last voltage
    lastVoltage = averageVoltage;
    return averageVoltage;
}

////////////////////////////// turn on and off enable <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
void VFDConfig::checkVoltage()
{ 
    float voltage = readVoltage();
    uint32_t currentTime = millis();
    if(buttonState == false) {
        disable();
        return;
    } //if button off, disable vfd (return
    if (!enabled) {
        if (voltage >= TURN_ON_VOLTAGE) {
            if (voltageCheckTime == 0) {
                voltageCheckTime = currentTime;
            } else if (currentTime - voltageCheckTime >= TURN_ON_VOLTAGE_DELAY) {
                enable();
            }
        } else {
            voltageCheckTime = 0;
        }
    } else {
        if (voltage <= TURN_OFF_VOLTAGE_LOW) {
            if (currentTime - voltageCheckTime >= CRITICAL_VOLTAGE_DELAY) {
                disable();
            }
        } else if (voltage <= TURN_OFF_VOLTAGE_HIGH) {
            if (currentTime - voltageCheckTime >= LOW_VOLTAGE_DELAY) {
                disable();
            }
        } else {
            voltageCheckTime = currentTime;
        }
    }
    lastVoltage = voltage;
}


void VFDConfig::updateFrequency() {
    if (millis() - lastPotRead >= 20) {
        int potValue = readPot();
        targetFreq = map(potValue, 0, 1023, MIN_FREQ * 10, MAX_FREQ * 10) / 10.0f;
        lastPotRead = millis();
    }
}

void VFDConfig::updatePWM() {
    if (!enabled) return;

    static uint32_t lastUpdate = 0;
    static uint16_t index = 0;
    
    // Calculate delay based on target frequency
    uint32_t updateInterval = 1000000UL / (uint32_t)(targetFreq * SINE_STEPS);
    
    if (micros() - lastUpdate >= updateInterval) {        
        // Two phase operation (180° shift)
        uint16_t phase2Index = (index + SINE_STEPS / 2) % SINE_STEPS; 
        
        // Arduino PWM write
        Timer1.pwm(PHASE_A_PIN, sineTable[index]);
        Timer1.pwm(PHASE_B_PIN, sineTable[phase2Index]);
        
        index = (index + 1) % SINE_STEPS;
        lastUpdate = micros();
    }
}

void VFDConfig::enable() {
    enabled = true;
    digitalWrite(RELAY_PIN, HIGH);  // Turn on relay
    relayState = true;

    Timer1.pwm(PHASE_A_PIN, 0);
    Timer1.pwm(PHASE_B_PIN, 0);

    updateLEDs();
}

void VFDConfig::disable() {
    enabled = false;
    digitalWrite(RELAY_PIN, LOW);
    relayState = false;
    
    Timer1.pwm(PHASE_A_PIN, 0);
    Timer1.pwm(PHASE_B_PIN, 0);

    updateLEDs();
}

void VFDConfig::generateSineTable() {
    for (int i = 0; i < SINE_STEPS; i++) {
        // Generate sine wave values scaled to PWM range
        float angle = (2.0 * PI * i) / SINE_STEPS;
        float sineValue = sin(angle);
        // Convert to PWM value (0 to PWM_MAX)
        sineTable[i] = (uint16_t)((sineValue + 1.0) * PWM_MAX / 2.0);
    }
}

