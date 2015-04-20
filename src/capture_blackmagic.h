/* Blackmagic Capture Code
 *
 * Author: Idan Warsawski
 */


#ifndef CAPTURE_BLACKMAGIC_H
#define CAPTURE_BLACKMAGIC_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <pthread.h>

#define BLACKMAGIC_SVIDEO 0
#define BLACKMAGIC_COMPOSITE 1


#ifdef __cplusplus
extern "C" {
#endif


  struct capture_blackmagic
  {                                           
    int video_source;
    //enum video_source v;
    FILE * fp;
    
    void (*video_frame_callback)(uint8_t * bytes, uint32_t size, uint64_t timestamp, void * priv);
    void * callback_priv;

    void * priv;
  };
  
  void capture_blackmagic_init(struct capture_blackmagic * cbm);
  void capture_blackmagic_start(struct capture_blackmagic * cbm);
  void capture_blackmagic_block(struct capture_blackmagic * cbm);
  void capture_blackmagic_unblock(struct capture_blackmagic * cbm);

#ifdef __cplusplus
}
#endif



#endif
