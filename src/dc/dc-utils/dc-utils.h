#ifndef _DC_UTILS_H
#define _DC_UTILS_H

#include <string>

size_t ix(size_t x, size_t y, size_t width);

size_t middle(size_t a, size_t b);

enum dc_channels { RED=0, GREEN=1, BLUE=2, NONE=3 };

typedef struct _dc_corners {
    size_t x0, y0;
    size_t x1, y1;
} dc_corners;

class dc_pixel {
    public:
        dc_pixel();
        dc_pixel(unsigned char _r, unsigned char _g, unsigned char _b);
        ~dc_pixel();
        void increase_channels(unsigned char _r, unsigned char _g, unsigned char _b);
        void set_channels(unsigned char _r, unsigned char _g, unsigned char _b);
        void increase_channel(unsigned char value, dc_channels channel);
        void set_channel(unsigned char value, dc_channels channel);
        void set_r(unsigned char value) { r = value; }
        void set_g(unsigned char value) { g = value; }
        void set_b(unsigned char value) { b = value; }
        unsigned char get_channel(dc_channels channel);
        unsigned char get_r() { return r; }
        unsigned char get_g() { return g; }
        unsigned char get_b() { return b; }
    private:
        unsigned char r;
        unsigned char g;
        unsigned char b;
};

typedef struct _dc_heavy_sector {
    dc_corners corners;
    dc_pixel pix;
    unsigned char value;
    unsigned char splits;
} dc_heavy_sector;

typedef struct _dc_light_sector {
    dc_pixel pix;
    unsigned char value;
    unsigned char splits;
} dc_light_sector;

class dc_pixel_matrix {
    public:
        dc_pixel_matrix();
        ~dc_pixel_matrix();
        void set_pixels(dc_pixel *value) { pixels = value; }
        void set_width(size_t value) { width = value; }
        void set_height(size_t value) { height = value; }
        size_t get_delta(dc_corners &cor, dc_channels channel);
        void get_average(dc_corners &cor, dc_heavy_sector &h_sector, dc_channels channel);
        dc_pixel *get_pixels() { return pixels; }
        size_t get_width() { return width; }
        size_t get_height() { return height; }
        void draw_n_occupy(dc_corners &cor, unsigned char *mat, dc_light_sector &l_sector, dc_channels channel);
        void print();
    private:
        dc_pixel *pixels;
        size_t width;
        size_t height;
};

#endif