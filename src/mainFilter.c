#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>

#include "image_processing.h"
#include "load_image.h"

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Usage: %s <image_path>\n", argv[0]);
        return 1;
    }

    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        printf("SDL initialization error: %s\n", SDL_GetError());
        return 1;
    }

    // Load the image and convert to grayscale
    SDL_Surface *original_image = IMG_Load(argv[1]);
    if (!original_image)
    {
        SDL_Quit();
        return 1;
    }

    SDL_Surface *filtered_image = apply_median_filter(original_image, 5);
    SDL_FreeSurface(original_image); // Free the original image after conversion



    // Ask the user for a rotation angle
    int angle;
    printf("Enter the rotation angle (in degrees): ");
    scanf("%d", &angle);

    // Apply the rotation
    SDL_Surface *rotated_image = rotate_image(filtered_image, angle);


    SDL_FreeSurface(filtered_image); // Free the contrasted image after rotation

    if (!rotated_image)
    {
        SDL_Quit();
        return 1;
    }

    // Create a window to display the rotated image
    SDL_Window *window = SDL_CreateWindow(
        "Rotated Image", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        rotated_image->w, rotated_image->h, SDL_WINDOW_SHOWN);
    if (!window)
    {
        printf("Error creating window: %s\n", SDL_GetError());
        SDL_FreeSurface(rotated_image);
        SDL_Quit();
        return 1;
    }

    // Create the renderer
    SDL_Renderer *renderer =
        SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer)
    {
        printf("Error creating renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_FreeSurface(rotated_image);
        SDL_Quit();
        return 1;
    }

    // Convert the rotated image to a texture and display it
    SDL_Texture *texture =
        SDL_CreateTextureFromSurface(renderer, rotated_image);
    SDL_FreeSurface(rotated_image); // Free the surface after creating the texture

    if (!texture)
    {
        printf("Error creating texture: %s\n", SDL_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Display the texture
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);

    // Event loop to keep the window open until the user closes it
    int quit = 0;
    SDL_Event event;
    while (!quit)
    {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            { // Quit if the user closes the window
                quit = 1;
            }
            if (event.type == SDL_KEYDOWN
                && event.key.keysym.sym == SDLK_ESCAPE)
            { // Quit if the user presses Escape
                quit = 1;
            }
        }
    }

    // Cleanup
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
