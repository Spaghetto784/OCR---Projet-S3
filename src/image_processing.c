#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>


SDL_Surface* preprocess_image(SDL_Surface* surface) {
    // Vérifier si la surface est valide
    if (!surface) {
        printf("Surface nulle, impossible de prétraiter l'image.\n");
        return NULL;
    }

    // Créer une nouvelle surface pour l'image en niveaux de gris
    SDL_Surface* gray_surface = SDL_CreateRGBSurface(0, surface->w, surface->h, 32, 0, 0, 0, 0);
    if (!gray_surface) {
        printf("Erreur lors de la création de la surface grise : %s\n", SDL_GetError());
        return NULL;
    }

    // Convertir l'image en niveaux de gris
    Uint32* pixels = (Uint32*)surface->pixels;
    Uint32* gray_pixels = (Uint32*)gray_surface->pixels;

    for (int y = 0; y < surface->h; ++y) {
        for (int x = 0; x < surface->w; ++x) {
            Uint32 pixel = pixels[y * surface->w + x];
            Uint8 r, g, b;
            SDL_GetRGB(pixel, surface->format, &r, &g, &b);

            // Calculer la luminosité en utilisant la formule de luminance
            Uint8 gray = (Uint8)(0.299 * r + 0.587 * g + 0.114 * b);
            gray_pixels[y * gray_surface->w + x] = SDL_MapRGB(gray_surface->format, gray, gray, gray);
        }
    }

    return gray_surface;
}


