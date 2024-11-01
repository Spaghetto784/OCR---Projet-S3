#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "image_processing.h"
#include "load_image.h"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Usage: %s <image_path>\n", argv[0]);
        return 1;
    }

    // Initialiser SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("Erreur d'initialisation de SDL: %s\n", SDL_GetError());
        return 1;
    }

    // Charger l'image et convertir en niveaux de gris
    SDL_Surface* original_image = IMG_Load(argv[1]);
    if (!original_image) {
        SDL_Quit();
        return 1;
    }

    SDL_Surface* grayscale_image = preprocess_image(original_image);
    SDL_FreeSurface(original_image);  // Libérer l'image d'origine après conversion

    // Demander à l'utilisateur un angle de rotation
    int angle;
    printf("Entrez l'angle de rotation (en degrés) : ");
    scanf("%d", &angle);

    // Appliquer la rotation
    SDL_Surface* rotated_image = rotate_image(grayscale_image, angle);
    SDL_FreeSurface(grayscale_image);  // Libérer l'image en niveaux de gris après rotation

    if (!rotated_image) {
        SDL_Quit();
        return 1;
    }

    // Créer la fenêtre pour afficher l'image rotatée
    SDL_Window* window = SDL_CreateWindow("Image Rotatée", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                          rotated_image->w, rotated_image->h, SDL_WINDOW_SHOWN);
    if (!window) {
        printf("Erreur lors de la création de la fenêtre : %s\n", SDL_GetError());
        SDL_FreeSurface(rotated_image);
        SDL_Quit();
        return 1;
    }

    // Créer le rendu
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        printf("Erreur lors de la création du rendu : %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_FreeSurface(rotated_image);
        SDL_Quit();
        return 1;
    }

    // Convertir l'image rotatée en texture et afficher
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, rotated_image);
    SDL_FreeSurface(rotated_image);  // Libérer la surface après création de la texture

    if (!texture) {
        printf("Erreur lors de la création de la texture : %s\n", SDL_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Afficher la texture
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);

    // Boucle d'événements pour garder la fenêtre ouverte jusqu'à ce que l'utilisateur ferme la fenêtre
    int quit = 0;
    SDL_Event event;
    while (!quit) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {  // Quitter si l'utilisateur ferme la fenêtre
                quit = 1;
            }
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {  // Quitter si l'utilisateur appuie sur Échap
                quit = 1;
            }
        }
    }

    // Nettoyage
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
