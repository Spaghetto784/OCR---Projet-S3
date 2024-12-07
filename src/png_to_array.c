#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <err.h>

#define TARGET_SIZE 28

// Function to load a PNG image
SDL_Surface* load_image(const char *filename) {
    SDL_Surface *image = IMG_Load(filename);
    if (!image) {
        fprintf(stderr, "Error: Could not load image %s. SDL_Image Error: %s\n", filename, IMG_GetError());
        return NULL;
    }

    // Convert the image to RGBA32 format
    SDL_Surface *converted_image = SDL_ConvertSurfaceFormat(image, SDL_PIXELFORMAT_RGBA32, 0);
    SDL_FreeSurface(image);  // Free the original image
    if (!converted_image) {
        fprintf(stderr, "Error: Could not convert image to RGBA32. SDL Error: %s\n", SDL_GetError());
        return NULL;
    }

    printf("Image loaded and converted to RGBA32 format.\n");
    return converted_image;
}

// Function to scale an SDL_Surface to TARGET_SIZE x TARGET_SIZE
SDL_Surface* scale_image(SDL_Surface *image) {
    SDL_Surface *scaled_image = SDL_CreateRGBSurfaceWithFormat(
        0, TARGET_SIZE, TARGET_SIZE, 32, SDL_PIXELFORMAT_RGBA32);
    if (!scaled_image) {
        fprintf(stderr, "Error: Could not create scaled surface. SDL Error: %s\n", SDL_GetError());
        return NULL;
    }

    SDL_Rect dest_rect = {0, 0, TARGET_SIZE, TARGET_SIZE};
    if (SDL_BlitScaled(image, NULL, scaled_image, &dest_rect) < 0) {
        fprintf(stderr, "Error: Could not scale image. SDL Error: %s\n", SDL_GetError());
        SDL_FreeSurface(scaled_image);
        return NULL;
    }

    printf("Image successfully scaled to %dx%d.\n", TARGET_SIZE, TARGET_SIZE);
    return scaled_image;
}

// Function to convert an image to a grayscale normalized array
void image_to_grayscale(SDL_Surface *image, double output[TARGET_SIZE][TARGET_SIZE]) {
    Uint32 *pixels = (Uint32*)image->pixels;
    int pitch = image->pitch / 4; // Pixels per row (pitch in bytes divided by sizeof(Uint32))

    for (int y = 0; y < TARGET_SIZE; y++) {
        for (int x = 0; x < TARGET_SIZE; x++) {
            Uint32 pixel = pixels[y * pitch + x];

            // Extract RGBA components
            Uint8 r, g, b, a;
            SDL_GetRGBA(pixel, image->format, &r, &g, &b, &a);

            // Convert to grayscale
            float gray = 0.3f * r + 0.59f * g + 0.11f * b;

            // Normalize to [0, 1] and print the grayscale value for debugging
            float bruh = (gray / 255.0f);
	    if (bruh < 0.5f) 
		    output[y][x] = 1;
	    else
		    output[y][x] = 0;


            // Debug: Print the grayscale values
            if (y == 0 && x < 10) { // Print only first few pixels to debug
                printf("Pixel (%d, %d): R=%d, G=%d, B=%d, Gray=%.2f\n", x, y, r, g, b, output[y][x]);
            }
        }
    }
    printf("Grayscale conversion completed.\n");
}

double* flatten_grayscale_array(double grayscale_array[TARGET_SIZE][TARGET_SIZE]) {
    int size = TARGET_SIZE * TARGET_SIZE;
    double *flattened_array = (double*)malloc(size * sizeof(double));
    if (!flattened_array) {
        fprintf(stderr, "Error: Memory allocation failed for flattened array.\n");
        return NULL;
    }

    // Copy values from 2D array to 1D array
    for (int y = 0; y < TARGET_SIZE; y++) {
        for (int x = 0; x < TARGET_SIZE; x++) {
            flattened_array[y * TARGET_SIZE + x] = grayscale_array[y][x];
        }
    }

    printf("Flattened grayscale array created successfully.\n");
    return flattened_array;
}

double* Output_Array(char *filename)
{

    // Initialize SDL2 and SDL2_image
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        errx(EXIT_FAILURE, "Error: Could not initialize SDL. SDL Error: %s\n", SDL_GetError());
    }

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        fprintf(stderr, "Error: Could not initialize SDL_Image. SDL_Image Error: %s\n", IMG_GetError());
        SDL_Quit();
        errx(EXIT_FAILURE, "Error: Could not initialize SDL.");

    }

    // Load the image
    SDL_Surface *image = load_image(filename);
    if (!image) {
        IMG_Quit();
        SDL_Quit();
        errx(EXIT_FAILURE, "Error: Could not load the image.");
    }
    // Scale the image to 28x28
    SDL_Surface *scaled_image = scale_image(image);
    SDL_FreeSurface(image);  // Free the original image
    if (!scaled_image) {
        IMG_Quit();
        SDL_Quit();
        errx(EXIT_FAILURE, "Error: Could not load the image.");
    }

    // Convert the scaled image to a grayscale array
    double grayscale_array[TARGET_SIZE][TARGET_SIZE];
    image_to_grayscale(scaled_image, grayscale_array);
    SDL_FreeSurface(scaled_image);  // Free the scaled image

    // Print the grayscale array
    printf("Grayscale 28x28 array:\n");
    for (int y = 0; y < TARGET_SIZE; y++) {
        for (int x = 0; x < TARGET_SIZE; x++) {
            printf("%i ",(int) grayscale_array[y][x]);
        }
        printf("\n");
    }

    // Clean up
    IMG_Quit();
    SDL_Quit();
    return flatten_grayscale_array(grayscale_array);
}





