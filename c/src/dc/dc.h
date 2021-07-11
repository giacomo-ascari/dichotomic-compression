#ifndef _DC_H
#define _DC_H

#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>

typedef struct _dc_pixel {
    char r;
    char g;
    char b;
} dc_pixel;

typedef struct _dc_pixel_matrix {
    dc_pixel *pixels;
    int width;
    int height;
} dc_pixel_matrix;

typedef struct _dc_compression {
    char verbose;
    char *input;
    char *output;
    dc_pixel_matrix *pixel_matrix;
} dc_compression;


void dc_compress(char *input, char *output, char verbose);

#endif