#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

// Prototypes des fonctions
SDL_Texture* load_image(const char* file_path, SDL_Renderer* renderer);
SDL_Texture* rotate_image(SDL_Texture* texture, double angle, SDL_Renderer* renderer);
SDL_Surface* preprocess_image(SDL_Surface* surface); // Déclaration de la fonction de prétraitement

// Fonction de prétraitement pour convertir l'image en niveaux de gris

int main(int argc, char* argv[]) {
    (void)argc;  // Indique que argc est intentionnellement inutilisé
    (void)argv;  // Indique que argv est intentionnellement inutilisé

    // Initialisation de SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Erreur lors de l'initialisation de SDL : %s\n", SDL_GetError());
        return 1;
    }

    // Création de la fenêtre
    SDL_Window* window = SDL_CreateWindow("Image Loader and Rotator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_SHOWN);
    if (!window) {
        printf("Erreur lors de la création de la fenêtre : %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // Création du renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        printf("Erreur lors de la création du renderer : %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Chargement de l'image pour le prétraitement
    SDL_Surface* surface = IMG_Load("data/examples/level1.png");
    if (!surface) {
        printf("Erreur lors du chargement de l'image pour le prétraitement : %s\n", IMG_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Prétraitement de l'image
    SDL_Surface* preprocessed_surface = preprocess_image(surface);
    if (preprocessed_surface) {
        // Convertir la surface prétraitée en texture
        SDL_Texture* preprocessed_texture = SDL_CreateTextureFromSurface(renderer, preprocessed_surface);
        if (preprocessed_texture) {
            // Afficher l'image prétraitée
            SDL_RenderClear(renderer);
            SDL_RenderCopy(renderer, preprocessed_texture, NULL, NULL);
            SDL_RenderPresent(renderer);
            SDL_DestroyTexture(preprocessed_texture); // Libération de la texture
            printf("Prétraitement effectué avec succès !\n");
        } else {
            printf("Erreur lors de la création de la texture pour l'image prétraitée : %s\n", SDL_GetError());
        }
        SDL_FreeSurface(preprocessed_surface); // Libération de la surface prétraitée
    } else {
        printf("Erreur lors du prétraitement de l'image.\n");
    }
    SDL_FreeSurface(surface); // Libération de la surface d'origine

    // Chargement de l'image originale
    surface = IMG_Load("data/examples/level1.png"); // Recharger l'image originale
    if (!surface) {
        printf("Erreur lors du chargement de l'image originale : %s\n", IMG_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Convertir la surface originale en texture
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
        printf("Erreur lors de la création de la texture pour l'image originale : %s\n", SDL_GetError());
        SDL_FreeSurface(surface);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    SDL_FreeSurface(surface); // Libération de la surface d'origine

    // Afficher l'image originale
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);

    // Demander un angle de rotation à l'utilisateur
    double angle;
    printf("Entrez l'angle de rotation en degrés (0 pour aucun) : ");
    scanf("%lf", &angle);

    // Rotation de l'image
    SDL_Texture* rotated_texture = rotate_image(texture, angle, renderer);
    if (rotated_texture) {
        // Afficher l'image rotatée
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, rotated_texture, NULL, NULL);
        SDL_RenderPresent(renderer);
        SDL_DestroyTexture(rotated_texture); // Libération de la texture
        printf("Image bien rotatée !\n");  // Affichage du message
    } else {
        printf("Erreur lors de la rotation de l'image.\n");
    }

    // Boucle d'événements pour quitter
    SDL_Event event;
    int running = 1;
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            }
            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    running = 0;  // Quitte si la touche Échap est pressée
                }
            }
        }
    }

    // Libération des ressources
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
