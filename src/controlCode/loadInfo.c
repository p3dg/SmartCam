// "loadInfo.c"
// uses "bbL.txt", "bbM.txt", "bbR.txt", "controlInfo.txt"

#include "controlSys.h"



/* getBoardCoords() collects all the info on the boards, 
   returns a pointer to a block of memory w/ 3 blackboards
     this is done by reading 3 files named "bbX.txt" X in {L, M, R}
     the fscanf part is ugly but makes sense 
(if you open one of the files, you can see for yourself)*/
struct BlackBoard * getBoardCoords(void)
{
 
  FILE * bbL; // left blackboard file
  FILE * bbM; // middle blackboard file
  FILE * bbR; // right blackboard file
 
  struct BlackBoard * boards;
  size_t boardSize = sizeof(struct BlackBoard);
  boards = (struct BlackBoard *)calloc(3, boardSize);

 // COLLECT LEFT BOARD INFO
  bbL = fopen("bbL.txt", "r");
  if(bbL == NULL)
    {
      puts("error: left blackboard info not found!!!");
    }
  else
    {
      fscanf(bbL, "%*[^:]%*c%lf%*[^:]%*c%lf%*[^:]%*c%lf%*[^:]%*c%lf%*[^:]%*c%lf%*[^:]%*c%lf%*[^:]%*c%lf%*[^:]%*c%lf",
	     &boards[0].cornerTR.pan, &boards[0].cornerTR.tilt,
	     &boards[0].cornerTL.pan, &boards[0].cornerTL.tilt,
	     &boards[0].cornerBL.pan, &boards[0].cornerBL.tilt,
	     &boards[0].cornerBR.pan, &boards[0].cornerBR.tilt);
    }
  fclose(bbL);
  
  // COLLECT MIDDLE BOARD INFO
  bbM = fopen("bbM.txt", "r");
  if(bbM == NULL)
    {
      puts("error: middle blackboard info not found!!!");
    }
  else
    {
      fscanf(bbM, "%*[^:]%*c%lf%*[^:]%*c%lf%*[^:]%*c%lf%*[^:]%*c%lf%*[^:]%*c%lf%*[^:]%*c%lf%*[^:]%*c%lf%*[^:]%*c%lf",
	     &boards[1].cornerTR.pan, &boards[1].cornerTR.tilt,
	     &boards[1].cornerTL.pan, &boards[1].cornerTL.tilt,
	     &boards[1].cornerBL.pan, &boards[1].cornerBL.tilt,
	     &boards[1].cornerBR.pan, &boards[1].cornerBR.tilt);
    }
  fclose(bbM);
  
  // COLLECT RIGHT BOARD INFO
  bbR = fopen("bbR.txt", "r");
  if(bbR == NULL)
    {
      puts("error: right blackboard info not found!!!");
    }
  else
    {
      fscanf(bbR, "%*[^:]%*c%lf%*[^:]%*c%lf%*[^:]%*c%lf%*[^:]%*c%lf%*[^:]%*c%lf%*[^:]%*c%lf%*[^:]%*c%lf%*[^:]%*c%lf",
	     &boards[2].cornerTR.pan, &boards[2].cornerTR.tilt,
	     &boards[2].cornerTL.pan, &boards[2].cornerTL.tilt,
	     &boards[2].cornerBL.pan, &boards[2].cornerBL.tilt,
	     &boards[2].cornerBR.pan, &boards[2].cornerBR.tilt);
    }
  fclose(bbR);
  
  return boards;
}// getBoardCoords()











struct Control * loadControlInfo(void)
{
  struct Control control;
  struct Control * controlp = &control;
  /* similarly, this loads some info from a file
     file is named "controlInfo.txt" */
     
  FILE * fp;
  fp = fopen("controlInfo.txt", "r");

  fscanf(fp, "%*[^:]%*c%lf%*[^:]%*c%lf%*[^:]%*c%lf%*[^:]%*c%lf%*[^:]%*c%lf%*[^:]%*c%lf%*[^:]%*c%lf%*[^:]%*c%lf", 
	 &control.cornerTR.pan, &control.cornerTR.tilt, 
	 &control.cornerTL.pan, &control.cornerTL.tilt,
	 &control.cornerBL.pan, &control.cornerBL.tilt,
	 &control.cornerBR.pan, &control.cornerBR.tilt);


  fscanf(fp, "%*[^:]%*c%lf%*[^:]%*c%lf%*[^:]%*c%lf", 
	 &control.panBuff, &control.negTiltBuff, &control.posTiltBuff);

  return controlp;
} //loadControlInfo()


