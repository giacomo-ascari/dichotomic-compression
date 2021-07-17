#include "dc.h"

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image.h"
#include "stb/stb_image_write.h"

// COMPRESS
// unpack
// sectorize+sort
// serialize

// DECOMPRESS
// deserialize
// render
// pack

int ix(int x, int y, int width) {
    return x + (y * width);
}

void _dc_print_corners(dc_corners *corp) {
    printf("(%d, %d), (%d, %d)\n", corp->x0, corp->y0, corp->x1, corp->y1);
}

void _dc_print_heavy_sector(dc_heavy_sector *hp) {
    printf("%d (%02x%02x%02x) - ", hp->splits, hp->pix.r, hp->pix.g, hp->pix.b);
    _dc_print_corners(&(hp->corners));
}

void _dc_print_light_sector(dc_light_sector *lp) {
    printf("[%p] %d (%02x%02x%02x) [%p] [%p] \n", lp, lp->splits, lp->pix.r, lp->pix.g, lp->pix.b, lp->prev, lp->next);
}

void _dc_print_pixel_matrix(dc_pixel_matrix *pmp, dc_corners *corp) {
    dc_pixel *pix = pmp->pixels;
    int w = pmp->width;
    for (size_t y = corp->y0; y < corp->y1; y++) {
        for (size_t x = corp->x0; x < corp->x1; x++) {
            printf("%02x%02x%02x ", pix[ix(x, y, w)].r, pix[ix(x, y, w)].g, pix[ix(x, y, w)].b);
        }
        printf("\n");
    }
}

dc_compression *_dc_compression_init(unsigned char *input, unsigned char *output, unsigned char thr, int verbose) {
    dc_compression *cp = malloc(sizeof(dc_compression));
    cp->verbose = verbose;
    cp->input = input;
    cp->output = output;
    cp->thr = thr;
    cp->pixel_matrix = malloc(sizeof(dc_pixel_matrix));
    cp->pixel_matrix->width = 0;
    cp->pixel_matrix->height = 0;
    cp->h_sectors = NULL;
    return cp;
}

void _dc_compression_destroy(dc_compression *cp) {
    free(cp->pixel_matrix->pixels);
    free(cp->pixel_matrix);
    free(cp->h_sectors);
    free(cp);
}

int _dc_get_mid(int a, int b) {
    return a + (b - a) / 2;
}

void _dc_get_avg(dc_compression *cp, dc_corners *corp, dc_pixel *pp) {
    int denom = (corp->x1 - corp->x0) * (corp->y1 - corp->y0);
    double r = 0., g = 0., b = 0.;
    int w = cp->pixel_matrix->width;
    dc_pixel *pix = cp->pixel_matrix->pixels;
    for (size_t y = corp->y0; y < corp->y1; y++) {
        for (size_t x = corp->x0; x < corp->x1; x++) {
            r += (double)pix[ix(x, y, w)].r / denom;
            g += (double)pix[ix(x, y, w)].g / denom;
            b += (double)pix[ix(x, y, w)].b / denom;
        }
    }
    pp->r = r;
    pp->g = g;
    pp->b = b;
}

int _dc_get_delta(dc_compression *cp, dc_corners *corp) {
    dc_pixel max = {0, 0, 0};
    dc_pixel min = {255, 255, 255};
    int w = cp->pixel_matrix->width;
    dc_pixel *pix = cp->pixel_matrix->pixels;
    for (size_t y = corp->y0; y < corp->y1; y++) {
        for (size_t x = corp->x0; x < corp->x1; x++) {
            dc_pixel *cur = &pix[ix(x, y, w)];
            if (cur->r > max.r) max.r = cur->r;
            if (cur->g > max.g) max.g = cur->g;
            if (cur->b > max.b) max.b = cur->b;
            if (cur->r < min.r) min.r = cur->r;
            if (cur->g < min.g) min.g = cur->g;
            if (cur->b < min.b) min.b = cur->b;
        }
    }
    int delta = (max.r - min.r + max.g - min.g + max.b - min.b) / 3;
    return delta;
}

void _dc_unpack(dc_compression *cp) {
    int width, height, comp;
    unsigned char *data = stbi_load(cp->input, &width, &height, &comp, 3);
    if (!data) {
        fprintf(stderr, "Cannot open file\n");
        exit(1);
    }
    cp->pixel_matrix->width = width;
    cp->pixel_matrix->height = height;
    cp->pixel_matrix->pixels = malloc(sizeof(dc_pixel) * width * height);
    size_t j = 0;
    for (size_t i = 0; i < width * height; i++) {
        cp->pixel_matrix->pixels[i].r = data[j++];
        cp->pixel_matrix->pixels[i].g = data[j++];
        cp->pixel_matrix->pixels[i].b = data[j++];
    }
    stbi_image_free(data);
    cp->h_sectors = malloc(sizeof(dc_heavy_sector*) * width * height);
    for (size_t i = 0; i < width * height; i++) cp->h_sectors[i] = NULL;
}

