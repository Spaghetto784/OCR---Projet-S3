#include <SDL2/SDL.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h> 
#include <string.h>
#include <stdbool.h>
#include <SDL_image.h>


/*
int get_letter_top_bound(SDL_Surface *surface, int startX, int startY)
{
    Uint32 white_pixel = SDL_MapRGB(surface->format, 255, 255, 255);
    int top_bound = startY;

    for (int y = startY; y >= 0; y--) {
        int non_white_row = 0;
        for (int x = startX; x < surface->w; x++) {
            Uint32 pixel = ((Uint32 *)surface->pixels)[y * surface->w + x];
            if (pixel != white_pixel) {
                non_white_row = 1;
                break;
            }
        }
        if (non_white_row) {
            top_bound = y;
        } else {
            break;
        }
    }
    return top_bound;
}

int get_letter_bottom_bound(SDL_Surface *surface, int startX, int startY)
{
    Uint32 white_pixel = SDL_MapRGB(surface->format, 255, 255, 255);
    int bottom_bound = startY;

    for (int y = startY; y < surface->h; y++) {
        int non_white_row = 0;
        for (int x = startX; x < surface->w; x++) {
            Uint32 pixel = ((Uint32 *)surface->pixels)[y * surface->w + x];
            if (pixel != white_pixel) {
                non_white_row = 1;
                break;
            }
        }
        if (non_white_row) {
            bottom_bound = y;
        } else if (bottom_bound > startY) {
            break;
        }
    }
    return bottom_bound;
}

int get_letter_left_bound(SDL_Surface *surface, int startX, int startY)
{
    Uint32 white_pixel = SDL_MapRGB(surface->format, 255, 255, 255);
    int left_bound = startX;

    for (int x = startX; x >= 0; x--) {
        int non_white_column = 0;
        for (int y = startY; y < surface->h; y++) {
            Uint32 pixel = ((Uint32 *)surface->pixels)[y * surface->w + x];
            if (pixel != white_pixel) {
                non_white_column = 1;
                break;
            }
        }
        if (non_white_column) {
            left_bound = x;
        } else {
            break;
        }
    }
    return left_bound;
}

int get_letter_right_bound(SDL_Surface *surface, int startX, int startY)
{
    Uint32 white_pixel = SDL_MapRGB(surface->format, 255, 255, 255);
    int right_bound = startX;

    for (int x = startX; x < surface->w; x++) {
        int non_white_column = 0;
        for (int y = startY; y < surface->h; y++) {
            Uint32 pixel = ((Uint32 *)surface->pixels)[y * surface->w + x];
            if (pixel != white_pixel) {
                non_white_column = 1;
                break;
            }
        }
        if (non_white_column) {
            right_bound = x;
        } else {
            break;
        }
    }
    return right_bound;
}

void draw_box(SDL_Surface *surface, int x1, int y1, int x2, int y2,
 Uint8 r, Uint8 g, Uint8 b)
{
    // Map the specified color to the format of the surface
    Uint32 box_color = SDL_MapRGB(surface->format, r, g, b);

    // Clamp coordinates within surface bounds
    x1 = x1 < 0 ? 0 : (x1 >= surface->w ? surface->w - 1 : x1);
    y1 = y1 < 0 ? 0 : (y1 >= surface->h ? surface->h - 1 : y1);
    x2 = x2 < 0 ? 0 : (x2 >= surface->w ? surface->w - 1 : x2);
    y2 = y2 < 0 ? 0 : (y2 >= surface->h ? surface->h - 1 : y2);

    // Check for any pixel with the specified color inside the box area
    for (int y = y1; y <= y2; y++) {
        for (int x = x1; x <= x2; x++) {
            if (((Uint32 *)surface->pixels)[y * surface->w + x] == box_color) {
                return;  // Exit if any matching pixel is found within the box area
            }
        }
    }

    // Draw the top and bottom edges
    for (int x = x1; x <= x2; x++) {
        ((Uint32 *)surface->pixels)[y1 * surface->w + x] = box_color;  // Top
        ((Uint32 *)surface->pixels)[y2 * surface->w + x] = box_color;  // Bottom
    }

    // Draw the left and right edges
    for (int y = y1; y <= y2; y++) {
        ((Uint32 *)surface->pixels)[y * surface->w + x1] = box_color;  // Left
        ((Uint32 *)surface->pixels)[y * surface->w + x2] = box_color;  // Right
    }
}



void getSpaces(SDL_Surface *surface, int *spaceGrid, int *spaceList) {
    if (!surface || !spaceGrid || !spaceList) return;

    int width = surface->w;
    int height = surface->h;
    Uint32 *pixels = (Uint32 *)surface->pixels;

    int totalGridSpaces = 0, countGridSpaces = 0;
    int totalListSpaces = 0, countListSpaces = 0;

    int currentSpace = 0;
    bool isGrid = true; // Default assumption: starting in the grid.

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            // Get pixel color.
            Uint32 pixel = pixels[y * width + x];
            Uint8 r, g, b;
            SDL_GetRGB(pixel, surface->format, &r, &g, &b);

            bool isBlack = (r == 0 && g == 0 && b == 0);

            if (isBlack) {
                if (currentSpace > 0) {
                    // Classify the space.
                    if (isGrid) {
                        totalGridSpaces += currentSpace;
                        countGridSpaces++;
                    } else {
                        totalListSpaces += currentSpace;
                        countListSpaces++;
                    }
                    currentSpace = 0;
                }
            } else {
                currentSpace++;
            }
        }
        // Switch between grid and list when encountering large gaps (heuristic).
        if (currentSpace > 0) {
            if (currentSpace > 10) { // Adjust this threshold as needed.
                isGrid = true;
            } else {
                isGrid = false;
            }
        }
        currentSpace = 0; // Reset at the end of each row.
    }

    // Compute averages.
    *spaceGrid = (countGridSpaces > 0) ? (totalGridSpaces / countGridSpaces) : 0;
    *spaceList = (countListSpaces > 0) ? (totalListSpaces / countListSpaces) : 0;
}

void add_square_to_letter(SDL_Surface *surface, int startx, int starty, int endx, int endy)
{
    // Lock the surface for pixel access
    SDL_LockSurface(surface);
    Uint32 black_pixel = SDL_MapRGB(surface->format, 0, 0, 0);
    int width = surface->w;
    int spaceGrid, spaceList;
    getSpaces(surface, &spaceGrid, &spaceList);
    printf("spaceGrid: %d, spaceList: %d\n", spaceGrid, spaceList);

    // Iterate through each pixel to detect letter-like clusters
    for (int y = starty; y < endy; y++) {
		int top, bottom, left, right=0, space;
        for (int x = startx; x < endx; x++) {
            Uint32 pixel = ((Uint32 *)surface->pixels)[y * width + x];
		
            if (pixel == black_pixel) {
				left = get_letter_left_bound(surface, x, y);
                top = get_letter_top_bound(surface, x, y);
                bottom = get_letter_bottom_bound(surface, x, y);
                space = left - right;
                right = get_letter_right_bound(surface, x, y);
                
				
				printf("Lettre at (%d, %d): left=%d, right=%d, space=%d\n", x, y, left, right, space);
                if (space>spaceList + 10 && left > 100){
                    draw_box(surface, left, top, right, bottom, 255, 0, 0);
				}
				else{
					draw_box(surface, left, top, right, bottom, 0, 0, 255);
				}
                
             
                
                

                // Skip over the processed letter cluster to avoid multiple boxes
                x = right;  // Move to the right of the current cluster
            }
        }
    }

    // Unlock the surface after processing
    SDL_UnlockSurface(surface);
}

void extract_and_save_letters(SDL_Surface *surface, const char *output_folder_list, const char *output_folder_grid) {
    if (!surface || !output_folder_list || !output_folder_grid) return;

    // Create the output directories if they don't exist
    struct stat st = {0};
    if (stat(output_folder_list, &st) == -1) {
        if (mkdir(output_folder_list, 0700) != 0) {
            fprintf(stderr, "Failed to create directory '%s': %s\n", output_folder_list, strerror(errno));
            return;
        }
    }
    if (stat(output_folder_grid, &st) == -1) {
        if (mkdir(output_folder_grid, 0700) != 0) {
            fprintf(stderr, "Failed to create directory '%s': %s\n", output_folder_grid, strerror(errno));
            return;
        }
    }

    Uint32 white_pixel = SDL_MapRGB(surface->format, 255, 255, 255);
    Uint32 red_pixel = SDL_MapRGB(surface->format, 255, 0, 0);
    Uint32 blue_pixel = SDL_MapRGB(surface->format, 0, 0, 255);

    Uint32 *pixels = (Uint32 *)surface->pixels;
    int width = surface->w;
    int height = surface->h;

    bool **visited = calloc(height, sizeof(bool *));
    for (int y = 0; y < height; y++) {
        visited[y] = calloc(width, sizeof(bool));
    }

    int xLetterGrid = 0, yLetterGrid = 0, xLetterList = 0, yLetterList = 0;

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            // Skip visited or white pixels
            if (visited[y][x] || pixels[y * width + x] == white_pixel) {
                continue;
            }

            // Detect letter bounds
            int top = get_letter_top_bound(surface, x, y);
            int bottom = get_letter_bottom_bound(surface, x, y);
            int left = get_letter_left_bound(surface, x, y);
            int right = get_letter_right_bound(surface, x, y);

            // Mark pixels within the bounds as visited
            for (int j = top; j <= bottom; j++) {
                for (int i = left; i <= right; i++) {
                    visited[j][i] = true;
                }
            }

            // Determine the bounding box color
            Uint32 bounding_color = white_pixel;
            for (int j = left; j <= right; j++) {
                if (pixels[top * width + j] == red_pixel || pixels[top * width + j] == blue_pixel) {
                    bounding_color = pixels[top * width + j];
                    break;
                }
            }

            // Create a new surface for the letter
            int letter_width = right - left + 1;
            int letter_height = bottom - top + 1;
            SDL_Surface *letter_surface = SDL_CreateRGBSurfaceWithFormat(
                0, letter_width, letter_height, 32, surface->format->format);
            if (!letter_surface) {
                fprintf(stderr, "Failed to create letter surface: %s\n", SDL_GetError());
                continue;
            }

            // Copy letter pixels to the new surface
            Uint32 *letter_pixels = (Uint32 *)letter_surface->pixels;
            for (int j = 0; j < letter_height; j++) {
                for (int i = 0; i < letter_width; i++) {
                    letter_pixels[j * letter_width + i] =
                        pixels[(top + j) * width + (left + i)];
                }
            }

            // Save the letter surface as an image in the corresponding folder
            char filename[256];
            if (bounding_color == blue_pixel) {
                snprintf(filename, sizeof(filename), "%s/letter_%03d_%03d.png", output_folder_list, xLetterList, yLetterList);
                xLetterList++;
            } else if (bounding_color == red_pixel) {
                snprintf(filename, sizeof(filename), "%s/letter_%03d_%03d.png", output_folder_grid, xLetterGrid, yLetterGrid);
                xLetterGrid++;
            } else {
                fprintf(stderr, "Unknown bounding box color at (%d, %d)\n", x, y);
                SDL_FreeSurface(letter_surface);
                continue;
            }

            if (IMG_SavePNG(letter_surface, filename) != 0) {
                fprintf(stderr, "Failed to save letter image as PNG: %s\n", SDL_GetError());
            }


            // Free the letter surface
            SDL_FreeSurface(letter_surface);
        }

        // Reset horizontal counters for a new row of letters
        if (xLetterList > 0) {
            xLetterList = 0;
            yLetterList++;
        }
        if (xLetterGrid > 0) {
            xLetterGrid = 0;
            yLetterGrid++;
        }
    }


    // Free visited array
    for (int y = 0; y < height; y++) {
        free(visited[y]);
    }
    free(visited);

}



void detect(SDL_Surface *surface)
{
    if (surface->format->BytesPerPixel != 3
        && surface->format->BytesPerPixel != 4)
    {
        printf("Unsupported image format for grayscale conversion.\n");
        return;
    }

    int width = surface->w;
    int height = surface->h;
    add_square_to_letter(surface, 0, 0, width, height);
    //extract_and_save_letters(surface, "letterList", "letterGrid");
}
*/

