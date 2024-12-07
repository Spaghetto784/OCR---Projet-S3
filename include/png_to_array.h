#ifndef PNG_TO_ARRAY_H
#define PNG_TO_ARRAY_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <err.h>

// Define target size for scaled images
#define TARGET_SIZE 28

// Function declarations

/**
 * @brief Loads a PNG image and converts it to RGBA32 format.
 * @param filename Path to the image file.
 * @return Pointer to the loaded SDL_Surface or NULL on failure.
 */
SDL_Surface* load_image(const char *filename);

/**
 * @brief Scales an SDL_Surface to a fixed size (28x28).
 * @param image Pointer to the SDL_Surface to be scaled.
 * @return Pointer to the scaled SDL_Surface or NULL on failure.
 */
SDL_Surface* scale_image(SDL_Surface *image);

/**
 * @brief Converts a scaled SDL_Surface to a normalized grayscale 2D array.
 * @param image Pointer to the SDL_Surface to be converted.
 * @param output 2D array to store grayscale values.
 */
void image_to_grayscale(SDL_Surface *image, double output[TARGET_SIZE][TARGET_SIZE]);

/**
 * @brief Flattens a 2D grayscale array into a 1D array.
 * @param grayscale_array 2D array of grayscale values.
 * @return Pointer to the flattened 1D array (dynamically allocated).
 */
double* flatten_grayscale_array(double grayscale_array[TARGET_SIZE][TARGET_SIZE]);

/**
 * @brief Loads an image, processes it to a flattened grayscale array.
 * @param filename Path to the image file.
 * @return Pointer to the flattened grayscale 1D array (dynamically allocated).
 */
double* Output_Array(char *filename);

#endif //PNG_TO_ARRAY_H

