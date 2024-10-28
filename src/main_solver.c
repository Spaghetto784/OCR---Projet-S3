#include <stdio.h>
#include <stdlib.h>
#include "solver.h"

#define MAX_GRID_SIZE 100

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <fichier_grille> <mot>\n", argv[0]);
        return EXIT_FAILURE;
    }

    char grid[MAX_GRID_SIZE][MAX_GRID_SIZE];
    int rows, cols;

    // Lire la grille à partir du fichier
    if (!read_grid(argv[1], grid, &rows, &cols)) {
        return EXIT_FAILURE;
    }

    // Chercher le mot dans la grille
    find_word_in_grid(grid, rows, cols, argv[2]);

    return EXIT_SUCCESS;
}
