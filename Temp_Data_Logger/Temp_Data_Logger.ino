/*
   Arduino Data logger for n DS18B20 tempurature sensors.
*/

#include <Wire.h>                           //Libraries we need
#include "RTClib.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#include <SPI.h>
#include <SD.h>

#define chipSelect 10                       //CS pin of your data logger shield.Maybe not yours!!
#define numberOfSensors 3
#define sensorResolution 12
#define ONE_WIRE_BUS_PIN 5                  // The pin that we are using for sensors
#define defaultFilename "Logfile.csv"

OneWire oneWire(ONE_WIRE_BUS_PIN);          // Setup oneWire
DallasTemperature sensors(&oneWire);        // oneWire to Dallas Temperature.
RTC_DS1307 RTC;                             // define the Real Time Clock object

File Logfile;                        // Filehandle
String tempString = "";

void writeSdCardData(String data)
{
  //Open log file
  Logfile = SD.open(defaultFilename, FILE_WRITE);

  //If log file is NULL the ardunio will reboot
  if ( !Logfile ) {
    Serial.print("Cannot open ");  
    Serial.println(defaultFilename );
    /* retry, the arduino will apear to hang here,
     * TODO place a retry counter here and print error message to serial
     *
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

void setup()
{
  SD.begin(chipSelect);                     //Initialize the libraries
  Wire.begin();
  RTC.begin();
  sensors.begin();

  //For loop to setup multiple sensor's,
  for ( int i = 0; i < numberOfSensors; i++) {
    sensors.setResolution(i, sensorResolution);
  }

  Serial.begin(9600);
  Serial.println("\ntemp Datalogger V0.2");
}



void logTemperatureSensors()
{
  // Command all devices on bus to read temperature
  sensors.requestTemperatures();

  //Clear our ouput string
  tempString = "";

  //Loop through available sensors
  for ( int i = 0; i < numberOfSensors; i++) {
    tempString += String(sensors.getTempCByIndex(i), 2);
    //Print only needed default CSV seperators.
    if ( i < numberOfSensors - 1) {
      tempString += ",";
    }
  }

  //end the line
  tempString += "\n";
  
  //Write data to SD Card
  writeSdCardData(tempString);
}

void logTime(DateTime now)
{

  //Clear our string
  tempString = "";

  //Create our time stamp, openoffice and excel compatable
  tempString += String(now.year());
  tempString += "/";
  tempString += String(now.month());
  tempString += "/";
  tempString += String(now.day());
  tempString += " ";
  tempString += String(now.hour());
  tempString += ":";
  tempString += String(now.minute());
  tempString += ":";
  tempString += String(now.second());
  //Default csv seperator
  tempString += ",";

  //Write data to SD Card
  writeSdCardData(tempString);

}

void sampleHour(DataTime now, int timeFrame)
{
   //Sample at a modulas of the Hour from 1 minute to 59 minuts
  if ((now.hour() % timeFrame) == 0 && now.second() == 0) {
    Serial.println("Log hour");
    logTime(now);
    logTemperatureSensors();
  }
}

void sample(DateTime now, int timeFrame)
{
  //Sample at a modulas of the minute from 1 minute to 59 minuts
  if ((now.minute() % timeFrame) == 0 && now.second() == 0) {
    Serial.println("Log min");
    logTime(now);
    logTemperatureSensors();
  }
}

void loop()
{

  DateTime now = RTC.now(); // Clock call
  //record once every 1 min
  sample(now, 1);
  //enough of a delay to record once
  delay(1000);
}
