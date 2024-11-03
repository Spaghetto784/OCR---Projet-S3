#include <SDL2/SDL.h>
#include <stdio.h>

int get_letter_top_bound(SDL_Surface *surface, int startX, int startY) {
    int top_bound = startY; // Initialize top boundary
    Uint32 pixel;
    Uint8 r, g, b;

    // Check upward from startY to find the top boundary
    for (int y = startY; y >= 0; y--) {
        int black_row = 0; // Flag to check if there's a black pixel in this row
        for (int x = startX; x < surface->w; x++) {
            pixel = ((Uint32 *)surface->pixels)[y * surface->w + x];
            SDL_GetRGB(pixel, surface->format, &r, &g, &b);

            if (r < 50 && g < 50 && b < 50) { // Black pixel threshold
                black_row = 1;  // Found a black pixel in this row
                break;
            }
        }

        if (black_row) {
            top_bound = y; // Update top boundary
        } else {
            break; // End of letter height
        }
    }

    return top_bound; // Return the top boundary
}

int get_letter_bottom_bound(SDL_Surface *surface, int startX, int startY) {
    int bottom_bound = startY; // Initialize bottom boundary
    Uint32 pixel;
    Uint8 r, g, b;

    // Check downward from startY to find the bottom boundary
    for (int y = startY; y < surface->h; y++) {
        int black_row = 0; // Flag to check if there's a black pixel in this row
        for (int x = startX; x < surface->w; x++) {
            pixel = ((Uint32 *)surface->pixels)[y * surface->w + x];
            SDL_GetRGB(pixel, surface->format, &r, &g, &b);

            if (r < 50 && g < 50 && b < 50) { // Black pixel threshold
                black_row = 1;  // Found a black pixel in this row
                break;
            }
        }

        if (black_row) {
            bottom_bound = y; // Update bottom boundary
        } else if (bottom_bound > startY) {
            // End of letter height
            break;
        }
    }

    return bottom_bound; // Return the bottom boundary
}

int get_letter_left_bound(SDL_Surface *surface, int startX, int startY) {
    int left_bound = startX; // Initialize left bound
    Uint32 pixel;
    Uint8 r, g, b;

    // Check leftward from startX to find the left boundary
    for (int x = startX; x >= 0; x--) {
        int black_column = 0; // Flag to check if there's a black pixel in this column
        for (int y = startY; y < surface->h; y++) {
            pixel = ((Uint32 *)surface->pixels)[y * surface->w + x];
            SDL_GetRGB(pixel, surface->format, &r, &g, &b);

            if (r < 50 && g < 50 && b < 50) { // Black pixel threshold
                black_column = 1;  // Found a black pixel in this column
                break;
            }
        }

        if (black_column) {
            left_bound = x;  // Update left boundary if a black pixel is found
        } else {
            break;  // End of black pixels to the left
        }
    }

    return left_bound; // Return the left boundary
}

int get_letter_right_bound(SDL_Surface *surface, int startX, int startY) {
    int right_bound = startX; // Initialize right bound
    Uint32 pixel;
    Uint8 r, g, b;

    // Check rightward from startX to find the right boundary
    for (int x = startX; x < surface->w; x++) {
        int black_column = 0; // Flag to check if there's a black pixel in this column
        for (int y = startY; y < surface->h; y++) {
            pixel = ((Uint32 *)surface->pixels)[y * surface->w + x];
            SDL_GetRGB(pixel, surface->format, &r, &g, &b);

            if (r < 50 && g < 50 && b < 50) { // Black pixel threshold
                black_column = 1;  // Found a black pixel in this column
                break;
            }
        }

        if (black_column) {
            right_bound = x;  // Update right boundary if a black pixel is found
        } else {
            break;  // End of black pixels to the right
        }
    }

    return right_bound; // Return the right boundary
}

int get_letter_width(SDL_Surface *surface, int startX, int startY) {
    int left_bound = get_letter_left_bound(surface, startX, startY);
    int right_bound = get_letter_right_bound(surface, startX, startY);

    // Width is the distance between the leftmost and rightmost black pixels
    return (right_bound - left_bound + 1);
}

int get_letter_height(SDL_Surface *surface, int startX, int startY) {
    int top_bound = get_letter_top_bound(surface, startX, startY);
    int bottom_bound = get_letter_bottom_bound(surface, startX, startY);

    // Height is the distance between the topmost and bottommost black pixels
    return (bottom_bound - top_bound + 1);
}




