#include "GeminiStateMachine.h"

void setup() {
  //Serial coms/debug setup:
  Serial.begin(9600);
  Serial.println("Gemini");
  //Setup Periphials
  //Initalise gemini executor
  geminiFlightLogic();
  Serial.println("Error Logic has unexpectidly quit\n restart arduino");
}

void loop() {
  // put your main code here, to run repeatedly:

}
