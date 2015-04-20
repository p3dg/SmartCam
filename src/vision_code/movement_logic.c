/* Movement Logic Code
 * 
 * Author: Peter Galvin
 */

#include "movement_logic.h"
#include "cJSON.h"

#define ML_LAST_POS_LEFT (-1)
#define ML_LAST_POS_CENTER 0
#define ML_LAST_POS_RIGHT 1

/*
static struct movement_cordinate sample_cordinates[] = {
  {.pan = -23, .tilt = 1, .zoom = 6691}, //left
  {.pan = -4, .tilt = 1, .zoom = 6691},  //middle
  {.pan = 19, .tilt = 1, .zoom = 6691}  //right
};*/


static struct movement_cordinate sample_cordinates[] = {
  {.pan = -20, .tilt = 1, .zoom = 6691}, //0 left
  {.pan = -8, .tilt = 0, .zoom = 4583},  //1 left-middle
  {.pan = -3, .tilt = 0, .zoom = 4583},  //2 middle-left
  {.pan = 0, .tilt = 0, .zoom = 6691},   //3 middle
  {.pan = 3, .tilt = 0, .zoom = 4583},   //4 middle-right
  {.pan = 8, .tilt = 0, .zoom = 4583},   //5 right-middle
  {.pan = 20, .tilt = 1, .zoom = 6691}   //6 right
};


struct movement_logic * movement_logic_init(int frame_width, int frame_height, int time_needed, float trigger_percent)
{
  struct movement_logic * ml = (struct movement_logic *) malloc(sizeof(struct movement_logic));
  memset(ml, 0x00, sizeof(struct movement_logic));

  ml->frame_width = frame_width;
  ml->frame_height = frame_height;
  ml->time_needed = time_needed;

  //symetric trigger pixel borders
  ml->x_trig_left = frame_width * trigger_percent / 100.0;
  ml->x_trig_right = frame_width - ml->x_trig_left;

  ml->priv.redis = redisConnect("127.0.0.1", 6379);
  if (ml->priv.redis->err) 
    {
      printf("Error Connecting to Redis Server: %s\n", ml->priv.redis->errstr);
      redisFree(ml->priv.redis);
      exit(1); //assert, who cares?
    }


  ml->fsm = fsm_init_machine();
  return ml;
}

void movement_logic_init_fsm(struct movement_logic * ml, char * redis_path)
{
  redisReply * reply;
  cJSON * root;
  cJSON * states, * state_jumps;
  reply = redisCommand(ml->priv.redis, "GET %s", redis_path);
  if(reply->type != REDIS_REPLY_STRING)
    {
      fprintf(stderr, "ERROR: Cannot find key %s in redis db, init default config\n", redis_path);
      freeReplyObject(reply);
      movement_logic_init_sample_fsm(ml);
    }
  root = cJSON_Parse(reply->str);
  printf("Parsing Config %s\n", cJSON_GetObjectItem(root, "name")->valuestring);

  redisCommand(ml->priv.redis, "SET vision:movement:roomname %s", cJSON_GetObjectItem(root, "name")->valuestring);
  redisCommand(ml->priv.redis, "DEL vision:movement:states");

  states = cJSON_GetObjectItem(root, "states")->child;
  redisCommand(ml->priv.redis, "SET vision:movement:position %s", cJSON_GetObjectItem(states, "name")->valuestring);
  do {
    struct movement_cordinate * cordinates = malloc(sizeof(struct movement_cordinate));

    printf("State: %s %d %d %d\n", 
           cJSON_GetObjectItem(states, "name")->valuestring, 
           cJSON_GetObjectItem(states, "pan")->valueint, 
           cJSON_GetObjectItem(states, "tilt")->valueint,
           cJSON_GetObjectItem(states, "zoom")->valueint
           );
    redisCommand(ml->priv.redis, "LPUSH vision:movement:states %s", cJSON_GetObjectItem(states, "name")->valuestring);
    cordinates->pan = cJSON_GetObjectItem(states, "pan")->valueint;
    cordinates->tilt = cJSON_GetObjectItem(states, "tilt")->valueint;
    cordinates->zoom = cJSON_GetObjectItem(states, "zoom")->valueint;

    fsm_add_state(ml->fsm, cJSON_GetObjectItem(states, "name")->valuestring, (void *) cordinates);
  } while(states = states->next);

  state_jumps = cJSON_GetObjectItem(root, "state_jump")->child;
  do {
    cJSON * left, * right;
    left = cJSON_GetObjectItem(state_jumps, "left");
    right = cJSON_GetObjectItem(state_jumps, "right");

    printf("State_Jump: %s %s %s\n", 
           cJSON_GetObjectItem(state_jumps, "name")->valuestring, 
           (left->type == cJSON_NULL) ? "NULL" : left->valuestring,
           (right->type == cJSON_NULL) ? "NULL" : right->valuestring
           );
    fsm_set_state_jump(ml->fsm, 
                       cJSON_GetObjectItem(state_jumps, "name")->valuestring, 
                       (left->type == cJSON_NULL) ? "NULL" : left->valuestring,
                       (right->type == cJSON_NULL) ? "NULL" : right->valuestring
                       );
  } while(state_jumps = state_jumps->next);


  cJSON_Delete(root);
  freeReplyObject(reply);
}

