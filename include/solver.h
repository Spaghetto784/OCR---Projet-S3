#ifndef SOLVER_H
#define SOLVER_H

#define MAX_GRID_SIZE 100  // Assurer la cohérence avec solver.c

// Déclarations des fonctions
int read_grid(const char *filename, char grid[MAX_GRID_SIZE][MAX_GRID_SIZE], int *rows, int *cols);
void find_word_in_grid(char grid[MAX_GRID_SIZE][MAX_GRID_SIZE], int rows, int cols, const char *word);

#endif
