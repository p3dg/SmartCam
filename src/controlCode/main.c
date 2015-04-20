// "main.c"

#include "controlSys.h"
#include "hiredis/hiredis.h" ALL REDIS STUFF COMMENTED OUT


int main(int argc, char * argv[])
{
  short int bool = 0; // bool = 0 means (x, y) outdated on redis

  // used for ttl manipulation and decision making via clock()
  static double time = 0;
  static double prevTime = 0; //

  // some structures used to hold data conveniently
  struct GridCoord grid; //struct GridCoord holds int x, y and posn //
  struct Position camPosn; // Position has members .pan, .tilt, .zoom //
  struct Line posn; // Lines have members .pan, .tilt ////

  // create a "struct Room" and load its control varibles
  struct Room room;
  struct Room * roomp = &room;
  struct BlackBoard * boardp;
  boardp = getBoardCoords();
  room.boards[0] = *boardp;
  room.boards[1] = *(boardp + 1);
  room.boards[2] = *(boardp + 2);
  room.control = *loadControlInfo();//

  // create a connection to the Redis server
  redisContext * c;
  redisReply * reply;
  c = redisConnect("127.0.0.1", 6379);
  if (c->err)
    {
      puts("Error Connecting to Redis Server: %s\n");
      redisFree(c);
      return 1;
    }//
  else
    puts("\nConnected to Redis\n");
 
  // initialize the camera values to something simple:
  // push values onto Redis server, execute command to move cam, set expiration time
  char buf[256]; // holds string sent to redis server
  snprintf(buf, 256, "MSET visca:camera:pan %i visca:camera:tilt %i visca:camera:zoom %i",0,0,0);
  redisCommand(c, buf);
  redisCommand(c, "lpush visca:command m");
  redisCommand(c, "expire visca:command 2"); //
  
  // this loop runs continuously:
  // 1) pulls values from the redis server, places in (struct GridCoord)grid
  // 2) converts from grid to (P, T)
  // 3) updates ttl's on blackboards, finds best camera (P, T, Z) frame
  // 4) pushes new (P, T, Z) values to redis server, execute move command
  // 5) update grid with current camera position
  clock();
  puts("Setup complete");
  while(1)
    {
      // 1) pull values from redis server, place in grid:
      reply = redisCommand(c, "MGET vision:x vision:y vision:bool");
      if (reply->type == REDIS_REPLY_ARRAY) 
	{
	  grid.x = atoi(reply->element[0]->str);
	  grid.z = atoi(reply->element[1]->str);
	  //bool = atoi(reply->element[2]->str);
	}//
      
      bool = 1;
      // if valid x, y exists on redis server
      if(bool)
        {
          // 2) converts from grid to (P, T)
          posn = coordTrans(grid); // 
	  
	  // 3) updates ttl's on blackboards, finds best camera (P, T, Z) frame
          prevTime = time;
          time = clock()/CLOCKS_PER_SEC; // time = seconds passed since updateRoom() last called
          updateRoom(posn, roomp, time - prevTime); // update room with latest speaker pos'n, etc.
          camPosn = updateCamPosn(roomp); // calculate ideal camera (P, T, Z) //
          
	  // 4) pushes new (P, T, Z) values to redis server, execute move command
          // push values onto Redis server:
          snprintf(buf, 256, "MSET visca:camera:pan %i visca:camera:tilt %i visca:camera:zoom %i",
                   (int)camPosn.pan, (int)camPosn.tilt, (int)camPosn.zoom); //
          // execute the redis Command to move the camera
          redisCommand(c, buf);
          redisCommand(c, "lpush visca:command m");
          redisCommand(c, "expire visca:command 5");//
          
          // 5) update grid with current camera position
          grid.posn.pan = camPosn.pan;
          grid.posn.tilt = camPosn.tilt;
          grid.posn.zoom = camPosn.zoom; //
        } // 70,if
      
      else //if redis server has invalid x y values (line 74)
        {
          /* zoom all the way out:
	  snprintf(buf, 256, "MSET visca:camera:pan 0 visca:camera:tilt 0 visca:camera:zoom 0");
          redisCommand(c, buf);
          redisCommand(c, "lpush visca:command m");
          redisCommand(c, "expire visca:command 5"); */
        }

} //while, 57

}//7,main