void draw_red_box(SDL_Surface *surface, int x1, int y1, int x2, int y2)
{
    Uint32 red_pixel = SDL_MapRGB(surface->format, 255, 0, 0);

    // Ensure the coordinates are within bounds
    if (x1 < 0)
        x1 = 0;
    if (y1 < 0)
        y1 = 0;
    if (x2 >= surface->w)
        x2 = surface->w - 1;
    if (y2 >= surface->h)
        y2 = surface->h - 1;

    // Check for red pixels in the bounding box
    for (int y = y1; y <= y2; y++)
    {
        for (int x = x1; x <= x2; x++)
        {
            if (x >= 0 && x < surface->w && y >= 0 && y < surface->h)
            {
                Uint32 pixel = ((Uint32 *)surface->pixels)[y * surface->w + x];
                if (pixel == red_pixel)
                {
                    // A red pixel is found, exit the function without drawing
                    // the box
                    return;
                }
            }
        }
    }

    // No red pixels found, proceed to draw the red box
    for (int x = x1; x <= x2; x++)
    {
        if (x >= 0 && x < surface->w)
        {
            if (y1 >= 0 && y1 < surface->h)
            {
                ((Uint32 *)surface->pixels)[y1 * surface->w + x] =
                    red_pixel; // Top edge
            }
            if (y2 >= 0 && y2 < surface->h)
            {
                ((Uint32 *)surface->pixels)[y2 * surface->w + x] = red_pixel;
            }
        }
    }
    for (int y = y1; y <= y2; y++)
    {
        if (y >= 0 && y < surface->h)
        {
            if (x1 >= 0 && x1 < surface->w)
            {
                ((Uint32 *)surface->pixels)[y * surface->w + x1] = red_pixel;
            }
            if (x2 >= 0 && x2 < surface->w)
            {
                ((Uint32 *)surface->pixels)[y * surface->w + x2] = red_pixel;
            }
        }
    }
}

void draw_green_box(SDL_Surface *surface, int x1, int y1, int x2, int y2)
{
    Uint32 red_pixel = SDL_MapRGB(surface->format, 0, 255, 0);

    // Ensure the coordinates are within bounds
    if (x1 < 0)
        x1 = 0;
    if (y1 < 0)
        y1 = 0;
    if (x2 >= surface->w)
        x2 = surface->w - 1;
    if (y2 >= surface->h)
        y2 = surface->h - 1;

    // Check for red pixels in the bounding box
    for (int y = y1; y <= y2; y++)
    {
        for (int x = x1; x <= x2; x++)
        {
            if (x >= 0 && x < surface->w && y >= 0 && y < surface->h)
            {
                Uint32 pixel = ((Uint32 *)surface->pixels)[y * surface->w + x];
                if (pixel == red_pixel)
                {
                    // A red pixel is found, exit the function without drawing
                    // the box
                    return;
                }
            }
        }
    }

    // No red pixels found, proceed to draw the red box
    for (int x = x1; x <= x2; x++)
    {
        if (x >= 0 && x < surface->w)
        {
            if (y1 >= 0 && y1 < surface->h)
            {
                ((Uint32 *)surface->pixels)[y1 * surface->w + x] =
                    red_pixel; // Top edge
            }
            if (y2 >= 0 && y2 < surface->h)
            {
                ((Uint32 *)surface->pixels)[y2 * surface->w + x] = red_pixel;
            }
        }
    }
    for (int y = y1; y <= y2; y++)
    {
        if (y >= 0 && y < surface->h)
        {
            if (x1 >= 0 && x1 < surface->w)
            {
                ((Uint32 *)surface->pixels)[y * surface->w + x1] = red_pixel;
            }
            if (x2 >= 0 && x2 < surface->w)
            {
                ((Uint32 *)surface->pixels)[y * surface->w + x2] = red_pixel;
            }
        }
    }
}

void add_square_to_letter(SDL_Surface *surface, int startx, int starty,
                          int endx, int endy)
{
    // Lock the surface for pixel access
    SDL_LockSurface(surface);

    Uint8 r, g, b;
    Uint32 pixel;
    int width = surface->w;


    // Iterate through each pixel to detect letter-like clusters
    for (int y = starty; y < endy; y++)
    {
        for (int x = startx; x < endx; x++)
        {
            pixel = ((Uint32 *)surface->pixels)[y * width + x];
            SDL_GetRGB(pixel, surface->format, &r, &g, &b);

            // Detect black pixels (threshold values may need adjustment)
            if (r < 50 && g < 50 && b < 50)
            {
				int lwidth = get_letter_width(surface,  x,  y);
                int lheight = get_letter_height(surface,  x,  y);
                int left = get_letter_left_bound(surface, x, y);
                // Draw a red box around this letter cluster
                draw_red_box(surface, left, y,
                             left + lwidth, y + lheight);
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