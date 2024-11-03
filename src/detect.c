#include <SDL2/SDL.h>
#include <stdio.h>


// Function to return the height of a letter in the SDL surface
int get_letter_height(SDL_Surface *surface, int startX, int startY) {
    int height = 0;

    // Iterate through each row starting from startY
    for (int y = startY; y < surface->h; y++) {
        int foundBlackPixel = 0; // To check if we found a black pixel in this row

        // Check each pixel in the column at startX
        for (int x = startX; x < surface->w; x++) {
            Uint32 pixel = ((Uint32*)surface->pixels)[y * surface->w + x];
            Uint8 r, g, b;

            SDL_GetRGB(pixel, surface->format, &r, &g, &b);

            // Check if the pixel is black (assuming black is (0,0,0))
            if (r == 0 && g == 0 && b == 0) {
                foundBlackPixel = 1;
                break;
            }
        }

        // If a black pixel was found in this row, increment height
        if (foundBlackPixel) {
            height++;
        } else if (height > 0) {
            // If no black pixel is found after finding height, break
            break;
        }
    }

    return height;
}

// Function to return the width of a letter in the SDL surface
int get_max_letter_width(SDL_Surface *surface) {
    int maxWidth = 0;

    // Iterate through each row in the surface
    for (int row = 0; row < surface->h; row++) {
        int currentWidth = 0;

        // Iterate through each column in the current row
        for (int x = 0; x < surface->w; x++) {
            Uint32 pixel = ((Uint32*)surface->pixels)[row * surface->w + x];
            Uint8 r, g, b;

            SDL_GetRGB(pixel, surface->format, &r, &g, &b);

            // Check if the pixel is black (assuming black is (0,0,0))
            if (r == 0 && g == 0 && b == 0) {
                // Count the width of the current letter
                currentWidth++;
            } else {
                // If we encounter a white pixel and a current letter width is counted
                if (currentWidth > maxWidth) {
                    maxWidth = currentWidth;
                }
                currentWidth = 0; // Reset currentWidth for the next letter
            }
        }

        // Check if the last letter width needs to be accounted for
        if (currentWidth > maxWidth) {
            maxWidth = currentWidth;
        }
    }

    return maxWidth;
}


void draw_red_box(SDL_Surface *surface, int x1, int y1, int x2, int y2) {
    Uint32 red_pixel = SDL_MapRGB(surface->format, 255, 0, 0);
    
    // Ensure the coordinates are within bounds
    if (x1 < 0) x1 = 0;
    if (y1 < 0) y1 = 0;
    if (x2 >= surface->w) x2 = surface->w - 1;
    if (y2 >= surface->h) y2 = surface->h - 1;

    // Check for red pixels in the bounding box
    for (int y = y1; y <= y2; y++) {
        for (int x = x1; x <= x2; x++) {
            if (x >= 0 && x < surface->w && y >= 0 && y < surface->h) {
                Uint32 pixel = ((Uint32*)surface->pixels)[y * surface->w + x];
                if (pixel == red_pixel) {
                    // A red pixel is found, exit the function without drawing the box
                    return;
                }
            }
        }
    }

    // No red pixels found, proceed to draw the red box
    for (int x = x1; x <= x2; x++) {
        if (x >= 0 && x < surface->w) {
            if (y1 >= 0 && y1 < surface->h) {
                ((Uint32*)surface->pixels)[y1 * surface->w + x] = red_pixel; // Top edge
            }
            if (y2 >= 0 && y2 < surface->h) {
                ((Uint32*)surface->pixels)[y2 * surface->w + x] = red_pixel; // Bottom edge
            }
        }
    }
    for (int y = y1; y <= y2; y++) {
        if (y >= 0 && y < surface->h) {
            if (x1 >= 0 && x1 < surface->w) {
                ((Uint32*)surface->pixels)[y * surface->w + x1] = red_pixel; // Left edge
            }
            if (x2 >= 0 && x2 < surface->w) {
                ((Uint32*)surface->pixels)[y * surface->w + x2] = red_pixel; // Right edge
            }
        }
    }
}

void add_square_to_letter(SDL_Surface *surface, int startx, int starty, int endx, int endy) {
    // Lock the surface for pixel access
    SDL_LockSurface(surface);

    Uint8 r, g, b;
    Uint32 pixel;
    int width = surface->w;
    
    // Get letter height and maximum width
    int lheight = get_letter_height(surface, 0, 0);  // Replace with actual function
    int lwidth = get_max_letter_width(surface);        // Replace with actual function

    // Iterate through each pixel to detect letter-like clusters
    for (int y = starty; y < endy; y++) {
        for (int x = startx; x < endx; x++) {
            pixel = ((Uint32*)surface->pixels)[y * width + x];
            SDL_GetRGB(pixel, surface->format, &r, &g, &b);

            // Detect black pixels (threshold values may need adjustment)
            if (r < 50 && g < 50 && b < 50) {
                // Draw a red box around this letter cluster
                draw_red_box(surface, x - lwidth / 2, y - lheight / 2, x + lwidth, y +lheight);
            }
        }
    }

    // Unlock the surface after processing
    SDL_UnlockSurface(surface);
}

void detect(SDL_Surface *surface) {
    if (surface->format->BytesPerPixel != 3 && surface->format->BytesPerPixel != 4) {
        printf("Unsupported image format for grayscale conversion.\n");
        return;
    }
    
    int width = surface->w;
    int height = surface->h;
    add_square_to_letter(surface, 0, 0, width, height);
}

