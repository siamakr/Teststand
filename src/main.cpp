//#include <Arduino.h>
#include "Teststand.h"
#include <Wire.h>
#include "Actuator.h"


//Actuator a;
Teststand ts;
bool flag{false};

void setup() {
  Serial.begin(115200);
  delay(100);
  //Wire.begin();
  Serial.println("serial started...");

  ts.init_edf();
  ts.initScale();
//  a.init();
}

void loop() {
  // Serial.println("test");
  // delay(1000);
  ts.user_input_loop();
  


}