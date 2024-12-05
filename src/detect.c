#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>


// Structure to hold information about a file
typedef struct {
    char original_name[256];
    int x;
    int y;
} LetterFile;

// Define a cluster
typedef struct {
    int minX, minY, maxX, maxY;
    int area;
    int centerX, centerY; // Cluster center
} Cluster;

// Calculate the distance between two points
float distance(int x1, int y1, int x2, int y2) {
    return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
}

// Flood fill function to detect clusters
void flood_fill(SDL_Surface *surface, int x, int y, int* visited, int width, int height, int* minX, int* minY, int* maxX, int* maxY) {
    if (x < 0 || x >= width || y < 0 || y >= height) return;
    if (visited[y * width + x]) return;
    Uint32 pixel = ((Uint32*)surface->pixels)[y * width + x];
    Uint8 r, g, b;
    SDL_GetRGB(pixel, surface->format, &r, &g, &b);
    if (r == 0 && g == 0 && b == 0) { // Black pixel
        visited[y * width + x] = 1;

        if (x < *minX) *minX = x;
        if (x > *maxX) *maxX = x;
        if (y < *minY) *minY = y;
        if (y > *maxY) *maxY = y;

        flood_fill(surface, x + 1, y, visited, width, height, minX, minY, maxX, maxY);
        flood_fill(surface, x - 1, y, visited, width, height, minX, minY, maxX, maxY);
        flood_fill(surface, x, y + 1, visited, width, height, minX, minY, maxX, maxY);
        flood_fill(surface, x, y - 1, visited, width, height, minX, minY, maxX, maxY);
    }
}

//save the cluster of letters
void save_cluster(SDL_Surface *surface, int minX, int minY, int maxX, int maxY, const char *folder, int x_index, int y_index) {
    // Verify input dimensions are valid
    if (minX < 0 || minY < 0 || maxX >= surface->w || maxY >= surface->h || minX > maxX || minY > maxY) {
        printf("Invalid cluster dimensions.\n");
        return;
    }

    // Dimensions of the cluster
    int width = maxX - minX + 1;
    int height = maxY - minY + 1;

    // Create a surface for the cluster
    SDL_Surface *cluster_surface = SDL_CreateRGBSurface(0, width, height, surface->format->BitsPerPixel,
                                                        surface->format->Rmask, surface->format->Gmask,
                                                        surface->format->Bmask, surface->format->Amask);
    if (!cluster_surface) {
        printf("Error creating cluster surface: %s\n", SDL_GetError());
        return;
    }

    // Copy pixels from the original surface to the cluster surface
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            Uint32 pixel = ((Uint32 *)surface->pixels)[(minY + y) * surface->w + (minX + x)];
            ((Uint32 *)cluster_surface->pixels)[y * width + x] = pixel;
        }
    }

    // Create a 24x24 white target surface
    int target_size = 24;
    SDL_Surface *final_surface = SDL_CreateRGBSurface(0, target_size, target_size, surface->format->BitsPerPixel,
                                                      surface->format->Rmask, surface->format->Gmask,
                                                      surface->format->Bmask, surface->format->Amask);
    if (!final_surface) {
        printf("Error creating target surface: %s\n", SDL_GetError());
        SDL_FreeSurface(cluster_surface);
        return;
    }

    // Fill the target surface with white
    Uint32 white_pixel = SDL_MapRGB(final_surface->format, 255, 255, 255);
    SDL_FillRect(final_surface, NULL, white_pixel);

    // Calculate the dimensions for resizing while maintaining the aspect ratio
    int available_height = target_size - 6; // 3 pixels margin at the top and bottom
    int available_width = target_size;

    float scale = fmin((float)available_height / height, (float)available_width / width);
    int resized_width = width * scale;
    int resized_height = height * scale;

    // Calculate the centered position for pasting the resized cluster
    int paste_x = (target_size - resized_width) / 2;
    int paste_y = 3; // 3 pixels from the top

    // Resize and paste the cluster into the white 24x24 surface
    for (int y = 0; y < resized_height; y++) {
        for (int x = 0; x < resized_width; x++) {
            // Calculate the source position in the original cluster
            int src_x = x / scale;
            int src_y = y / scale;

            // Ensure the source position is within bounds
            if (src_x < width && src_y < height) {
                Uint32 pixel = ((Uint32 *)cluster_surface->pixels)[src_y * width + src_x];
                ((Uint32 *)final_surface->pixels)[(paste_y + y) * target_size + (paste_x + x)] = pixel;
            }
        }
    }

    // Generate the filename
    char filename[256];
    snprintf(filename, sizeof(filename), "%s/lettre_%03d_%03d.png", folder, x_index, y_index);

    // Save the 24x24 image
    if (IMG_SavePNG(final_surface, filename) != 0) {
        printf("Error saving PNG: %s\n", IMG_GetError());
    } else {
        printf("Cluster saved in %s\n", filename);
    }

    // Free surfaces
    SDL_FreeSurface(cluster_surface);
    SDL_FreeSurface(final_surface);
}

