#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "solver.h"

#define MAX_GRID_SIZE 100

// Convertir une chaîne de caractères en majuscules
void to_uppercase(char *str) {
    for (int i = 0; str[i]; i++) {
        str[i] = toupper((unsigned char)str[i]);
    }
}

// Fonction pour lire la grille depuis un fichier
int read_grid(const char *filename, char grid[MAX_GRID_SIZE][MAX_GRID_SIZE], int *rows, int *cols) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Erreur : Impossible d'ouvrir le fichier %s\n", filename);
        return 0;
    }

    *rows = 0;
    *cols = 0;
    char line[MAX_GRID_SIZE];

    while (fgets(line, sizeof(line), file) && *rows < MAX_GRID_SIZE) {
        int len = strlen(line);
        if (line[len - 1] == '\n') {
            line[len - 1] = '\0';  // Supprimer le saut de ligne
            len--;
        }
        
        for (int j = 0; j < len && j < MAX_GRID_SIZE; j++) {
            grid[*rows][j] = toupper((unsigned char)line[j]);
        }
        *cols = len > *cols ? len : *cols;  // Mettre à jour le nombre de colonnes si nécessaire
        (*rows)++;
    }

    fclose(file);
    return 1;
}

// Fonction pour rechercher un mot dans toutes les directions de la grille
void find_word_in_grid(char grid[MAX_GRID_SIZE][MAX_GRID_SIZE], int rows, int cols, const char *word) {
    char word_upper[MAX_GRID_SIZE];
    strncpy(word_upper, word, MAX_GRID_SIZE - 1);
    word_upper[MAX_GRID_SIZE - 1] = '\0';
    to_uppercase(word_upper);

    int word_len = strlen(word_upper);

    // Directions (haut, bas, gauche, droite, diagonales)
    int directions[8][2] = {
        {0, 1}, {1, 0}, {0, -1}, {-1, 0}, {1, 1}, {1, -1}, {-1, -1}, {-1, 1}
    };

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (grid[i][j] == word_upper[0]) {  // Première lettre correspond
                for (int d = 0; d < 8; d++) {
                    int x = i, y = j, k;
                    for (k = 1; k < word_len; k++) {
                        x += directions[d][0];
                        y += directions[d][1];
                        if (x < 0 || x >= rows || y < 0 || y >= cols || grid[x][y] != word_upper[k]) {
                            break;
                        }
                    }
                    if (k == word_len) {  // Mot trouvé
                        printf("(%d,%d)(%d,%d)\n", j, i, y, x);
                        return;
                    }
                }
            }
        }
    }

    printf("Not Found\n");
}
