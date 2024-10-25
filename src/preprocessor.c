#include <stdlib.h>
#include <stdint.h>
#include "preprocessor.h"

// Charge une image depuis un fichier (chemin donné) et retourne un pointeur vers l'objet Image.
Image* load_image(const char* path) {
    // Simulation du chargement d'image
    Image* img = malloc(sizeof(Image));
    // Configuration d'image par défaut (éventuellement remplacer par la bibliothèque d'image plus tard)
    img->width = 100;
    img->height = 100;
    img->channels = 3;
    img->data = malloc(img->width * img->height * img->channels);
    return img;
}

// Conversion de l'image en niveaux de gris
void convert_to_grayscale(Image* img) {
    for (int i = 0; i < img->width * img->height; ++i) {
        uint8_t* pixel = &img->data[i * img->channels];
        uint8_t grayscale_value = (pixel[0] + pixel[1] + pixel[2]) / 3;
        pixel[0] = grayscale_value;
        pixel[1] = grayscale_value;
        pixel[2] = grayscale_value;
    }
}

// Conversion de l'image en noir et blanc avec un seuil
void binarize_image(Image* img, uint8_t threshold) {
    for (int i = 0; i < img->width * img->height; ++i) {
        uint8_t* pixel = &img->data[i * img->channels];
        uint8_t grayscale_value = pixel[0];
        uint8_t bin_value = grayscale_value > threshold ? 255 : 0;
        pixel[0] = bin_value;
        pixel[1] = bin_value;
        pixel[2] = bin_value;
    }
}

// Libération de la mémoire d'image
void free_image(Image* img) {
    free(img->data);
    free(img);
}