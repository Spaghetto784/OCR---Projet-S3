#include <SDL2/SDL.h>
#include <stdio.h>

int get_letter_top_bound(SDL_Surface *surface, int startX, int startY)
{
    Uint32 black_pixel = SDL_MapRGB(surface->format, 0, 0, 0);
    int top_bound = startY;

    for (int y = startY; y >= 0; y--) {
        int black_row = 0;
        for (int x = startX; x < surface->w; x++) {
            Uint32 pixel = ((Uint32 *)surface->pixels)[y * surface->w + x];
            if (pixel == black_pixel) {
                black_row = 1;
                break;
            }
        }
        if (black_row) {
            top_bound = y;
        } else {
            break;
        }
    }
    return top_bound;
}

int get_letter_bottom_bound(SDL_Surface *surface, int startX, int startY)
{
    Uint32 black_pixel = SDL_MapRGB(surface->format, 0, 0, 0);
    int bottom_bound = startY;

    for (int y = startY; y < surface->h; y++) {
        int black_row = 0;
        for (int x = startX; x < surface->w; x++) {
            Uint32 pixel = ((Uint32 *)surface->pixels)[y * surface->w + x];
            if (pixel == black_pixel) {
                black_row = 1;
                break;
            }
        }
        if (black_row) {
            bottom_bound = y;
        } else if (bottom_bound > startY) {
            break;
        }
    }
    return bottom_bound;
}

int get_letter_left_bound(SDL_Surface *surface, int startX, int startY)
{
    Uint32 black_pixel = SDL_MapRGB(surface->format, 0, 0, 0);
    int left_bound = startX;

    for (int x = startX; x >= 0; x--) {
        int black_column = 0;
        for (int y = startY; y < surface->h; y++) {
            Uint32 pixel = ((Uint32 *)surface->pixels)[y * surface->w + x];
            if (pixel == black_pixel) {
                black_column = 1;
                break;
            }
        }
        if (black_column) {
            left_bound = x;
        } else {
            break;
        }
    }
    return left_bound;
}

int get_letter_right_bound(SDL_Surface *surface, int startX, int startY)
{
    Uint32 black_pixel = SDL_MapRGB(surface->format, 0, 0, 0);
    int right_bound = startX;

    for (int x = startX; x < surface->w; x++) {
        int black_column = 0;
        for (int y = startY; y < surface->h; y++) {
            Uint32 pixel = ((Uint32 *)surface->pixels)[y * surface->w + x];
            if (pixel == black_pixel) {
                black_column = 1;
                break;
            }
        }
        if (black_column) {
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





void add_square_to_letter(SDL_Surface *surface, int startx, int starty, int endx, int endy)
{
    // Lock the surface for pixel access
    SDL_LockSurface(surface);
    Uint32 black_pixel = SDL_MapRGB(surface->format, 0, 0, 0);
    int width = surface->w;

    // Iterate through each pixel to detect letter-like clusters
    for (int y = starty; y < endy; y++) {
        for (int x = startx; x < endx; x++) {
            Uint32 pixel = ((Uint32 *)surface->pixels)[y * width + x];

            if (pixel == black_pixel) {
                int top = get_letter_top_bound(surface, x, y);
                int bottom = get_letter_bottom_bound(surface, x, y);
                int left = get_letter_left_bound(surface, x, y);
                int right = get_letter_right_bound(surface, x, y);

                // Use the draw_box function to draw the box around the detected letter
                draw_box(surface, left, top, right, bottom, 250, 0, 0);

                // Skip over the processed letter cluster to avoid multiple boxes
                x = right;  // Move to the right of the current cluster
            }
        }
    }

    // Unlock the surface after processing
    SDL_UnlockSurface(surface);
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
}
