/* Image Conversion Routines
 *
 * Author: Peter Galvin
 */

#ifdef __cplusplus
extern "C" {
#endif

void uyvy_to_rgb(uint8_t * out, uint8_t * in, unsigned int width, unsigned int height);

void uyvy_to_bgr(uint8_t * out, uint8_t * in, unsigned int width, unsigned int height);

#ifdef __cplusplus
}
#endif
