#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include "load_image.h"

// Fonction pour convertir l'image en niveaux de gris
void convert_to_grayscale(SDL_Surface* surface) {
    if (surface->format->BytesPerPixel != 3 && surface->format->BytesPerPixel != 4) {
        printf("Format d'image non supporté pour la conversion en niveaux de gris.\n");
        return;
    }
    SDL_LockSurface(surface);
    Uint8 r, g, b;
    Uint32 pixel;
    for (int y = 0; y < surface->h; y++) {
        for (int x = 0; x < surface->w; x++) {
            pixel = ((Uint32*)surface->pixels)[y * surface->w + x];
            SDL_GetRGB(pixel, surface->format, &r, &g, &b);
            Uint8 gray = 0.3 * r + 0.59 * g + 0.11 * b;
            pixel = SDL_MapRGB(surface->format, gray, gray, gray);
            ((Uint32*)surface->pixels)[y * surface->w + x] = pixel;
        }
    }

    SDL_UnlockSurface(surface);
}

// Fonction pour charger une image et la convertir en texture
SDL_Texture* load_image(const char* file_path, SDL_Renderer* renderer) {
    SDL_Surface* surface = IMG_Load(file_path);
    if (!surface) {
        printf("Erreur lors du chargement de l'image : %s\n", IMG_GetError());
        return NULL;
    }

    // Conversion en niveaux de gris
    convert_to_grayscale(surface);

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return texture;
}

SDL_Surface* rotate_image(SDL_Surface* surface, int angle) {
    // Convertir l'angle en radians
    double radians = angle * M_PI / 180.0;

    // Calculer les nouvelles dimensions de la surface
    int original_width = surface->w;
    int original_height = surface->h;
    int new_width = (int)(fabs(original_width * cos(radians)) + fabs(original_height * sin(radians)));
    int new_height = (int)(fabs(original_width * sin(radians)) + fabs(original_height * cos(radians)));

    // Créer une nouvelle surface avec les dimensions ajustées
    SDL_Surface* rotated_surface = SDL_CreateRGBSurface(0, new_width, new_height, surface->format->BitsPerPixel,
                                                        surface->format->Rmask, surface->format->Gmask,
                                                        surface->format->Bmask, surface->format->Amask);
    if (!rotated_surface) {
        printf("Erreur lors de la création de la surface : %s\n", SDL_GetError());
        return NULL;
    }

    // Remplir la nouvelle surface avec du noir comme couleur de fond
    SDL_FillRect(rotated_surface, NULL, SDL_MapRGB(rotated_surface->format, 0, 0, 0));

    // Définir le centre de l'image originale et de la nouvelle image
    int cx = original_width / 2;
    int cy = original_height / 2;
    int ncx = new_width / 2;
    int ncy = new_height / 2;

    // Parcourir chaque pixel de la nouvelle surface et déterminer sa couleur d'après la surface d'origine
    for (int y = 0; y < new_height; y++) {
        for (int x = 0; x < new_width; x++) {
            // Calculer la position inversement transformée
            int original_x = (int)((x - ncx) * cos(-radians) - (y - ncy) * sin(-radians) + cx);
            int original_y = (int)((x - ncx) * sin(-radians) + (y - ncy) * cos(-radians) + cy);

            // Vérifier si la position est dans les limites de l'image originale
            if (original_x >= 0 && original_x < original_width && original_y >= 0 && original_y < original_height) {
                // Obtenir la couleur du pixel d'origine
                Uint32 pixel = ((Uint32*)surface->pixels)[original_y * original_width + original_x];
                // Définir la couleur du pixel dans la nouvelle image
                ((Uint32*)rotated_surface->pixels)[y * new_width + x] = pixel;
            }
        }
    }

    return rotated_surface;
}
