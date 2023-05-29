//#include <Arduino.h>
#include "Teststand.h"
#include <Wire.h>



Teststand ts;

void setup() {
  Serial.begin(115200);
  delay(100);
  //Wire.begin();
  Serial.println("serial started...");
    //indicator data for matlab script 
 // ts.init();
 //ts.initScale();
 ts.init_edf();
 ts.initScale() ;
}

void loop() {
  // Serial.println("test");
  // delay(1000);
  ts.user_input_loop();

}