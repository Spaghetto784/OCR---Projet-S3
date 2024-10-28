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

SDL_Texture* rotate_image(SDL_Texture* texture, double angle, SDL_Renderer* renderer) {
    // Créez une surface à partir de la texture
    SDL_Surface* surface;
    int w, h;
    SDL_QueryTexture(texture, NULL, NULL, &w, &h);
    
    surface = SDL_CreateRGBSurface(0, w, h, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0);
    if (!surface) {
        printf("Erreur lors de la création de la surface : %s\n", SDL_GetError());
        return NULL;
    }

    SDL_RenderReadPixels(renderer, NULL, surface->format->format, surface->pixels, surface->pitch);

    // Créez une nouvelle texture pour l'image rotatée
    SDL_Texture* rotated_texture = SDL_CreateTexture(renderer, surface->format->format, SDL_TEXTUREACCESS_TARGET, h, w);
    if (!rotated_texture) {
        printf("Erreur lors de la création de la texture rotatée : %s\n", SDL_GetError());
        SDL_FreeSurface(surface);
        return NULL;
    }

    // Définir la texture comme cible de rendu
    SDL_SetRenderTarget(renderer, rotated_texture);
    SDL_RenderClear(renderer);

    // Effectuer la rotation
    SDL_Rect dst_rect = { 0, 0, h, w };
    SDL_RenderCopyEx(renderer, texture, NULL, &dst_rect, angle, NULL, SDL_FLIP_NONE);

    // Réinitialiser la cible de rendu
    SDL_SetRenderTarget(renderer, NULL);

    // Libérer la surface
    SDL_FreeSurface(surface);

    return rotated_texture;
}
