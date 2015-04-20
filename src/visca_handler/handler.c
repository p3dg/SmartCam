/*
 * Camera Control Handler via Redis interface
 *
 * Author: Peter Galvin
 */

#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include "libvisca.h"
#include "hiredis/hiredis.h"

struct visca_handler
{
  char * serial_path;

  VISCAInterface_t visca_iface;
  VISCACamera_t visca_camera;
  int camera_num;
};


void visca_close_serial(struct visca_handler * vh)
{
  VISCA_close_serial(&vh->visca_iface);
}

int visca_init(struct visca_handler * vh)
{
  if(VISCA_open_serial(&vh->visca_iface, vh->serial_path) != VISCA_SUCCESS)
    {
      fprintf(stderr, "Could not open serial port %s\n", vh->serial_path);
      return -1;
    }

  vh->visca_iface.broadcast = 0;
  if(VISCA_set_address(&vh->visca_iface, &vh->camera_num) != VISCA_SUCCESS)
    {
      fprintf(stderr, "Could not set address\n");
      visca_close_serial(vh);
      return -1;
    }

  vh->visca_camera.address = 1;

  if(VISCA_clear(&vh->visca_iface, &vh->visca_camera) != VISCA_SUCCESS)
    {
      fprintf(stderr, "Could not clear interface\n");
      visca_close_serial(vh);
      return -1;
    }

  if(VISCA_get_camera_info(&vh->visca_iface, &vh->visca_camera) != VISCA_SUCCESS) 
    {
      fprintf(stderr, "Could not get camera info\n");
      visca_close_serial(vh);
      return -1;
    }
}



int main(int argc, char ** argv)
{
  redisContext *c;
  redisReply * reply; 
  int quit = 0;
  struct visca_handler vh;
  char buf[256];


  c = redisConnect("127.0.0.1", 6379);
  if (c->err) {
    printf("Error Connecting to Redis Server: %s\n", c->errstr);
    redisFree(c);
    return 1;
  }
  

  vh.serial_path = argv[1];
  visca_init(&vh);
  
  fprintf(stderr, "Camera Initialized\n");

  while(!quit)
    {
      int target_pan, target_tilt, remain_pan, remain_tilt;
      int use_absolute;
      uint32_t zoom;
      char code;

      reply = redisCommand(c, "BLPOP visca:command 0");
      if (reply->type == REDIS_REPLY_ARRAY) {
        if (reply->elements >= 2) {
          code = reply->element[1]->str[0];
        }
      }
      freeReplyObject(reply);

      reply = redisCommand(c, "MGET visca:camera:pan visca:camera:tilt");
      if (reply->type == REDIS_REPLY_ARRAY) {
        target_pan = atoi(reply->element[0]->str);
        target_tilt = atoi(reply->element[1]->str);
      }
      
      freeReplyObject(reply);

      switch(code)
        {
        case 'r': 
          redisCommand(c, "SET visca:camera:moving true");
          VISCA_set_pantilt_reset(&vh.visca_iface, &vh.visca_camera);
          redisCommand(c, "SET visca:camera:moving false");
          break;
          
        case 'h':
          redisCommand(c, "SET visca:camera:moving true");
          VISCA_set_pantilt_home(&vh.visca_iface, &vh.visca_camera);
          redisCommand(c, "SET visca:camera:moving false");
          break;
          
          /*a 880 330
            g
            Pan 65.729350, Tilt 24.800937
            a -879 -329            
            g
            Pan -65.584633, Tilt -24.585480*/
        case 'm':
          redisCommand(c, "SET visca:camera:moving true");
          if(target_pan > 170) target_pan = 170;
          if(target_pan < -170) target_pan = -170;
          if(target_tilt > 90) target_tilt = 90;
          if(target_tilt < -30) target_tilt = -30;
          
          if(fabs(target_pan) > 65.5 || fabs(target_tilt) > 24.5)
            {
              int current_pan, current_tilt;
              use_absolute = 0;
              VISCA_get_pantilt_position(&vh.visca_iface, &vh.visca_camera, &current_pan, &current_tilt);
              current_pan = (current_pan/36416.0) * 170;
              current_tilt = (current_tilt/19215.0) * 90;

              remain_pan = target_pan - current_pan;
              remain_tilt = target_tilt - current_tilt;

              target_pan  = ((double) target_pan / 65.5) * 880;
              target_tilt = ((double) target_tilt / 24.5) * 330;

              remain_pan  = ((double) remain_pan / 65.5) * 880;
              remain_tilt = ((double) remain_tilt / 24.5) * 330;
              
            }
          else 
            {
              use_absolute = 1;
              target_pan  = ((double) target_pan / 65.5) * 880;
              target_tilt = ((double) target_tilt / 24.5) * 330;
              remain_pan = 0;
              remain_tilt = 0;

            }

          //Inline Macros in the middle of a gigantic case statement?
          //This is clean code if I've ever seen it...
#define OVERFLOW_GT(val, overflow, limit) if(val > limit) { overflow = val - limit; val = limit; }
#define OVERFLOW_LT(val, overflow, limit) if(val < limit) { overflow = val - limit; val = limit; }

          if(use_absolute)
            {
              /*              OVERFLOW_GT(target_pan, remain_pan, 880);
              OVERFLOW_LT(target_pan, remain_pan, -879);
              OVERFLOW_GT(target_tilt, remain_tilt, 330);
              OVERFLOW_LT(target_tilt, remain_tilt, -329);*/
              VISCA_set_pantilt_absolute_position(&vh.visca_iface, &vh.visca_camera, 24, 20, target_pan, target_tilt);            
            }
          else
            {         
              while(remain_tilt != 0 || remain_pan != 0)
                {
                  int tmp_pan = 0, tmp_tilt = 0;

                  OVERFLOW_GT(remain_pan,  tmp_pan, 880);
                  OVERFLOW_LT(remain_pan,  tmp_pan, -879);
                  OVERFLOW_GT(remain_tilt, tmp_tilt, 330);
                  OVERFLOW_LT(remain_tilt, tmp_tilt, -329);
                  VISCA_set_pantilt_relative_position(&vh.visca_iface, &vh.visca_camera, 24, 20, remain_pan, remain_tilt);

                  remain_pan = tmp_pan; tmp_pan = 0;
                  remain_tilt = tmp_tilt; tmp_tilt = 0;
                }
            }
          redisCommand(c, "SET visca:camera:moving false");

          break;

        case 'g':
	  VISCA_get_pantilt_position(&vh.visca_iface, &vh.visca_camera, &target_pan, &target_tilt);
          target_pan = (target_pan/36416.0) * 170;
          target_tilt = (target_tilt/19215.0) * 90;

          VISCA_get_zoom_value(&vh.visca_iface, &vh.visca_camera, &zoom);

          snprintf(buf, 256, "MSET visca:camera:pan %i visca:camera:tilt %i visca:camera:zoom", target_pan, target_tilt, zoom);
          redisCommand(c, buf);
          break;

        case 'z':
          redisCommand(c, "SET visca:camera:moving true");

          reply = redisCommand(c, "GET visca:camera:zoom");
          if (reply->type == REDIS_REPLY_STRING) {
            zoom = strtoul(reply->str, NULL, 0);
          }
          freeReplyObject(reply);

          VISCA_set_zoom_value(&vh.visca_iface, &vh.visca_camera, zoom);

          redisCommand(c, "SET visca:camera:moving false");

          break;

        case 'q':
          quit = 1;
          break;
        }
    }

  redisFree(c);
  visca_close_serial(&vh);
  return 0;
}
