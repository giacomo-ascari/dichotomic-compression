#include <string>
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "../stb/stb_image.h"

#include "dc-compressor.h"

using namespace std;

dc_compressor::dc_compressor(string _input, string _output, bool _verbose, int _version, int _thr): dc(_input, _output, _verbose) {
    version = _version;
    thr = _thr;
}

dc_compressor::~dc_compressor() { }

void dc_compressor::compress() {
    if (verbose) cout << "[COMPRESS] Input: " << input << endl;
    if (verbose) cout << "[COMPRESS] Output: " << output << endl;
    if (verbose) cout << "[COMPRESS] Thr: " << thr << ", version: " << version << endl;

    if (verbose) cout << "[COMPRESS] Unpack... ";
    switch (version) {
        case 1: _unpack(false); break;
        case 2: _unpack(true); break;
        default: break;
    }
    if (verbose) cout << "DONE (" << pixel_matrix.get_width() << "x" << pixel_matrix.get_height() << ", " << input_filesize/1000 << "KB)" << endl;

    if (verbose) cout << "[COMPRESS] Sectorize... ";
    switch (version) {
        case 1:
            _sectorize((dc_corners){0, 0, pixel_matrix.get_width(), pixel_matrix.get_height()}, 0, NONE);
            break;
        case 2:
            _sectorize((dc_corners){0, 0, pixel_matrix.get_width(), pixel_matrix.get_height()}, 0, RED);
            _sectorize((dc_corners){0, 0, pixel_matrix.get_width(), pixel_matrix.get_height()}, 0, GREEN);
            _sectorize((dc_corners){0, 0, pixel_matrix.get_width(), pixel_matrix.get_height()}, 0, BLUE);
            break;
        default: cerr << "Unacceptable version value" << endl; break;
    }
    if (verbose) cout << "DONE (" << sectors_count << " sectors)" << endl;

    if (verbose) cout << "[COMPRESS] Serialize... ";
    switch (version) {
        case 1: _serialize(false); break;
        case 2: _serialize(true); break;
        default: break;
    }
    size_t filesize;
    int multiplier;
    if (verbose) multiplier = version == 1 ? 4 : version == 2 ? 2 : 0;
    if (verbose) filesize = sectors_count * multiplier + 10;
    if (verbose) cout << "DONE (" << filesize / 1000 << " KB)" << endl;

    size_t comp_ratio;
    if (verbose) comp_ratio = filesize * 100 / input_filesize;
    if (verbose) cout << "[COMPRESS] Finished (CR " << comp_ratio << ")\n";
}

void dc_compressor::_unpack(bool channel_sensitive) {
    int width, height, comp;
    unsigned char *data = stbi_load(input.c_str(), &width, &height, &comp, 3);
    if (!data) {
        cerr << "Cannot open file" << endl;
        exit(1);
    } else {
        FILE *fp = fopen(input.c_str(), "rb");
        fseek(fp, 0, SEEK_END);
        input_filesize = ftell(fp);
        rewind(fp);
        fclose(fp);
    }
    pixel_matrix.set_width(width);
    pixel_matrix.set_height(height);
    pixel_matrix.set_pixels(new dc_pixel[width * height]);
    int j = 0;
    for (int i = 0; i < width * height; i++) {
        pixel_matrix.get_pixels()[i].set_r(data[j++]);
        pixel_matrix.get_pixels()[i].set_g(data[j++]);
        pixel_matrix.get_pixels()[i].set_b(data[j++]);
    }
    stbi_image_free(data);
    int length;
    if (channel_sensitive) {
        length = width * height * 3;
    } else {
        length = width * height;
    }
}

void dc_compressor::_sectorize(dc_corners cor, int splits, dc_channels channel) {
    int delta = pixel_matrix.get_delta(cor, channel);
    int dx = cor.x1 - cor.x0;
    int dy = cor.y1 - cor.y0;
    int area = dx * dy;
    int mid = 0;
    dc_corners temp = {0, 0, 0, 0};
    dc_heavy_sector h_sector;
    if (area > 1 && delta > thr) {
        splits++;
        if (dx > dy) {
            mid = middle(cor.x0, cor.x1);
            temp = (dc_corners){cor.x0, cor.y0, mid, cor.y1};
            _sectorize(temp, splits, channel);
            temp = (dc_corners){mid, cor.y0, cor.x1, cor.y1};
            _sectorize(temp, splits, channel);
        } else {
            mid = middle(cor.y0, cor.y1);
            temp = (dc_corners){cor.x0, cor.y0, cor.x1, mid};
            _sectorize(temp, splits, channel);
            temp = (dc_corners){cor.x0, mid, cor.x1, cor.y1};
            _sectorize(temp, splits, channel);
        }
    } else {
        sectors_count++;
        size_t index, length;
        if (channel == NONE) {
            index = ix(cor.x0, cor.y0, pixel_matrix.get_width());
        } else {
            length = pixel_matrix.get_width() * pixel_matrix.get_height();
            index = ix(cor.x0, cor.y0, pixel_matrix.get_width()) + length * (int)channel;
        }
        pixel_matrix.get_average(cor, h_sector, channel);
        h_sector.corners = cor;
        h_sector.splits = splits;
        h_sectors.insert(pair<size_t, dc_heavy_sector>(index, h_sector));
    }
}

void dc_compressor::_serialize(bool channel_sensitive) {
    FILE *file = fopen(output.c_str(), "w");
    if (!file) {
        fprintf(stderr, "Cannot save file\n");
        exit(1);
    }
    fprintf(file, "%c", (unsigned char)version);
    fprintf(file, "%c", (unsigned char)thr);
    int w = pixel_matrix.get_width();
    int h = pixel_matrix.get_height();
    unsigned char b[4];
    b[0] = (w >> 24) & 0xFF; b[1] = (w >> 16) & 0xFF;  b[2] = (w >> 8) & 0xFF; b[3] = w & 0xFF;
    fprintf(file, "%c%c%c%c", b[3], b[2], b[1], b[0]);
    b[0] = (h >> 24) & 0xFF; b[1] = (h >> 16) & 0xFF;  b[2] = (h >> 8) & 0xFF; b[3] = h & 0xFF;
    fprintf(file, "%c%c%c%c", b[3], b[2], b[1], b[0]);
    int length = w * h;
    dc_pixel *pp;
    dc_heavy_sector *hp;
    if (channel_sensitive) {
        for (auto heavy_iter=h_sectors.begin(); heavy_iter!=h_sectors.end(); heavy_iter++) {
            fprintf(file, "%c%c", heavy_iter->second.splits, heavy_iter->second.value);
        }
    } else {
        for (auto heavy_iter=h_sectors.begin(); heavy_iter!=h_sectors.end(); heavy_iter++) {
            pp = &(heavy_iter->second.pix);
            fprintf(file, "%c%c%c%c", heavy_iter->second.splits, pp->get_r(), pp->get_g(), pp->get_b());
        }
    }
    fclose(file);
}