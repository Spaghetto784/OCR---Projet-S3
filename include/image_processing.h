#ifndef IMAGE_PROCESSING_H
#define IMAGE_PROCESSING_H

#include <SDL2/SDL.h>


SDL_Surface* preprocess_image(SDL_Surface* surface);


SDL_Surface* convert_to_bw(SDL_Surface* grayscaleSurface, Uint8 threshold);


SDL_Surface* rotate_image(SDL_Surface* surface, int angle);


#endif // IMAGE_PROCESSING_H