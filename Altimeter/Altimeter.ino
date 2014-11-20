#include <Wire.h>
#include <LiquidCrystal.h>
#include <BaroSensor.h> 

//Constants used in altitude calculation.
#define P_SEA_LEVEL 101325.0f
#define T_SEA_LEVEL 288.15f
#define GAS_CONST   287.05f
#define TEMP_GRAD   -0.0065f
#define GRAV        9.80665f
#define PR_EXP (( GAS_CONST * TEMP_GRAD) / GRAV )
//ONE millibar  = 100 n/m^2
#define MBAR_NM2(x) ( x * 100 )

//Wish I could use enum properly grrrrrrrr.
#define MODE_QNE 0
#define MODE_QFE 2
#define MODE_QNH 4
unsigned int altMode = MODE_QNE;

//Keyboard constants.
//ADC Readings
#define BUTTON_ADC_PIN A0
#define RIGHT_ADC  0
#define UP_ADC     145
#define DOWN_ADC   329
#define LEFT_ADC   505
#define SELECT_ADC 741
#define HYSTERESIS 10

//return values for readButtons()
#define BUTTON_NONE   0
#define BUTTON_RIGHT  1
#define BUTTON_UP     2
#define BUTTON_DOWN   3
#define BUTTON_LEFT   4
#define BUTTON_SELECT 5

byte buttonPressed = false;
byte buttonReleased = false;
byte buttonWas = BUTTON_NONE;

//Altimeter settings mBar, set at startup.
float qfe = 1013.25f;      //Airfield
float qnh = 1013.25f;      //mean sea level / transition
float qne = 1013.25f;      //Std presure dataum, fixed.

//display refressh
unsigned long oldTime;
unsigned long curTime;

//Standard freetronics LCD setup
LiquidCrystal lcd( 8,9,4,5,6,7 );
void setup(void)
{
  //debugging
  Serial.begin(9600);

  //Keypad input setup
  pinMode( BUTTON_ADC_PIN, INPUT );
  digitalWrite( BUTTON_ADC_PIN, LOW );

  //Setup the Barometer sensor
  BaroSensor.begin();

  //LCD display setup complete
  lcd.clear();
  lcd.begin( 16, 2);
  lcd.print("Altimeter");
  altimeter_setup();
  oldTime = millis();
}

//float pressure = 1013.25f;
float height = 0.0f;
float baroReading = 0;


void loop(void)
{
  
  byte adcButton;
  curTime = millis();
  
  //get the current input state
  if((adcButton = readButtons()) == BUTTON_SELECT)
  {
    //key time out, could add milis() timer
    delay(250);
    altimeter_setup();
  }

  

  if(!BaroSensor.isOK()) //repace the ! for release...
  {
    lcd.setCursor(0,0);
    lcd.print("Sensor not found" );
    lcd.setCursor(0,1);
    lcd.print("error code: ");
    lcd.print(BaroSensor.getError());
    BaroSensor.begin();
  }
  else if ((curTime - oldTime) > 250 )
  {
    lcd.clear();
    
    //BaroSensor returns mBar as a FLOAT
    baroReading = BaroSensor.getPressure(); //- (float)( 2 * 101325);
  
   //float dp = 0.016f;

    lcd.setCursor(0,0);
    lcd.print("Altitude:");

    if( altMode == MODE_QFE ) lcd.print(" QFE");
    else if( altMode == MODE_QNH ) lcd.print( "QNH");
    else lcd.print(" QNE");

    Serial.println( baroReading );

    lcd.setCursor(0,1);
    //lcd.print( get_height( pressure ) );
    lcd.print( get_height( baroReading*100 ));
    lcd.print(" meters");
    //pressure -= dp;
    
    oldTime = curTime;
  }
}  

/*
 Having these as globals means persistant data!!!
 when select is pressed in display mode.
*/
char cQFE[] = "1013.25";
char cQNH[] = "1013.25";

/*
 Usining the input keypad set the current base pressure for QNH and QNE
 A big mess of input data!
 */
