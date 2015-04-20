#include "libvisca.h"
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>

static void signal_catcher(int signal) {
    puts("Interactive attention signal caught.");
    //goto fail;
}

int main(int argc, char ** argv)
{
  VISCAInterface_t visca_iface;
  VISCACamera_t visca_camera;
  int camera_num;
  char * line_in;
  int line_size = 64;
  line_in = malloc(line_size);
  //signal(SIGINT, signal_catcher);
  int quit = 0;

  if(VISCA_open_serial(&visca_iface, argv[1]) != VISCA_SUCCESS)
    {
      fprintf(stderr, "Could not open serial port %s\n", argv[1]);
      exit(1);
    }

  visca_iface.broadcast = 0;
  if(VISCA_set_address(&visca_iface, &camera_num) != VISCA_SUCCESS)
    {
      fprintf(stderr, "Could not set address\n");
      goto fail;
    }

  visca_camera.address = 1;


  if(VISCA_clear(&visca_iface, &visca_camera) != VISCA_SUCCESS)
    {
      fprintf(stderr, "Could not clear interface\n");
      goto fail;
    }

  if(VISCA_get_camera_info(&visca_iface, &visca_camera) != VISCA_SUCCESS) 
    {
      fprintf(stderr, "Could not get camera info\n");
      goto fail;
    }

  fprintf(stderr, "Camera Initialized\n");

  while(!quit)
    {
      int pan, tilt, dir;
      int pan_speed, tilt_speed;
      float hist1, hist2;
      float target_pan, target_tilt;
      char command;
      double diff;
      
      memset(line_in, 0x00, 64);
      fgets(line_in, 64, stdin);
      //scanf("%c 
      switch(line_in[0])
	{
	case 'r': 
	  VISCA_set_pantilt_reset(&visca_iface, &visca_camera);
	  break;

	case 'h':
	  VISCA_set_pantilt_home(&visca_iface, &visca_camera);
	  break;
	  
            /*a 880 330
             g
             Pan 65.729350, Tilt 24.800937
             a -879 -329            
             g
             Pan -65.584633, Tilt -24.585480*/
	case 'a':
	case 'b':
	  //sscanf(line_in, "%c %i %i", &command, &pan, &tilt);
            sscanf(line_in, "%c %f %f", &command, &target_pan, &target_tilt);
            target_pan  = ((double) target_pan / 65.5) * 880;
            target_tilt = ((double) target_tilt / 24.5) * 330;
            if(target_pan > 880)  target_pan  = 880;
            if(target_pan < -879) target_pan  = -879;
            if(target_tilt > 330) target_tilt = 330;
            if(target_tilt < -329) target_tilt = -329;
	  if(command == 'a')
	    VISCA_set_pantilt_absolute_position(&visca_iface, &visca_camera, 24, 20, target_pan, target_tilt);
	  else
	    VISCA_set_pantilt_relative_position(&visca_iface, &visca_camera, 24, 20, target_pan, target_tilt);
	  break;
	case 'm':
	  pan_speed = 5;
	  tilt_speed = 5;
            hist1 = 36416.0 * 0.75 / 170.0;//0.75;
            hist2 = 36416.0 * 0.30 / 170.0;//0.3;

	  goto ang_move;
	case 'f':
	  pan_speed = 18;
	  tilt_speed = 14;
            hist1 = 36416.0 * 2.0 / 170.0;//1.5;
	  hist2 = 36416.0 * 1.0 / 170.0;//1;

	ang_move:
	  sscanf(line_in, "%c %f %f", &command, &target_pan, &target_tilt);
	  target_pan  = ((double) target_pan / 170.0) * 36416;
	  target_tilt = ((double) target_tilt / 90.0) * 19215;
	  if(target_pan > 36416)  target_pan  = 36416;
	  if(target_pan < -36416) target_pan  = -36416;
	  if(target_tilt > 19215) target_tilt = 19215;
	  if(target_tilt < -6593) target_tilt = -6593;

	  VISCA_get_pantilt_position(&visca_iface, &visca_camera, &pan, &tilt);
	try_pan:
	  diff = target_pan - pan;
	  //diff /= 100;
	  if(diff > hist1)
	    {
	      dir = 1;
	      VISCA_set_pantilt_right(&visca_iface, &visca_camera, pan_speed, 0);
	    }
	  else if(diff < -hist1)
	    {
	      dir = -1;
	      VISCA_set_pantilt_left(&visca_iface, &visca_camera, pan_speed, 0);
	    }
	  else goto try_tilt;

	  do {
	    VISCA_get_pantilt_position(&visca_iface, &visca_camera, &pan, &tilt);
	    diff = target_pan - pan;
	    //diff /= 100;
	    diff *= dir;
	    //if(diff < 0.5) break;
	    //if(diff < 0) diff = -diff;
	  } while(diff > hist2);
	  VISCA_set_pantilt_stop(&visca_iface, &visca_camera, 0, 0);
	try_tilt:
	  diff = target_tilt - tilt;
	  //diff /= 100;
	  if(diff > hist1)
	    {
	      dir = 1;
	      VISCA_set_pantilt_up(&visca_iface, &visca_camera, 0, tilt_speed);
	    }
	  else if(diff < -hist1)
	    {
	      dir = -1;
	      VISCA_set_pantilt_down(&visca_iface, &visca_camera, 0, tilt_speed);
	    }
	  else goto nevermind;

	  do {
	    VISCA_get_pantilt_position(&visca_iface, &visca_camera, &pan, &tilt);
	    diff = target_tilt - tilt;
	    //diff /= 100;
	    diff *= dir;
	    //if(diff < 0.5) break;
	    //if(diff < 0) diff = -diff;
	  } while(diff > hist2);
	  VISCA_set_pantilt_stop(&visca_iface, &visca_camera, 0, 0);

	nevermind:
	case 'g':
	  VISCA_get_pantilt_position(&visca_iface, &visca_camera, &pan, &tilt);
	  fprintf(stdout, "Pan %f, Tilt %f\n", ((double) pan/36416.0)*170, ((double) tilt/19215.0)*90);
	  break;

	case 'q':
	  quit = 1;
	  break;
       
	default:
	  fprintf(stderr, "Unknown command %c\n", line_in[0]);
	}
    }

      free(line_in);
 fail:
  VISCA_close_serial(&visca_iface);
  exit(1);
}
