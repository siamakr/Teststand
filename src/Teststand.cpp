
#include <Arduino.h>
#include "Teststand.h"
#include <stdlib.h>

Teststand::Teststand()
{

}

void Teststand::init()
{
    Wire.begin();
    delay(500);
    //Wire.setClock(400000); //Qwiic Scale is capable of running at 400kHz if desired
    init_edf();
    initScale();
    

}

void Teststand::init_edf()
{
    Serial.println("Teststand init started...");
    edf.attach(EDF_PIN,1000,2000);
    delay(200);
    //make sure esc is off
    wedf(900);
    delay(300);
    Serial.println("edf ready");
}

void Teststand::prime_edf(int seconds)
{
    wedf(EDF_PRIMING_PWM);
    delay(seconds*1000); 
}

// write edf - to make coding faster
void Teststand::wedf(int pwm)
{
    edf.writeMicroseconds(pwm);
}

//writes to edf and holds of ms milliseconds
void Teststand::wedf_hold(int pwm, int ms)
{
    edf.writeMicroseconds(pwm);
    delay(ms);
}

void Teststand::print_menu()
{

}

void Teststand::wedf_manual(void)
{
    //     //Get delay time from user and store 
    // Serial.print(F("pwm command: "));
    // while (Serial.available()) Serial.read(); //Clear anything in RX buffer
    // while (Serial.available() == 0) delay(10); //Wait for user to press key

    // //Read user input
    // command = Serial.parseInt();
    // Serial.println();  
    user_read_int(command, "enter pwm µs: ");
    wedf(command);
}


void Teststand::user_input_loop(void)
{
    if (Serial.available())
    {
        byte incoming = Serial.read();

        switch (incoming)
        {
            case '1':     //set pwm = 1000
                wedf(1000);
                delay(100);
            break;
            
            case '2':     //set pwm = 2000
                wedf(2000);
                delay(100);
            break;

            case 'm':       //Allows user to enter desired PWM in µs 
                wedf_manual();

            break;
            
            case 'p':     //Sequences through required ESC procedure to calib gov mode
                //writes 1500 to ESC and holds for 5 seconds.
                Serial.println("Setting throttle to 50% (1500us)");
                    wedf(1500);
                    delay(7000);
                Serial.println("Setting throttle to 0% (1000us)");
                    wedf(1000);
                    delay(1000);
                Serial.println("RPM Standardization Complete...");
                Serial.println("Going into wait mode...");
                    incoming = 'w';
            break;
            
            case 's':     //start testing campaign 
                edf_test_loop(delay_time_ms, step_time_us, pwm_start_us, pwm_stop_us);
                //edf_test_loop(DELAY_TIME_MS, STEP_TIME_US,PWM_START_US,PWM_STOP_US);
                Serial.println("Testing done... Going into wait mode...");
                incoming = 'w';
            break;
            
            case 'g':   //takes single measurement from scale 
                Serial.print("gweight_avg: ");
                Serial.println(get_weight_avg()); 
                delay(500); 
            break;
            case 'h':   //takes single measurement from scale 
                Serial.print("gweight_avg_2: ");
                Serial.println(get_weight_avg_2()); 
                delay(500); 
            break;
            case 'j':   //takes single measurement from scale 
                Serial.print("gweight_avg_3: ");
                simple_measure(); 
                delay(500); 
            break;
            case 'n':   //takes single measurement from scale 
                Serial.print("gweight: ");
                Serial.println(myScale.getWeight()); 
                delay(500); 
            break;
                
            case 'w':     //go in waiting loop 
                Serial.println("waiting..");
                delay(5000);
            break;
            
            case 't':     //tare scale
                myScale.calculateZeroOffset();
            break;
            
            case 'c':     //calibrate scale
                calibrateScale();
            break;

            case 'C':     //Configurator    
                user_input_configurator();
            break;
            
            default:
            break;
        }
    }
//Serial.println("test");



}

void Teststand::user_input_configurator()
{
    Serial.println();
    Serial.println();
    Serial.println(F("Test Loop Configurator..."));
    
    user_read_int(delay_time_ms, "Delay time / step (µs):  ");Serial.println(delay_time_ms);
    user_read_int(step_time_us, "step time (µs) :  "); Serial.println(delay_time_ms);
    user_read_int(pwm_start_us, "start time (µs):  "); Serial.println(pwm_start_us);
    user_read_int(pwm_stop_us, "Stop time (µs):  "); Serial.println(pwm_stop_us);

    Serial.print("Delay per step (µs):   ");
    Serial.println(delay_time_ms);
    Serial.print("Steps per iteration (µs):   ");
    Serial.println(delay_time_ms);
    Serial.print("Start (µs):    ");
    Serial.println(pwm_start_us);
    Serial.print("Stop (µs):    ");
    Serial.println(pwm_stop_us);
    delay(1000);

}