void movement_logic_init_sample_fsm(struct movement_logic * ml)
{
  char buf[256];
  struct movement_cordinate * mc;
  redisReply * reply;
  fsm_add_state(ml->fsm, "Middle", (void *) &sample_cordinates[3]);
  fsm_add_state(ml->fsm, "Middle-Right", (void *) &sample_cordinates[4]);
  fsm_add_state(ml->fsm, "Middle-Left", (void *) &sample_cordinates[2]);

  fsm_add_state(ml->fsm, "Left", (void *) &sample_cordinates[0]);
  fsm_add_state(ml->fsm, "Left-Middle", (void *) &sample_cordinates[1]);
  fsm_add_state(ml->fsm, "Right", (void *) &sample_cordinates[6]);
  fsm_add_state(ml->fsm, "Right-Middle", (void *) &sample_cordinates[5]);

  fsm_set_state_jump(ml->fsm, "Left", NULL, "Left-Middle");
  fsm_set_state_jump(ml->fsm, "Left-Middle", "Left", "Middle");
  fsm_set_state_jump(ml->fsm, "Middle-Left", "Left", "Middle");
  fsm_set_state_jump(ml->fsm, "Middle", "Middle-Left", "Middle-Right");
  fsm_set_state_jump(ml->fsm, "Middle-Right", "Middle", "Right");
  fsm_set_state_jump(ml->fsm, "Right-Middle", "Middle", "Right");
  fsm_set_state_jump(ml->fsm, "Right", "Right-Middle", NULL);

  /*  fsm_set_state_jump(ml->fsm, "Left Blackboard", NULL, "Middle Blackboard");
  fsm_set_state_jump(ml->fsm, "Middle Blackboard", "Left Blackboard", "Right Blackboard");
  fsm_set_state_jump(ml->fsm, "Right Blackboard", "Middle Blackboard", NULL);
  */
  //reset to middle
  mc = fsm_get_current_state_data(ml->fsm);
  snprintf(buf, 256, "MSET visca:camera:pan %i "
           "visca:camera:tilt %i " 
           "visca:camera:zoom %i ",
           mc->pan, 
           mc->tilt, 
           mc->zoom);
  printf("%s\n", buf);
  reply = redisCommand(ml->priv.redis, buf);
  if (reply->type == REDIS_REPLY_ERROR) printf("ERR1 %s\n", reply->str);
  freeReplyObject(reply);

  reply = redisCommand(ml->priv.redis, "SET vision:movement:position %s", fsm_get_current_state_name(ml->fsm));
  if (reply->type == REDIS_REPLY_ERROR) printf("ERR2 %s\n", reply->str);
  freeReplyObject(reply);
  
  reply = redisCommand(ml->priv.redis, "lpush visca:command m");
  if (reply->type == REDIS_REPLY_ERROR) printf("ERR3 %s\n", reply->str);
  freeReplyObject(reply);
  
  reply = redisCommand(ml->priv.redis, "expire visca:command 5");
  if (reply->type == REDIS_REPLY_ERROR) printf("ERR4 %s\n", reply->str);
  freeReplyObject(reply);
  
  reply = redisCommand(ml->priv.redis, "lpush visca:command z");
  if (reply->type == REDIS_REPLY_ERROR) printf("ERR5 %s\n", reply->str);
  freeReplyObject(reply);
  
  reply = redisCommand(ml->priv.redis, "expire visca:command 5");
  if (reply->type == REDIS_REPLY_ERROR) printf("ERR6 %s\n", reply->str);
  freeReplyObject(reply);
}

