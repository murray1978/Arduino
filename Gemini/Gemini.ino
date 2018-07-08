#include <Key.h>
#include <Keypad.h>

#include "GeminiStateMachine.h"

void setup() {
  //Serial coms/debug setup:
  Serial.begin(9600);
  Serial.println("Gemini");
  //Setup Periphials
  //Initalise gemini executor
 
  
}

void loop() {
  // put your main code here, to run repeatedly:
   geminiFlightLogic();
   Serial.println("Error Logic has unexpectidly quit\n restarting logic");
}
