#ifndef _EDFSR_TESTSTAND_H
#define _EDFSR_TESTSTAND_H

#include <Wire.h>
#include "SparkFun_Qwiic_Scale_NAU7802_Arduino_Library.h"
#include "Servo.h"
#include <stdio.h>
//#include <iostream>


//Select the Microcontroller you are using
//by uncommenting the µController you want to use

//#define TEENSY 
#define UNO 

//EEPROM locations to store 4-byte variables
#define LOCATION_CALIBRATION_FACTOR 0 //Float, requires 4 bytes of EEPROM
#define LOCATION_ZERO_OFFSET 10 //Must be more than 4 away from previous spot. Long, requires 4 bytes of EEPROM
#define AVG_SIZE 1

//Testing loop indicator constants
#define DELAY_TIME_MS 250 
#define STEP_TIME_US 50
#define PWM_START_US 1500
#define PWM_STOP_US 2200
 #define EDF_PRIMING_PWM 1500

//Teensy 4.1 pin definitions
#ifdef TEENSY
    #include <EEPROM.h> //Needed to record user settings
    #define EDF_PIN 36
#endif

#ifdef UNO
    #define EDF_PIN 6
#endif


//Math stuff
#define g 9.807                 //Gravity acceleration (m/s2)
#define G2N g*1000          //Grams to Newtons





class Teststand
{
public:


NAU7802 myScale; //Create instance of the NAU7802 class
Servo edf;
int delay_time_ms{DELAY_TIME_MS};
int step_time_us{STEP_TIME_US};
int pwm_start_us{PWM_START_US};
int pwm_stop_us{PWM_STOP_US};
//Create an array to take average of weights. This helps smooth out jitter.
bool flag{false};
float avgWeights[AVG_SIZE];
byte avgWeightSpot{0};
float settingCalibrationFactor{427.10}; //Value used to convert the load cell reading to lbs or kg
long settingZeroOffset{-57034}; //Zero value that is found when scale is tared
//data capture arrays/vars
// float pwm[100];
// float RPM[100];
// float force_grams[100];
// float force_newtons[100];
int command{1000}; 
bool settingsDetected{false}; //Used to prompt user to calibrate their scale

    
    Teststand();

    void init_edf();

    //write edf (to shorten writeMicroseconds)
    void wedf(int pwm);

    void wedf_hold(int pwm, int ms);

    void wedf_manual(void);

    void read_edf_command();

    

    void print_menu();

    //edf must be primed at 1500us due to esc gov mode
    void prime_edf(int seconds);

    void init();

    void user_input_loop();

    void user_input_configurator();

    void edf_test_loop(int& delaytime_ms, int& num_steps, int& start_us, int& stop_us);

    float get_weight_avg();
    
    float get_weight_avg_2();

    void update_weight(float & weight );

    void calib_input_check();

    void simple_measure();

    void initScale();

    void calibrateScale();

    void recordSystemSettings();

    void readSystemSettings();






private:

    void user_read_int(int & value, String message);
    
    void user_read_float(float & value, String message);

    void user_read_anykey(String message);
};


#endif
