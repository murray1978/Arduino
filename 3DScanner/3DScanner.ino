/*
    3D Scanner based on a VL53L0X sensor,
    Max range about 1.8m
    By Murray Smith 2018

    view via meshlab after files procesed
*/

#include <Servo.h>
#include <Math.h>
#include "Seeed_vl53l0x.h"

Seeed_vl53l0x VL53L0X;

//Current program version
static int siVersion = 1;    //read as 0.2, ie 10 = 1.0

//Serial Commands
#define SETUP         0
#define START         2
#define STOP          4

/*
   Character buffers.
*/
#define MAX_CHAR_BUFFER 80
char cBuffer[MAX_CHAR_BUFFER] = { 0 };

/*
   Vertical axis Servo
*/
#define ASCPIN 10          // Ascention Servo pin, vertical angle
#define ASCMAX 100         // Assention, vertical angle
#define ASCMIN 10
int ascDecValue = -1;       // used to reverse servo
int posAsc = 0;           // assention servo position
Servo asc;

/*
  horizontal Axis Servo
*/
#define DECMAX 90    // Declanation, horizontal angle
#define DECMIN 0
#define DECPIN 9
int posDec = 0;             // Declination servo position 0-180 deg, Horizontal angle
Servo dec;

/*
   logic control
*/
bool bScanDone = false;

/*
   Calculate range,
   Given a time of flight for a sonar "pulse" calculate distance
   using current humidity and tempurature
*/
float getRange( ) {
    VL53L0X_RangingMeasurementData_t RangingMeasurementData;
    VL53L0X_Error Status = VL53L0X_ERROR_NONE;

    memset(&RangingMeasurementData,0,sizeof(VL53L0X_RangingMeasurementData_t));
    Status=VL53L0X.PerformSingleRangingMeasurement(&RangingMeasurementData);
    
    if(VL53L0X_ERROR_NONE==Status)
    {
      return (float)RangingMeasurementData.RangeMilliMeter;
    }
    return (float)0.0f;
}

/*
   Setup the hardware.
*/
void setup() {

  Serial.begin(115200);

  asc.attach(ASCPIN);
  posAsc = ASCMAX;
  asc.write(posAsc);

  dec.attach(DECPIN);

  VL53L0X_Error Status = VL53L0X_ERROR_NONE;
  Status = VL53L0X.VL53L0X_common_init();

  if (VL53L0X_ERROR_NONE != Status)
  {
    Serial.println("start vl53l0x mesurement failed!");
    VL53L0X.print_pal_error(Status);
    while (1);
  }
  
  VL53L0X.VL53L0X_long_distance_ranging_init();
  
  if (VL53L0X_ERROR_NONE != Status)
  {
    Serial.println("start vl53l0x mesurement failed!");
    VL53L0X.print_pal_error(Status);
    while (1);
  }


  delay(500);
}

double degToRad(float deg)
{
  return (double)(deg * (PI / 180.0f));
}

/*
   Main sonar processing loop
   decStepper.step(1);
*/
void processSonar() {

  float x, y, z;
  float range;
  char xBuff[25] = { 0 };
  char yBuff[25] = { 0 };
  char zBuff[25] = { 0 };

  for (posDec = DECMIN; posDec <= DECMAX; posDec += 1) { // goes from DECMIN degrees to DECMAX degrees
    dec.write(posDec);
    range = getRange();

    /* x = (float)((float)range * sin(degToRad((double)posDec)) * cos(degToRad((double)posAsc)));
      y = (float)((float)range * sin(degToRad((double)posDec)) * sin(degToRad((double)posAsc)));
      z = (float)((float)range * cos(degToRad((double)posDec)));
    */

    x = (range * sin(degToRad(posAsc)) * cos(degToRad(posDec)));
    y = (range * sin(degToRad(posDec)) * sin(degToRad(posAsc)));
    z = (range * cos(degToRad(posAsc)));

    dtostrf( x, 8, 5, xBuff);
    dtostrf( y, 8, 5, yBuff);
    dtostrf( z, 8, 5, zBuff);

    sprintf(cBuffer, "%s,%s,%s\n", xBuff, yBuff, zBuff);
    Serial.print( cBuffer );

  }

  for (posDec = DECMAX; posDec >= DECMIN; posDec -= 1) { // goes from DECMAX degrees to DECMIN degrees
    dec.write(posDec);              // tell servo to go to position in variable 'pos'
    delay(20);                       // waits 15ms for the servo to reach the position
  }

  if ( posAsc >= ASCMAX ) {
    ascDecValue = -1;
  } else if ( posAsc <= ASCMIN )
  {
    ascDecValue = 1;
    Serial.println("Scann finished");
    while (true);            //TODO replace with return
  }

  posAsc = posAsc + ascDecValue;
  asc.write(posAsc);
  delay(20);  //wait untill servo has moved
}

/*

*/
void processSerialIn() {
}

void loop() {
  processSonar();
}

