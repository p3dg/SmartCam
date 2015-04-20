//testFuncs.h
//(some functions for testing functionality of other code)
#include "controlSys.h"
#include "loadInfo.c"

int main(int argc, char ** argv)
{
  // first let's test every function in loadInfo.c
  struct BlackBoard * bbp;
  bbp = getBoardCoords();

  struct Control cont;
  cont = *loadControlInfo();  

  struct Room room;
  room.control = cont;
  room.boards[0] = *(bbp);
  room.boards[1] = *(bbp + 1);
  room.boards[3] = *(bbp + 2);

  printBoardInfo(room.boards[0]);
}//main

// print out blackboard information 
void printBoardInfo(struct BlackBoard bb, int i)
{
  printf("board %d pan (TR): %f\n", i, bb.cornerTR.pan);
  printf("board %d tilt (TR): %f\n", i, bb.cornerTR.tilt);
  printf("board %d pan (TL): %f\n", i, bb.cornerTL.pan);
  printf("board %d tilt (TL): %f\n", i, bb.cornerTL.tilt);
  printf("board %d pan (BL): %f\n", i, bb.cornerBL.pan);
  printf("board %d tilt (BL): %f\n", i, bb.cornerBL.tilt);
  printf("board %d pan (BR): %f\n", i, bb.cornerBR.pan);
  printf("board %d tilt (BR): %f\n", i, bb.cornerBR.tilt);
}//printBoardInfo


