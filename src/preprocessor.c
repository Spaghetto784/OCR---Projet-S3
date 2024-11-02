#include "preprocessor.h"

#include <SDL.h>
#include <SDL_image.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h> // Added for printf and fprintf
#include <stdlib.h>

SDL_Surface *load_image(const char *path)
{
    SDL_Surface *img =
        IMG_Load(path); // Use SDL_image to load different formats
    if (!img)
    {
        fprintf(stderr, "Unable to load image: %s\n", IMG_GetError());
    }
    return img;
}

// Convert the image to grayscale
void convert_to_grayscale(Image *img)
{
    for (int i = 0; i < img->width * img->height; ++i)
    {
        uint8_t *pixel = &img->data[i * img->channels];
        uint8_t grayscale_value = (pixel[0] + pixel[1] + pixel[2]) / 3;
        pixel[0] = grayscale_value;
        pixel[1] = grayscale_value;
        pixel[2] = grayscale_value;
    }
}

// Convert the image to black and white using a threshold
void binarize_image(Image *img, uint8_t threshold)
{
    for (int i = 0; i < img->width * img->height; ++i)
    {
        uint8_t *pixel = &img->data[i * img->channels];
        uint8_t grayscale_value = pixel[0];
        uint8_t bin_value = grayscale_value > threshold ? 255 : 0;
        pixel[0] = bin_value;
        pixel[1] = bin_value;
        pixel[2] = bin_value;
    }
}

// Function to create a new surface after rotation
SDL_Surface *rotate_surface(SDL_Surface *src, double angle)
{
    // Calculate new surface dimensions
    double radians = angle * (M_PI / 180.0);
    int new_width = abs(src->w * cos(radians)) + abs(src->h * sin(radians));
    int new_height = abs(src->h * cos(radians)) + abs(src->w * sin(radians));

    SDL_Surface *rotated = SDL_CreateRGBSurface(
        0, new_width, new_height, src->format->BitsPerPixel, src->format->Rmask,
        src->format->Gmask, src->format->Bmask, src->format->Amask);
    if (!rotated)
    {
        fprintf(stderr, "Unable to create surface for rotation: %s\n",
                SDL_GetError());
        return NULL;
    }

    // Initialize for rotation
    SDL_Rect dest;
    dest.x = (new_width - src->w) / 2;
    dest.y = (new_height - src->h) / 2;

    // Blit the source image onto the new surface
    SDL_SetSurfaceBlendMode(src, SDL_BLENDMODE_BLEND);
    SDL_BlitScaled(src, NULL, rotated, &dest);

    return rotated;
}

// Manual rotation of the image
void rotate_image(SDL_Surface **img, double angle)
{
    SDL_Surface *rotated_img = rotate_surface(*img, angle);
    if (rotated_img)
    {
        SDL_FreeSurface(*img);
        *img = rotated_img; // Update the original surface
    }
}

// Automatic image rotation
void auto_rotate_image(SDL_Surface **img)
{
    // Placeholder for the actual angle detection logic
    double angle = 15.0; // Replace this with the actual calculation
    rotate_image(img, angle);
}

// Noise reduction
void remove_noise(SDL_Surface *img)
{
    // Simple example of noise reduction through blurring
    SDL_LockSurface(img);
    Uint32 *pixels = (Uint32 *)img->pixels;

    // Implement a simple technique to reduce noise
    for (int y = 1; y < img->h - 1; y++)
    {
        for (int x = 1; x < img->w - 1; x++)
        {
            // Example implementation for averaging surrounding pixels can be
            // added here
        }
    }

    SDL_UnlockSurface(img);
}

// Contrast enhancement
void enhance_contrast(SDL_Surface *img)
{
    SDL_LockSurface(img);
    Uint32 *pixels = (Uint32 *)img->pixels;

    for (int i = 0; i < img->w * img->h; i++)
    {
        Uint8 r, g, b;
        SDL_GetRGB(pixels[i], img->format, &r, &g, &b);

        // Apply a transformation to increase contrast
        r = (Uint8)fmin(255, fmax(0, r * 1.2)); // Example of contrast increase
        g = (Uint8)fmin(255, fmax(0, g * 1.2));
        b = (Uint8)fmin(255, fmax(0, b * 1.2));

        pixels[i] = SDL_MapRGB(img->format, r, g, b);
    }

    SDL_UnlockSurface(img);
}

// Free image memory
void free_image(Image *img)
{
    free(img->data);
    free(img);
}
