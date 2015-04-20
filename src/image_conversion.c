/*
 * Color Space Converter
 * Author: Peter Galvin
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "pixfc-sse.h"

void uyvy_to_rgb(uint8_t * out, uint8_t * in, unsigned int width, unsigned int height)
{
  struct PixFcSSE * pixfc;

  create_pixfc(&pixfc, PixFcUYVY, PixFcRGB24, width, height, PixFcFlag_Default);

  pixfc->convert(pixfc, in, out);

  destroy_pixfc(pixfc);
}

void uyvy_to_bgr(uint8_t * out, uint8_t * in, unsigned int width, unsigned int height)
{
  struct PixFcSSE * pixfc;

  create_pixfc(&pixfc, PixFcUYVY, PixFcBGR24, width, height, PixFcFlag_Default);

  pixfc->convert(pixfc, in, out);

  destroy_pixfc(pixfc);
}


