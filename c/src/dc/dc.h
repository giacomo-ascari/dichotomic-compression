#ifndef _DC_H
#define _DC_H

#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

typedef struct _dc_pixel {
    unsigned char r;
    unsigned char g;
    unsigned char b;
} dc_pixel;

typedef struct _dc_corners {
    int x0, y0;
    int x1, y1;
} dc_corners;

typedef struct _dc_pixel_matrix {
    dc_pixel *pixels;
    int width;
    int height;
} dc_pixel_matrix;

typedef struct _dc_heavy_sector {
    dc_corners corners;
    dc_pixel pix;
    int splits;
} dc_heavy_sector;

typedef struct _dc_light_sector {
    dc_pixel pix;
    int splits;
    struct _dc_light_sector *next;
    struct _dc_light_sector *prev;
} dc_light_sector;

typedef struct _dc_compression {
    int verbose;
    unsigned char *input;
    unsigned char *output;
    unsigned char thr;
    dc_pixel_matrix *pixel_matrix;
    dc_heavy_sector **h_sectors;
} dc_compression;

typedef struct _dc_decompression {
    int verbose;
    unsigned char *input;
    unsigned char *output;
    dc_pixel_matrix *pixel_matrix;
    dc_light_sector *l_sector;
} dc_decompression;

void dc_compress(unsigned char *input, unsigned char *output, unsigned char thr, int verbose);

void dc_decompress(unsigned char *input, unsigned char *output, int verbose);

#endif