#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <sys/stat.h>  // Pour créer des dossiers
#include <stdio.h>     // Pour les opérations sur les fichiers
#include <math.h>
#define THRESHOLD 10 // Threshold for determining a new row

// Calculer la distance entre deux points
float distance(int x1, int y1, int x2, int y2) {
    return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
}

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




// Fonction flood fill pour détecter les clusters
void flood_fill(SDL_Surface *surface, int x, int y, int* visited, int width, int height, int* minX, int* minY, int* maxX, int* maxY) {
    if (x < 0 || x >= width || y < 0 || y >= height) return;
    if (visited[y * width + x]) return;
    Uint32 pixel = ((Uint32*)surface->pixels)[y * width + x];
    Uint8 r, g, b;
    SDL_GetRGB(pixel, surface->format, &r, &g, &b);
    if (r == 0 && g == 0 && b == 0) { // Pixel noir
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

void detect_clusters(SDL_Surface *surface) {
    int width = surface->w;
    int height = surface->h;
    Uint32* pixels = (Uint32*)surface->pixels;

    int* visited = (int*)calloc(width * height, sizeof(int));

    // Définition d'un cluster
    typedef struct {
        int minX, minY, maxX, maxY;
        int area;
        int centerX, centerY; // Centre du cluster
    } Cluster;

    Cluster clusters[1000];
    int cluster_count = 0;

    // Parcourir l'image pour détecter les clusters
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            Uint32 pixel = pixels[y * width + x];
            Uint8 r, g, b;
            SDL_GetRGB(pixel, surface->format, &r, &g, &b);
            if (r == 0 && g == 0 && b == 0 && !visited[y * width + x]) {
                int minX = x, minY = y, maxX = x, maxY = y;
                flood_fill(surface, x, y, visited, width, height, &minX, &minY, &maxX, &maxY);

                // Calculer l'aire (taille) du cluster
                int area = (maxX - minX + 1) * (maxY - minY + 1);

                // Calculer le centre du cluster
                int centerX = (minX + maxX) / 2;
                int centerY = (minY + maxY) / 2;

                // Ajouter ce cluster à la liste des clusters détectés
                clusters[cluster_count++] = (Cluster){minX, minY, maxX, maxY, area, centerX, centerY};
            }
        }
    }

    // Seuil pour la distance entre les clusters
    float distance_threshold = 20.0; // Ajustez selon vos besoins
    int lastY = clusters[0].centerY;

    for (int i = 0; i < cluster_count; i++) {
        int is_in_grid = 0;

        // Comparer avec les autres clusters
        for (int j = 0; j < cluster_count; j++) {
            if (i != j) {
                float dist = distance(clusters[i].centerX, clusters[i].centerY, clusters[j].centerX, clusters[j].centerY);
                if (dist < distance_threshold) {
                    is_in_grid = 1; // Le cluster appartient à la grille
                    break;
                }
            }
        }

        // Attribuer une couleur selon la classification
        Uint32 color;
        const char *folder;

        if (is_in_grid) {
            color = SDL_MapRGB(surface->format, 255, 0, 0); // Rouge pour la grille
            folder = "letterList";
            //printf("Cluster %d est une lettre dans la grille\n", i);
        } else {
            color = SDL_MapRGB(surface->format, 0, 0, 255); // Bleu pour la liste
            folder = "letterGrid";
            //printf("Cluster %d est une lettre dans la liste\n", i);
        }
        if (clusters[i].centerY - lastY >= 10){
            lastY = clusters[i].centerY;
        }

        // Sauvegarder le cluster dans le dossier approprié avec des indices séparés
        save_cluster(surface, clusters[i].minX, clusters[i].minY, clusters[i].maxX, clusters[i].maxY, folder, clusters[i].centerX, lastY);


        // Dessiner un carré autour du cluster
        for (int x = clusters[i].minX; x <= clusters[i].maxX; x++) {
            if (clusters[i].minY >= 0 && clusters[i].minY < height) pixels[clusters[i].minY * width + x] = color; // Ligne du haut
            if (clusters[i].maxY >= 0 && clusters[i].maxY < height) pixels[clusters[i].maxY * width + x] = color; // Ligne du bas
        }
        for (int y = clusters[i].minY; y <= clusters[i].maxY; y++) {
            if (clusters[i].minX >= 0 && clusters[i].minX < width) pixels[y * width + clusters[i].minX] = color; // Colonne de gauche
            if (clusters[i].maxX >= 0 && clusters[i].maxX < width) pixels[y * width + clusters[i].maxX] = color; // Colonne de droite
        }
    }
    free(visited);
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>

#define THRESHOLD 10 // Threshold for determining a new row

// Structure to hold information about a file
typedef struct {
    char original_name[256];
    int x;
    int y;
} LetterFile;

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
        if (abs(files[i].y - last_y) > THRESHOLD) {
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

void traceLigne(SDL_Surface *surface, int x1, int y1, int x2, int y2, int epaisseur) {
    // Vérifie si la surface est valide
    if (!surface) {
        printf("Surface invalide.\n");
        return;
    }

    // Verrouille la surface si nécessaire
    if (SDL_MUSTLOCK(surface) && SDL_LockSurface(surface) != 0) {
        printf("Erreur lors du verrouillage de la surface : %s\n", SDL_GetError());
        return;
    }

    // Détermine les différences entre les coordonnées
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;

    int err = dx - dy;

    // Fonction pour dessiner un pixel avec une certaine déviation autour de la ligne
    void dessinerPixel(int x, int y) {
        if (x >= 0 && x < surface->w && y >= 0 && y < surface->h) {
            Uint32 *pixels = (Uint32 *)surface->pixels;
            Uint32 pixel_color = SDL_MapRGB(surface->format, 0, 0, 0); // Couleur noire
            pixels[y * surface->w + x] = pixel_color;
        }
    }

    // Trace la ligne avec épaisseur
    while (1) {
        // Trace la ligne principale
        dessinerPixel(x1, y1);

        // Dessine des pixels autour de la ligne pour l'épaisseur
        for (int i = -epaisseur / 2; i <= epaisseur / 2; i++) {
            for (int j = -epaisseur / 2; j <= epaisseur / 2; j++) {
                if (i != 0 || j != 0) {  // Evite de redessiner le pixel central
                    dessinerPixel(x1 + i, y1 + j);
                }
            }
        }

        // Vérifie si la ligne est terminée
        if (x1 == x2 && y1 == y2) {
            break;
        }

        int e2 = 2 * err;

        // Mise à jour de x et y en fonction de l'erreur
        if (e2 > -dy) {
            err -= dy;
            x1 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y1 += sy;
        }
    }

    // Déverrouille la surface si nécessaire
    if (SDL_MUSTLOCK(surface)) {
        SDL_UnlockSurface(surface);
    }
}


#include <SDL2/SDL.h>
#include <stdio.h>

// Fonction pour tracer des lignes à partir d'une liste d'entiers
void traceLignesDepuisListe(SDL_Surface *surface, int *liste, int taille) {
    if (!surface) {
        printf("Surface invalide.\n");
        return;
    }
    if (taille < 4) {
        printf("La liste doit contenir au moins 4 entiers pour tracer une ligne.\n");
        return;
    }
    if (taille % 2 != 0) {
        printf("La liste doit contenir un nombre pair d'entiers et 4.\n");
        return;
    }

    // Parcourt la liste pour tracer des lignes entre chaque couple de points
    for (int i = 0; i < taille - 2; i += 4) {
        int x1 = liste[i];
        int y1 = liste[i + 1];
        int x2 = liste[i + 2];
        int y2 = liste[i + 3];

        // Trace une ligne entre les points (x1, y1) et (x2, y2)
        traceLigne(surface, x1, y1, x2, y2, 3);
    }
}


#include <SDL2/SDL.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void extraireCoordonneesDepuisFichiers(const char *dossier, int *listedebase, int tailleListe, int **listeResultat, int *tailleListeResultat) {
    DIR *dir = opendir(dossier);
    if (!dir) {
        printf("Erreur lors de l'ouverture du dossier : %s\n", strerror(errno));
        return;
    }

    struct dirent *entry;
    
    // Parcours de chaque paire de coordonnées dans listedebase
    for (int i = 0; i < tailleListe; i += 2) {
        int x1 = listedebase[i];
        int y1 = listedebase[i + 1];

        // Recherche dans le dossier un fichier qui commence par x1_y1
        rewinddir(dir);  // On réinitialise la position du dossier pour chaque itération
        while ((entry = readdir(dir)) != NULL) {
            // Vérifier si le fichier a l'extension .png et commence par x1_y1
            if (strstr(entry->d_name, ".png") != NULL) {
                int x2, y2;
                // Exemple de nom de fichier : "03_01_086_179.png"
                if (sscanf(entry->d_name, "%d_%d_%d_%d.png", &x1, &y1, &x2, &y2) == 4) {
                    // Vérifier si le fichier correspond aux coordonnées x1, y1
                    if (x1 == listedebase[i] && y1 == listedebase[i + 1]) {
                        // Ajouter les coordonnées suivantes (x2, y2) à la nouvelle liste
                        *listeResultat = realloc(*listeResultat, (*tailleListeResultat + 2) * sizeof(int));
                        if (*listeResultat == NULL) {
                            printf("Erreur d'allocation mémoire\n");
                            closedir(dir);
                            return;
                        }

                        (*listeResultat)[*tailleListeResultat] = x2;    // Ajoute x2
                        (*listeResultat)[*tailleListeResultat + 1] = y2; // Ajoute y2
                        *tailleListeResultat += 2; // Augmente la taille de la liste
                        break; // Sortir dès que le fichier correspondant est trouvé
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
    extraireCoordonneesDepuisFichiers(gridPath, listedebaselevel11, tailleListeBaselevel11, &listeCoordonneeslevel11, &tailleListeCoordonneeslevel11);
    traceLignesDepuisListe(surface, listeCoordonneeslevel11, tailleListeCoordonneeslevel11);
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

    
    extraireCoordonneesDepuisFichiers(gridPath, listedebaselevel12, tailleListeBaselevel12, &listeCoordonneeslevel12, &tailleListeCoordonneeslevel12);
    traceLignesDepuisListe(surface, listeCoordonneeslevel12, tailleListeCoordonneeslevel12);
    */
}
