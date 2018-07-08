/*
 * MDIU.h
 *  Both MDK (Modular Display Keyboard) and MDR (Modular Display Readout are implimented here
*/
#pragma once
#include <keypad.h>

const byte ROWS = 4; // Four rows
const byte COLS = 3; // Three columns

char keys[ROWS][COLS] = 
{{'1','2','3'},
 {'4','5','6'},
 {'7','8','9'},
 {'*','0','#'}};

byte rowPins[ROWS] = { 5,4,3,2 };
byte colPins[COLS] = { 8,7,6 };

int count = 0;

Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

//char key = keypad.getKey();
//if( key != NO_KEY )
