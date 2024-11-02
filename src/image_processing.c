#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>

// Constants for grayscale conversion
#define RED_WEIGHT 0.3
#define GREEN_WEIGHT 0.59
#define BLUE_WEIGHT 0.11

// Function to convert an image to grayscale
SDL_Surface *preprocess_image(SDL_Surface *surface)
{
    if (surface == NULL)
    {
        printf("Input surface is NULL\n");
        return NULL; // Handle null input
    }

    // Create a new surface for the grayscale image
    SDL_Surface *grayscaleSurface =
        SDL_CreateRGBSurface(0, surface->w, surface->h, 32, 0x00FF0000,
                             0x0000FF00, 0x000000FF, 0xFF000000);
    if (grayscaleSurface == NULL)
    {
        printf("Unable to create grayscale surface: %s\n", SDL_GetError());
        return NULL;
    }

    // Lock the surfaces for direct pixel access
    SDL_LockSurface(surface);
    SDL_LockSurface(grayscaleSurface);

    // Iterate over each pixel
    for (int y = 0; y < surface->h; y++)
    {
        for (int x = 0; x < surface->w; x++)
        {
            // Get the pixel color
            Uint32 pixel =
                ((Uint32 *)surface->pixels)[y * (surface->pitch / 4) + x];
            Uint8 r, g, b;
            SDL_GetRGB(pixel, surface->format, &r, &g, &b);

            // Calculate the grayscale value
            Uint8 gray =
                (Uint8)(RED_WEIGHT * r + GREEN_WEIGHT * g + BLUE_WEIGHT * b);

            // Set the pixel color in the grayscale image
            Uint32 grayPixel =
                SDL_MapRGB(grayscaleSurface->format, gray, gray, gray);
            ((Uint32 *)grayscaleSurface
                 ->pixels)[y * (grayscaleSurface->pitch / 4) + x] = grayPixel;
        }
    }

    // Unlock the surfaces
    SDL_UnlockSurface(surface);
    SDL_UnlockSurface(grayscaleSurface);

    return grayscaleSurface;
}
