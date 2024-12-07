#ifndef LOAD_IMAGE_H
#define LOAD_IMAGE_H

#include <SDL2/SDL.h>


SDL_Texture* load_image(const char* path, SDL_Renderer* renderer);
void convert_to_grayscale(SDL_Surface* surface); 
void load_dummy_inputs(const char* image_dir);
#endif // LOAD_IMAGE_H