//detect the cluster of letters
void detect_clusters(SDL_Surface *surface) {
    int width = surface->w;
    int height = surface->h;
    Uint32* pixels = (Uint32*)surface->pixels;

    int* visited = (int*)calloc(width * height, sizeof(int));


    Cluster clusters[1000];
    int cluster_count = 0;

    // Traverse the image to detect clusters
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            Uint32 pixel = pixels[y * width + x];
            Uint8 r, g, b;
            SDL_GetRGB(pixel, surface->format, &r, &g, &b);
            if (r == 0 && g == 0 && b == 0 && !visited[y * width + x]) {
                int minX = x, minY = y, maxX = x, maxY = y;
                flood_fill(surface, x, y, visited, width, height, &minX, &minY, &maxX, &maxY);

                // Calculate the cluster's area (size)
                int area = (maxX - minX + 1) * (maxY - minY + 1);

                // Calculate the cluster center
                int centerX = (minX + maxX) / 2;
                int centerY = (minY + maxY) / 2;

                // Add this cluster to the detected clusters list
                clusters[cluster_count++] = (Cluster){minX, minY, maxX, maxY, area, centerX, centerY};
            }
        }
    }

    // Threshold for distance between clusters
    float distance_threshold = 20.0; // Adjust as needed
    int lastY = clusters[0].centerY;

    for (int i = 0; i < cluster_count; i++) {
        int is_in_grid = 0;

        // Compare with other clusters
        for (int j = 0; j < cluster_count; j++) {
            if (i != j) {
                float dist = distance(clusters[i].centerX, clusters[i].centerY, clusters[j].centerX, clusters[j].centerY);
                if (dist < distance_threshold) {
                    is_in_grid = 1; // The cluster belongs to the grid
                    break;
                }
            }
        }

        // Assign a color based on classification
        Uint32 color;
        const char *folder;

        if (is_in_grid) {
            color = SDL_MapRGB(surface->format, 255, 0, 0); // Red for the grid
            folder = "letterList";
        } else {
            color = SDL_MapRGB(surface->format, 0, 0, 255); // Blue for the list
            folder = "letterGrid";
        }
        if (clusters[i].centerY - lastY >= 10) {
            lastY = clusters[i].centerY;
        }

        // Save the cluster in the appropriate folder with separate indices
        save_cluster(surface, clusters[i].minX, clusters[i].minY, clusters[i].maxX, clusters[i].maxY, folder, clusters[i].centerX, lastY);

        // Draw a square around the cluster
        for (int x = clusters[i].minX; x <= clusters[i].maxX; x++) {
            if (clusters[i].minY >= 0 && clusters[i].minY < height) pixels[clusters[i].minY * width + x] = color; // Top line
            if (clusters[i].maxY >= 0 && clusters[i].maxY < height) pixels[clusters[i].maxY * width + x] = color; // Bottom line
        }
        for (int y = clusters[i].minY; y <= clusters[i].maxY; y++) {
            if (clusters[i].minX >= 0 && clusters[i].minX < width) pixels[y * width + clusters[i].minX] = color; // Left column
            if (clusters[i].maxX >= 0 && clusters[i].maxX < width) pixels[y * width + clusters[i].maxX] = color; // Right column
        }
    }
    free(visited);
}


// Comparator for sorting by y first, then x
int compareFile(const void *a, const void *b) {
    LetterFile *fileA = (LetterFile *)a;
    LetterFile *fileB = (LetterFile *)b;

    if (fileA->y != fileB->y) {
        return fileA->y - fileB->y; // Sort by y-coordinate
    }
    return fileA->x - fileB->x; // Sort by x-coordinate
}

// Function to parse the filename and extract x and y coordinates
int parse_filename(const char *filename, int *x, int *y) {
    return sscanf(filename, "lettre_%d_%d.png", x, y) == 2;
}