float setBase( unsigned int mode )
{
  unsigned int length = 6;
  byte adcButton;

  //current selected number
  int currentNum = 0;

  //current lodation in string
  unsigned int currentLoc = 0;
  lcd.cursor();

  switch( mode ){
  case MODE_QNH:
    currentLoc = 0;
    adcButton = BUTTON_NONE;
    lcd.clear();
    lcd.setCursor( 0, 0);
    lcd.print("Set QNH");
    //lcd.setCursor( currentLoc, 1 );   
    //Get input from keypad
    while( ((adcButton = readButtons()) != BUTTON_SELECT) )
    {
      //Serial.print( currentLoc);
      //Serial.print(  " : " );
      //Serial.println(adcButton );
      lcd.setCursor( 0, 1);
      lcd.print(cQNH);
      lcd.print(" mBar");
      lcd.setCursor( currentLoc, 1 );   
      // 0 = (char)30, 9 = (char)39
      if( adcButton == BUTTON_UP && buttonPressed )
      {
        cQNH[currentLoc] += 1;
        if( cQNH[currentLoc] > '9' ) cQNH[currentLoc] = '0';
        delay(150);
      }
      if(adcButton == BUTTON_DOWN && buttonPressed )
      {
        cQNH[currentLoc] -= 1;
        if( cQNH[currentLoc] < '0' ) cQNH[currentLoc] = '9';
        delay(150);
      }
      if(adcButton == BUTTON_RIGHT && buttonPressed )
      {
        currentLoc += 1;
        if( currentLoc >= length) currentLoc = length;
        if( cQNH[currentLoc] == '.') currentLoc++;
        delay(150);
      }
      if(adcButton == BUTTON_LEFT && buttonPressed )
      {
        currentLoc -= 1;
        if( currentLoc == length) currentLoc = 0;
        if( cQNH[currentLoc] == '.') currentLoc--;
        delay(150);
      }
      delay(50);
    } 
    lcd.noCursor();
    return String(cQNH).toFloat(); 
    break;
  case MODE_QFE:
    currentLoc = 0;
    lcd.clear();
    lcd.setCursor( 0, 0);
    lcd.print("Set QFE");
    //lcd.setCursor( currentLoc, 1 );   
    //Get input from keypad
    while( (adcButton = readButtons()) != BUTTON_SELECT )
    {
      lcd.setCursor( 0, 1);
      lcd.print(cQFE);
      lcd.print(" mBar");
      lcd.setCursor( currentLoc, 1 );
      // 0 = (char)30, 9 = (char)39
      if( adcButton == BUTTON_UP && buttonPressed )
      {
        cQFE[currentLoc] += 1;
        if(  cQFE[currentLoc] > '9' )  cQFE[currentLoc] = '0';
        delay(150);
      }
      if(adcButton == BUTTON_DOWN && buttonPressed )
      {
         cQFE[currentLoc] -= 1;
        if(  cQFE[currentLoc] < '0' )  cQFE[currentLoc] = '9';
        delay(150);
      }
      if(adcButton == BUTTON_RIGHT && buttonPressed )
      {
        currentLoc += 1;
        if( currentLoc >= length) currentLoc = length;
        if(  cQFE[currentLoc] == '.') currentLoc++;
        delay(150);
      }
      if(adcButton == BUTTON_LEFT && buttonPressed )
      {
        currentLoc -= 1;
        if( currentLoc == length) currentLoc = 0;
        if(  cQFE[currentLoc] == '.') currentLoc--;
        delay(150);
      }
      delay(50);
    } 
    lcd.noCursor();
    return  String(cQFE).toFloat();
    break;
  default:
    break; 
  }
}  

