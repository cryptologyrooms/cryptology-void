#ifndef _DISPLAY_H_
#define _DISPLAY_H_

#define PIXELS_PER_STRIP 5

void get_rgb_value(uint8_t pixel, uint32_t countdown, uint32_t countdown_max, uint8_t * rgb, uint8_t const * const full_rgb);

#endif