// Function to rename files
void rename_files(const char *directory) {
    DIR *dir = opendir(directory);
    if (!dir) {
        perror("opendir");
        exit(EXIT_FAILURE);
    }

    LetterFile files[1000]; // Array to store file information
    int file_count = 0;

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        // Process files starting with "lettre_" and ending with ".png"
        if (strncmp(entry->d_name, "lettre_", 7) == 0 && strstr(entry->d_name, ".png")) {
            int x, y;
            if (parse_filename(entry->d_name, &x, &y)) {
                strcpy(files[file_count].original_name, entry->d_name);
                files[file_count].x = x;
                files[file_count].y = y;
                file_count++;
                printf("DEBUG: Added file: %s (x=%d, y=%d)\n", entry->d_name, x, y);
            }
        }
    }

    closedir(dir);

    // Check if any files were collected
    if (file_count == 0) {
        printf("No files to process in directory: %s\n", directory);
        return;
    }

    printf("DEBUG: Total files collected: %d\n", file_count);

    // Sort files by y-coordinate first, then x-coordinate
    qsort(files, file_count, sizeof(LetterFile), compareFile);

    // Rename files based on the new naming scheme
    int current_row = 0;
    int last_y = files[0].y;
    int file_in_row = 0; // Track the file position in the current row

    for (int i = 0; i < file_count; i++) {
        // Increment the row when y changes significantly
        if (abs(files[i].y - last_y) > 10) {
            current_row++;       // Move to the next row
            last_y = files[i].y; // Update the reference y-coordinate
            file_in_row = 0;     // Reset file position for the new row
        }

        // Create the new filename
        char new_name[256];
        snprintf(new_name, sizeof(new_name), "%02d_%02d_%03d_%03d.png", current_row, file_in_row, files[i].x, files[i].y);

        // Build full paths for renaming
        char old_path[512], new_path[512];
        snprintf(old_path, sizeof(old_path), "%s/%s", directory, files[i].original_name);
        snprintf(new_path, sizeof(new_path), "%s/%s", directory, new_name);

        // Rename the file
        if (rename(old_path, new_path) != 0) {
            perror("rename");
        } else {
            printf("Renamed %s to %s\n", old_path, new_name);
        }

        // Increment the file position in the row
        file_in_row++;
    }

}

void traceLigne(SDL_Surface *surface, int x1, int y1, int x2, int y2, int thickness) {
    // Check if the surface is valid
    if (!surface) {
        printf("Invalid surface.\n");
        return;
    }

    // Lock the surface if required
    if (SDL_MUSTLOCK(surface) && SDL_LockSurface(surface) != 0) {
        printf("Error locking the surface: %s\n", SDL_GetError());
        return;
    }

    // Determine the differences between the coordinates
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;

    int err = dx - dy;

    // Function to draw a pixel with some deviation around the line
    void drawPixel(int x, int y) {
        if (x >= 0 && x < surface->w && y >= 0 && y < surface->h) {
            Uint32 *pixels = (Uint32 *)surface->pixels;
            Uint32 pixel_color = SDL_MapRGB(surface->format, 0, 0, 0); // Black color
            pixels[y * surface->w + x] = pixel_color;
        }
    }

    // Draw the line with thickness
    while (1) {
        // Draw the main line
        drawPixel(x1, y1);

        // Draw pixels around the line for thickness
        for (int i = -thickness / 2; i <= thickness / 2; i++) {
            for (int j = -thickness / 2; j <= thickness / 2; j++) {
                if (i != 0 || j != 0) { // Avoid redrawing the central pixel
                    drawPixel(x1 + i, y1 + j);
                }
            }
        }

        // Check if the line is complete
        if (x1 == x2 && y1 == y2) {
            break;
        }

        int e2 = 2 * err;

        // Update x and y based on the error
        if (e2 > -dy) {
            err -= dy;
            x1 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y1 += sy;
        }
    }

    // Unlock the surface if required
    if (SDL_MUSTLOCK(surface)) {
        SDL_UnlockSurface(surface);
    }
}

// Function to draw lines from a list of integers
void traceLinesFromList(SDL_Surface *surface, int *list, int size) {
    if (!surface) {
        printf("Invalid surface.\n");
        return;
    }
    if (size < 4) {
        printf("The list must contain at least 4 integers to draw a line.\n");
        return;
    }
    if (size % 2 != 0) {
        printf("The list must contain an even number of integers.\n");
        return;
    }

    // Iterate through the list to draw lines between each pair of points
    for (int i = 0; i < size - 2; i += 4) {
        int x1 = list[i];
        int y1 = list[i + 1];
        int x2 = list[i + 2];
        int y2 = list[i + 3];

        // Draw a line between points (x1, y1) and (x2, y2)
        traceLigne(surface, x1, y1, x2, y2, 3);
    }
}

