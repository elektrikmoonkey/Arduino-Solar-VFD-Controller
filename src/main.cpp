#include "VFD_config.h"
#include "Temperature.h"

VFDConfig vfd;
Temperature temp(vfd);  // Pass the vfd object to Temperature

static uint32_t lastPrint = 0;
bool debug = true;
bool INPANIC = false;

void debugPrint(); // Declare the function prototype

void setup() {
    Serial.begin(115200);
    vfd.begin(); 
    temp.begin();
}

void loop() {
    temp.updateFanSpeed();            // Check Temp and update fan speed

    if (INPANIC) { // TEMPERATURE IS OVER THE LIMIT and will turn back on if it drops below MAX TEMP 
        if (temp.readTemperature() <= TEMP_MAX) {       
            INPANIC = false;
            //delay(5000);
        }
    } else if (temp.readTemperature() >= TEMP_PANIC) { // Check if temperature exceeds TEMP_PANIC
        vfd.disable();                // Disable VFD if temperature exceeds panic threshold
        INPANIC = true;
    } else {
        vfd.checkVoltage();           // Check voltage 
        
        if (vfd.checkButton()) {      // Only if button is ON
            if (vfd.isEnabled()) {    // If voltage is good
                vfd.updateFrequency(); // Update frequency from pot
                vfd.updatePWM();      // Update PWM outputs
            } 
        } else {
            vfd.disable();            // Turn everything off if button is OFF
        }
    }

    vfd.updateLEDs();     
    if (debug) {
        debugPrint();
    }   
}

void debugPrint()
{
   if (millis() - lastPrint >= 1000) {     
        Serial.print("Voltage: ");
        Serial.print(vfd.getVoltage());
        Serial.print("V, potentiometer: ");            
        Serial.print(vfd.readPot());
        Serial.print("V, Frequency: ");
        Serial.print(vfd.readFreq());
        Serial.print("Hz, Temperature: ");
        Serial.print(temp.readTemperature());
        Serial.print("C, Fan Speed: ");
        Serial.print((temp.getFanSpeed() * 100) / 255); // Convert PWM value to percentage
        Serial.print("%");
        Serial.print(", Button: ");
        Serial.print(vfd.checkButton() ? "ON" : "OFF");
        Serial.print(", Enabled: ");
        Serial.print(vfd.isEnabled() ? "True" : "False");
        Serial.print(", Relay: ");
        Serial.println(vfd.getRelayState() ? "ON" : "OFF");
        lastPrint = millis();
    }
}

//Test code for the VFD
/*
To use this test functionality:

Open Serial Monitor at 115200 baud
Press 't' within 3 seconds of startup to enter test mode
Use commands 1-7 to test individual components
Use 'x' to exit test mode and resume normal operation
This allows you to:

Test each PWM output separately
Verify fan control
Check relay operation
Test indicator LEDs
Read voltage sensor values
Read temperature sensor values
*/
void runHardwareTest() {


    ////////////////////////// ARUDUINO TEST HARDWARE AND CODE /////////////////////////

    float voltage;
    float tempReading;

    Serial.println(F("Hardware Test Mode"));
    Serial.println(F("Commands:"));
    Serial.println(F("1: Test Phases (Both MOSFETs)"));
    Serial.println(F("2: Test Fan"));
    Serial.println(F("3: Test Relay"));
    Serial.println(F("4: Test LEDs"));
    Serial.println(F("5: Read Voltage"));
    Serial.println(F("6: Read Temperature"));
    Serial.println(F("7: Read Potentiometer"));
    Serial.println(F("x: Exit Test Mode"));
    
    while(true) {
        if(Serial.available()) {
            char cmd = Serial.read();
            
            switch(cmd) {
                case '1':
                    // First enable relay
                    digitalWrite(RELAY_PIN, HIGH);
                    delay(100); // Wait for relay to engage
                    
                    // Test both phases at low duty cycle
                    Serial.println(F("Testing phases, please check if button is on ? "));
                    vfd.enable();
                    for(int i = 0; i < 12000 ; i++) {
                        temp.updateFanSpeed();   
                        if (vfd.checkButton()) {      // Only if button is ON
                            if (vfd.isEnabled()) {    // If voltage is good
                                vfd.updateFrequency(); // Update frequency from pot
                                vfd.updatePWM();      // Update PWM outputs
                            }
                        } else {
                            vfd.disable();            // Turn everything off if button is OFF
                        }
                        vfd.updateLEDs();  

                        debugPrint(); /// rpint debug info
                        delay(5);                        
                    }
                    vfd.disable();
                   
                    break;
                    
                case '2':
                    Serial.println(F("Testing Fan"));
                    analogWrite(FAN_PIN, FAN_MAX_SPEED);
                    Serial.println(F("Fan ON - Full Speed"));
                    delay(2000);
                    analogWrite(FAN_PIN, 0);
                    break;
                    
                case '3':
                    Serial.println(F("Testing Relay"));
                    digitalWrite(RELAY_PIN, HIGH);
                    Serial.println(F("Relay ON"));
                    delay(2000);
                    digitalWrite(RELAY_PIN, LOW);
                    break;
                    
                case '4':
                    Serial.println(F("Testing LEDs"));
                    digitalWrite(LED_GREEN_PIN, HIGH);
                    digitalWrite(LED_RED_PIN, HIGH);
                    Serial.println(F("LEDs ON"));
                    delay(2000);
                    digitalWrite(LED_GREEN_PIN, LOW);
                    digitalWrite(LED_RED_PIN, LOW);
                    break;
                    
                case '5':
                // Read voltage from voltage sensor
                    Serial.println(F("Reading Voltage"));
                    for(int i = 0; i < 30; i++) {
                        voltage = analogRead(VOLTAGE_PIN) * VOLTAGE_CALIBRATION_FACTOR;
                        Serial.print(F("Voltage Reading: "));
                        Serial.println(voltage);
                        delay(1000);                        
                    }
                    
                    break;
                    
                case '6':
                    Serial.println(F("Reading Temperature"));
                    for (int i = 0; i < 30; i++)
                    {
                        tempReading = analogRead(NTC_PIN);
                        Serial.print(F("Temperature Raw: "));
                        Serial.println(tempReading);
                        delay(1000);
                    }

                    break;
                case '7':   
                    Serial.println(F("Reading Potentiometer"));       
                    for(int i = 0; i < 30; i++) {
                        Serial.print(F("Potentiometer Reading: "));
                        Serial.println(analogRead(POT_PIN));
                        delay(1000);
                    }                         
                    break;  
                case 'x':
                    Serial.println(F("Exiting Test Mode"));
                    return;
            }
        }
    }
}