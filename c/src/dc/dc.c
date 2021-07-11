#include "dc.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

// COMPRESS
// unpack
// sectorize
// sort
// serialize

// DECOMPRESS
// deserialize
// render
// pack

dc_compression *_dc_compression_init(char *input, char *output, char verbose) {
    dc_compression *compression = malloc(sizeof(dc_compression));
    compression->verbose = verbose;
    compression->input = input;
    compression->output = output;
    compression->pixel_matrix = malloc(sizeof(dc_pixel_matrix));
    compression->pixel_matrix->width = 0;
    compression->pixel_matrix->height = 0;
    return compression;
}

void _dc_compression_destroy(dc_compression *compression) {
    free(compression->pixel_matrix->pixels);
    free(compression->pixel_matrix);
    free(compression);
}

void _dc_unpack(dc_compression *compression) {
    int width, height, comp;
    unsigned char *data = stbi_load(compression->input, &width, &height, &comp, 3);
    if (!data) {
        fprintf(stderr, "Cannot open file\n");
        exit(1);
    }
    compression->pixel_matrix->width = width;
    compression->pixel_matrix->height = height;
    compression->pixel_matrix->pixels = malloc(sizeof(dc_pixel) * width * height);
    size_t j = 0;
    for (size_t i = 0; i < width * height; i++) {
        compression->pixel_matrix->pixels[i].r = data[j++];
        compression->pixel_matrix->pixels[i].g = data[j++];
        compression->pixel_matrix->pixels[i].b = data[j++];
    }
    stbi_image_free(data);
}

void dc_compress(char *input, char *output, char verbose) {

    printf("COMPRESS\n");
    dc_compression *compression = _dc_compression_init(input, output, verbose);

    printf("\tUNPACK\n");
    _dc_unpack(compression);

    _dc_compression_destroy(compression);
}
