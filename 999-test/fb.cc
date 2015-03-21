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

void FB::save(const char *filename, const char *tmpname) {
    FILE *file = fopen(tmpname, "w");
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

    res = rename(tmpname, filename);
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

void FB::draw_line(ssize_t x1, ssize_t y1, ssize_t x2, ssize_t y2,
                   const Color &c) {
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

class LineIterator {
public:
    LineIterator(ssize_t x1, ssize_t y1, ssize_t x2, ssize_t y2)
        : x2_(x2), y2_(y2),
          sx_((x1 <= x2) ? 1 : -1),
          dx_(2 * sx_ * (x2 - x1)), dy_(2 * (y2 - y1)),
          x_(x1), y_(y1),
          d_(dy_ / 2 - dx_) /* set d to get the midpoint for each line */ {
        assert(y1 <= y2);
    }
    operator bool() const {
        return y_ != y2_;
    }
    FB::Point next() {
        FB::Point p({x_, y_});
        // go to the midpoint of the next line
        while ((d_ < 0) || ((sx_ > 0) && (d_ == 0))) {
            x_ += sx_;
            d_ += dy_;
        }
        y_ += 1;
        d_ -= dx_;
        return p;
    }
private:
    ssize_t x2_, y2_, sx_, dx_, dy_, x_, y_, d_;
};

class PolyIterator {
public:
    PolyIterator(size_t first, size_t last, size_t length, size_t direction,
                 const FB::Point *points)
        : next_(first), last_(last), length_(length), direction_(direction),
          points_(points), it_(0, 0, 0, 0) {
        advance();
    }
    operator bool() const {
        return (next_ != last_) || it_;
    }
    void advance() {
        while (!it_ && (next_ != last_)) {
            size_t t = (next_ + direction_ + length_) % length_;
            it_ = LineIterator(points_[next_].x, points_[next_].y,
                               points_[t].x, points_[t].y);
            next_ = t;
        }
    }
    FB::Point next() {
        FB::Point p = it_.next();
        advance();
        return p;
    }
private:
    size_t next_, last_, length_, direction_;
    const FB::Point *points_;
    LineIterator it_;
};

void FB::fill(const Color &c, size_t length, const Point *points) {
    assert(length > 1);
    // find top and bottom of polygon
    ssize_t top = points[0].y;
    ssize_t bottom = points[0].y;
    size_t first = 0, last = 0;
    for (size_t i = 1; i < length; ++i) {
        if (top > points[i].y) {
            top = points[i].y;
            first = i;
        }
        if (bottom < points[i].y) {
            bottom = points[i].y;
            last = i;
        }
    }
    // points are counter clockwise
    PolyIterator left(first, last, length, 1, points);
    PolyIterator right(first, last, length, -1, points);
    while (left) {
        const Point p1 = left.next();
        const Point p2 = right.next();
        assert(p1.y == p2.y);
        for (ssize_t x = p1.x; x < p2.x; ++x) {
            at(x, p1.y) = c;
        }
    }
}