void Teststand::edf_test_loop(int& delaytime_ms, int& num_steps, int& start_us, int& stop_us){
    float currentGrams{get_weight_avg()};
    //command = start_us + STEP_TIME_US;
    command = start_us;
    int i{0};
    int num_of_iterations{((stop_us-start_us)/num_steps)};
    float timestamp{0};
    
    //Prime EDF for 5 seconds as per ESC guidlines
    prime_edf(5);
    //Start the test bed loop
    Serial.print(num_of_iterations);
    Serial.print(",");
    Serial.print(delaytime_ms);
    Serial.print(",");
    Serial.print(start_us);
    Serial.print(",");
    Serial.println(stop_us);

    float start_timer{millis()};

    for (; i <= (num_of_iterations); i++)
    {
        
        timestamp = millis() - start_timer;
        //wedf_hold(command, delaytime_ms);
        wedf(command);
        delay(delaytime_ms);
        currentGrams = get_weight_avg_2();

        
        // float currentWeight_kg = currentWeight / 1000;
        // force_newtons[i] = currentWeight * 9.8067 / 1000; 
        // //update the result arrays
        // force_grams[i] = currentWeight;
        // pwm[i] = command;
        
        Serial.print(command);
        Serial.print(",");
        Serial.print((currentGrams * 9.807 / 1000), 5);
        Serial.print(",");
        Serial.print(currentGrams,5);
        Serial.print(",");
        Serial.print(timestamp/1000.00f);
        // Serial.print(",");
        // Serial.print(force_newtons[i], 5);
        Serial.println("");

        //update command for next iteration
        command += num_steps; 
    }
    //Checks to see if the end of the test run is reached
    //resets the test and stops the motor

      command = 1000;
      wedf(command);


}


//gets weight reading from scale and takes an average of
//AVG_SIZE many readings 
float Teststand::get_weight_avg(void){
    if (myScale.available() == true)
    {
        long currentReading = myScale.getReading();
        float currentWeight = myScale.getWeight();
        float avgWeight = 0;

        avgWeights[avgWeightSpot++] = currentWeight;
        if(avgWeightSpot == AVG_SIZE) avgWeightSpot = 0;

        for (int x = 0 ; x < AVG_SIZE ; x++)
        {
            avgWeight += avgWeights[x];
            avgWeight /= AVG_SIZE;
        }
        return avgWeight;
    }
}
//gets weight reading from scale and takes an average of
//AVG_SIZE many readings W. a different interpertation 
//to check something
float Teststand::get_weight_avg_2(void)
{
    float current_weigh[AVG_SIZE] ={0};
    float all_weights{0};

    if(myScale.available())
    {
        for(int i = 0; i < AVG_SIZE; i++)
        {
            //current_weigh[i] = myScale.getWeight();
            all_weights += myScale.getWeight();
        }
    }

    return (all_weights/AVG_SIZE);
}

void Teststand::calib_input_check(void)
{
    if (Serial.available())
    {
        byte incoming = Serial.read();

        //Tare the scale
        if (incoming == 't') myScale.calculateZeroOffset();
        if (incoming == 'c') calibrateScale();
        
    }
}

void Teststand::simple_measure(void){
    float grams{0};
    grams += myScale.getWeight();
    delay(2);
    grams += myScale.getWeight();
    delay(2);
    grams += myScale.getWeight();
    delay(2);
    grams += myScale.getWeight();
    delay(2);

    grams = grams/4;

    Serial.print("weight: ");
    Serial.println(grams);
   // calib_input_check();
}

void Teststand::initScale()
{
    Wire.begin();
    delay(100);
    if (myScale.begin() == false)
    {
        Serial.println("Scale not detected. Please check wiring. Freezing...");
        while (1);
    }
    Serial.println("Scale detected!");

    readSystemSettings(); //Load zeroOffset and calibrationFactor from EEPROM

    myScale.setSampleRate(NAU7802_SPS_320); //Increase to max sample rate
    myScale.calibrateAFE(); //Re-cal analog front end when we change gain, sample rate, or channel 

    Serial.print("Zero offset: ");
    Serial.println(myScale.getZeroOffset());
    Serial.print("Calibration factor: ");
    Serial.println(myScale.getCalibrationFactor());
}

