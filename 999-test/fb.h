/* Copyright (C) 2015 Goswin von Brederlow <goswin-v-b@web.de>

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

/* Framebuffer
 */

#ifndef KERNEL_FB_H
#define KERNEL_FB_H

#include <cstdint>
#include <cassert>

class FB {
public:
    struct Color {
        uint8_t r;
        uint8_t g;
        uint8_t b;
        uint8_t a;
    };

    static constexpr Color BLACK   = {0x00, 0x00, 0x00, 0xFF};
    static constexpr Color GREY    = {0x80, 0x80, 0x80, 0xFF};
    static constexpr Color WHITE   = {0xFF, 0xFF, 0xFF, 0xFF};
    static constexpr Color RED     = {0xFF, 0x00, 0x00, 0xFF};
    static constexpr Color GREEN   = {0x00, 0xFF, 0x00, 0xFF};
    static constexpr Color BLUE    = {0x00, 0x00, 0xFF, 0xFF};
    static constexpr Color MAGENTA = {0xFF, 0x00, 0xFF, 0xFF};
    
    class Column {
    public:
        Color & operator[](ssize_t y) {
            return fb_.at(x_, y);
        }
    private:
        friend class FB;
        Column(FB &fb, ssize_t x) : fb_(fb), x_(x) {}
        FB &fb_;
        const ssize_t x_;
    };
    
    class ConstColumn {
    public:
        const Color & operator[](ssize_t y) const {
            return fb_.at(x_, y);
        }
    private:
        friend class FB;
        ConstColumn(const FB &fb, ssize_t x) : fb_(fb), x_(x) {}
        const FB &fb_;
        const ssize_t x_;
    };
    
    FB(size_t width, size_t height, size_t stride, Color *pixels);

    Column operator[](ssize_t x) {
        return Column(*this, x);
    }

    const ConstColumn operator[](ssize_t x) const {
        return ConstColumn(*this, x);
    }

    size_t width() const { return width_; }
    size_t height() const { return height_; }

    const Color & at(ssize_t x, ssize_t y) const {
        assert((x >= 0) && (size_t(x) < width_));
        assert((y >= 0) && (size_t(y) < height_));
        return pixel_[x + y * stride_];
    }

    Color & at(ssize_t x, ssize_t y) {
        assert((x >= 0) && (size_t(x) < width_));
        assert((y >= 0) && (size_t(y) < height_));
        return pixel_[x + y * stride_];
    }

    void clear(const Color &c = BLACK);
    void save(const char *filename, const char *tmpname);
    void draw_line(ssize_t x1, ssize_t y1, ssize_t x2, ssize_t y2, const Color &c);
    class Point {
    public:
        ssize_t x, y;
    };

    void fill(const Color &c, size_t length, const Point *points);
private:
    const size_t width_;
    const size_t height_;
    const size_t stride_;
    Color *pixel_;
};

#endif // #ifndef KERNEL_FRAMEBUFFER_H
