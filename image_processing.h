#ifndef IMAGE_PROCESSING_H
#define IMAGE_PROCESSING_H

#include <SDL2/SDL.h>

// Fonction pour convertir une image en niveaux de gris
SDL_Surface* preprocess_image(SDL_Surface* surface);

// Fonction pour effectuer une rotation d'une image selon un angle donné
SDL_Surface* rotate_image(SDL_Surface* surface, int angle);

#endif // IMAGE_PROCESSING_H