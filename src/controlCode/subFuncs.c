// "subFuncs.h"
#ifndef MATH
#define MATH
#include <math.h>
#endif

#include "controlSys.h"

// slope relating changes in zoom to changes in phi1
// and the max phi1 (phi1 @ zoom = 0):
#define Z_SLOPE_PAN -.001559
#define Z_MAX_PAN 23.807 //
// analagously, slope relating changes in zoom to changes in phi1
// and the max phi1 (phi1 @ zoom = 0):
#define Z_SLOPE_TILT -.001239
#define Z_MAX_TILT 18.882//
// ^ I will put the MATLAB code on git (maybe), contains my data for these vals


double * phiDetermine(double zoom)
{
  // allocate memory to hold phi, phi2 (only allocate once)
  static double * phip;
  static short int bool = 1;
  if(bool)
    {
      bool--;
      phip =  (double *)calloc(2, sizeof(double));
    }//
  
  // figure out phi1 and phi2
  double phi1, phi2;
  phi1 = Z_MAX_PAN - zoom*Z_SLOPE_PAN;
  phi2 = Z_MAX_TILT - zoom*Z_SLOPE_TILT;
  *phip = phi1;
  *(phip + 1) = phi2;//
  return phip;
}


// this function compiles but doesn't do what it's supposed to do
double zDetermine(double phiX, char wat)
{
  double zoom;

  if(wat == '1') //if phiX = phi1
    {
      // COMPUTE phi1 -> Z conversion
      zoom = 0;
    }  
  else //if phiX = phi2
    {
      // COMPUTE phi2 -> Z conversion
      zoom = 0;
    }

    return zoom;
}


double inline ttlUpdate(double ttl, double timeSinceLast, short int isUp)
{
  if(isUp == 1)
    {
      if(ttl < MAX_TTL)
	ttl = ttl + timeSinceLast;
    }
  else
    {
      ttl = ttl - timeSinceLast;
      if(ttl < 0)
	ttl = 0;
    }
  
  return ttl;
}


double * distanceFind(double pan, double tilt, double zoom)
{ // given a zoom, returns pointers to the length in meters of x, z
  // if this doesn't make sense, refer to the CDR trig slide //
  
  // allocate memory for two doubles once (length of x and z)
  static double * dp;
  static short int bool = 1;
  if(bool)
    {
      bool--; 
      dp = (double *)calloc(2, sizeof(double));
    }//

  double rL, rR; // distance from cam to leftmost, rightmost parts of frame //
  double a, c; // distance of left and rightmost pixels from center (P, T) pos'n //
  double phi, phi2, *phip; // zoom angle with repsect to x and z, and pointer //

  // find phi as a function of Z
  phip = phiDetermine(zoom);
  phi = *phip;
  phi2 = *(phip + 1); // done with (double *)phip //
  
  // let's find the distance on the x part of the frame:
  rL = DIST/(sin(pan + phi) * (180/PI)); // the 180/PI changes rad -> degrees 
  rR = DIST/(sin(pan - phi) * (180/PI));
  a = sqrt(pow(rL, 2) - pow(DIST, 2));
  c = sqrt(pow(rR, 2) - pow(DIST, 2));
  *dp = abs(c - a); //
  
  // now the same for z part:
  rL = DIST/(sin(tilt + phi2) * (180/PI));
  rR = DIST/(sin(tilt - phi2) * (180/PI));
  a = sqrt(pow(rL, 2) - pow(DIST, 2));
  c = sqrt(pow(rR, 2) - pow(DIST, 2));
  *(dp + 1) = abs(c - a); //
  
  return dp;
}

double returnHigherValue(double d1, double d2)
{
  if(d1 > d2)
    return d1;
  else
    return d2;
}


double returnLowerValue(double d1, double d2)
{
  if(d1 < d2)
    return d1;
  else
    return d2;
}


// given (P, T, Z) returns the associated values in an array:
// {maxPan, minPan, maxTilt, minTilt} (essentially spherical coords)
double * minMaxPT(double P, double T, double Z)
{
  double * phi;
  double * phi2;
  
  //dArr[0] = maxPan, [1] = minPan, [2] = maxTilt, [3] = minTilt//
  static double * dArr;
  static short int bool = 1;
  if(bool)
    {
      bool--;
      dArr = (double *)calloc(4, sizeof(double));
    }
  
  phi = phiDetermine(Z);
  phi2 = phi + 1;

  *dArr = P + *phi;
  *(dArr+1) = P - *phi;
  *(dArr+2) = T + *phi2;
  *(dArr+3) = T - *phi2;

  return dArr;
}// 128



