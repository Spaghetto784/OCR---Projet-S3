#include "load_image.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>

// Function to convert the image to grayscale
void convert_to_grayscale(SDL_Surface *surface)
{
    if (surface->format->BytesPerPixel != 3
        && surface->format->BytesPerPixel != 4)
    {
        printf("Unsupported image format for grayscale conversion.\n");
        return;
    }
    SDL_LockSurface(surface);
    Uint8 r, g, b;
    Uint32 pixel;
    for (int y = 0; y < surface->h; y++)
    {
        for (int x = 0; x < surface->w; x++)
        {
            pixel = ((Uint32 *)surface->pixels)[y * surface->w + x];
            SDL_GetRGB(pixel, surface->format, &r, &g, &b);
            Uint8 gray = 0.3 * r + 0.59 * g + 0.11 * b;
            pixel = SDL_MapRGB(surface->format, gray, gray, gray);
            ((Uint32 *)surface->pixels)[y * surface->w + x] = pixel;
        }
    }

    SDL_UnlockSurface(surface);
}

// Function to load an image and convert it to a texture
SDL_Texture *load_image(const char *file_path, SDL_Renderer *renderer)
{
    SDL_Surface *surface = IMG_Load(file_path);
    if (!surface)
    {
        printf("Error loading image: %s\n", IMG_GetError());
        return NULL;
    }

    // Convert to grayscale
    convert_to_grayscale(surface);

    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return texture;
}

SDL_Surface *rotate_image(SDL_Surface *surface, int angle)
{
    // Convert the angle to radians
    double radians = angle * M_PI / 180.0;

    // Calculate the new dimensions of the surface
    int original_width = surface->w;
    int original_height = surface->h;
    int new_width = (int)(fabs(original_width * cos(radians))
                          + fabs(original_height * sin(radians)));
    int new_height = (int)(fabs(original_width * sin(radians))
                           + fabs(original_height * cos(radians)));

    // Create a new surface with the adjusted dimensions
    SDL_Surface *rotated_surface = SDL_CreateRGBSurface(
        0, new_width, new_height, surface->format->BitsPerPixel,
        surface->format->Rmask, surface->format->Gmask, surface->format->Bmask,
        surface->format->Amask);
    if (!rotated_surface)
    {
        printf("Error creating surface: %s\n", SDL_GetError());
        return NULL;
    }

    // Fill the new surface with black as background color
    SDL_FillRect(rotated_surface, NULL,
                 SDL_MapRGB(rotated_surface->format, 255, 255, 255));

    // Set the center of the original image and the new image
    int cx = original_width / 2;
    int cy = original_height / 2;
    int ncx = new_width / 2;
    int ncy = new_height / 2;

    // Loop through each pixel of the new surface and determine its color from
    // the original surface
    for (int y = 0; y < new_height; y++)
    {
        for (int x = 0; x < new_width; x++)
        {
            // Calculate the inverse transformed position
            int original_x = (int)((x - ncx) * cos(-radians)
                                   - (y - ncy) * sin(-radians) + cx);
            int original_y = (int)((x - ncx) * sin(-radians)
                                   + (y - ncy) * cos(-radians) + cy);

            // Check if the position is within the bounds of the original image
            if (original_x >= 0 && original_x < original_width
                && original_y >= 0 && original_y < original_height)
            {
                // Get the color of the original pixel
                Uint32 pixel =
                    ((Uint32 *)surface
                         ->pixels)[original_y * original_width + original_x];
                // Set the color of the pixel in the new image
                ((Uint32 *)rotated_surface->pixels)[y * new_width + x] = pixel;
            }
        }
    }

    return rotated_surface;
}
