/*
 * Common.h
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

#define KP361 7825
#define KP367 16

//LC = Logic Control
struct LOGIC_CONTROL{
  //LC = Logic Control
   uchar_t LC18:1;
   uchar_t LC20:1;
   uchar_t LC21:1;
   uchar_t LC24:1;
   uchar_t LC29:1;
   uchar_t LC17:1;
   uchar_t LC4A:1;    //Standby Initalize
   uchar_t LC4B:1;    //Ascent active
   uchar_t LC4C:1;  
   uchar_t LC4D:1;
   uchar_t LC4E:1;
   uchar_t LC4F:1;
   uchar_t LC30:1;
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

  int acc;      //Accumilator

  int CP164;
  int CP165;    // Delta b0 range, yaw?
  int CP135;
  int CP136;
  int CP137;
};

struct DATAIN{
  /*
   * Mode Control
   *  DI11 + DI10 = Standby
   *  DI11 + !DI10 + DI13 = Ascent
   *  DI11 + !DI10 + !DI13 = CatchUp
   *  !DI11 + DI13 = Rendezvous
   *  !DI11 + !DI13 = ReEntry
  */
  int DI10;     // Mode Control
  int DI11;     // Mode Control
  int DI13;     // Mode Control
  
};

struct DATAOUT{
  //Data Out objects
  int DO02;   // Pitch resolution control? 
  int DO03;   // Yaw resolution control?
  int DO04;
  int DO44;   // Yaw ladder buffer
  int DO61;   // Gain Change
  int DO62:1; // start computations light
  int DO05:1; // Computer running ligh
  int DO64:1; // SECO light
};

struct DATAOUT dout;
struct DATAIN  din;
struct LOGIC_CONTROL logicContol;
struct GIMBAL gimbal;
struct ACCELERATION accel;
struct VALUES values;

