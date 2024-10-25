#ifndef PREPROCESSOR_H
#define PREPROCESSOR_H

#include <stdint.h>

typedef struct {
    uint8_t* data;
    int width;
    int height;
    int channels;
} Image;

Image* load_image(const char* path);
void convert_to_grayscale(Image* img);
void binarize_image(Image* img, uint8_t threshold);

void free_image(Image* img);

#endif