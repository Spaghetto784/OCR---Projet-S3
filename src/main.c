#include <stdio.h>
#include "preprocessor.h"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Usage: %s <image_path>\n", argv[0]);
        return 1;
    }

    printf("Loading image from path: %s\n", argv[1]);
    Image* img = load_image(argv[1]);
    if (img == NULL) {
        printf("Error: Could not load image.\n");
        return 1;
    }
    printf("Image loaded successfully: %dx%d pixels\n", img->width, img->height);

    printf("Converting to grayscale...\n");
    convert_to_grayscale(img);
    printf("Grayscale conversion done.\n");

    printf("Binarizing image...\n");
    binarize_image(img, 128);
    printf("Binarization done.\n");

    printf("Image processing completed successfully.\n");

    free_image(img);
    return 0;
}