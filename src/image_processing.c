#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>

// Constants for grayscale conversion
#define RED_WEIGHT 0.2126
#define GREEN_WEIGHT 0.7152
#define BLUE_WEIGHT 0.0722

// Function to convert an image to grayscale
SDL_Surface* preprocess_image(SDL_Surface* surface) {
    if (surface == NULL) {
        printf("Input surface is NULL\n");
        return NULL; // Handle null input
    }

    // Create a new surface for the grayscale image
    SDL_Surface* grayscaleSurface = SDL_CreateRGBSurface(0, surface->w, surface->h, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
    if (grayscaleSurface == NULL) {
        printf("Unable to create grayscale surface: %s\n", SDL_GetError());
        return NULL;
    }

    // Lock the surfaces for direct pixel access
    SDL_LockSurface(surface);
    SDL_LockSurface(grayscaleSurface);

    // Iterate over each pixel
    for (int y = 0; y < surface->h; y++) {
        for (int x = 0; x < surface->w; x++) {
            // Get the pixel color
            Uint32 pixel = ((Uint32*)surface->pixels)[y * (surface->pitch / 4) + x];
            Uint8 r, g, b;
            SDL_GetRGB(pixel, surface->format, &r, &g, &b);

            // Calculate the grayscale value
            Uint8 gray = (Uint8)(RED_WEIGHT * r + GREEN_WEIGHT * g + BLUE_WEIGHT * b);

            // Set the pixel color in the grayscale image
            Uint32 grayPixel = SDL_MapRGB(grayscaleSurface->format, gray, gray, gray);
            ((Uint32*)grayscaleSurface->pixels)[y * (grayscaleSurface->pitch / 4) + x] = grayPixel;
        }
    }

    // Unlock the surfaces
    SDL_UnlockSurface(surface);
    SDL_UnlockSurface(grayscaleSurface);

    return grayscaleSurface;
}

Uint8 calculate_threshold(SDL_Surface* surface) {
    if (surface == NULL) {
        printf("Input surface is NULL\n");
        return 0;
    }

    // Calculer le spectre des couleurs pour déterminer le seuil adaptatif
    Uint64 pixelSum = 0;
    Uint64 pixelCount = 0;

    SDL_LockSurface(surface);

    for (int y = 0; y < surface->h; y++) {
        for (int x = 0; x < surface->w; x++) {
            Uint32 pixel = ((Uint32*)surface->pixels)[y * (surface->pitch / 4) + x];
            Uint8 r, g, b;
            SDL_GetRGB(pixel, surface->format, &r, &g, &b);

            // Utiliser une luminance standard (pondérations pour r, g, b)
            Uint8 luminance = (Uint8)(0.2126 * r + 0.7152 * g + 0.0722 * b);
            pixelSum += luminance;
            pixelCount++;
        }
    }

    // Calculer le seuil adaptatif comme moyenne des luminances
    Uint8 threshold = (Uint8)(pixelSum / pixelCount);

    SDL_UnlockSurface(surface);
    return threshold;
}

// Function to convert a grayscale image to black and white
SDL_Surface* convert_to_bw(SDL_Surface* grayscaleSurface, Uint8 threshold) {
    if (grayscaleSurface == NULL) {
        printf("Input grayscale surface is NULL\n");
        return NULL; // Handle null input
    }

    // Create a new surface for the black and white image
    SDL_Surface* bwSurface = SDL_CreateRGBSurface(0, grayscaleSurface->w, grayscaleSurface->h, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
    if (bwSurface == NULL) {
        printf("Unable to create black and white surface: %s\n", SDL_GetError());
        return NULL;
    }

    // Lock the surfaces for direct pixel access
    SDL_LockSurface(grayscaleSurface);
    SDL_LockSurface(bwSurface);

    // Iterate over each pixel
    for (int y = 0; y < grayscaleSurface->h; y++) {
        for (int x = 0; x < grayscaleSurface->w; x++) {
            // Get the grayscale value
            Uint32 pixel = ((Uint32*)grayscaleSurface->pixels)[y * (grayscaleSurface->pitch / 4) + x];
            Uint8 gray;
            SDL_GetRGB(pixel, grayscaleSurface->format, &gray, &gray, &gray);

            // Apply the threshold to convert to black or white
            Uint8 bwColor = (gray > threshold) ? 255 : 0;

            // Set the pixel color in the black and white image
            Uint32 bwPixel = SDL_MapRGB(bwSurface->format, bwColor, bwColor, bwColor);
            ((Uint32*)bwSurface->pixels)[y * (bwSurface->pitch / 4) + x] = bwPixel;
        }
    }

    // Unlock the surfaces
    SDL_UnlockSurface(grayscaleSurface);
    SDL_UnlockSurface(bwSurface);

    return bwSurface;
}

int restrict_val(int x, int mi, int ma)
{
    return x<mi ? mi : (x>ma ? ma : x);
}

SDL_Surface* enhance_contrast(SDL_Surface* surface, double contrast, double gamma) {
    if (surface == NULL) {
        printf("Input surface is NULL\n");
        return NULL; // Handle null input
    }

    // Create a new surface for the contrasted image
    SDL_Surface* contrastedSurface = SDL_CreateRGBSurface(0, surface->w, surface->h, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
    if (contrastedSurface == NULL) {
        printf("Unable to create the contrasted surface: %s\n", SDL_GetError());
        return NULL;
    }

    // Lock the surfaces for direct pixel access
    SDL_LockSurface(surface);
    SDL_LockSurface(contrastedSurface);

    // Iterate over each pixel
    for (int y = 0; y < surface->h; y++) {
        for (int x = 0; x < surface->w; x++) {
            // Get the pixel color
            Uint32 pixel = ((Uint32*)surface->pixels)[y * (surface->pitch / 4) + x];
            Uint8 r, g, b;
            SDL_GetRGB(pixel, surface->format, &r, &g, &b);

            //calculate value of contrasted pixel
            Uint8 nr,ng,nb;
            nr = (Uint8)(restrict_val((contrast*(r-128)+128+gamma),0,255));
            ng = (Uint8)(restrict_val((contrast*(g-128)+128+gamma),0,255));
            nb = (Uint8)(restrict_val((contrast*(b-128)+128+gamma),0,255));


            // Set the pixel color in the contrasted image
            Uint32 contrastedPixel = SDL_MapRGB(contrastedSurface->format, nr, ng, nb);
            ((Uint32*)contrastedSurface->pixels)[y * (contrastedSurface->pitch / 4) + x] = contrastedPixel;
        }
    }

    // Unlock the surfaces
    SDL_UnlockSurface(surface);
    SDL_UnlockSurface(contrastedSurface);

    return contrastedSurface;
}


//function to create a gaussian kernel
void generateGaussianKernel(float *kernel, int kernelSize, float sigma) {
    int halfSize = kernelSize / 2;
    float sum = 0.0f;

    for (int y = -halfSize; y <= halfSize; y++) {
        for (int x = -halfSize; x <= halfSize; x++) {
            float value = expf(-(x * x + y * y) / (2 * sigma * sigma));
            kernel[(y + halfSize) * kernelSize + (x + halfSize)] = value;
            sum += value;
        }
    }

    // Kernel normalization
    for (int i = 0; i < kernelSize * kernelSize; i++) {
        kernel[i] /= sum;
    }
}

// Function to apply gaussian filter
SDL_Surface *apply_gaussian_filter(SDL_Surface *surface, int kernelSize, float sigma) {
    if (surface == NULL) return NULL;

    // Gaussian Kernel Generation
    float *kernel = malloc(kernelSize * kernelSize * sizeof(float));
    if (kernel == NULL) return NULL;
    generateGaussianKernel(kernel, kernelSize, sigma);

   
    SDL_Surface *result = SDL_CreateRGBSurfaceWithFormat(
        0, surface->w, surface->h, surface->format->BitsPerPixel, surface->format->format);
    if (result == NULL) {
        free(kernel);
        return NULL;
    }

    SDL_LockSurface(surface);
    SDL_LockSurface(result);

    Uint32 *srcPixels = (Uint32 *)surface->pixels;
    Uint32 *dstPixels = (Uint32 *)result->pixels;

    int halfSize = kernelSize / 2;

    for (int y = 0; y < surface->h; y++) {
        for (int x = 0; x < surface->w; x++) {
            float r = 0, g = 0, b = 0;
            float kernelSum = 0.0f;

            for (int ky = -halfSize; ky <= halfSize; ky++) {
                for (int kx = -halfSize; kx <= halfSize; kx++) {
                    int px = x + kx;
                    int py = y + ky;

                    if (px >= 0 && px < surface->w && py >= 0 && py < surface->h) {
                        Uint32 pixel = srcPixels[py * surface->w + px];
                        Uint8 pr, pg, pb;
                        SDL_GetRGB(pixel, surface->format, &pr, &pg, &pb);

                        float kernelValue = kernel[(ky + halfSize) * kernelSize + (kx + halfSize)];
                        r += pr * kernelValue;
                        g += pg * kernelValue;
                        b += pb * kernelValue;
                        kernelSum += kernelValue;
                    }
                }
            }

        
            r /= kernelSum;
            g /= kernelSum;
            b /= kernelSum;

            dstPixels[y * surface->w + x] = SDL_MapRGB(result->format, (Uint8)r, (Uint8)g, (Uint8)b);
        }
    }

    SDL_UnlockSurface(surface);
    SDL_UnlockSurface(result);

    free(kernel);
    return result;
}


// Function which compare two numbers (useful for qsort)
int compare(const void *a, const void *b) {
    return (*(Uint8 *)a - *(Uint8 *)b);
}

// function to apply median filter
SDL_Surface *apply_median_filter(SDL_Surface *surface, int kernelSize) {
    if (surface == NULL || kernelSize < 3 || kernelSize % 2 == 0) return NULL;

    // Creation of new surface for result
    SDL_Surface *result = SDL_CreateRGBSurfaceWithFormat(
        0, surface->w, surface->h, surface->format->BitsPerPixel, surface->format->format);
    if (result == NULL) {
        return NULL;
    }

    SDL_LockSurface(surface);
    SDL_LockSurface(result);

    Uint32 *srcPixels = (Uint32 *)surface->pixels;
    Uint32 *dstPixels = (Uint32 *)result->pixels;

    int halfSize = kernelSize / 2;
    int windowSize = kernelSize * kernelSize;

    Uint8 *rValues = malloc(windowSize * sizeof(Uint8));
    Uint8 *gValues = malloc(windowSize * sizeof(Uint8));
    Uint8 *bValues = malloc(windowSize * sizeof(Uint8));

    if (rValues == NULL || gValues == NULL || bValues == NULL) {
        SDL_UnlockSurface(surface);
        SDL_UnlockSurface(result);
        SDL_FreeSurface(result);
        free(rValues);
        free(gValues);
        free(bValues);
        return NULL;
    }

    // Iterate over pixels
    for (int y = 0; y < surface->h; y++) {
        for (int x = 0; x < surface->w; x++) {
            int count = 0;

        
            for (int ky = -halfSize; ky <= halfSize; ky++) {
                for (int kx = -halfSize; kx <= halfSize; kx++) {
                    int px = x + kx;
                    int py = y + ky;

                    
                    if (px >= 0 && px < surface->w && py >= 0 && py < surface->h) {
                        Uint32 pixel = srcPixels[py * surface->w + px];
                        Uint8 r, g, b;
                        SDL_GetRGB(pixel, surface->format, &r, &g, &b);

                        rValues[count] = r;
                        gValues[count] = g;
                        bValues[count] = b;
                        count++;
                    }
                }
            }

            // sorting values to find median
            qsort(rValues, count, sizeof(Uint8), compare);
            qsort(gValues, count, sizeof(Uint8), compare);
            qsort(bValues, count, sizeof(Uint8), compare);

            Uint8 medianR = rValues[count / 2];
            Uint8 medianG = gValues[count / 2];
            Uint8 medianB = bValues[count / 2];

            // maping the pixel after treatment 
            dstPixels[y * surface->w + x] = SDL_MapRGB(result->format, medianR, medianG, medianB);
        }
    }

    SDL_UnlockSurface(surface);
    SDL_UnlockSurface(result);

    free(rValues);
    free(gValues);
    free(bValues);

    return result;
}

SDL_Surface* invert_colors(SDL_Surface* surface) {
    if (surface == NULL) {
        printf("Input surface is NULL\n");
        return NULL; // Gérer le cas où l'entrée est NULL
    }

    // Créer une nouvelle surface pour l'image avec les couleurs inversées
    SDL_Surface* invertedSurface = SDL_CreateRGBSurface(0, surface->w, surface->h, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
    if (invertedSurface == NULL) {
        printf("Unable to create inverted surface: %s\n", SDL_GetError());
        return NULL;
    }

    // Verrouiller les surfaces pour un accès direct aux pixels
    SDL_LockSurface(surface);
    SDL_LockSurface(invertedSurface);

    // Parcourir chaque pixel
    for (int y = 0; y < surface->h; y++) {
        for (int x = 0; x < surface->w; x++) {
            // Obtenir la couleur du pixel
            Uint32 pixel = ((Uint32*)surface->pixels)[y * (surface->pitch / 4) + x];
            Uint8 r, g, b;
            SDL_GetRGB(pixel, surface->format, &r, &g, &b);

            // Inverser les couleurs (255 - composante)
            Uint8 invertedR = 255 - r;
            Uint8 invertedG = 255 - g;
            Uint8 invertedB = 255 - b;

            // Mapper les couleurs inversées dans le format de la nouvelle surface
            Uint32 invertedPixel = SDL_MapRGB(invertedSurface->format, invertedR, invertedG, invertedB);
            ((Uint32*)invertedSurface->pixels)[y * (invertedSurface->pitch / 4) + x] = invertedPixel;
        }
    }

    // Déverrouiller les surfaces
    SDL_UnlockSurface(surface);
    SDL_UnlockSurface(invertedSurface);

    return invertedSurface;
}



