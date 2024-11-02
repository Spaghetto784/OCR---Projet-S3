#include <stdio.h>
#include <stdlib.h>

#include "solver.h"

#define MAX_GRID_SIZE 100

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        fprintf(stderr, "Usage: %s <grid_file> <word>\n", argv[0]);
        return EXIT_FAILURE;
    }

    char grid[MAX_GRID_SIZE][MAX_GRID_SIZE];
    int rows, cols;

    // Read the grid from the file
    if (!read_grid(argv[1], grid, &rows, &cols))
    {
        return EXIT_FAILURE;
    }

    // Search for the word in the grid
    find_word_in_grid(grid, rows, cols, argv[2]);

    return EXIT_SUCCESS;
}
