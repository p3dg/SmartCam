/* Blackmagic Capture Code Test Bench
 *
 * Author: Idan Warsawski
 */



#include <stdio.h>
#include <stdint.h>
#include "../capture_blackmagic.h"

FILE * output_file;

void callback(uint8_t * bytes, uint32_t size, uint64_t timestamp, void * priv)
{
  //FILE * output_file = (FILE *) priv;
fprintf(stderr, "Received output frame with timestamp %lu and size %u\n", timestamp, size);

  fwrite(bytes, size, 1, output_file);
}

int main(int argc, char ** argv)
{
  struct capture_blackmagic cbm;
  FILE * fp;


  if(argc != 2)
    {
      fprintf(stderr, "Invalid # of arguments\n");
      exit(1);
    }

  output_file = fopen(argv[1], "w");
  if(!output_file)
    {
      fprintf(stderr, "COuld not open file %s\n", argv[1]);
      exit(1);
    }
  
  cbm.video_frame_callback = callback;
  cbm.priv = fp;

  capture_blackmagic_init(&cbm);
  capture_blackmagic_start(&cbm);
  capture_blackmagic_block(&cbm);
}
