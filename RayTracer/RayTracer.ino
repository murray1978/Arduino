/*
 Arduino RayTracer 18/12/14, Murray Smith.
 Using code from My Cosc343? Graphics paper, and a Freetronics OLED display 128x128.
*/
#include <SPI.h>
#include <SD.h>
#include <FTOLED.h>

#include "matrix.h"
#include "vector.h"

const byte pin_cs = 7;
const byte pin_dc = 2;
const byte pin_reset = 3;

OLED oled( pin_cs, pin_dc, pin_reset );

void setup()
{
  oled.begin();
}

//May leeve this loop empty
void loop(){}


