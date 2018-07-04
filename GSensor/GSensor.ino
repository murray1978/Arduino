#include <SD.h>
#include <SPI.h>
#include <FTOLED.h>
#include <FTOLED_Colours.h>
#include <progmem_compat.h>
#include <fonts/SystemFont5x7.h>

/* Pins for the OLED Display */
const byte pin_cs = 7;
const byte pin_dc = 2; 
const byte pin_reset = 3;

/* pins or input */
const byte acc_x = 0;
const byte acc_y = 1;
const byte analog_x = 2;
const byte analog_y = 3;
const byte acc_z = 4;
const byte pin_button = 6;
const byte pin_piezo = 9;

OLED oled( pin_cs, pin_dc, pin_reset);
OLED_TextBox text(oled, 0, 115, 128, 13);

int samples[5][128];
//calibration for 1.5g
int cal_zero_x = 256;
int cal_zero_y = 256;
int cal_max_x = 512;
int cal_max_y = 512;

void setup() {
  oled.begin();
  pinMode( pin_button, INPUT );
  pinMode( pin_piezo, OUTPUT );
  oled.selectFont(SystemFont5x7);
  text.setForegroundColour(DARKGREEN);
  text.println("ACC -1.5 : LAT R0.2");
  oled.drawBox(0,0,128,114,1, DARKGREEN);
  oled.drawCircle( 64, 54, 1, DARKGREEN);
  oled.drawCircle( 64, 54, 30, DARKGREEN );
  oled.drawCircle( 64, 54, 50, DARKGREEN );
  //G-Dot
  oled.drawCircle( 34, 72, 1, RED );
  //calibrate();
}

void loop() {
  // put your main code here, to run repeatedly:
  
}

void calibrate()
{
  //text.reset();
  text.println("Calibrate:");
}
