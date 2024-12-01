#ifndef IMAGE_PROCESSING_H
#define IMAGE_PROCESSING_H

#include <SDL2/SDL.h>


SDL_Surface* preprocess_image(SDL_Surface* surface);


SDL_Surface* convert_to_bw(SDL_Surface* grayscaleSurface, Uint8 threshold);

SDL_Surface* enhance_contrast(SDL_Surface* surface, double contrast, double gamma);

SDL_Surface *apply_gaussian_filter(SDL_Surface *surface, int kernelSize, float sigma);

SDL_Surface *apply_median_filter(SDL_Surface *surface, int kernelSize);

SDL_Surface* invert_colors(SDL_Surface* surface);

Uint8 calculate_threshold(SDL_Surface* surface);


SDL_Surface* rotate_image(SDL_Surface* surface, int angle);


#endif // IMAGE_PROCESSING_H