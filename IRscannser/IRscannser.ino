/*
  Scanning IR sensor image
  Version 0.1
    inital IR image program
  Version 0.2
    change servo.write to servo.writeMicroseconds.
    added extra const's to reflect change.
    decreased delay() values to speed up image scan.
   Version 0.3
    removal of redundant code, delays.
    testing full int's for data
   Version 0.4
     added PAN and TILT step to speed up image aqcuistion.
     Removed a for loop.
     will only image once per reset.
*/
#include "IRTemp.h"
#include <Servo.h>

static const byte PIN_DATA = 6;
static const byte PIN_CLOCK = 2;
static const byte PIN_AQUIRE = 4;

static const bool SCALE = false;
static const bool SCAN_COMPLETE = false;

IRTemp irTemp( PIN_AQUIRE, PIN_CLOCK, PIN_DATA);

//using writeMicroseconds() limits are 1000 to 2000, midpoint = 1500
//pan servo settings
Servo panServo;
int panPos = 0;
static const int PAN_STEP = 4;
static const int PAN_RANGE = 200;
static const int PAN_POS_MAX = 1500 + ( PAN_RANGE / 2);
static const int PAN_POS_MIN = 1500 - ( PAN_RANGE / 2);
static const int PAN_SERVO = 5;

//tilt servo settings
Servo tiltServo;
int tiltPos = 0;
static const int TILT_STEP = 4;
static const int TILT_RANGE = 200;
static const int TILT_POS_MAX = 1500 + ( TILT_RANGE / 2);
static const int TILT_POS_MIN = 1500 - ( TILT_RANGE / 2);
static const int TILT_SERVO = 3;

void setup(void){
  //write out file header.
   Serial.begin(9600);
   Serial.println("P2");
   Serial.println("#IR Image v0.4");
   Serial.print(PAN_RANGE/PAN_STEP);
   Serial.print(" ");
   Serial.println(TILT_RANGE/TILT_STEP);
   Serial.println("65535");
   
   panServo.attach(PAN_SERVO);
   tiltServo.attach(TILT_SERVO);
   panServo.writeMicroseconds(PAN_POS_MAX);
   tiltServo.writeMicroseconds(TILT_POS_MAX);
}

unsigned int irData = 0;

void loop(void){

  tiltPos = TILT_POS_MAX;
  
  while( tiltPos > TILT_POS_MIN ){
     //scan a line.
     for(panPos = PAN_POS_MAX; panPos > PAN_POS_MIN; panPos -= TILT_STEP ){
       panServo.writeMicroseconds(panPos);
       irData = (unsigned int)( irTemp.getIRTemperature(SCALE)* 1000);
       //write out raw data
       Serial.println(irData);
 
     }
     
    //update tilt 
    tiltPos -= TILT_STEP;
    tiltServo.writeMicroseconds(tiltPos);
  }
  //halt image capture.
  for(;;){}
}
