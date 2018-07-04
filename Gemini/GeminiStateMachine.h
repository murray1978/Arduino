/*
 * The flow diagrams for Matghflow 7 are a state machine.
 * Trying to keep the flow as close as possible to the diagrams.
*/
#pragma once

#define uchar_t unsigned char
#define uint_t  unsigned int
#define TRUE 1
#define FALSE !TRUE

#define KWNO  5.2593  // B4
#define KP220 175     // B10 sec
#define KVGO  750     // B15 ft/sec
#define KVAO  150     // B15 ft/sec
#define KIFO  250     // B10 ft/sec
#define KQO   1.66    // B4
#define KP12  100     // B17 sec
#define KP104 PI/2    // B3 ad
#define KXAO  0       // B25 ft
#define KZA0  0       // B25 ft
#define KYA0  -20909749  // B25 ft
#define KP226 0.5     // B1
#define KP336 0.93969 // B1
#define KTST2 156.1   // B17 sec 

struct STATES{
   uchar_t LC18:1;
   uchar_t LC20:1;
   uchar_t LC21:1;
   uchar_t LC24:1;
   uchar_t LC29:1;
   uchar_t LC17:1;
   uchar_t LC4A:1; // 
   uchar_t LC4B:1;
   uchar_t LC4C:1;  
   uchar_t LC4D:1;
   uchar_t LC4E:1;
   uchar_t DO61:1;  //
   uchar_t DO62:1; 
   uchar_t DO05:1;
   uchar_t DO64:1;
};

struct GIMBAL{
  int x;
  int y;
  int z;
  int dx;
  int dy;
  int dz;
};

struct ACCELERATION
{
  int x;
  int y;
  int z;
  int dx;
  int dy;
  int dz;
};

struct VALUES
{
  //59, pg17
  long Wno;     //kinematic value, time step?
  int Vg;       //Velocity loss due to gravity
  int af;       //Final value of thrust acceleration
  int qno;      //?
  long thetaN;  //gimbal angle
  int zni;       
  int cp150;
  int tr;       //time to start roll program
};

uint_t CP135, CP136, CP137;

struct STATES state;
struct GIMBAL gimbal;
struct ACCELERATION accel;
struct VLAUES values;

void ioa(){}
void io(){}

//94.4
void iosub(){}

void gonogo(){}
void gclock(){}

//29.1 gimbal angle
void gimbalAngle(){}
{
  //i2c read of gimbals
}

uchar_t age(){ return FALSE; }
uchar_t modeControlDI11(){ return FALSE; }
uchar_t modeControlDI10(){ return FALSE; }
uchar_t modeControlDI13(){ return FALSE; }

//And the big flight logic
void geminiFlightLogic()
{
  //Page 23, COL 78
  state.LC4A = TRUE;
  state.LC4B = TRUE;
  state.LC4C = TRUE;
  state.LC4D = TRUE;
  state.LC4E = TRUE;
EXECIN:                       //78.2
  CP135 = CP136 = CP137 = 0;
  state.DO62 = TRUE;
  state.DO05 = TRUE;
  state.DO64 = TRUE;
  state.DO61 = TRUE;
EXECTR:                       //78.3
  
  if( state.LC4B == TRUE){ //or LC48
    io();  
  }else{
    ioa();   //ascent falst loop
  }
  
  gonogo();
  gclock();
  
  if( age() == FALSE ) goto EXECTR;
  
  if( state.LC4B == TRUE){ //or LC48
    io();  
  }
  else
  {
    ioa();  //ascent falst loop
  }
  
  if( modeControlDI11() == TRUE ){    //78.5
    if( modeControlDI10() == TRUE ){
      goto STANDBY;                   //93.1
    }
    else                              //78.7
    {
      if( modeControlDI13() == TRUE ){
        goto ASCENT;                  //59.1
      }
      else
      {
        goto CATCHUP;                 //1.2
      }
    }
  }
  else                                //78.6
  {
    if( modeControlDI13() == TRUE )
    {
        goto RENDEZVOUS;              //1.1
    }
    else
    {
        goto REENTRY;                  //35.1 
    }
  }

STANDBY:                                //93.1
  //Standby startup
  if( state.LC4A  == TRUE )
  {
    state.LC4E = state.LC4B = state.LC4C = state.LC4D = TRUE;
    state.LC4A = FALSE;
    gimbal.dx = gimbal.dy = gimbal.dz = 0;
    state.DO05 = TRUE;
  }
  else
  {
    //Sanity Check of memory, uses a compiled time checksum? to check memory.
  }
  iosub();
  gimbalAngle();
  goto EXECTR;
  
ASCENT:                                    //59.1
  if( state.LC4B == TRUE )                 //First pass/initalise 
  {
    state.LC4A = state.LC4C = state.LC4D = state.LC4F = TRUE;
    state.LC18 = state.LC20 = state.LC21 = state.LC24 = state.LC29 = TRUE;
    state.LC4B = state.LC17 = FALSE;
  }
  goto EXECTR;
CATCHUP:
  goto EXECTR;
RENDEZVOUS:
  goto EXECTR;
REENTRY:
  goto EXECTR;
//Non Mathflow 7 
ORBIT_PROGRADE:
  goto EXECTR;
ORBIT_RETROGRADE:
  goto EXECTR;
ORBIT_NORMAL:
  goto EXECTR;
ORBIT:
  goto EXECTR;
}