void _dc_sectorize(dc_compression *cp, dc_corners cor, int splits) {
    int delta = _dc_get_delta(cp, &cor);
    int dx = cor.x1 - cor.x0;
    int dy = cor.y1 - cor.y0;
    int area = dx * dy;
    int mid = 0;
    dc_corners temp = {0, 0, 0, 0};
    if (area > 1 && delta > cp->thr) {
        splits++;
        if (dx > dy) {
            mid = _dc_get_mid(cor.x0, cor.x1);
            temp = (dc_corners){cor.x0, cor.y0, mid, cor.y1};
            _dc_sectorize(cp, temp, splits);
            temp = (dc_corners){mid, cor.y0, cor.x1, cor.y1};
            _dc_sectorize(cp, temp, splits);
        } else {
            mid = _dc_get_mid(cor.y0, cor.y1);
            temp = (dc_corners){cor.x0, cor.y0, cor.x1, mid};
            _dc_sectorize(cp, temp, splits);
            temp = (dc_corners){cor.x0, mid, cor.x1, cor.y1};
            _dc_sectorize(cp, temp, splits);
        }
    } else {
        cp->h_sectors[ix(cor.x0, cor.y0, cp->pixel_matrix->width)] = malloc(sizeof(dc_heavy_sector));
        dc_heavy_sector *hp = cp->h_sectors[ix(cor.x0, cor.y0, cp->pixel_matrix->width)];
        _dc_get_avg(cp, &cor, &(hp->pix));
        hp->corners = cor;
        hp->splits = splits;
    }
}

void _dc_serialize(dc_compression *cp) {
    FILE *file = fopen(cp->output, "w");
    if (!file) {
        fprintf(stderr, "Cannot save file\n");
        exit(1);
    }
    fprintf(file, "%c%c", 1, cp->thr);
    int w = cp->pixel_matrix->width;
    int h = cp->pixel_matrix->height;
    unsigned char b[4];
    b[0] = (w >> 24) & 0xFF; b[1] = (w >> 16) & 0xFF;  b[2] = (w >> 8) & 0xFF; b[3] = w & 0xFF;
    fprintf(file, "%c%c%c%c", b[3], b[2], b[1], b[0]);
    b[0] = (h >> 24) & 0xFF; b[1] = (h >> 16) & 0xFF;  b[2] = (h >> 8) & 0xFF; b[3] = h & 0xFF;
    fprintf(file, "%c%c%c%c", b[3], b[2], b[1], b[0]);
    for (size_t i = 0; i < cp->pixel_matrix->width * cp->pixel_matrix->height; i++) {
        if (cp->h_sectors[i]) {
            dc_pixel *pp = &(cp->h_sectors[i]->pix);
            fprintf(file, "%c%c%c%c", cp->h_sectors[i]->splits, pp->r, pp->g, pp->b);
        }
    }
    fclose(file);
}

void dc_compress(unsigned char *input, unsigned char *output, unsigned char thr, int verbose) {
    if (verbose) printf("[ %s | %s]\n", input, output);
    if (verbose) printf("COMPRESS (thr: %d)\n", thr);
    dc_compression *cp = _dc_compression_init(input, output, thr, verbose);
    if (verbose) printf("\tUNPACK... ");
    if (verbose) fflush(stdout);
    _dc_unpack(cp);
    if (verbose) printf("\tDONE (%dx%d)\n", cp->pixel_matrix->width, cp->pixel_matrix->height);

    if (verbose) printf("\tSECTORIZE... ");
    if (verbose) fflush(stdout);
    _dc_sectorize(cp, (dc_corners){0, 0, cp->pixel_matrix->width, cp->pixel_matrix->height}, 0);
    size_t count = 0;
    if (verbose) for (size_t i = 0; i < cp->pixel_matrix->width * cp->pixel_matrix->height; i++) count += cp->h_sectors[i] ? 1 : 0;
    if (verbose) printf("\tDONE (%ld sectors)\n", count);

    if (verbose) printf("\tSERIALIZE... ");
    if (verbose) fflush(stdout);
    _dc_serialize(cp);
    if (verbose) printf("\tDONE\n");

    _dc_compression_destroy(cp);
    if (verbose) printf("FINISHED\n");
}

dc_decompression *_dc_decompression_init(unsigned char *input, unsigned char *output, int verbose) {
    dc_decompression *dp = malloc(sizeof(dc_decompression));
    dp->verbose = verbose;
    dp->input = input;
    dp->output = output;
    dp->pixel_matrix = malloc(sizeof(dc_pixel_matrix));
    dp->pixel_matrix->width = 0;
    dp->pixel_matrix->height = 0;
    dp->l_sector = NULL;
    return dp;
}

void _dc_decompression_destroy(dc_decompression *dp) {
    dc_light_sector *lp = dp->l_sector;
    dc_light_sector *temp = NULL;
    while (lp) {
        temp = lp;
        lp = lp->next;
        free(temp);
    }
    free(dp->pixel_matrix->pixels);
    free(dp->pixel_matrix);
    free(dp);
}

