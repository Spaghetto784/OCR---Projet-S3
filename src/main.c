#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

SDL_Surface* load_image(const char* path) {
    SDL_Surface* img = IMG_Load(path);
    if (!img) {
        fprintf(stderr, "Unable to load image: %s\n", IMG_GetError());
    }
    return img;
}

// Autres fonctions de prétraitement ici...

int main(int argc, char* argv[]) {
    // Initialiser SDL et SDL_image
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "SDL_Init Error: %s\n", SDL_GetError());
        return 1;
    }
    if (IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG) == 0) {
        fprintf(stderr, "IMG_Init Error: %s\n", IMG_GetError());
        SDL_Quit();
        return 1;
    }

    // Charger une image pour le test
    SDL_Surface* img = load_image("data/examples/Level 1 Image.png"); // Remplacez par le chemin de votre image
    if (!img) {
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    // Appliquer les fonctions de prétraitement
    printf("Processing image...\n");
    
    // Redressement manuel
    rotate_image(img, 45.0);
    printf("Image rotated by 45 degrees.\n");
    
    // Redressement automatique
    auto_rotate_image(img);
    printf("Image auto-rotated.\n");
    
    // Élimination de bruit
    remove_noise(img);
    printf("Noise removed from image.\n");
    
    // Renforcement des contrastes
    enhance_contrast(img);
    printf("Contrast enhanced.\n");

    // Libération de la surface et fermeture de SDL
    SDL_FreeSurface(img);
    IMG_Quit();
    SDL_Quit();
    return 0;
}