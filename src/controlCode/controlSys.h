// "controlSys.h"
// Kevin Lovely

#ifndef MATH
#define MATH
#include <math.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

// distance in meters from cam->wall
#ifndef DIST
#define DIST 100
#endif//

// number of pixels in x, z directions
#ifndef PIXX
#define PIXX 620 
#define PIXZ 480
#endif//

// PI (math.h?)
#ifndef PI
#define PI 3.141592
#endif//

// min, max TTL values (seconds)
#ifndef MIN_TTL
#define MIN_TTL 2
#define MAX_TTL 25
#endif//

struct Line // collection of points at a certain (Pan,Tilt) (center)
{
  double pan;
  double tilt;
};//

struct Position // complete camera position (may be passed to camera)
{
  double pan;
  double tilt;
  double zoom;
};//

struct GridCoord // used for grid coordinates
{
  int x;
  int z; 
  struct Position posn;
};//

struct BlackBoard
{
  struct Line cornerTR; // top right corner
  struct Line cornerTL; // top left corner
  struct Line cornerBL; // bottom left corner
  struct Line cornerBR; // bottom right corner
  short int isActive; // = 1 if blackboard is active
  double ttl; // "time to live" (seconds)
};

struct Person
{
  struct Line coord; // speaker's last known coordinates in (P, T)
  short int isPresenting; // not used
};

struct Control
{
  // room dimensions in (P, Z)
  struct Line cornerTR;
  struct Line cornerTL;
  struct Line cornerBL;
  struct Line cornerBR;
  // camera buffer distances for speaker
  double panBuff;
  double posTiltBuff;
  double negTiltBuff;
};

struct Room
{
  struct BlackBoard boards[3];
  struct Person presenter;
  struct Control control;
};



// FUNCTION PROTOTYPES:
//------loadInfo.h:---------------------------------------------------------

struct BlackBoard * getBoardCoords(void);
//returns pointer to struct Blackboard w/ appropriate info
//load from files "bbL.txt", et al

struct Control * loadControlInfo(void);
//returns a pointer to a struct Control

//------subFuncs.c:---------------------------------------------------------

double * phiDetermine(double);
// given Z returns pointer to phi and phi2 (phi2 is in pointer+1)

double * minMaxPT(double, double, double);
// returns {maxPan, minPan, maxTilt, minTilt}

double * distanceFind(double, double, double);
// given a zoom, returns pointers to the length in meters of x, z
// (length of frame)

double returnLowerValue(double, double);
// returns the lower value of the two values

double returnHigherValue(double, double);
// returns the greater value of the two values

//------controlFuncs.c:-----------------------------------------------------

void findSpeaker(void);
// prints error message explaining that function isn't complete

struct Line coordTrans(struct GridCoord);
// returns the (P, T) coord of a gridCoord
// i.e, given (P, T, Z) of cam and (x, y) posn of object in frame 
// returns (P, T) or object in frame 

void updateRoom(struct Line, struct Room *, double);
// given a * to room, the speakers (P, T) coords, and the time since last call,
// updates all that room.BlackBoard.ttl's and all room.BlackBoard.isActive bools
// also updates room's presenter w/ latest coords

struct Position updateCamPosn(struct Room *);
//^calculate the best camera pos'n for all "hot" objects
// return that position
