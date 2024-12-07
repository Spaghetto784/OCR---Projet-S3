#include "neural_net.h"
#include "png_to_array.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> 

void train_and_save(char *model_path) {
    NeuralNetwork nn;
    initialize_network(&nn);

    // Allouer dynamiquement les tableaux volumineux
    double (*targets)[26] = malloc(26 * sizeof(*targets));
    double (*inputs)[784] = malloc(2600 * sizeof(*inputs));
    if (targets == NULL || inputs == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }

    // Initialisation des cibles
    for (int j = 0; j < 26; j++) {
        for (int i = 0; i < 26; i++) {
            targets[j][i] = (j == i) ? 1 : 0;
        }
    }

    // Chargement des entrées
    int cur = 0;
    int valid_samples = 0;

    for (int i = 0; i < 2600; i++) {
        char filename[40];
        int written = snprintf(filename, sizeof(filename), "data/dataset/image_%d.png", cur);
        if (written < 0 || written >= (int)sizeof(filename)) {
            fprintf(stderr, "Filename creation failed or truncated: %s\n", filename);
            cur++;
            continue;
        }

        // Vérifier si le fichier existe
        if (access(filename, F_OK) == -1) {
            fprintf(stderr, "File not found: %s. Skipping.\n", filename);
            cur++;
            continue;
        }

        double *temp = Output_Array(filename);
        if (temp == NULL) {
            fprintf(stderr, "Failed to load image: %s. Skipping.\n", filename);
            cur++;
            continue;
        }

        for (int j = 0; j < 784; j++) {
            inputs[valid_samples][j] = temp[j];
        }
        free(temp);

        valid_samples++;
        if (cur % 50 == 49) cur += 50;
        cur++;
    }

    if (valid_samples > 0) {
        train(&nn, inputs, targets, valid_samples);
        save_network(model_path, &nn, 784, 256, 26);
    } else {
        fprintf(stderr, "No valid samples found. Training aborted.\n");
    }

    free(targets);
    free(inputs);
    free_network(&nn);
}


void predict_character(char *model_path, char *image_path) {
    char predicted_char = predict_char(image_path, model_path);
    if (predicted_char != '\0') {
        printf("The predicted character is: %c\n", predicted_char);
    } else {
        fprintf(stderr, "Prediction failed.\n");
    }
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <mode> <model_path> [<image_path>]\n", argv[0]);
        return 1;
    }

    char *mode = argv[1];
    char *model_path = argv[2];

    if (strcmp(mode, "train") == 0) {
        train_and_save(model_path);
        return 0; // Empêche de continuer à exécuter la prédiction
    } else if (strcmp(mode, "predict") == 0) {
        if (argc < 4) {
            fprintf(stderr, "Image path is required for prediction.\n");
            return 1;
        }
        char *image_path = argv[3];
        predict_character(model_path, image_path);
        return 0;
    } else {
        fprintf(stderr, "Invalid mode. Use 'train' or 'predict'.\n");
        return 1;
    }
}
