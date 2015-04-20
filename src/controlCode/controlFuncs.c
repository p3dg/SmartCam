/* "controlFuncs.h"
   This is contains all the functions defined in main
   the subfunctions these functions call are in "subFuncs.h" */

#include "controlSys.h" // includes "#include <math.h> and subfunction definitions

#ifndef MATH
#define MATH
#include <math.h>
#endif

// distance in meters from cam->wall
#ifndef DIST
#define DIST 100
#endif//

// number of pixels in x, z directions
#ifndef PIXX
#define PIXX 620 
#define PIXZ 480
#endif//

// PI
#ifndef PI
#define PI 3.141592
#endif//

void findSpeaker(void); //
// ^ moves camera around slowly while continually calling getSpeakerPosn
// returns only when getSpeakerPosn returns 1, else keeps going forever

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


void findSpeaker(void)
{
  puts("THIS FUNCTION NOT WRITTEN YET");
}


// given a room, find the best (smallest) camera posn
// this posn accounts for all hot blackboards
// also, presenter + relevant buffers
struct Position updateCamPosn(struct Room * roomp)
{
  // first must find the min, max values of both P and Z
  
  struct Position posn;
  double z1, z2;

  // initialize to absurd values
  double minP = 1000;
  double maxP = -1000;
  double minT = 1000;
  double maxT = -1000;//

  // let's do the blackboards first:
  int i;
  for(i = 0; i < 3; i++)
    {
      if((*roomp).boards[i].isActive)
	{
	  minP = returnLowerValue((*roomp).boards[i].cornerBL.pan, minP);
	  maxP = returnHigherValue((*roomp).boards[i].cornerBR.pan, maxP);
	  minT = returnLowerValue((*roomp).boards[i].cornerBR.tilt, minT);
	  maxT = returnHigherValue((*roomp).boards[i].cornerTL.tilt, maxT);
	}
    }//

  // now, the presenter should be in the mix, too
  minP = returnLowerValue(minP, 
			  (*roomp).presenter.coord.pan - (*roomp).control.panBuff);
  
  maxP = returnHigherValue(maxP,
			   (*roomp).presenter.coord.pan + (*roomp).control.panBuff);
  
  minT = returnLowerValue(minT,
			  (*roomp).presenter.coord.tilt - (*roomp).control.negTiltBuff);
  
  maxT = returnHigherValue(maxT,
			   (*roomp).presenter.coord.tilt + (*roomp).control.posTiltBuff);

  // at this point, we have the max, min values for both pan and tilt
  // but we need to calculate what frame will contain these values

  // average minP, maxP; also average minT, maxT to find center (P, T) value for camera pos'n
  posn.pan = (minP + maxP)/2;
  posn.tilt = (minT + maxT)/2;
  // difference between middle and furthest points for pan and tilt are the angles phi1 and phi2
  z1 = zDetermine(posn.pan - minP, '1');
  z2 = zDetermine(posn.tilt - minT, '2');
  posn.zoom = returnLowerValue(z1, z2); // wider zoom should be used
  
  return posn;
} //47, updateCamPosn








struct Line coordTrans(struct GridCoord grid)
{
  double phi, phi2, *phip;
  double xDist, zDist, *distp;
  double theta, theta2; // angle added to pan value

  struct Line presenter;

  // let middle of frame be (0,0) as opposed to (PIXX/2, PIXZ/2)
  grid.x = (int)grid.x - (PIXX/2);
  grid.z = (int)grid.z - (PIXZ/2);//
  
  // find phi, phi2 based on Z
  phip = phiDetermine(grid.posn.zoom);
  phi = *phip;
  phi2 = *(phip+1);//

  // find the lengths (m) of the frame in x and z directions
  distp = distanceFind(grid.posn.pan, grid.posn.tilt, grid.posn.zoom);
  xDist = *distp;
  zDist = *(distp + 1);//
  
  // find the angles the presenter is at
  // (e.g., actual pan = pan + theta)
  theta = atan(xDist/DIST);
  theta2 = atan(zDist/DIST);//

  // convert to degrees (from radians)
  theta *= 180/PI;
  theta2 *= 180/PI;//
  
  presenter.pan = grid.posn.pan + theta;
  presenter.tilt = grid.posn.tilt + theta2;

  return presenter;
}






// checks for presenter/blackboard overlaps for each blackboard
// updates all ttl values accordingly
// updates all isActive ("hot" boolean) int values based off of ttl
void updateRoom(struct Line presPosn, struct Room * roomp, double timeSinceLast)
{
  short register int i;

  // update blackboards:
  for(i = 0; i < 3; i++)
    {
      if((*roomp).presenter.coord.pan <  (*roomp).boards[i].cornerBR.pan
	 && (*roomp).presenter.coord.pan > (*roomp).boards[i].cornerBL.pan)
	{
	  //if the blackboard overlaps w/ presenter, increment ttl
	  (*roomp).boards[i].ttl = ttlUpdate((*roomp).boards[i].ttl, timeSinceLast, 1);
	}
      else
	{
	  //if the blackboard isn't overlapping w/ presenter, decrement ttl
	  (*roomp).boards[i].ttl = ttlUpdate((*roomp).boards[i].ttl, timeSinceLast, 0);
	}

      // decide if each board is "hot" or not, update. 
      // ("hot" = "should be included in the frame")
      if((*roomp).boards[i].ttl > MIN_TTL) // a bb is hot if the ttl > TTL_MIN
	(*roomp).boards[i].isActive = 1;
      else
	(*roomp).boards[i].isActive = 0;
    }

  //update speaker:
  (*roomp).presenter.coord.pan = presPosn.pan;
  (*roomp).presenter.coord.tilt = presPosn.tilt;

  return;
}



