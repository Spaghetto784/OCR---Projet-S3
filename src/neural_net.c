#include <math.h>
#include <stdlib.h>
#include <stdio.h> 
#include <SDL.h>
#include <SDL_image.h>
#include <dirent.h>  
#include <sys/types.h> 



const char test[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";


#define NombreDeInput 576  
#define NombreDeNoeudCache 80 
#define NombreDeSortis 52
#define entrainement 618

SDL_Surface* Createur_surface(char *a) {
    SDL_Surface *surface = IMG_Load(a);
    if (surface == NULL) {
        printf("Problème avec l'image %s: %s\n", a, SDL_GetError());
        return NULL;
    }
    return surface;
}

int max_index(double *array, int size) {
    int max_idx = 0;
    for (int i = 1; i < size; i++) {
        if (array[i] > array[max_idx]) {
            max_idx = i;
        }
    }
    return max_idx;
}

void softmax(double *output, int size) {
    double sum = 0.0;
    for (int i = 0; i < size; i++) {
        output[i] = exp(output[i]);
        sum += output[i];
    }
    for (int i = 0; i < size; i++) {
        output[i] /= sum;
    }
}

double sigmoid(double x){
    return 1.0 / (1.0 + exp(-x));  // Fonction sigmoid
}

double derive(double x){
    return x * (1.0 - x);  // Dérivée de sigmoid, où x = sigmoid(z)
}

void melange(int *tableau, size_t n){ 
    for (size_t i = 0; i < n-1; i++) {
        size_t j = i + rand() / (RAND_MAX / (n - i) + 1 );
        int t  = tableau[j];
        tableau[j] = tableau[i];
        tableau[i] = t;
    }
} 

double poiddd(){
    return ((double)rand()) / ((double)RAND_MAX) - 0.5; // Valeurs entre -0.5 et 0.5
}


char get_letter_from_one_hot(double *one_hot, int size) {
    for (int i = 0; i < size; i++) {
        if (one_hot[i] == 1.0) {
            return test[i];
        }
    }
    return '?';
}

int get_letter_index(char letter) {
    for (size_t i = 0; i < sizeof(test) - 1; i++) { 
        if (test[i] == letter) {
            return i; 
        }
    }
    return -1;
}


// Fonction pour redimensionner une image à 24x24 pixels
SDL_Surface* resize_image(SDL_Surface* original, int width, int height) {
    SDL_Surface* resized = SDL_CreateRGBSurface(0, width, height, original->format->BitsPerPixel,
                                               original->format->Rmask,
                                               original->format->Gmask,
                                               original->format->Bmask,
                                               original->format->Amask);
    if (resized == NULL) {
        printf("Erreur lors de la création de la surface redimensionnée : %s\n", SDL_GetError());
        return NULL;
    }

    // Effectuer le redimensionnement
    if (SDL_BlitScaled(original, NULL, resized, NULL) != 0) {
        printf("Erreur lors du redimensionnement de l'image : %s\n", SDL_GetError());
        SDL_FreeSurface(resized);
        return NULL;
    }

    return resized;
}


void process_training_files_combined(const char *directory, double TrainingInputs[entrainement][NombreDeInput], double TrainingOutputs[entrainement][NombreDeSortis]) {
    struct dirent *entry;
    DIR *dir = opendir(directory);

    if (dir == NULL) {
        printf("Erreur : Impossible d'ouvrir le répertoire %s\n", directory);
        return;
    }

    int file_count = 0;
    while ((entry = readdir(dir)) != NULL && file_count < entrainement) {
        if (entry->d_name[0] == '.') {
            continue;
        }
        char path[256];
        snprintf(path, sizeof(path), "%s/%s", directory, entry->d_name);
        SDL_Surface *image = IMG_Load(path);
        if (image == NULL) {
            printf("Erreur lors du chargement de l'image %s : %s\n", path, SDL_GetError());
            continue;
        }

        // Redimensionner l'image à 24x24 pixels si nécessaire
        if (image->w != 24 || image->h != 24) {
            printf("Redimensionnement de l'image %s de %dx%d en 24x24 pixels.\n", path, image->w, image->h);
            SDL_Surface *resized_image = resize_image(image, 24, 24);
            SDL_FreeSurface(image); // Libérer la surface originale
            if (resized_image == NULL) {
                printf("Erreur lors du redimensionnement de l'image %s.\n", path);
                continue;
            }
            image = resized_image; // Utiliser l'image redimensionnée
        }

        Uint32 *pixel_data = (Uint32 *)image->pixels;
        for (int y = 0; y < 24; y++) {
            for (int x = 0; x < 24; x++) {
                Uint32 pixel = pixel_data[y * image->w + x];
                Uint8 r, g, b;
                SDL_GetRGB(pixel, image->format, &r, &g, &b);
                double grayscale_value = (0.3 * r + 0.59 * g + 0.11 * b) / 255.0;
                TrainingInputs[file_count][y * 24 + x] = grayscale_value > 0.5 ? 1.0 : 0.0; 
            }
        }

        // Traitement des sorties
        char letter = entry->d_name[0];
        int index = get_letter_index(letter);
        if (index != -1) {
            TrainingOutputs[file_count][index] = 1.0;
            printf("Fichier : %s, Lettre : %c, Index : %d\n", entry->d_name, letter, index);
            file_count++;
        } else {
            printf("Erreur : Lettre '%c' non trouvée pour le fichier %s\n", letter, entry->d_name);
        }

        printf("Image traitée : %s\n", path);
        SDL_FreeSurface(image);
    }

    closedir(dir);
}
void save_parameters(const char *filename, 
                     double HiddenWeights[NombreDeInput][NombreDeNoeudCache],
                     double OutputWeights[NombreDeNoeudCache][NombreDeSortis],
                     double HiddenLayerBias[NombreDeNoeudCache],
                     double OutputLayerBias[NombreDeSortis]) {
    FILE *file = fopen(filename, "wb");
    if (file == NULL) {
        printf("Erreur : Impossible d'ouvrir le fichier %s pour l'écriture\n", filename);
        return;
    }

    // Sauvegarder HiddenWeights
    fwrite(HiddenWeights, sizeof(double), NombreDeInput * NombreDeNoeudCache, file);

    // Sauvegarder OutputWeights
    fwrite(OutputWeights, sizeof(double), NombreDeNoeudCache * NombreDeSortis, file);

    // Sauvegarder HiddenLayerBias
    fwrite(HiddenLayerBias, sizeof(double), NombreDeNoeudCache, file);

    // Sauvegarder OutputLayerBias
    fwrite(OutputLayerBias, sizeof(double), NombreDeSortis, file);

    fclose(file);
    printf("Les paramètres ont été sauvegardés dans %s\n", filename);
}

int main()
{
    const double pa = 0.1;  

    double HiddenLayer[NombreDeNoeudCache]; 
    double OutputLayer[NombreDeSortis]; // Liste des noeuds de sorties

    double HiddenLayerBias[NombreDeNoeudCache]; // Biais pour la couche cachée
    double OutputLayerBias[NombreDeSortis]; // Biais pour la sortie

    double HiddenWeights[NombreDeInput][NombreDeNoeudCache]; 
    double OutputWeights[NombreDeNoeudCache][NombreDeSortis]; 

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("Erreur lors de l'initialisation de SDL : %s\n", SDL_GetError());
        return 1;
    }
    if ((IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG) != IMG_INIT_PNG) {
        printf("Erreur lors de l'initialisation de SDL_image : %s\n", IMG_GetError());
        SDL_Quit();
        return 1;
    }

    double TrainingInputs[entrainement][NombreDeInput];
    double TrainingOutputs[entrainement][NombreDeSortis];


    process_training_files_combined("data/dataset", TrainingInputs, TrainingOutputs);



    // Initialisation des poids
    for (int i = 0; i < NombreDeInput; i++){
        for (int j = 0; j < NombreDeNoeudCache; j++){
            HiddenWeights[i][j] = poiddd(); // Poids entre l'entrée et les noeuds cachés
        }
    }
    
    for (int i = 0; i < NombreDeNoeudCache; i++){
        for (int j = 0; j < NombreDeSortis; j++){
            OutputWeights[i][j] = poiddd(); // Poids entre les noeuds cachés et les sorties
        }
    }
    
    for (int i = 0; i < NombreDeSortis; i++){
        OutputLayerBias[i] = poiddd();
    }

    for (int i = 0; i < NombreDeNoeudCache; i++) {
        HiddenLayerBias[i] = poiddd();
    }

    int trainingSetOrder[entrainement];
    for (int i = 0; i < entrainement; i++) {
        trainingSetOrder[i] = i;
    }

    int numberOfEpochs = 100; // Nombre d'époques

    // Entraînement du réseau de neurones
    for (int epoch = 0; epoch < numberOfEpochs; epoch++)
    {
        melange(trainingSetOrder, entrainement);
        for (int x = 0; x < entrainement; x++)
        {
            int i = trainingSetOrder[x];

            // Réinitialiser la couche de sortie
            for (int j = 0; j < NombreDeSortis; j++) {
                OutputLayer[j] = 0.0;
            }

            // Calcul de l'activation de la couche cachée
            for (int j = 0; j < NombreDeNoeudCache; j++)
            {
                double activation = HiddenLayerBias[j];
                for (int k = 0; k < NombreDeInput; k++)
                {
                    activation += TrainingInputs[i][k] * HiddenWeights[k][j];
                }
                HiddenLayer[j] = sigmoid(activation);
            }

            // Calcul de l'activation de la couche de sortie
            for (int j = 0; j < NombreDeSortis; j++) {
                double activation = OutputLayerBias[j];
                for (int k = 0; k < NombreDeNoeudCache; k++) {
                    activation += HiddenLayer[k] * OutputWeights[k][j];
                }
                OutputLayer[j] = activation; // Pas de sigmoïde ici, softmax sera appliqué
            }
            softmax(OutputLayer, NombreDeSortis);

            printf("Input: %g %g  Predicted Letter: %c   Expected Letter: %c\n\n",
                TrainingInputs[i][0],
                TrainingInputs[i][1],
                test[max_index(OutputLayer, NombreDeSortis)], 
                get_letter_from_one_hot(TrainingOutputs[i], NombreDeSortis));

            // Calcul des deltas pour la sortie
            double deltaOutput[NombreDeSortis];
            for (int j = 0; j < NombreDeSortis; j++)
            {
                double error = (TrainingOutputs[i][j] - OutputLayer[j]);
                deltaOutput[j] = error;
            }

            // Calcul des deltas pour la couche cachée
            double deltaHidden[NombreDeNoeudCache];
            for (int j = 0; j < NombreDeNoeudCache; j++)
            {
                double error = 0.0;
                for (int k = 0; k < NombreDeSortis; k++)
                {
                    error += deltaOutput[k] * OutputWeights[j][k];
                }
                deltaHidden[j] = error * derive(HiddenLayer[j]);
            }

            for (int j = 0; j < NombreDeSortis; j++)
            {
                OutputLayerBias[j] += deltaOutput[j] * pa;
                for (int k = 0; k < NombreDeNoeudCache; k++)
                {
                    OutputWeights[k][j] += HiddenLayer[k] * deltaOutput[j] * pa;
                }
            }

            // Mise à jour des poids et biais de la couche cachée
            for (int j = 0; j < NombreDeNoeudCache; j++)
            {
                HiddenLayerBias[j] += deltaHidden[j] * pa;
                for (int k = 0; k < NombreDeInput; k++)
                {
                    HiddenWeights[k][j] += TrainingInputs[i][k] * deltaHidden[j] * pa;
                }
            }
        }
    }

    save_parameters("neural_network_parameters.bin", HiddenWeights, OutputWeights, HiddenLayerBias, OutputLayerBias);
    IMG_Quit();
    SDL_Quit();
    return 0;
}


