#ifndef IMAGE_PROCESSING_H
#define IMAGE_PROCESSING_H

#include <SDL2/SDL.h>

// Prototypes des fonctions
SDL_Surface* preprocess_image(SDL_Surface* surface);
SDL_Surface* rotate_image(SDL_Surface* surface);

#endif // IMAGE_PROCESSING_H
