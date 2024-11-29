#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>

// Constants for grayscale conversion
#define RED_WEIGHT 0.3
#define GREEN_WEIGHT 0.59
#define BLUE_WEIGHT 0.11

// Function to convert an image to grayscale
SDL_Surface* preprocess_image(SDL_Surface* surface) {
    if (surface == NULL) {
        printf("Input surface is NULL\n");
        return NULL; // Handle null input
    }

    // Create a new surface for the grayscale image
    SDL_Surface* grayscaleSurface = SDL_CreateRGBSurface(0, surface->w, surface->h, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
    if (grayscaleSurface == NULL) {
        printf("Unable to create grayscale surface: %s\n", SDL_GetError());
        return NULL;
    }

    // Lock the surfaces for direct pixel access
    SDL_LockSurface(surface);
    SDL_LockSurface(grayscaleSurface);

    // Iterate over each pixel
    for (int y = 0; y < surface->h; y++) {
        for (int x = 0; x < surface->w; x++) {
            // Get the pixel color
            Uint32 pixel = ((Uint32*)surface->pixels)[y * (surface->pitch / 4) + x];
            Uint8 r, g, b;
            SDL_GetRGB(pixel, surface->format, &r, &g, &b);

            // Calculate the grayscale value
            Uint8 gray = (Uint8)(RED_WEIGHT * r + GREEN_WEIGHT * g + BLUE_WEIGHT * b);

            // Set the pixel color in the grayscale image
            Uint32 grayPixel = SDL_MapRGB(grayscaleSurface->format, gray, gray, gray);
            ((Uint32*)grayscaleSurface->pixels)[y * (grayscaleSurface->pitch / 4) + x] = grayPixel;
        }
    }

    // Unlock the surfaces
    SDL_UnlockSurface(surface);
    SDL_UnlockSurface(grayscaleSurface);

    return grayscaleSurface;
}

// Function to convert a grayscale image to black and white
SDL_Surface* convert_to_bw(SDL_Surface* grayscaleSurface, Uint8 threshold) {
    if (grayscaleSurface == NULL) {
        printf("Input grayscale surface is NULL\n");
        return NULL; // Handle null input
    }

    // Create a new surface for the black and white image
    SDL_Surface* bwSurface = SDL_CreateRGBSurface(0, grayscaleSurface->w, grayscaleSurface->h, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
    if (bwSurface == NULL) {
        printf("Unable to create black and white surface: %s\n", SDL_GetError());
        return NULL;
    }

    // Lock the surfaces for direct pixel access
    SDL_LockSurface(grayscaleSurface);
    SDL_LockSurface(bwSurface);

    // Iterate over each pixel
    for (int y = 0; y < grayscaleSurface->h; y++) {
        for (int x = 0; x < grayscaleSurface->w; x++) {
            // Get the grayscale value
            Uint32 pixel = ((Uint32*)grayscaleSurface->pixels)[y * (grayscaleSurface->pitch / 4) + x];
            Uint8 gray;
            SDL_GetRGB(pixel, grayscaleSurface->format, &gray, &gray, &gray);

            // Apply the threshold to convert to black or white
            Uint8 bwColor = (gray > threshold) ? 255 : 0;

            // Set the pixel color in the black and white image
            Uint32 bwPixel = SDL_MapRGB(bwSurface->format, bwColor, bwColor, bwColor);
            ((Uint32*)bwSurface->pixels)[y * (bwSurface->pitch / 4) + x] = bwPixel;
        }
    }

    // Unlock the surfaces
    SDL_UnlockSurface(grayscaleSurface);
    SDL_UnlockSurface(bwSurface);

    return bwSurface;
}

int restrict_val(int x, int mi, int ma)
{
    return x<mi ? mi : (x>ma ? ma : x);
}

SDL_Surface* enhance_contrast(SDL_Surface* surface, double contrast, double gamma) {
    if (surface == NULL) {
        printf("Input surface is NULL\n");
        return NULL; // Handle null input
    }

    // Create a new surface for the contrasted image
    SDL_Surface* contrastedSurface = SDL_CreateRGBSurface(0, surface->w, surface->h, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
    if (contrastedSurface == NULL) {
        printf("Unable to create the contrasted surface: %s\n", SDL_GetError());
        return NULL;
    }

    // Lock the surfaces for direct pixel access
    SDL_LockSurface(surface);
    SDL_LockSurface(contrastedSurface);

    // Iterate over each pixel
    for (int y = 0; y < surface->h; y++) {
        for (int x = 0; x < surface->w; x++) {
            // Get the pixel color
            Uint32 pixel = ((Uint32*)surface->pixels)[y * (surface->pitch / 4) + x];
            Uint8 r, g, b;
            SDL_GetRGB(pixel, surface->format, &r, &g, &b);

            //calculate value of contrasted pixel
            Uint8 nr,ng,nb;
            nr = (Uint8)(restrict_val((contrast*(r-128)+128+gamma),0,255));
            ng = (Uint8)(restrict_val((contrast*(g-128)+128+gamma),0,255));
            nb = (Uint8)(restrict_val((contrast*(b-128)+128+gamma),0,255));


            // Set the pixel color in the contrasted image
            Uint32 contrastedPixel = SDL_MapRGB(contrastedSurface->format, nr, ng, nb);
            ((Uint32*)contrastedSurface->pixels)[y * (contrastedSurface->pitch / 4) + x] = contrastedPixel;
        }
    }

    // Unlock the surfaces
    SDL_UnlockSurface(surface);
    SDL_UnlockSurface(contrastedSurface);

    return contrastedSurface;
}

