#include <string>
#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <vector>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../stb/stb_image_write.h"

#include "dc-decompressor.h"

using namespace std;

dc_decompressor::dc_decompressor(string _input, string _output, bool _verbose): dc(_input, _output, _verbose) { }

dc_decompressor::~dc_decompressor() { }

void dc_decompressor::decompress() {
    if (verbose) cout << "[DECOMPRESS] Input:" << input << endl;
    if (verbose) cout << "[DECOMPRESS] Output:" << output << endl;

    if (verbose) cout << "[DECOMPRESS] Deserialize... ";
    _deserialize();
    int multiplier;
    if (verbose) multiplier = version == 1 ? 4 : version == 2 ? 2 : 0;
    if (verbose) cout << "DONE (" << (sectors_count * multiplier + 10) / 1000 << " KB, version:" << version << ", thr:" << thr << ")" << endl;

    if (verbose) cout << "[DECOMPRESS] Render... ";
    switch (version)
    {
        case 1: _render(false); break;
        case 2: _render(true); break;
        default: break;
    }
    if (verbose) cout << "DONE (" << sectors_count << " sectors)" << endl;

    if (verbose) cout << "[DECOMPRESS] Pack... ";
    _pack();
    if (verbose) cout << "DONE (" << pixel_matrix.get_width() << "x" << pixel_matrix.get_height() << ")" << endl;
    
    if (verbose) printf("[DECOMPRESS] Finished\n");
}

void dc_decompressor::_deserialize() {
    int fd = open(input.c_str(), O_RDWR, S_IRUSR);
    if (!fd) {
        fprintf(stderr, "Cannot open file\n");
        exit(1);
    }
    unsigned char b[8], buffer[1024];
    int readable, count = 0;
    uint32_t _w, _h;
    int i;
    if (read(fd, b, 2) == 2) {
        version = b[0];
        thr = b[1];
    } else {
        fprintf(stderr, "Missing version and threshold\n");
        exit(1);
    }
    if (read(fd, b, 8) == 8) {
        _w = b[0] + (b[1] << 8) + (b[2] << 16) + (b[3] << 24);
        _h = b[4] + (b[5] << 8) + (b[6] << 16) + (b[7] << 24);
    } else {
        fprintf(stderr, "Missing width and height\n");
        exit(1);
    }
    if (version < 1 || version > 2) {
        fprintf(stderr, "Version not supported\n");
        exit(1);
    }
    dc_light_sector light_sector;
    while (readable = read(fd, buffer, 1024)) {
        switch (version) {
            case 1:
                if (readable % 4 == 0) {
                    count += readable;
                    i = 0;
                    while(i < readable) {
                        light_sector.splits = buffer[i++];
                        light_sector.pix.set_r(buffer[i++]);
                        light_sector.pix.set_g(buffer[i++]);
                        light_sector.pix.set_b(buffer[i++]);
                        light_sector.value = 0;
                        l_sectors.push_back(light_sector);
                        sectors_count++;
                    }
                } else {
                    fprintf(stderr, "Corrupted sectors\n");
                    exit(1);
                }
                break;
            case 2:
                if (readable % 1 == 0) {
                    count += readable;
                    i = 0;
                    while (i < readable) {
                        light_sector.splits = buffer[i++];
                        light_sector.value = buffer[i++];
                        l_sectors.push_back(light_sector);
                        sectors_count++;
                    }
                } else {
                    fprintf(stderr, "Corrupted sectors\n");
                    exit(1);
                }
                break;
            default: break;
        }
    }
    close(fd);
    pixel_matrix.set_width((int)_w);
    pixel_matrix.set_height((int)_h);
    pixel_matrix.set_pixels(new dc_pixel[pixel_matrix.get_width() * pixel_matrix.get_height()]);
}

void dc_decompressor::_render(bool channel_sensitive) {
    int w = pixel_matrix.get_width();
    int h = pixel_matrix.get_height();
    unsigned char *mat = new unsigned char[w * h];
    dc_corners cor;
    int dx, dy, mid;
    dc_channels channel = RED;
    for (auto light_iter = l_sectors.begin(); light_iter!=l_sectors.end(); ) {
        for (int i = 0; i < w * h; i++) mat[i] = 0;
        for (int y = 0; y < h; y++) {
            for (int x = 0; x < w; x++) {
                if (mat[ix(x, y, w)] == 0) {
                    cor.x0 = 0; cor.y0 = 0;
                    cor.x1 = w; cor.y1 = h;
                    for (int i = 0; i < light_iter->splits; i++) {
                        dx = cor.x1 - cor.x0;
                        dy = cor.y1 - cor.y0;
                        if (dx > dy) {
                            mid = middle(cor.x0, cor.x1);
                            if (mid > x) cor.x1 = mid;
                            else cor.x0 = mid;
                        } else {
                            mid = middle(cor.y0, cor.y1);
                            if (mid > y) cor.y1 = mid;
                            else cor.y0 = mid;
                        }
                    }
                    if (channel_sensitive) {
                        pixel_matrix.draw_n_occupy(cor, mat, *light_iter, channel);
                    } else {
                        pixel_matrix.draw_n_occupy(cor, mat, *light_iter, NONE);
                    }
                    light_iter++;
                }
            }
        }
        channel = (dc_channels)(channel+1);
    }
}

void dc_decompressor::_pack() {
    size_t w = pixel_matrix.get_width();
    size_t h = pixel_matrix.get_height();
    unsigned char *data = new unsigned char[w * h * 3];
    size_t i = 0;
    for (size_t y = 0; y < h; y++) {
        for (size_t x = 0; x < w; x++) {
            dc_pixel &pix = pixel_matrix.get_pixels()[ix(x, y, w)];
            data[i++] = pix.get_g();
            data[i++] = pix.get_b();
            data[i++] = pix.get_r();
        }
    }
    stbi_write_png(output.c_str(), w, h, 3, data, 0); // todo: see why it takes so long
    delete[] data;
}