#include <string>
#include <iostream>

#include "dc-utils.h"

using namespace std;

int ix(int x, int y, int width) {
    return x + (y * width);
}

int middle(int a, int b) {
    return a + (b - a) / 2;
}

/*void _dc_print_corners(dc_corners *corp) {
    printf("(%d, %d), (%d, %d)\n", corp->x0, corp->y0, corp->x1, corp->y1);
}

void _dc_print_light_sector(dc_light_sector *lp) {
    printf("[%p] %d (%d)(%02x%02x%02x) [%p] [%p] \n", lp, lp->splits, lp->value, lp->pix.r, lp->pix.g, lp->pix.b, lp->prev, lp->next);
}

void _dc_print_heavy_sector(dc_heavy_sector *hp) {
    printf("%d (%02x%02x%02x) - ", hp->splits, hp->pix.get_r(), hp->pix.get_g(), hp->pix.get_b());
    _dc_print_corners(&(hp->corners));
}

void _dc_print_pixel_matrix(dc_pixel_matrix *pmp, dc_corners *corp) {
    dc_pixel *pix = pmp->get_pixels();
    int w = pmp->get_width();
    for (size_t y = corp->y0; y < corp->y1; y++) {
        for (size_t x = corp->x0; x < corp->x1; x++) {
            printf("%02x%02x%02x ", pix[ix(x, y, w)].get_r(), pix[ix(x, y, w)].get_g(), pix[ix(x, y, w)].get_b());
        }
        printf("\n");
    }
}*/