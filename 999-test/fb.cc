#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "fb.h"

constexpr FB::Color FB::BLACK;
constexpr FB::Color FB::GREY;
constexpr FB::Color FB::WHITE;
constexpr FB::Color FB::RED;
constexpr FB::Color FB::GREEN;
constexpr FB::Color FB::BLUE;
constexpr FB::Color FB::MAGENTA;

void FB::save(const char *filename) {
    FILE *file = fopen(filename, "w");
    assert(file);
    fprintf(file, "P6\n");
    fprintf(file, "# Copyright 2015 Goswin von Brederlow <goswin-v-b@web.de>\n");
    fprintf(file, "%zu %zu\n", width_, height_);
    fprintf(file, "255\n");
    for (size_t y = 0; y < height_; ++y) {
        for (size_t x = 0; x < width_; ++x) {
            size_t len = fwrite((const void *)&at(x, y), 1, 3, file);
            assert(len == 3);
        }
    }
    fflush(file);
    
    int res = fclose(file);
    assert(res == 0);    
}

FB::FB(size_t width, size_t height, size_t stride, Color *pixels)
    : width_(width), height_(height), stride_(stride), pixel_(pixels) {
    clear();
}

void FB::clear(const Color &c) {
    for (size_t y = 0; y < height_; ++y) {
        for (size_t x = 0; x < width_; ++x) {
            at(x, y) = c;
        }
    }
}

void FB::line(ssize_t x1, ssize_t y1, ssize_t x2, ssize_t y2, const Color &c) {
    ssize_t sx = (x1 <= x2) ? 1 : -1;
    ssize_t sy = (y1 <= y2) ? 1 : -1;
    ssize_t dx = 2 * sx * (x2 - x1);
    ssize_t dy = 2 * sy * (y2 - y1);
    ssize_t x = x1;
    ssize_t y = y1;
    ssize_t d = 0;
    if (dx <= dy) {
        d = dy / 2 - dx;
    } else {
        d = dy - dx / 2;
    }

    // draw at least the starting point
    at(x1, y1) = c;

    // printf("### (%zd, %zd) - (%zd, %zd)\n", x1, y1, x2, y2);
    while ((x != x2) || (y != y2)) {
        // printf("(%zd, %zd) %zd\n", x, y, d);
        if ((d < 0) || ((sx > 0) && (d == 0))) {
            x += sx;
            d += dy;
            if (dx > dy) at(x, y) = c;
        } else {
	    y += sy;
	    d -= dx;
            if (dx <= dy) at(x, y) = c;
        }
    }
    // printf("(%zd, %zd) %zd\n", x, y, d);
}

/*
void fb_line(int x1, int y1, int x2, int y2, Color c) {
    int sx = (x1 <= x2) ? 1 : -1;
    int sy = (y1 <= y2) ? 1 : -1;
    int dx = 2 * sx * (x2 - x1);
    int dy = 2 * sy * (y2 - y1);
    int x = x1;
    int y = y1;

    // up/down, one point per screen line
    if (dy > 0) {
	int d = dy / 2 - dx;
	fb_set(x, y, c);
	while (y != y2) {
	    while ((d < 0) || ((sx > 0) && (d == 0))) {
		x += sx;
		d += dy;
	    }
	    y += sy;
	    d -= dx;
	    fb_set(x, y, c);
	}
    }
}
*/
