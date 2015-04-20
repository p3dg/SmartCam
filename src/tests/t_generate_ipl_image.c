/* Blackmagic Capture Code 
 * to OpenCV IPLImage Test Bench
 *
 * Author: Idan Warsawski
 */



#include <stdio.h>
#include <stdint.h>
#include <cv.h>
#include <highgui.h>

#include "../capture_blackmagic.h"
#include "../image_conversion.h"

/*
IplImage* img=cvCreateImage(cvSize(640,480),IPL_DEPTH_32F,3);
((float *)(img->imageData + i*img->widthStep))[j*img->nChannels + 0]=111; // B
((float *)(img->imageData + i*img->widthStep))[j*img->nChannels + 1]=112; // G
((float *)(img->imageData + i*img->widthStep))[j*img->nChannels + 2]=113; // R
*/

IplImage * img;
uint8_t * rgb_tmp_buf;

void callback(uint8_t * bytes, uint32_t size, uint64_t timestamp, void * priv)
{
  int i, j;
  fprintf(stderr, "Received output frame with timestamp %lu and size %u\n", timestamp, size);
  uyvy_to_bgr(rgb_tmp_buf, bytes, 720, 486);
  cvSetData(img, rgb_tmp_buf, 720*3);
/*for(i = 0; i < 486; i++)
    for(j = 0; j < 720; j = j + 3)
      {
        ((uint8_t *)(img->imageData + i * img->widthStep))[j*img->nChannels + 0] = rgb_tmp_buf[i*720*3 + j + 2];
        ((uint8_t *)(img->imageData + i * img->widthStep))[j*img->nChannels + 1] = rgb_tmp_buf[i*720*3 + j + 1];
        ((uint8_t *)(img->imageData + i * img->widthStep))[j*img->nChannels + 2] = rgb_tmp_buf[i*720*3 + j + 0];
      }*/

  cvShowImage("video", img);
  cvWaitKey(1);
}

int main(int argc, char ** argv)
{
  struct capture_blackmagic cbm;
 
  //img = cvCreateImage(cvSize(720, 486), IPL_DEPTH_8U, 3);
  img = cvCreateImageHeader(cvSize(720, 486), IPL_DEPTH_8U, 3);
  rgb_tmp_buf = malloc(720*486*3); //resolution * 3bbp

  cvNamedWindow("video", CV_WINDOW_NORMAL);

  cbm.video_frame_callback = callback;

  capture_blackmagic_init(&cbm);
  capture_blackmagic_start(&cbm);
  capture_blackmagic_block(&cbm);
}
