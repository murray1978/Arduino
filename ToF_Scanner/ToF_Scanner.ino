/*
    ToF Scanner
    Stand Alone version
    By Murray Smith 2018

    Data is stored on SDCARD or sent to a PC via usb-serial

    view via meshlab after files procesed
    
    TODO:
      +Serial commands, to/from sonar firmware
      +Keypad commands in via LCD/Keypad Sheild,
      +Status out via a Serial LCD
*/

#include <Servo.h>
#include <SPI.h>
#include <SD.h>
#include <Stepper.h>
#include <Math.h>

//Current program version
static int siVersion = 1;    //read as 0.2, ie 10 = 1.0

//Serial Commands
#define SETUP         0
#define START         2
#define STOP          4

/*
   Character buffers.
*/
#define MAX_CHAR_BUFFER 256/2
char cBuffer[MAX_CHAR_BUFFER] = { 0 };
char fBuffer[32] = {0};                 //float buffer

/*
   Vertical angle Servo
*/
#define ASCPIN 9          // Ascention Servo pin, vertical angle
#define ASCMAX 60         // Assention, vertical angle
#define ASCMIN 30
int ascDecValue = -1;       // used to reverse servo
int posAsc = 0;           // assention servo position
Servo asc;

/*
   Horizontal angle stepper
   4096 steps turn on OUTPUT shaft, 64 steps per motor turn
   360/4096 = 0.09 deg per step
*/
#define STEPS 4096     // 1 revolution of the OUTPUT shaft 
#define STEPSPERDEG 6  // A guess, should be about 7.something
#define SPEED 5        // Trial and error........your milage may vary
#define DECMAX 90    // Declanation, horizontal angle
#define DECMIN 0
int posDec = 0;             // Declination servo position 0-180 deg, Horizontal angle
Stepper decStepper( STEPS, 4, 5, 6, 7);


/*
   SD Card
*/
#define SDCARD_CS 10  //change for your brand of SD Card reader

/*
   Local SD Card Storage
*/
File dataFile;
File root;
char cXYZDefaultFileName[] = "sonar.xyz";
char cXYZFileName[] = "sonar";
char cXYZExtension[] = ".xyz";

/*
   logic control
*/
bool bXYZBySerial;          // Send data via serial
bool bXYZBySdCard;           // or directly to an SD Card
bool bUseDefaultFileName = true;    // Use Default Filename for SDCard file
bool bScanDone = false;

/*
   Limit switch's on A0 and A1,
   10K Pullup resistors on each pin 0 = closed, 255 > open
*/
static int LIMIT_RIGHT = 0;
static int LIMIT_LEFT  = 1;

/*
   Calculate range,
   Given a time of flight for a sonar "pulse" calculate distance
   using current humidity and tempurature
*/
float getRange( ) {
  return 2.0f;
}

/*
   Send current/default params via serial
*/
void sendParams() {
  sprintf( cBuffer, "%i,%i,%i\n", siVersion, temp, humidity);
  Serial.print(cBuffer);
}

/*
   Send tempurature via serial
*/
void sendTemp() {
  sprintf( cBuffer, "%i\n", temp );
  Serial.print(cBuffer);
}

/*
   Send humidity via serial
*/
void sendHumidity() {
  sprintf( cBuffer, "%i\n", humidity );
  Serial.print(cBuffer);
}

/*
   setupDataFile, if a file exists, append next number in sequence to new file.
*/
void setupDataFile() {

  //Serial.println("MSG: Setting up output file");

    int i = 0;

    //Copy defalut file name to
    strcpy( cBuffer, cXYZDefaultFileName);

    // and check if it exsists
    while (SD.exists(cBuffer) )
    {
      //Serial.print("MSG: ");
      Serial.print(cBuffer);
      //Serial.println(" exsists'");
      //Append number to file name
      sprintf( cBuffer, "%s%i%s", cXYZFileName, i++ , cXYZExtension );
    }

  //Serial.print("MSG: "); Serial.print("opening "); Serial.println(cBuffer);
  dataFile = SD.open(cBuffer, FILE_WRITE);

  if ( !dataFile ) {
    Serial.println("FATAL: could not open data file");
    return;
  }
}

/*
   Zero the horizontal angle stepper to the left
*/
void zeroLeft()
{
  while (analogRead(LIMIT_LEFT) > 256 ) {
    decStepper.step(-STEPSPERDEG);
  }
}

/*
   Zero the horizontal angle stepper to the right
*/
void zeroRight() {
  while (analogRead(LIMIT_RIGHT) > 256 ) {
    decStepper.step(STEPSPERDEG);
  }
}

/*
   Setup the hardware.
*/
void setup() {

  Serial.begin(115200);
  posAsc = ASCMAX;
  asc.write(posAsc);


  if ( !SD.begin(SDCARD_CS)) {
   // Serial.println("ERROR: SD Card not present or wrong chip select pin selected");
    //Serial.println("MSG: defaulting to serial output");
    bXYZBySdCard = false;
  } else {
    //Serial.println("MSG: SD Card Present");
    setupDataFile();
    bXYZBySdCard = true;
  }

  decStepper.setSpeed(SPEED);
  zeroRight();

  bXYZBySerial = true;          // Send data via serial

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
    //What happens if we hit a limit switch?
    decStepper.step(-STEPSPERDEG);
    range = getRange(); 

    x = (float)(range * sin(degToRad(posDec)) * cos(degToRad(posAsc)));                 
    y = (float)(range * sin(degToRad(posDec)) * sin(degToRad(posAsc)));
    z = (float)(range * cos(degToRad(posDec)));
    
    dtostrf( x, 5, 2, xBuff); 
    dtostrf( y, 5, 2, yBuff); 
    dtostrf( z, 5, 2, zBuff); 
    sprintf(cBuffer, "%s,%s,%s\n", xBuff, yBuff, zBuff);

    if (bXYZBySerial) {
      Serial.print( cBuffer );
    }
    else if (bXYZBySdCard ) {
      dataFile.print(cBuffer);
      dataFile.flush();
    }
    else
    {
      Serial.println("ERROR: no data destination selected");
    }
  }

  zeroRight();

  if ( posAsc >= ASCMAX ) {
    ascDecValue = -1;
  } else if ( posAsc <= ASCMIN )
  {
    ascDecValue = 1;
    if ( dataFile) {
      dataFile.close();
    }
    Serial.println("Scann finished");
    while (true);            //TODO replace with return
  }

  posAsc = posAsc + ascDecValue;
  asc.write(posAsc);
}

/*

*/
void processSerialIn() {
}

void loop() {
  processSonar();
}

