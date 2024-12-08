#include <math.h>
#include <stdlib.h>
#include <stdio.h> 
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <string.h>
#include <dirent.h>  // Pour opendir, readdir, closedir


#define NombreDeInput 576          // Noeud d'entrée (24x24)
#define NombreDeNoeudCache 80      // Noeud par couche cachée
#define NombreDeSortis 52          // Noeud de sorties (A-Z, a-z)

const char letters[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

// Structure pour stocker les prédictions de la grille
typedef struct {
    int row;
    int col;
    char letter;
} Prediction;

// Structure pour stocker les mots avec leurs lettres
typedef struct {
    int word_number;
    int max_position;
    char *letters; // tableau de lettres, indexé par (n-1)
} Word;

// Fonction sigmoid
double sigmoid(double x){
    return 1.0 / (1.0 + exp(-x));  
}

// Dérivée de sigmoid
double DXsigmoid(double x){
    return x * (1.0 - x); 
}

// Fonction softmax
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

// Trouver l'indice maximal dans un tableau
int max_index(double *array, int size) {
    int max_idx = 0;
    for (int i = 1; i < size; i++) {
        if (array[i] > array[max_idx]) {
            max_idx = i;
        }
    }
    return max_idx;
}

int get_letter_index(char letter) {
    for (size_t i = 0; i < sizeof(letters) - 1; i++) { 
        if (letters[i] == letter) {
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

// Fonction pour charger les poids et biais depuis un fichier binaire
int load_model_binary(const char *filename,
                      double HiddenWeights[NombreDeInput][NombreDeNoeudCache],
                      double OutputWeights[NombreDeNoeudCache][NombreDeSortis],
                      double HiddenLayerBias[NombreDeNoeudCache],
                      double OutputLayerBias[NombreDeSortis]) {
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        printf("Erreur : Impossible d'ouvrir le fichier %s pour la lecture.\n", filename);
        return -1;
    }

    // Charger HiddenWeights
    if (fread(HiddenWeights, sizeof(double), NombreDeInput * NombreDeNoeudCache, file) != NombreDeInput * NombreDeNoeudCache) {
        printf("Erreur lors de la lecture des HiddenWeights.\n");
        fclose(file);
        return -1;
    }

    // Charger OutputWeights
    if (fread(OutputWeights, sizeof(double), NombreDeNoeudCache * NombreDeSortis, file) != NombreDeNoeudCache * NombreDeSortis) {
        printf("Erreur lors de la lecture des OutputWeights.\n");
        fclose(file);
        return -1;
    }

    // Charger HiddenLayerBias
    if (fread(HiddenLayerBias, sizeof(double), NombreDeNoeudCache, file) != NombreDeNoeudCache) {
        printf("Erreur lors de la lecture des HiddenLayerBias.\n");
        fclose(file);
        return -1;
    }

    // Charger OutputLayerBias
    if (fread(OutputLayerBias, sizeof(double), NombreDeSortis, file) != NombreDeSortis) {
        printf("Erreur lors de la lecture des OutputLayerBias.\n");
        fclose(file);
        return -1;
    }

    fclose(file);
    printf("Modèle chargé avec succès depuis le fichier %s.\n", filename);
    return 0;
}

// Fonction pour prédire la lettre à partir d'une image
char predict_letter(const char *image_path,
                    double HiddenWeights[NombreDeInput][NombreDeNoeudCache],
                    double OutputWeights[NombreDeNoeudCache][NombreDeSortis],
                    double HiddenLayerBias[NombreDeNoeudCache],
                    double OutputLayerBias[NombreDeSortis]) {
    
    // Charger l'image
    SDL_Surface *image = IMG_Load(image_path);
    if (image == NULL) {
        printf("Erreur lors du chargement de l'image %s : %s\n", image_path, SDL_GetError());
        return '?';
    }

    // Redimensionner l'image à 24x24 pixels
    SDL_Surface *resized_image = resize_image(image, 24, 24);
    SDL_FreeSurface(image); // Libérer la surface originale
    if (resized_image == NULL) {
        return '?';
    }

    // Préparer le vecteur d'entrée
    double input[NombreDeInput];
    Uint32 *pixel_data = (Uint32 *)resized_image->pixels;
    for (int y = 0; y < 24; y++) {
        for (int x = 0; x < 24; x++) {
            Uint32 pixel = pixel_data[y * resized_image->w + x];
            Uint8 r, g, b;
            SDL_GetRGB(pixel, resized_image->format, &r, &g, &b);
            double grayscale_value = (0.3 * r + 0.59 * g + 0.11 * b) / 255.0;
            input[y * 24 + x] = grayscale_value > 0.5 ? 1.0 : 0.0;
        }
    }

    SDL_FreeSurface(resized_image); 

    // Propagation Avant

    // Calcul des activations de la couche cachée
    double HiddenLayer[NombreDeNoeudCache];
    for (int j = 0; j < NombreDeNoeudCache; j++) {
        double activation = HiddenLayerBias[j];
        for (int k = 0; k < NombreDeInput; k++) {
            activation += input[k] * HiddenWeights[k][j];
        }
        HiddenLayer[j] = sigmoid(activation);
    }

    // Calcul des activations de la couche de sortie
    double OutputLayer[NombreDeSortis];
    for (int j = 0; j < NombreDeSortis; j++) {
        double activation = OutputLayerBias[j];
        for (int k = 0; k < NombreDeNoeudCache; k++) {
            activation += HiddenLayer[k] * OutputWeights[k][j];
        }
        OutputLayer[j] = activation; 
    }
    softmax(OutputLayer, NombreDeSortis);
    int predicted_index = max_index(OutputLayer, NombreDeSortis);
    char predicted_letter = letters[predicted_index];
    //printf("La lettre prédite pour l'image %s est : %c\n", image_path, predicted_letter);

    return predicted_letter;
}

// Fonction pour vérifier si un fichier a une extension PNG
int is_png(const char *filename) {
    const char *dot = strrchr(filename, '.');
    if(!dot || dot == filename) return 0;
    return (strcasecmp(dot, ".png") == 0);
}

// Fonction pour extraire les positions de la grille à partir du nom du fichier
int parse_grid_position(const char *filename, int *row, int *col) {
    // Vérifier si le nom du fichier correspond au format grid(r,c).png ou grid(r,c).PNG
    if (sscanf(filename, "grid(%d,%d).png", row, col) == 2)
        return 1;
    if (sscanf(filename, "grid(%d,%d).PNG", row, col) == 2)
        return 1;
    return 0;
}

// Fonction pour extraire les positions des mots à partir du nom du fichier
int parse_word_position(const char *filename, int *word_num, int *letter_pos) {
    // Vérifier si le nom du fichier correspond au format word(m,n).png ou word(m,n).PNG
    if (sscanf(filename, "word(%d,%d).png", word_num, letter_pos) == 2)
        return 1;
    if (sscanf(filename, "word(%d,%d).PNG", word_num, letter_pos) == 2)
        return 1;
    return 0;
}

// Fonction de comparaison pour qsort (prédictions de la grille)
int compare_predictions(const void *a, const void *b) {
    Prediction *pa = (Prediction *)a;
    Prediction *pb = (Prediction *)b;
    if (pa->row != pb->row)
        return pa->row - pb->row;
    else
        return pa->col - pb->col;
}

// Fonction de comparaison pour qsort (mots)
int compare_words(const void *a, const void *b) {
    Word *wa = (Word *)a;
    Word *wb = (Word *)b;
    return wa->word_number - wb->word_number;
}

int main(int argc, char *argv[])
{
    // Déclaration des poids et biais
    double HiddenLayerBias[NombreDeNoeudCache];
    double OutputLayerBias[NombreDeSortis];
    double HiddenWeights[NombreDeInput][NombreDeNoeudCache];
    double OutputWeights[NombreDeNoeudCache][NombreDeSortis];

    // Initialisation de SDL et SDL_image
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("Erreur lors de l'initialisation de SDL : %s\n", SDL_GetError());
        return 1;
    }
    if ((IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG) != IMG_INIT_PNG) {
        printf("Erreur lors de l'initialisation de SDL_image : %s\n", IMG_GetError());
        SDL_Quit();
        return 1;
    }

    // Charger le modèle depuis le fichier binaire
    if (load_model_binary("neural_network_parameters.bin",
                          HiddenWeights,
                          OutputWeights,
                          HiddenLayerBias,
                          OutputLayerBias) != 0) {
        printf("Erreur lors du chargement du modèle. Assurez-vous que le fichier model_parameters.bin existe.\n");
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    // Définir le répertoire de test
    const char *test_directory;
    if (argc > 1) {
        test_directory = argv[1];
    } else {
        test_directory = "./test";
    }

    printf("Chemin du dossier de test utilisé : %s\n", test_directory);

    // Ouvrir le dossier
    DIR *dir = opendir(test_directory);
    if (dir == NULL) {
        printf("Erreur : Impossible d'ouvrir le dossier %s\n", test_directory);
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    struct dirent *entry;
    int processed_files = 0;

    // Définir une capacité initiale pour les prédictions de la grille
    int capacity_predictions = 100;
    Prediction *predictions = malloc(capacity_predictions * sizeof(Prediction));
    if (predictions == NULL) {
        printf("Erreur : Mémoire insuffisante pour stocker les prédictions.\n");
        closedir(dir);
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    // Définir une capacité initiale pour les mots
    int word_capacity = 10;
    int word_count = 0;
    Word *words = malloc(word_capacity * sizeof(Word));
    if (words == NULL) {
        printf("Erreur : Mémoire insuffisante pour stocker les mots.\n");
        free(predictions);
        closedir(dir);
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    // Parcourir chaque fichier dans le dossier
    while ((entry = readdir(dir)) != NULL) {
        // Ignorer les fichiers spéciaux "." et ".."
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        // Construire le chemin complet du fichier
        char filepath[512];
        snprintf(filepath, sizeof(filepath), "%s/%s", test_directory, entry->d_name);

        // Vérifier si c'est un fichier PNG
        if (!is_png(entry->d_name)) {
            printf("Ignoré (pas un fichier PNG) : %s\n", filepath);
            continue;
        }

        // Extraire le nom de fichier sans chemin
        char *filename = strdup(entry->d_name);
        if (filename == NULL) {
            printf("Erreur : Mémoire insuffisante pour copier le nom de fichier.\n");
            continue;
        }

        // Supprimer les caractères de retour chariot et nouvelle ligne
        filename[strcspn(filename, "\n")] = 0;
        size_t len = strlen(filename);
        if (len > 0 && filename[len - 1] == '\r') {
            filename[len - 1] = '\0';
        }

        // Vérifier si le fichier commence par "grid" ou "word"
        if (strncmp(filename, "grid", 4) == 0) {
            // Traitement des fichiers de grille
            int row, col;
            if (!parse_grid_position(filename, &row, &col)) {
                printf("Ignoré (nom de fichier grid non conforme) : %s\n", filepath);
                free(filename);
                continue;
            }

            // Prédire la lettre
            char predicted = predict_letter(filepath,
                                            HiddenWeights,
                                            OutputWeights,
                                            HiddenLayerBias,
                                            OutputLayerBias);
            printf("Grille - Image : %s, Lettre prédite : %c (Row: %d, Col: %d)\n", filepath, predicted, row, col);

            // Ajouter la prédiction à la liste
            if (processed_files >= capacity_predictions) {
                // Doubler la capacité si nécessaire
                capacity_predictions *= 2;
                Prediction *temp = realloc(predictions, capacity_predictions * sizeof(Prediction));
                if (temp == NULL) {
                    printf("Erreur : Mémoire insuffisante lors de l'agrandissement des prédictions.\n");
                    free(filename);
                    free(predictions);
                    // Libérer les mots
                    for (int j = 0; j < word_count; j++) {
                        free(words[j].letters);
                    }
                    free(words);
                    closedir(dir);
                    IMG_Quit();
                    SDL_Quit();
                    return 1;
                }
                predictions = temp;
            }

            predictions[processed_files].row = row;
            predictions[processed_files].col = col;
            predictions[processed_files].letter = predicted;
            processed_files++;
        }
        else if (strncmp(filename, "word", 4) == 0) {
            // Traitement des fichiers de mots
            int word_num, letter_pos;
            if (!parse_word_position(filename, &word_num, &letter_pos)) {
                printf("Ignoré (nom de fichier word non conforme) : %s\n", filepath);
                free(filename);
                continue;
            }

            // Prédire la lettre
            char predicted = predict_letter(filepath,
                                            HiddenWeights,
                                            OutputWeights,
                                            HiddenLayerBias,
                                            OutputLayerBias);
            printf("Mot - Image : %s, Lettre prédite : %c (Word: %d, Position: %d)\n", filepath, predicted, word_num, letter_pos);

            // Trouver ou créer le mot correspondant
            int found = 0;
            for (int i = 0; i < word_count; i++) {
                if (words[i].word_number == word_num) {
                    // Mettre à jour la lettre dans la position n
                    if (letter_pos > words[i].max_position) {
                        // Réallouer l'espace pour les nouvelles positions
                        char *temp_letters = realloc(words[i].letters, letter_pos * sizeof(char));
                        if (temp_letters == NULL) {
                            printf("Erreur : Mémoire insuffisante lors de la réallocation des lettres.\n");
                            free(filename);
                            // Libérer la mémoire avant de quitter
                            for (int j = 0; j < word_count; j++) {
                                free(words[j].letters);
                            }
                            free(words);
                            free(predictions);
                            closedir(dir);
                            IMG_Quit();
                            SDL_Quit();
                            return 1;
                        }
                        words[i].letters = temp_letters;
                        // Initialiser les nouvelles positions avec des caractères vides
                        for (int p = words[i].max_position; p < letter_pos; p++) {
                            words[i].letters[p] = '\0';
                        }
                        words[i].max_position = letter_pos;
                    }
                    // Assigner la lettre à la position n (index n-1)
                    words[i].letters[letter_pos - 1] = predicted;
                    found = 1;
                    break;
                }
            }

            if (!found) {
                // Ajouter un nouveau mot
                if (word_count >= word_capacity) {
                    // Doubler la capacité si nécessaire
                    word_capacity *= 2;
                    Word *temp = realloc(words, word_capacity * sizeof(Word));
                    if (temp == NULL) {
                        printf("Erreur : Mémoire insuffisante lors de l'agrandissement des mots.\n");
                        free(filename);
                        // Libérer la mémoire avant de quitter
                        for (int j = 0; j < word_count; j++) {
                            free(words[j].letters);
                        }
                        free(words);
                        free(predictions);
                        closedir(dir);
                        IMG_Quit();
                        SDL_Quit();
                        return 1;
                    }
                    words = temp;
                }
                // Initialiser le nouveau mot
                words[word_count].word_number = word_num;
                words[word_count].max_position = letter_pos;
                words[word_count].letters = malloc(letter_pos * sizeof(char));
                if (words[word_count].letters == NULL) {
                    printf("Erreur : Mémoire insuffisante pour stocker les lettres du mot.\n");
                    free(filename);
                    // Libérer la mémoire avant de quitter
                    for (int j = 0; j < word_count; j++) {
                        free(words[j].letters);
                    }
                    free(words);
                    free(predictions);
                    closedir(dir);
                    IMG_Quit();
                    SDL_Quit();
                    return 1;
                }
                // Initialiser toutes les positions avec des caractères vides
                for (int p = 0; p < letter_pos; p++) {
                    words[word_count].letters[p] = '\0';
                }
                // Assigner la lettre à la position n (index n-1)
                words[word_count].letters[letter_pos - 1] = predicted;
                word_count++;
            }
        }
        free(filename);
    }

    closedir(dir);

    // Vérifier et traiter les prédictions de la grille
    if (processed_files > 0) {
        // Trier les prédictions par ligne, puis par colonne
        qsort(predictions, processed_files, sizeof(Prediction), compare_predictions);

        // Déterminer le nombre de lignes et de colonnes
        int max_row = 0, max_col = 0;
        for (int i = 0; i < processed_files; i++) {
            if (predictions[i].row > max_row)
                max_row = predictions[i].row;
            if (predictions[i].col > max_col)
                max_col = predictions[i].col;
        }

        // Créer une grille
        char **grid = malloc((max_row + 1) * sizeof(char *));
        if (grid == NULL) {
            printf("Erreur : Mémoire insuffisante pour créer la grille.\n");
            // Libérer la mémoire avant de quitter
            for (int j = 0; j < word_count; j++) {
                free(words[j].letters);
            }
            free(words);
            free(predictions);
            IMG_Quit();
            SDL_Quit();
            return 1;
        }
        for (int r = 0; r <= max_row; r++) {
            grid[r] = malloc((max_col + 1) * sizeof(char));
            if (grid[r] == NULL) {
                printf("Erreur : Mémoire insuffisante pour créer la grille.\n");
                // Libérer la mémoire allouée
                for (int k = 0; k < r; k++) {
                    free(grid[k]);
                }
                free(grid);
                for (int j = 0; j < word_count; j++) {
                    free(words[j].letters);
                }
                free(words);
                free(predictions);
                IMG_Quit();
                SDL_Quit();
                return 1;
            }
            // Initialiser la grille avec des espaces ou un autre caractère de remplissage
            for (int c = 0; c <= max_col; c++) {
                grid[r][c] = ' '; // Remplissage par défaut
            }
        }

        // Remplir la grille avec les prédictions
        for (int i = 0; i < processed_files; i++) {
            if (predictions[i].row <= max_row && predictions[i].col <= max_col) {
                grid[predictions[i].row][predictions[i].col] = predictions[i].letter;
            }
        }

        // Écrire la grille dans grid.txt
        FILE *grid_file = fopen("grid.txt", "w");
        if (grid_file == NULL) {
            printf("Erreur : Impossible de créer le fichier grid.txt.\n");
            // Libérer la mémoire allouée
            for (int r = 0; r <= max_row; r++) {
                free(grid[r]);
            }
            free(grid);
            for (int j = 0; j < word_count; j++) {
                free(words[j].letters);
            }
            free(words);
            free(predictions);
            IMG_Quit();
            SDL_Quit();
            return 1;
        }

        for (int r = 0; r <= max_row; r++) {
            for (int c = 0; c <= max_col; c++) {
                fprintf(grid_file, "%c", grid[r][c]);
            }
            fprintf(grid_file, "\n"); // Saut de ligne après chaque rangée
        }

        fclose(grid_file);
        printf("Grille enregistrée dans le fichier grid.txt.\n");

        // Libérer la grille
        for (int r = 0; r <= max_row; r++) {
            free(grid[r]);
        }
        free(grid);
    }
    else {
        printf("Aucune prédiction de grille trouvée.\n");
    }

    // Vérifier et traiter les prédictions des mots
    if (word_count > 0) {
        // Trier les mots par numéro de mot
        qsort(words, word_count, sizeof(Word), compare_words);

        // Ouvrir le fichier de sortie pour les mots
        FILE *word_file = fopen("mot.txt", "w");
        if (word_file == NULL) {
            printf("Erreur : Impossible de créer le fichier mot.txt.\n");
            // Libérer la mémoire avant de quitter
            for (int j = 0; j < word_count; j++) {
                free(words[j].letters);
            }
            free(words);
            free(predictions);
            IMG_Quit();
            SDL_Quit();
            return 1;
        }

        // Écrire les mots dans le fichier sans espaces
        for (int i = 0; i < word_count; i++) {
            for (int p = 0; p < words[i].max_position; p++) {
                if (words[i].letters[p] != '\0') { // Ignorer les positions non assignées
                    fprintf(word_file, "%c", words[i].letters[p]);
                }
            }
            fprintf(word_file, "\n"); // Saut de ligne après chaque mot
        }

        fclose(word_file);
        printf("Mots enregistrés dans le fichier mot.txt.\n");

        // Libérer la mémoire allouée pour les mots
        for (int i = 0; i < word_count; i++) {
            free(words[i].letters);
        }
        free(words);
    }
    else {
        printf("Aucun mot trouvé.\n");
    }

    // Libérer la mémoire allouée pour les prédictions de la grille
    free(predictions);

    // Nettoyage SDL
    IMG_Quit();
    SDL_Quit();

    return 0;
}