#include <cmath>
#include <iostream>

#include "dc-utils.h"

using namespace std;

dc_pixel_matrix::dc_pixel_matrix() {
    width = 0;
    height = 0;
    pixels = NULL;
}

dc_pixel_matrix::~dc_pixel_matrix() {
    delete[] pixels;
}

int dc_pixel_matrix::get_delta(dc_corners &cor, dc_channels channel) {
    dc_pixel max_pix(0, 0, 0);
    dc_pixel min_pix(255, 255, 255);
    int delta, mx = 0, mn = 255;
    for (size_t y = cor.y0; y < cor.y1; y++) {
        for (size_t x = cor.x0; x < cor.x1; x++) {
            dc_pixel *current = &pixels[ix(x, y, width)];
            int cur_channel;
            if (channel == NONE) {
                max_pix.set_r(max(current->get_r(), max_pix.get_r()));
                max_pix.set_g(max(current->get_g(), max_pix.get_g()));
                max_pix.set_b(max(current->get_b(), max_pix.get_b()));
                min_pix.set_r(min(current->get_r(), min_pix.get_r()));
                min_pix.set_g(min(current->get_g(), min_pix.get_g()));
                min_pix.set_b(min(current->get_b(), min_pix.get_b()));
            } else {
                cur_channel = current->get_channel(channel);
                mx = max(cur_channel, mx);
                mn = min(cur_channel, mn);
            }
        }
    }
    if (channel == NONE) {
        delta = (max_pix.get_r() - min_pix.get_r()
                + max_pix.get_g() - min_pix.get_g()
                + max_pix.get_b() - min_pix.get_b()) / 3;
    } else {
        delta = mx - mn;
    }
    return delta;
}

void dc_pixel_matrix::get_average(dc_corners &cor, dc_heavy_sector &h_sector, dc_channels channel) {
    int denom = (cor.x1 - cor.x0) * (cor.y1 - cor.y0);
    double r = 0., g = 0., b = 0., val = 0.;
    for (size_t y = cor.y0; y < cor.y1; y++) {
        for (size_t x = cor.x0; x < cor.x1; x++) {
            if (channel == NONE) {
                r += (double)pixels[ix(x, y, width)].get_r() / denom;
                g += (double)pixels[ix(x, y, width)].get_g() / denom;
                b += (double)pixels[ix(x, y, width)].get_b() / denom;
            } else {
                val += (double)pixels[ix(x, y, width)].get_channel(channel) / denom;
            }
        }
    }
    if (channel == NONE) {
        h_sector.pix.set_r(r);
        h_sector.pix.set_g(g);
        h_sector.pix.set_b(b);
    } else {
        h_sector.value = val;
    }
}

void dc_pixel_matrix::draw_n_occupy(dc_corners &cor, unsigned char *mat, dc_light_sector &l_sector, dc_channels channel) {
    size_t i;
    for (size_t y = cor.y0; y < cor.y1; y++) {
        for (size_t x = cor.x0; x < cor.x1; x++) {
            i = ix(x, y, width);
            mat[i] = 1;
            if (channel == NONE) {
                pixels[i].set_channels(l_sector.pix.get_r(), l_sector.pix.get_g(), l_sector.pix.get_b());
            } else {
                pixels[i].increase_channel(l_sector.value, channel);
            }
        }
    }
}