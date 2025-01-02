#ifndef CONFIG_H
#define CONFIG_H

// Constants
#define SINE_STEPS 256  // Sine wave definition


// Frequency limits
#define MIN_FREQ 10.0f    // Minimum frequency in Hz
#define MAX_FREQ 60.0f    // Maximum frequency in Hz

#define PWM_FREQ 20000  // Default 20kHz
#define PWM_MAX 1023

// Pin definitions
#define PHASE_A_PIN 5 
#define PHASE_B_PIN 6 

#define BUTTON_PIN 2 
#define FAN_PIN 9 
#define LED_GREEN_PIN 10  
#define LED_RED_PIN 11 
#define NTC_PIN 32 


// Arduino  pin definitions...
//#define PHASE_A_PIN 5  // R! 200k                                                                                           
//#define PHASE_B_PIN 6  // R2 200k  
#define VOLTAGE_PIN A0          // Analog input for voltage sensing    
#define POT_PIN A1 // Analog pin for potentiometer                                                                       
// #define BUTTON_PIN 2        // Digital pin for button input (active low) >> with resistor to ground                         
// #define FAN_PIN 9 // PWM pin for fan control                                                                             
//  #define NTC_PIN A2 // Analog pin for NTC thermistor                                                                      
#define RELAY_PIN 3        // Use negative instead to drive 12v with help >>> BC547 <<<                                  
// #define LED_GREEN_PIN 10 // Status LED (running)                                                                          
// #define LED_RED_PIN 11   // Error/disabled LED    

//#define PWM_FREQ 20000  // Default 20kHz
//  #define PWM_MAX 1023    // 10-bit resolution
                                                                                    
//#define PHASE_C_PIN 13  // 3 fase not in use 

///////////////////////////////////////////////////
// Voltage sensing parameters

const float VR1 = 147.2;        //R1 = 147.2kΩ (to positive)<<<<<<<<<<  146.7
const float VR2 = 46.6;         //R2 = 46.6kkΩ (to ground) <<<<<<<<<<<   39.83 

const float ratio = (VR1 + VR2)/VR2;                   //4.0  If you dont use VR3 
//const float ratio = (VR1 + VR2 + VR3) / VR2;             //  If you use VR3
const float ScalingFactor = 5.0 * ratio;    //20.0
#define VOLTAGE_CALIBRATION_FACTOR ScalingFactor * 1.037 // 1.037 //1.034  //Ddjust this value to calibrate the voltage reading
 

// Pin definitions

// NTC thermistor parameters
#define REFERENCE_TEMP 25.0            // Room temperature in Celsius
//#define REFERENCE_RESISTANCE 100000.0  // Resistance at room temp (100k)
#define REFERENCE_RESISTANCE 10000.0  
#define B_VALUE 3950.0                 // B-value of the thermistor
#define SERIES_RESISTANCE 100000.0      // Value of the series resistor (100k)
//#define SERIES_RESISTANCE 10000.0      // Value of the series resistor (10k)
#define NOMINAL_TEMPERATURE 298.15     // Room temp in Kelvin (25°C + 273.15)


// Fan control parameters
const float TEMP_MIN = 35.0;   // Below this temperature, fan at minimum speed
const float TEMP_MAX = 70.0;   // Above this temperature, fan at maximum speed
const float TEMP_PANIC = 90.0; 
const int FAN_MIN_SPEED = 85;   // Minimum PWM value
const int FAN_MAX_SPEED = 255;  // Maximum PWM value                                                            

////////////////////////////////////////////////////
                                                                      
// Voltage thresholds
#define TURN_ON_VOLTAGE 14.2

#define TURN_OFF_VOLTAGE_HIGH 12.2
#define TURN_OFF_VOLTAGE_LOW 12.0

#define CRITICAL_VOLTAGE_HIGH 14.5 
#define CRITICAL_VOLTAGE_LOW 11.5 

#define TURN_ON_VOLTAGE_DELAY 5000//120000  // 2 minutes in ms To turn ON 
#define LOW_VOLTAGE_DELAY 60000    // 1 minute in ms
#define CRITICAL_VOLTAGE_DELAY 5000 // 5 second in ms

#endif // CONFIG_H


