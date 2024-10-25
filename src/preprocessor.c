#include <stdlib.h>
#include <stdint.h>
#include "preprocessor.h"
#include <SDL.h>
#include <math.h>
#include <SDL_image.h>

SDL_Surface* load_image(const char* path) {
    SDL_Surface* img = IMG_Load(path); // Utilisation de SDL_image pour charger différents formats
    if (!img) {
        fprintf(stderr, "Unable to load image: %s\n", IMG_GetError());
    }
    return img;
}

// Conversion de l'image en niveaux de gris
void convert_to_grayscale(Image* img) {
    for (int i = 0; i < img->width * img->height; ++i) {
        uint8_t* pixel = &img->data[i * img->channels];
        uint8_t grayscale_value = (pixel[0] + pixel[1] + pixel[2]) / 3;
        pixel[0] = grayscale_value;
        pixel[1] = grayscale_value;
        pixel[2] = grayscale_value;
    }
}

// Conversion de l'image en noir et blanc avec un seuil
void binarize_image(Image* img, uint8_t threshold) {
    for (int i = 0; i < img->width * img->height; ++i) {
        uint8_t* pixel = &img->data[i * img->channels];
        uint8_t grayscale_value = pixel[0];
        uint8_t bin_value = grayscale_value > threshold ? 255 : 0;
        pixel[0] = bin_value;
        pixel[1] = bin_value;
        pixel[2] = bin_value;
    }
}
// Fonction pour créer une nouvelle surface après rotation
SDL_Surface* rotate_surface(SDL_Surface* src, double angle) {
    // Calcul des dimensions de la nouvelle surface
    double radians = angle * (M_PI / 180.0);
    int new_width = abs(src->w * cos(radians)) + abs(src->h * sin(radians));
    int new_height = abs(src->h * cos(radians)) + abs(src->w * sin(radians));

    SDL_Surface* rotated = SDL_CreateRGBSurface(0, new_width, new_height, src->format->BitsPerPixel,
                                                  src->format->Rmask, src->format->Gmask,
                                                  src->format->Bmask, src->format->Amask);
    if (!rotated) {
        fprintf(stderr, "Unable to create surface for rotation: %s\n", SDL_GetError());
        return NULL;
    }

    // Initialisation pour la rotation
    SDL_Rect dest;
    dest.x = (new_width - src->w) / 2;
    dest.y = (new_height - src->h) / 2;

    // Rendre l'image source dans la nouvelle surface
    SDL_SetSurfaceBlendMode(src, SDL_BLENDMODE_BLEND);
    SDL_BlitScaled(src, NULL, rotated, &dest);
    
    return rotated;
}

// Redressement manuel de l’image
void rotate_image(SDL_Surface* img, double angle) {
    SDL_Surface* rotated_img = rotate_surface(img, angle);
    if (rotated_img) {
        SDL_FreeSurface(img);
        img = rotated_img;  // Vous devrez gérer la mise à jour de la surface d'origine
    }
}

// Redressement automatique de l’image
void auto_rotate_image(SDL_Surface* img) {
    // Vous pouvez ajouter ici le code pour déterminer l'angle d'inclinaison
    // Pour l'instant, utilisons un angle fixe comme exemple
    double angle = 15.0; // Remplacez cela par le calcul réel
    rotate_image(img, angle);
}

// Élimination de bruit
void remove_noise(SDL_Surface* img) {
    // Exemple simple de réduction de bruit par le flou
    SDL_LockSurface(img);
    Uint32* pixels = (Uint32*)img->pixels;

    for (int y = 1; y < img->h - 1; y++) {
        for (int x = 1; x < img->w - 1; x++) {
            // Implémentez ici une technique simple pour réduire le bruit
            // Par exemple, calculez la moyenne des pixels environnants
        }
    }

    SDL_UnlockSurface(img);
}

// Renforcement des contrastes
void enhance_contrast(SDL_Surface* img) {
    SDL_LockSurface(img);
    Uint32* pixels = (Uint32*)img->pixels;

    for (int i = 0; i < img->w * img->h; i++) {
        Uint8 r, g, b;
        SDL_GetRGB(pixels[i], img->format, &r, &g, &b);
        
        // Appliquez une transformation pour augmenter le contraste
        r = (Uint8)fmin(255, fmax(0, r * 1.2)); // Exemples d'augmentation de contraste
        g = (Uint8)fmin(255, fmax(0, g * 1.2));
        b = (Uint8)fmin(255, fmax(0, b * 1.2));
        
        pixels[i] = SDL_MapRGB(img->format, r, g, b);
    }

    SDL_UnlockSurface(img);
}
// Libération de la mémoire d'image
void free_image(Image* img) {
    free(img->data);
    free(img);
}