void _dc_draw_n_occupy(dc_decompression *dp, dc_corners *corp, char *mat, dc_light_sector *lp) {
    size_t i;
    for (size_t y = corp->y0; y < corp->y1; y++) {
        for (size_t x = corp->x0; x < corp->x1; x++) {
            i = ix(x, y, dp->pixel_matrix->width);
            mat[i] = 1;
            dp->pixel_matrix->pixels[i].r = lp->pix.r;
            dp->pixel_matrix->pixels[i].g = lp->pix.g;
            dp->pixel_matrix->pixels[i].b = lp->pix.b;
        }
    }
}

void _dc_deserialize(dc_decompression *dp) {
    int fd = open(dp->input, O_RDWR, S_IRUSR);
    if (!fd) {
        fprintf(stderr, "Cannot open file\n");
        exit(1);
    }
    unsigned char version, thr, b[8], buffer[1024];
    int w, h, readable, count = 0;
    uint32_t _w, _h;
    size_t i;
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
        w = (int)_w;
        h = (int)_h;
    } else {
        fprintf(stderr, "Missing width and height\n");
        exit(1);
    }
    if (version != 1) {
        fprintf(stderr, "Version not supported\n");
        exit(1);
    }
    while (readable = read(fd, buffer, 1024)) {
        if (readable % 4 == 0) {
            count += readable;
            i = 0;
            while(i < readable) {
                dc_light_sector *lp = malloc(sizeof(dc_light_sector));
                lp->splits = buffer[i++];
                lp->pix.r = buffer[i++];
                lp->pix.g = buffer[i++];
                lp->pix.b = buffer[i++];
                if (dp->l_sector) {
                    lp->next = NULL;
                    lp->prev = dp->l_sector->prev;
                    dp->l_sector->prev->next = lp;
                    dp->l_sector->prev = lp;
                } else {
                    lp->next = NULL;
                    lp->prev = lp;
                    dp->l_sector = lp;
                }
            }
        } else {
            fprintf(stderr, "Corrupted sectors\n");
            exit(1);
        }
    }
    close(fd);
    dp->pixel_matrix->width = w;
    dp->pixel_matrix->height = h;
    dp->pixel_matrix->pixels = malloc(sizeof(dc_pixel) * w * h);
}

void _dc_render(dc_decompression *dp) {
    size_t w = dp->pixel_matrix->width;
    size_t h = dp->pixel_matrix->height;
    char *mat = malloc(sizeof(char) * w * h);
    dc_corners cor;
    int dx, dy;
    int mid = 0;
    for (size_t i = 0; i < w * h; i++) mat[i] = 0;
    dc_light_sector *lp, *temp;
    for (size_t y = 0; y < h; y++) {
        for (size_t x = 0; x < w; x++) {
            if (mat[ix(x, y, w)] == 0 && dp->l_sector) {
                lp = dp->l_sector;
                cor.x0 = 0; cor.y0 = 0;
                cor.x1 = w; cor.y1 = h;
                for (int i = 0; i < lp->splits; i++) {
                    dx = cor.x1 - cor.x0;
                    dy = cor.y1 - cor.y0;
                    if (dx > dy) {
                        mid = _dc_get_mid(cor.x0, cor.x1);
                        if (mid > x) cor.x1 = mid;
                        else cor.x0 = mid;
                    } else {
                        mid = _dc_get_mid(cor.y0, cor.y1);
                        if (mid > y) cor.y1 = mid;
                        else cor.y0 = mid;
                    }
                }
                _dc_draw_n_occupy(dp, &cor, mat, lp);
                if (lp->next) {
                    temp = lp;
                    lp->next->prev = lp->next;
                    dp->l_sector = lp->next;
                    free(temp);
                } else {
                    free(dp->l_sector);
                    dp->l_sector = NULL;
                }
            }
        }
    }
}

void _dc_pack(dc_decompression *dp) {
    size_t w = dp->pixel_matrix->width;
    size_t h = dp->pixel_matrix->height;
    unsigned char *data = malloc(sizeof(char) * w * h * 3);
    size_t i = 0;
    for (size_t y = 0; y < h; y++) {
        for (size_t x = 0; x < w; x++) {
            dc_pixel *pp = &dp->pixel_matrix->pixels[ix(x, y, w)];
            data[i++] = pp->r;
            data[i++] = pp->g;
            data[i++] = pp->b;
        }
    }
    stbi_write_bmp(dp->output, w, h, 3, data);
    stbi_image_free(data);
}

void dc_decompress(unsigned char *input, unsigned char *output, int verbose) {
    if (verbose) printf("[ %s | %s]\n", input, output);
    if (verbose) printf("DECOMPRESS\n");
    dc_decompression *dp = _dc_decompression_init(input, output, verbose);
    if (verbose) printf("\tDESERIALIZE... ");
    if (verbose) fflush(stdout);
    _dc_deserialize(dp);
    if (verbose) printf("\tDONE\n");

    if (verbose) printf("\tRENDER... ");
    if (verbose) fflush(stdout);
    _dc_render(dp);
    if (verbose) printf("\tDONE\n");

    if (verbose) printf("\tPACK... ");
    if (verbose) fflush(stdout);
    _dc_pack(dp);
    if (verbose) printf("\tDONE\n");
    
    _dc_decompression_destroy(dp);
    if (verbose) printf("FINISHED\n");

}