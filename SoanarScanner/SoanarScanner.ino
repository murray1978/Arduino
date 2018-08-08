/* 
 *  Sonar Scanner 
 *  By Murray Smith 2018
 *  
*/

#include <Servo.h>
#include <NewPing.h>
#include <DHT.h>

//Servo's
static int ASCPIN = 10;
static int DECPIN = 9;
int posDec = 0;             // Declination servo position 0-180 deg
int posAsc = 0;             // assention servo position   0-180 deg
int ascDecValue = -1;
static int ASCMAX = 85;    // Assention, vertical angle
static int ASCMIN = 15;
static int DECMAX = 170;    // Declanation, horizontal angle
static int DECMIN = 10;
Servo dec; 
Servo asc;

//Ultasonic sensor
static int TRIGPIN = 2;
static int ECHOPIN = 3;
static int MAXDISTANCE = 400;
int iterations = 10;
NewPing sonar( TRIGPIN, ECHOPIN, MAXDISTANCE);

//DHT22
static int DHTPIN = 4;
float hum = 0.0f;
float temp = 0.0f;
DHT dht( DHTPIN, DHT22);

static float c = 331.4f;   // Speed of sound at 0C and x% humidity 
float deltaT;              // Time Delay
float distance;



float getRange( float dTime ){

  float speedSound = c + ( 0.606f * temp) + ( 0.0124f * hum );
  return float(( dTime/2.0f) * (speedSound/10000.0f));
}

void setup() {
  dec.attach(DECPIN);
  asc.attach(ASCPIN);
  Serial.begin(115200);
  //Serial.println("P2");
  //Serial.println("#SonarScanner v0.1");
  dht.begin();
  posAsc = ASCMAX;
  asc.write(posAsc);
  //Serial.println("160 70");
  //Serial.println("2048");
  //Serial.println("0");
}

void loop() {
  
  hum = dht.readHumidity();
  temp = dht.readTemperature();
  
  for (posDec = DECMIN; posDec <= DECMAX; posDec += 1) { // goes from DECMIN degrees to DECMAX degrees
    // in steps of 1 degree
    dec.write(posDec);              // tell servo to go to position in variable 'pos'
    delay(30);                       // waits 35ms for the servo to reach the position, 
    Serial.print(posDec);Serial.print(",");Serial.print(posAsc);Serial.print(",");
    Serial.println( getRange(sonar.ping_median(iterations)));
  }

  for (posDec = DECMAX; posDec >= DECMIN; posDec -= 1) { // goes from DECMAX degrees to DECMIN degrees
    dec.write(posDec);              // tell servo to go to position in variable 'pos'
    delay(1);                       // waits 15ms for the servo to reach the position
  }
  
  if( posAsc >= ASCMAX ){
    ascDecValue = -1;
  }else if( posAsc <= ASCMIN )
  {
     ascDecValue = 1;
     while(true);//stop
  }
  posAsc = posAsc + ascDecValue;
  asc.write(posAsc);
}

