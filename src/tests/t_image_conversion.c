/*
 * Image Conversion Routine Tests
 *
 * Author: Idan Warsawski
 */




#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "image_conversion.h"


//Note this only tests UYVY -> RGB conversion via flat files in

int main(int argc, char ** argv)
{
  FILE * fp;
  int width, height;
  uint8_t * in, *out;
  size_t size_in, size_out;
  int i;

  if(argc != 5)
    {
      fprintf(stderr, "Invalid number of arguments\n");
      exit(1);
    }

  width = atoi(argv[3]);
  height = atoi(argv[4]);

  size_out = width*height*3;
  out = malloc(size_out);
  if(!out) 
    {
      fprintf(stderr, "Could not allocate space for out\n");
      exit(1);
    }
  //checking mallocs are for real programmers

  fp = fopen(argv[1], "r");
  fseek(fp, 0L, SEEK_END);
  size_in = ftell(fp);
  fseek(fp, 0L, SEEK_SET);

  in = malloc(size_in);
  fread(in, size_in, 1, fp);
  fclose(fp);

  fp = fopen(argv[2], "w");

  fprintf(stderr, "Reading in %d bytes, converting to frame size of %d that is %dx%d\n", (int)size_in, (int)size_out, width, height);
  for(i = 0; i < size_in; i += width*height*4)
    {
      uyvy_to_rgb(out, in + i, width, height);
      fwrite(out, size_out, 1, fp);
    }
  fclose(fp);
  free(in);
  free(out);

  return 0;

}

