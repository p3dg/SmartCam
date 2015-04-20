/* Movement Logic Code
 * 
 * Author: Idan Warsawski, Peter Galvin
 */

#ifndef MOVEMENT_LOGIC_H
#define MOVEMENT_LOGIC_H

#include <stdlib.h>
#include <string.h>
#include "hiredis/hiredis.h"
#include "fsm_mover.h"
#include "cJSON.h"

#ifdef __cplusplus
extern "C" {
#endif

struct movement_cordinate
{
  int pan;
  int tilt;
  int zoom;
};

struct movement_logic
{
  struct fsm_mover * fsm;
  int frame_width;
  int frame_height;
  int x_trig_left;
  int x_trig_right;
  int time_needed;

  struct {
    int time_in_frame;
    int last_pos;
    redisContext * redis;
  } priv;
};

struct movement_logic * movement_logic_init(int frame_width, int frame_height, int time_needed, float trigger_percent);

void movement_logic_init_sample_fsm(struct movement_logic * ml);

void movement_logic_init_fsm(struct movement_logic * ml, char * redis_path);

void movement_logic_new_point(struct movement_logic * ml, int x, int y);

#ifdef __cplusplus
}
#endif

#endif
