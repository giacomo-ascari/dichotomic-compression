#include "dc-utils.h"

dc_pixel::dc_pixel() {
    r = 0;
    g = 0;
    b = 0;
}

dc_pixel::dc_pixel(unsigned char _r, unsigned char _g, unsigned char _b) {
    set_channels(_r, _g, _b);
}

dc_pixel::~dc_pixel() { }

void dc_pixel::increase_channels(unsigned char _r, unsigned char _g, unsigned char _b) {
    r += _r;
    g += _g;
    b += _b;
}

void dc_pixel::set_channels(unsigned char _r, unsigned char _g, unsigned char _b) {
    r = _r;
    g = _g;
    b = _b;
}

void dc_pixel::increase_channel(unsigned char value, dc_channels channel) {
    switch (channel) {
        case RED: r += value; break;
        case GREEN: g += value; break;
        case BLUE: b += value; break;
        default: break;
    }
}

void dc_pixel::set_channel(unsigned char value, dc_channels channel) {
    switch (channel) {
        case RED: r = value; break;
        case GREEN: g = value; break;
        case BLUE: b = value; break;
        default: break;
    }
}

unsigned char dc_pixel::get_channel(dc_channels channel) {
    switch (channel) {
        case RED: return r;
        case GREEN: return g;
        case BLUE: return b;
        default: return 0;
    }
}