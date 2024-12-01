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
#include <SDL2/SDL_image.h>
#include <string.h>
#include <sys/stat.h>  // Pour mkdir
#include <errno.h>      // Pour EEXIST

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

// Fonction pour enregistrer les lettres extraites dans un fichier PNG
void save_letter(SDL_Surface *surface, int minX, int minY, int maxX, int maxY, const char *folder, const char *filename) {
    int width = maxX - minX + 1;
    int height = maxY - minY + 1;
    
    // Créer une nouvelle surface pour la lettre
    SDL_Surface *letterSurface = SDL_CreateRGBSurfaceWithFormat(0, width, height, 32, surface->format->format);
    if (!letterSurface) {
        printf("Error creating letter surface: %s\n", SDL_GetError());
        return;
    }

    // Copier la portion de l'image correspondant à la lettre dans la nouvelle surface
    SDL_Rect srcRect = {minX, minY, width, height};
    SDL_Rect dstRect = {0, 0, width, height};
    SDL_BlitSurface(surface, &srcRect, letterSurface, &dstRect);

    // Créer le répertoire si nécessaire
    char dirPath[256];
    snprintf(dirPath, sizeof(dirPath), "./%s", folder);


    // Créer le chemin complet pour le fichier de sortie
    char filePath[256];
    snprintf(filePath, sizeof(filePath), "./%s/%s.png", folder, filename);

    // Enregistrer la lettre comme image PNG
    if (IMG_SavePNG(letterSurface, filePath) != 0) {
        printf("Error saving PNG: %s\n", IMG_GetError());
    } else {
        printf("Saved letter to %s\n", filePath);
    }

    SDL_FreeSurface(letterSurface);
}

// Fonction pour détecter les clusters
void detect_clusters(SDL_Surface *surface) {
    int width = surface->w;
    int height = surface->h;
    Uint32* pixels = (Uint32*)surface->pixels;

    int* visited = (int*)calloc(width * height, sizeof(int));

    // Liste pour stocker les coordonnées des clusters
    typedef struct {
        int minX, minY, maxX, maxY;
        int area; // Taille de la lettre (en pixels)
    } Cluster;

    // Tableau pour stocker les clusters détectés
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
                int area;
                if ((maxX - minX + 1) * (maxX - minX + 1) > (maxY - minY + 1) * (maxY - minY + 1)){
                    area = (maxX - minX + 1) * (maxX - minX + 1);
                }
                else{
                    area = (maxY - minY + 1) * (maxY - minY + 1);
                }
                

                // Ajouter ce cluster à la liste des clusters détectés
                clusters[cluster_count++] = (Cluster){minX, minY, maxX, maxY, area};
            }
        }
    }

    // Seuil pour déterminer si une lettre est petite ou grande
    int size_threshold = 200; // Ajustez ce seuil en fonction de la taille des lettres

    // Analyser les clusters pour déterminer s'ils sont dans la grille ou dans la liste
    for (int i = 0; i < cluster_count; i++) {
        // Classification des clusters : Grille ou Liste
        const char *folder = (clusters[i].area > size_threshold) ? "letterGrid" : "letterList";
        char filename[64];
        snprintf(filename, sizeof(filename), "letter_%d", i);

        // Enregistrer la lettre dans le répertoire approprié
        save_letter(surface, clusters[i].minX, clusters[i].minY, clusters[i].maxX, clusters[i].maxY, folder, filename);

        // Colorier le cluster pour le visualiser (rouge pour la grille, bleu pour la liste)
        Uint32 color = (clusters[i].area > size_threshold) ? SDL_MapRGB(surface->format, 255, 0, 0) : SDL_MapRGB(surface->format, 0, 0, 255);
        
        // Dessiner un carré autour du cluster (en rouge ou bleu)
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

// Fonction de détection avec grille/liste séparée
void detect(SDL_Surface *surface) {
    if (surface->format->BytesPerPixel != 3 && surface->format->BytesPerPixel != 4) {
        printf("Unsupported image format for grayscale conversion.\n");
        return;
    }

    detect_clusters(surface);
}

