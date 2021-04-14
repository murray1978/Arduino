/*
   Arduino Data logger for n DS18B20 tempurature sensors.
   Version 0.3
   Added  10/4/21
    RTC Clock Time set
    Check if logFile open, loop's until SD card is ready
    can define a custom CSV seperator
    Pass by reference to sample and sampleHour
   Fixed  10/4/21
    CSV file had an extra carrage return, now all data on a single line
   Added 14/4/20
    Checking if now == NULL, and handling the error 
    Checking if getTempCByIndex returns NaN
   TODO
    Add a retry counter for file access atempts, print an error message to serial on fail
    Put Debug Serial strings into a debug function
*/

#include <Wire.h>
#include "RTClib.h"
#include <OneWire.h>
#include <DallasTemperature.h>              // DS18B20  library
#include <SPI.h>
#include <SD.h>                             // SD Card Library

#define chipSelect 10                       // CS pin of your data logger shield.Maybe not yours!!
#define numberOfSensors 3                   // Number sensors on a pin
#define sensorResolution 12                 // Bits of resolution for reading
#define ONE_WIRE_BUS_PIN 5                  // The pin that we are using for sensors
#define defaultFilename "Logfile.csv"       // Change to suit
#define CSV_Seperator ","                   // Change if you need a data seperator

OneWire oneWire(ONE_WIRE_BUS_PIN);          // Setup oneWire
DallasTemperature sensors(&oneWire);        // oneWire to Dallas Temperature.
RTC_DS1307 RTC;                             // define the Real Time Clock object

File Logfile;                               // Current open file file handle
String tempString = "";

/*
   WriteSdCardData
   data = data to send to SD Card with the default filename
*/
void writeSdCardData(String data)
{
  //Open log file
  Logfile = SD.open(defaultFilename, FILE_WRITE);

  //If log file is NULL the ardunio will reboot
  if ( !Logfile ) {
    Serial.print("Cannot open ");
    Serial.println(defaultFilename );
    /*
        retry, the arduino will apear to hang here,
    */
    while ( Logfile == NULL )
    {
      Logfile = SD.open(defaultFilename, FILE_WRITE);
    }
  }

  Serial.print(data);
  Logfile.print(data);
  Logfile.close();

}

/*
   Setup, called each time Arduino is reset, (Hardware or POR)
*/
void setup()
{
  // Set CS for SD Card
  SD.begin(chipSelect);
  Wire.begin();

  // Initalise DS18B20 library
  sensors.begin();

  // Start Serial port and print program name
  Serial.begin(9600);
  Serial.println("\ntemp Datalogger V0.3");

  // Start RTC
  RTC.begin();

  // Set RTC if new board, __DATE__ and __TIME__ are compile time
  if ( !RTC.isrunning()) {
    Serial.println("RTC is NOT running, let's set the time!");
    RTC.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  //Set Sensor resolution for n Sensors
  for ( int i = 0; i < numberOfSensors; i++) {
    sensors.setResolution(i, sensorResolution);
  }

}

/*

*/
void logTemperatureSensors()
{
  float tempC;
  // Request all devices on bus to read temperature
  sensors.requestTemperatures();

  //Clear our ouput string
  tempString = "";

  //Loop through available sensors
  for ( int i = 0; i < numberOfSensors; i++) {
    tempC = sensors.getTempCByIndex(i);
    if ( !isnan(tempC)) {
    tempString += String(tempC, 2);
      //Print only needed default CSV seperators.
      if ( i < numberOfSensors - 1) {
        tempString += CSV_Seperator;
      }
    }
    else
    {
      Serial.print("Cannot read sensor ");
      Serial.println(i);
    }
  }

  //end the line
  tempString += "\n";

  //Write data to SD Card
  writeSdCardData(tempString);
}

/*

*/
void logTime(DateTime *now)
{

  //Clear our string
  tempString = "";

  //Create our time stamp, openoffice and excel compatable
  tempString += String(now->year());
  tempString += "/";
  tempString += String(now->month());
  tempString += "/";
  tempString += String(now->day());
  tempString += " ";
  tempString += String(now->hour());
  tempString += ":";
  tempString += String(now->minute());
  tempString += ":";
  tempString += String(now->second());
  //Default csv seperator
  tempString += CSV_Seperator;

  //Write data to SD Card
  writeSdCardData(tempString);

}

/*

*/
void sampleHour(DateTime *now, int timeFrame)
{
  //Sample at a modulas of the Hour from 1 minute to 59 minuts
  if ((now->hour() % timeFrame) == 0 && now->second() == 0) {
    Serial.println("Log hour");
    logTime(now);
    logTemperatureSensors();
  }
}

/*

*/
void sample(DateTime *now, int timeFrame)
{
  //Sample at a modulas of the minute from 1 minute to 59 minuts
  if ((now->minute() % timeFrame) == 0 && now->second() == 0) {
    Serial.println("Log min");
    logTime(now);
    logTemperatureSensors();
  }
}

/*

*/
void loop()
{

  //Get current date and time, (Having the now object instatiated every loop saves us 1% of program space!)
  DateTime now = RTC.now();
  if (now == NULL )
  {
    Serial.println("RTC.now() returned null");
  }
  else 
  {

    //record once every minute
    sample(&now, 1);

    //enough of a delay to record once
    delay(1000);
  }
}