/*
 Select the mode in which the altimeter is to run in
*/
unsigned int setMode( )
{
  byte adcButton;
  unsigned int selectedMode = MODE_QNE;
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Mode: ");
  lcd.setCursor(0,1);
  while( (adcButton = readButtons()) != BUTTON_SELECT )
  {

    printSelectedMode(selectedMode, 0, 1);

    if( adcButton == BUTTON_UP && buttonPressed )
    {
      switch(selectedMode)
      {
      case MODE_QNE: 
        selectedMode = MODE_QNH; 
        break;
      case MODE_QNH: 
        selectedMode = MODE_QFE; 
        break;
      case MODE_QFE: 
        selectedMode = MODE_QNE; 
        break;
      }
      delay(150);
    }
    if(adcButton == BUTTON_DOWN && buttonPressed )
    {
      switch( selectedMode )
      {
      case MODE_QFE: 
        selectedMode = MODE_QNH; 
        break;
      case MODE_QNH: 
        selectedMode = MODE_QNE; 
        break;
      case MODE_QNE: 
        selectedMode = MODE_QFE; 
        break;
      }
      delay(150);
    }
    delay(50);
  } 
  return selectedMode;
}

/*
 Print out the currnetly selected altimeter mode.
*/
void printSelectedMode( unsigned int mode, int col, int row)
{
  lcd.setCursor(col,row);
  switch(mode){
  case MODE_QNE: 
    lcd.print("QNE"); 
    break;
  case MODE_QFE: 
    lcd.print("QFE"); 
    break;
  case MODE_QNH: 
    lcd.print("QNH"); 
    break;
  }
} 

/*
 Setup QNF, QNH and display mode.
 */
void altimeter_setup(void)
{

  qfe = setBase(MODE_QFE);
  delay(250);
  qnh = setBase(MODE_QNH);
  delay(250);
  altMode = setMode();
  delay(250);
}

/*
 depending om mode display the height
*/
float get_height( float mBar)
{
  if( altMode == MODE_QNE )
  {
    return calc_height( mBar, qne );
  }
  else if( altMode == MODE_QNH )
  {
    return calc_height( mBar, qnh );
  }
  return calc_height( mBar, qfe );
} 

/*
 The equation for this is found in 
 "Elements of airplane performance, G.J.J Ruijgrok"
 pg-
*/
float calc_height( float mbar, float reference)
{
  float result = 0.0f;
  float pressure_ratio = ((float)mbar / MBAR_NM2(reference)) ;
  result = (pow( pressure_ratio, -PR_EXP) - 1) * ( T_SEA_LEVEL / TEMP_GRAD );
  return result;
}

/*
 Code borrowed from the freetronics website.
*/
byte readButtons(){
  unsigned int buttonVoltage;
  byte curButton = BUTTON_NONE;

  buttonVoltage = analogRead( BUTTON_ADC_PIN );

  //sense if the voltage falls within valid voltage windows
  if( buttonVoltage < ( RIGHT_ADC + HYSTERESIS ) )
  {
    curButton = BUTTON_RIGHT;
  }
  else if(   buttonVoltage >= ( UP_ADC - HYSTERESIS )
    && buttonVoltage <= ( UP_ADC + HYSTERESIS ) )
  {
    curButton = BUTTON_UP;
  }
  else if(   buttonVoltage >= ( DOWN_ADC - HYSTERESIS )
    && buttonVoltage <= ( DOWN_ADC + HYSTERESIS ) )
  {
    curButton = BUTTON_DOWN;
  }
  else if(   buttonVoltage >= ( LEFT_ADC - HYSTERESIS )
    && buttonVoltage <= ( LEFT_ADC + HYSTERESIS ) )
  {
    curButton = BUTTON_LEFT;
  }
  else if(   buttonVoltage >= ( SELECT_ADC - HYSTERESIS )
    && buttonVoltage <= ( SELECT_ADC + HYSTERESIS ) )
  {
    curButton = BUTTON_SELECT;
  }
  //handle button flags for just pressed and just released events
  if( ( buttonWas == BUTTON_NONE ) && ( curButton != BUTTON_NONE ) )
  {
    //the button was just pressed, set buttonJustPressed, this can optionally be used to trigger a once-off action for a button press event
    //it's the duty of the receiver to clear these flags if it wants to detect a new button change event
    buttonPressed  = true;
    buttonReleased = false;
  }
  if( ( buttonWas != BUTTON_NONE ) && ( curButton == BUTTON_NONE ) )
  {
    buttonPressed  = false;
    buttonReleased = true;
  }

  //save the latest button value, for change event detection next time round
  buttonWas = curButton;

  return( curButton );
}