void movement_logic_new_point(struct movement_logic * ml, int x, int y)
{
  redisReply * reply; 
  int is_moving, manual_control, new_state;
  int ret;
  struct movement_cordinate * mc;
  char buf[256];

  //check if the camera is moving, we don't want to process position
  //if we're moving
  reply = redisCommand(ml->priv.redis, "MGET visca:camera:moving vision:movement:controller vision:movement:position");
  if (reply->type == REDIS_REPLY_ARRAY) {
    if(!strcmp(reply->element[0]->str, "true"))
      is_moving = 1;
    else
      is_moving = 0;

    if(strcmp(reply->element[1]->str, "automatic"))
      manual_control = 1;
    else
      manual_control = 0;

    if(strcmp(fsm_get_current_state_name(ml->fsm), reply->element[2]->str))
      {
        new_state = 1;
        strncpy(buf, reply->element[2]->str, 256);
      }
    else
      new_state = 0;
  }
  freeReplyObject(reply);

  if(is_moving)
    return;


  if(new_state)
    {
      fsm_set_current_state(ml->fsm, buf);
      goto update_state_data;
    }

  if(manual_control)
    return;

  if(x <= ml->x_trig_left)
    {
      if(ml->priv.last_pos == ML_LAST_POS_LEFT)
        ml->priv.time_in_frame++;
      else
        {
          ml->priv.last_pos =  ML_LAST_POS_LEFT;
          ml->priv.time_in_frame = 0;
        }
      printf("LEFT %d %d\n", x, ml->priv.time_in_frame);
    }
  else if(x >= ml->x_trig_right)
    {
      if(ml->priv.last_pos == ML_LAST_POS_RIGHT)
        ml->priv.time_in_frame++;
      else
        {
          ml->priv.last_pos = ML_LAST_POS_RIGHT;
          ml->priv.time_in_frame = 0;
        }
      printf("RIGHT %d %d\n", x, ml->priv.time_in_frame);
    }
  else
    {
      ml->priv.last_pos = ML_LAST_POS_CENTER;
      ml->priv.time_in_frame = 0;
    }
  
  if(ml->priv.time_in_frame >= ml->time_needed)
    {
      
      ml->priv.time_in_frame = 0;
      
      switch(ml->priv.last_pos)
        {
        case ML_LAST_POS_RIGHT:
          ret = fsm_move_right(ml->fsm);
          printf("MOVING RIGHT\n");
          break;
        case ML_LAST_POS_LEFT:
          ret = fsm_move_left(ml->fsm);
          printf("MOVING LEFT\n");
          break;
        default:
          ret = -1;
        }

    update_state_data:
      //-1 is the new state does not exist
      //TODO: unbounded movement?
      if(ret == -1) 
        return;

      mc = fsm_get_current_state_data(ml->fsm);
      snprintf(buf, 256, "MSET visca:camera:pan %i "
               "visca:camera:tilt %i " 
               "visca:camera:zoom %i ",
               mc->pan, 
               mc->tilt, 
               mc->zoom);
      reply = redisCommand(ml->priv.redis, buf);
      if (reply->type == REDIS_REPLY_ERROR) printf("ERR1 %s\n", reply->str);
      freeReplyObject(reply);

      reply = redisCommand(ml->priv.redis, "SET vision:movement:position %s", fsm_get_current_state_name(ml->fsm));
      if (reply->type == REDIS_REPLY_ERROR) printf("ERR2 %s\n", reply->str);
      freeReplyObject(reply);
  
      reply = redisCommand(ml->priv.redis, "lpush visca:command m");
      if (reply->type == REDIS_REPLY_ERROR) printf("ERR3 %s\n", reply->str);
      freeReplyObject(reply);
  
      reply = redisCommand(ml->priv.redis, "expire visca:command 5");
      if (reply->type == REDIS_REPLY_ERROR) printf("ERR4 %s\n", reply->str);
      freeReplyObject(reply);
  
      reply = redisCommand(ml->priv.redis, "lpush visca:command z");
      if (reply->type == REDIS_REPLY_ERROR) printf("ERR5 %s\n", reply->str);
      freeReplyObject(reply);
  
      reply = redisCommand(ml->priv.redis, "expire visca:command 5");
      if (reply->type == REDIS_REPLY_ERROR) printf("ERR6 %s\n", reply->str);
      freeReplyObject(reply);
    }
}
