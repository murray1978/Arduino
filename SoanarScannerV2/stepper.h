#pragma once

/* 
 * stepper pins
 */
#define STEP1 7
#define STEP2 6
#define STEP3 5
#define STEP4 4
#define STEPPERMS 5

/* 
 * stepper
 */
byte steps8[] = {
  HIGH,  LOW,  LOW,  LOW,
  HIGH, HIGH,  LOW,  LOW,
   LOW, HIGH,  LOW,  LOW,
   LOW, HIGH, HIGH,  LOW,
   LOW,  LOW, HIGH,  LOW, 
   LOW,  LOW, HIGH, HIGH,
   LOW,  LOW,  LOW, HIGH,
  HIGH,  LOW,  LOW, HIGH,
};


void init()
{
  pinMode(STEP1, OUTPUT);
  pinMode(STEP2, OUTPUT);
  pinMode(STEP3, OUTPUT);
  pinMode(STEP4, OUTPUT);
}


/* 
 * stepper command
 */
void step8(int pin1, int pin2, int pin3, int pin4) {
  int i=0;
  while (i<32) {
    digitalWrite(pin1, steps8[i++]);
    digitalWrite(pin2, steps8[i++]);
    digitalWrite(pin3, steps8[i++]);
    digitalWrite(pin4, steps8[i++]);
    delay(STEPPERMS);
  }
} 

void stepperCommand(int steps) {
  while (steps) {
    if (steps > 0) {
      step8(STEP1,STEP2,STEP3,STEP4);
      steps--;
    }
    else {
      step8(STEP4,STEP3,STEP2,STEP1);
      steps++;
    }
  }
}