void extractCoordinatesFromFiles(const char *directory, int *baseList, int baseListSize, int **resultList, int *resultListSize) {
    DIR *dir = opendir(directory);
    if (!dir) {
        printf("Error opening directory: %s\n", strerror(errno));
        return;
    }

    struct dirent *entry;

    // Iterate through each pair of coordinates in baseList
    for (int i = 0; i < baseListSize; i += 2) {
        int x1 = baseList[i];
        int y1 = baseList[i + 1];

        // Search the directory for a file that starts with x1_y1
        rewinddir(dir);  // Reset the directory position for each iteration
        while ((entry = readdir(dir)) != NULL) {
            // Check if the file has a .png extension and starts with x1_y1
            if (strstr(entry->d_name, ".png") != NULL) {
                int x2, y2;
                // Example file name: "03_01_086_179.png"
                if (sscanf(entry->d_name, "%d_%d_%d_%d.png", &x1, &y1, &x2, &y2) == 4) {
                    // Check if the file matches the coordinates x1, y1
                    if (x1 == baseList[i] && y1 == baseList[i + 1]) {
                        // Add the next coordinates (x2, y2) to the new list
                        *resultList = realloc(*resultList, (*resultListSize + 2) * sizeof(int));
                        if (*resultList == NULL) {
                            printf("Memory allocation error\n");
                            closedir(dir);
                            return;
                        }

                        (*resultList)[*resultListSize] = x2;    // Add x2
                        (*resultList)[*resultListSize + 1] = y2; // Add y2
                        *resultListSize += 2; // Increase the list size
                        break; // Exit as soon as the matching file is found
                    }
                }
            }
        }
    }

    closedir(dir);
}


// Fonction de détection avec grille/liste séparée
void detect(SDL_Surface *surface) {
    if (surface->format->BytesPerPixel != 3 && surface->format->BytesPerPixel != 4) {
        printf("Unsupported image format for grayscale conversion.\n");
        return;
    }
    const char *gridPath = "letterGrid";
    const char *listPath = "letterList";

    mkdir(gridPath, 0777);
    mkdir(listPath, 0777);

    detect_clusters(surface);

    rename_files(gridPath);
    rename_files(listPath);

    /*
    int listedebaselevel11[] = {
    0, 2, 0, 11,   // 0, 2 -> 0, 11
    2, 4, 2, 9,    // 2, 4 -> 2, 9
    3, 3, 3, 7,    // 3, 3 -> 3, 7
    4, 2, 7, 2,    // 4, 2 -> 7, 2
    8, 3, 4, 3,    // 8, 3 -> 4, 3
    5, 5, 10, 5,   // 5, 5 -> 10, 5
    8, 6, 13, 11,  // 8, 6 -> 13, 11
    7, 10, 3, 10,  // 7, 10 -> 3, 10
    9, 8, 9, 11,   // 9, 8 -> 9, 11
    2, 4, 2, 9,    // 2, 4 -> 2, 9
    12, 8, 12, 0,  // 12, 8 -> 12, 0
    10, 0, 1, 0,   // 10, 0 -> 1, 0
    11, 8, 11, 0   // 11, 8 -> 11, 0
};
    int tailleListeBaselevel11 = sizeof(listedebaselevel11) / sizeof(listedebaselevel11[0]);

    // Liste de coordonnées résultantes
    int *listeCoordonneeslevel11 = NULL;
    int tailleListeCoordonneeslevel11 = 0;

    // Extraire les coordonnées depuis les fichiers
    extractCoordinatesFromFiles(gridPath, listedebaselevel11, tailleListeBaselevel11, &listeCoordonneeslevel11, &tailleListeCoordonneeslevel11);
    traceLinesFromList(surface, listeCoordonneeslevel11, tailleListeCoordonneeslevel11);
    */

   /*
    int listedebaselevel12[] = {
    5, 9, 5, 15,    // 3, 3 -> 3, 7
    0, 10, 4, 6,    // 4, 2 -> 7, 2
    1, 16, 4, 13,    // 8, 3 -> 4, 3
    13, 12, 13, 6,   // 5, 5 -> 10, 5
    1, 11, 6, 6,  // 8, 6 -> 13, 11
    1, 0, 4, 0,  // 7, 10 -> 3, 10
    6, 12, 12, 12,   // 9, 8 -> 9, 11
    12, 15, 7, 15,    // 2, 4 -> 2, 9
    10, 7, 7, 10,  // 12, 8 -> 12, 0
    10, 4, 0, 4,   // 10, 0 -> 1, 0
};

    
    
    int tailleListeBaselevel12 = sizeof(listedebaselevel12) / sizeof(listedebaselevel12[0]);

    

    // Liste de coordonnées résultantes
    int *listeCoordonneeslevel12 = NULL;
    int tailleListeCoordonneeslevel12 = 0;

    // Dossier contenant les fichiers d'image

    
    extractCoordinatesFromFiles(gridPath, listedebaselevel12, tailleListeBaselevel12, &listeCoordonneeslevel12, &tailleListeCoordonneeslevel12);
    traceLinesFromList(surface, listeCoordonneeslevel12, tailleListeCoordonneeslevel12);
    */
}
