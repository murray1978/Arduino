/*
    Sonar Scanner V2
    Stand Alone version
    By Murray Smith 2018


    Data is stored on SDCARD or sent to a PC via usb-serial

    TODO:
      +Serial commands, to/from sonar firmware
      +Keypad commands in via LCD/Keypad Sheild,
      +Status out via a Serial LCD
*/

#include <Servo.h>
#include <NewPing.h>
#include <DHT.h>
#include <SPI.h>
#include <SD.h>
#include <Stepper.h>

//Current program version
static int siVersion = 4;    //read as 0.2, ie 10 = 1.0

//Serial Commands
#define SETUP         0
#define START         2
#define STOP          4

#define SEND_TEMP     6
#define SEND_HUMIDITY 8
#define SEND_DATA     2


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
   Ultrasonic sensor
   Using a HC-SR04 ultrasonic sensor
   can be used in 3 pin mode if you run out of pins
*/
#define TRIGPIN 3
#define ECHOPIN 2
#define MAXDISTANCE 400
#define ITERATIONS 10
NewPing sonar( TRIGPIN, ECHOPIN, MAXDISTANCE);

/*
   DHT22, NOTE: Change to DHT11 sensor if you are using that
*/
#define DHTPIN 8
float humidity = 0.0f;
float temp = 0.0f;
DHT dht( DHTPIN, DHT22);

/*
   Distance calculation
*/
static float c = 331.4f;   // Speed of sound at 0C and 0%(?) humidity
float deltaT = 0.0f;              // Time Delay
float distance = 0.0f;

/*
   SD Card
*/
const int chipSelect = 10;  //change for your brand of SD Card reader

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
float calcRange( float dTime ) {
  float speedSound = c + ( 0.606f * temp) + ( 0.0124f * humidity );
  return (float)(( dTime / 2.0f) * (speedSound / 10000.0f));
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

  asc.attach(ASCPIN);
  Serial.begin(115200);
  dht.begin();
  posAsc = ASCMAX;
  asc.write(posAsc);


  if ( !SD.begin(chipSelect)) {
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

  //Inital read of DHT22
  humidity = dht.readHumidity();
  temp = dht.readTemperature();

  bXYZBySerial = true;          // Send data via serial

}

/*
   Main sonar processing loop
   decStepper.step(1);
*/
void processSonar() {

  humidity = dht.readHumidity();
  temp = dht.readTemperature();


  for (posDec = DECMIN; posDec <= DECMAX; posDec += 1) { // goes from DECMIN degrees to DECMAX degrees
    //What happens if we hit a limit switch?
    decStepper.step(-STEPSPERDEG);
    
    dtostrf( calcRange(sonar.ping_median(ITERATIONS)), 5, 2, fBuffer); //sprintf does not support %f in arduino 
    sprintf(cBuffer, "%i,%i,%s\n", posDec, posAsc, fBuffer);

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