//Gives user the ability to set a known weight on the scale and calculate a calibration factor
void Teststand::calibrateScale(void)
{
    //Read user input
    float weightOnScale;
    Serial.println(); Serial.println();
    Serial.println(F("Scale calibration"));

    user_read_anykey("Setup scale with no weight on it. Press a key when ready.");

    myScale.calculateZeroOffset(64); //Zero or Tare the scale. Average over 64 readings.
    Serial.print("New zero offset: "); Serial.println(myScale.getZeroOffset());

    user_read_anykey("Place known weight on scale. Press a key when weight is in place and stable.");    
    user_read_float(weightOnScale, "Please enter the weight, without units, currently sitting on the scale (for example '4.25'): ");

    myScale.calculateCalibrationFactor(weightOnScale, 64); //Tell the library how much weight is currently on it
    Serial.print("New cal factor: "); Serial.println(myScale.getCalibrationFactor(), 2);
    Serial.print("New Scale Reading: "); Serial.println(myScale.getWeight(), 2);

    recordSystemSettings(); //Commit these values to EEPROM
}

//Record the current system settings to EEPROM
void Teststand::recordSystemSettings(void)
{
//  //Get various values from the library and commit them to NVM
//  EEPROM.put(LOCATION_CALIBRATION_FACTOR, myScale.getCalibrationFactor());
//  EEPROM.put(LOCATION_ZERO_OFFSET, myScale.getZeroOffset());

  settingCalibrationFactor = myScale.getCalibrationFactor();
  settingZeroOffset = myScale.getZeroOffset();
}

//Reads the current system settings from EEPROM
//If anything looks weird, reset setting to default value
void Teststand::readSystemSettings(void)
{
#ifdef TEENSY 
    float settingCalibrationFactor; //Value used to convert the load cell reading to lbs or kg
    long settingZeroOffset; //Zero value that is found when scale is tared

    //Look up the calibration factor
    EEPROM.get(LOCATION_CALIBRATION_FACTOR, settingCalibrationFactor);
    if (settingCalibrationFactor == 0xFFFFFFFF)
    {
        settingCalibrationFactor = 0; //Default to 0
        EEPROM.put(LOCATION_CALIBRATION_FACTOR, settingCalibrationFactor);
    }

    //Look up the zero tare point
    EEPROM.get(LOCATION_ZERO_OFFSET, settingZeroOffset);
    if (settingZeroOffset == 0xFFFFFFFF)
    {
        settingZeroOffset = 1000L; //Default to 1000 so we don't get inf
        EEPROM.put(LOCATION_ZERO_OFFSET, settingZeroOffset);
    }

    //Pass these values to the library
    myScale.setCalibrationFactor(settingCalibrationFactor);
    myScale.setZeroOffset(settingZeroOffset);

    settingsDetected = true; //Assume for the moment that there are good cal values
    if (settingCalibrationFactor < 0.1 || settingZeroOffset == 1000)
        settingsDetected = false; //Defaults detected. Prompt user to cal scale.

#endif

#ifdef UNO 
    //Pass these values to the library
    myScale.setCalibrationFactor(settingCalibrationFactor);
    myScale.setZeroOffset(settingZeroOffset);

    settingsDetected = true; //Assume for the moment that there are good cal values
    if (settingCalibrationFactor < 0.1 || settingZeroOffset == 1000)
        settingsDetected = false; //Defaults detected. Prompt user to cal scale.
#endif
}

void Teststand::user_read_int(int & value, String message)
{
        //get start microseconds from user and store
    Serial.print(message);

    while (Serial.available()) Serial.read(); //Clear anything in RX buffer
    while (Serial.available() == 0) delay(10); //Wait for user to press key

    //Read user input
    value = (int)Serial.parseInt();
    Serial.println();  
}

void Teststand::user_read_float(float & value, String message)
{
        //get start microseconds from user and store
    Serial.print(message);

    while (Serial.available()) Serial.read(); //Clear anything in RX buffer
    while (Serial.available() == 0) delay(10); //Wait for user to press key

    //Read user input
    value = Serial.parseFloat();
    Serial.println();  
}

void Teststand::user_read_anykey(String message)
{
    Serial.println(message);
    while (Serial.available()) Serial.read(); //Clear anything in RX buffer
    while (Serial.available() == 0) delay(10); //Wait for user to press key
}
