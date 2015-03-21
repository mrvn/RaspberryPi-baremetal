// points in 6.10 fixed point
// sin/cos in 2.14 fixed point
// rotated points in 8.24 fixed point
// cropping to 2.14 fixed point
// scaling to 16.16 fixed point

#include <unistd.h>
#include <iostream>
#include "fixed.h"
#include "trig.h"
#include "vector.h"
#include "matrix.h"
#include "cube.h"
#include "fb.h"

using M = Matrix<Rot>;			// rotational matrix
using RFP = FixedPoint<8, 24>;		// rotated FixedPoint
using R = Vector<RFP>;			// rotated points
using PFP = FixedPoint<16, 16>;		// screen FixedPoint
using P = Vector<PFP>;			// screen points

void init_normals(Shape &shape) {
    for (int i = 0; i < shape.num_poly; ++i) {
        std::cerr << "Shape [" << i << "]:";
        for (int j = 0; j < shape.poly[i].num_points; ++j) {
            std::cerr << " " << shape.point[shape.poly[i].point[j]];
        }
        std::cerr << std::endl;
        DataPoint p0 = shape.point[shape.poly[i].point[0]];
        DataPoint p1 = shape.point[shape.poly[i].point[1]];
        DataPoint p2 = shape.point[shape.poly[i].point[2]];
        DataPoint v0 = p0 - p1;
        DataPoint v1 = p2 - p1;
        Vector<FixedPoint<12, 20> > n1 = v0 ^ v1;
        Vector<FixedPoint<12, 20> > n2 = n1.normalized();
        shape.poly[i].normal = DataPoint(n2);
        std::cerr << "  normal: " << v0 << " ^ " << v1 << " = " << shape.poly[i].normal << std::endl;
    }
};

constexpr size_t WIDTH = 1600;
constexpr size_t HEIGHT = 1024;

PFP X0(int(WIDTH / 2));
PFP Y0(int(HEIGHT / 2));
constexpr PFP::T SCALE = HEIGHT / 2;

FB::Color pixels[WIDTH * HEIGHT];
FB fb(WIDTH, HEIGHT, WIDTH, pixels);

int main() {
    /**/
    init_normals(cube);
    for (int deg = -72; deg < 72; ++deg) {
    Rad rx = Rad(M_PI / 180 * 1 * deg);
    Rad ry = Rad(M_PI / 180 * 3 * deg);
    Rad rz = Rad(M_PI / 180 * 5 * deg);
    M rotation = M::rotational(rx, ry, rz);
    R rotated[cube.num_points];
    P point[cube.num_points];
    R normal[cube.num_poly];
    bool visible[cube.num_poly];
    R camera(RFP(0.0), RFP(0.0), RFP(-2.0));
    // rotate and translate points, perspective tranform
    for (int i = 0; i < cube.num_points; ++i) {
        rotated[i] = rotation * cube.point[i] - camera;
        std::cerr << "Point [" << i << "]: " << rotated[i] << std::endl;
        if (rotated[i].z() <= 0) {
            // FIXME: must do z clipping
            assert(false);
        } else {
            FixedPoint<24, 8> d = FixedPoint<24, 8>(rotated[i].z());
            PFP x = rotated[i].x() / d * SCALE + X0;
            PFP y = rotated[i].y() / d * SCALE + Y0;
            point[i] = P(x, y, PFP(1.0));
            std::cerr << "               " << point[i] << std::endl;
        }
    }
    // compute visibility
    for (int i = 0; i < cube.num_poly; ++i) {
        normal[i] = rotation * cube.poly[i].normal;
        auto t = normal[i] * rotated[cube.poly[i].point[0]];
        visible[i] = (normal[i] * rotated[cube.poly[i].point[0]]) > 0;
        std::cerr << "Shape [" << i << "]: " << (visible[i] ? "VISIBLE" : "       ") << " normal = " << normal[i] << ", t = " << t << std::endl;
        std::cerr << " ";
        int num = cube.poly[i].num_points;
        for (int j = 0; j < num; ++j) {
            int k = cube.poly[i].point[j];
            std::cerr << " " << point[k];
        }
        std::cerr << std::endl;
    }
    // fill visible faces
    for (int i = 0; i < cube.num_poly; ++i) {
        if (!visible[i]) continue;
        int num = cube.poly[i].num_points;
        FB::Point poly_point[num];
        for (int j = 0; j < num; ++j) {
            int k = cube.poly[i].point[j];
            poly_point[j].x = PFP::T(point[k].x());
            poly_point[j].y = PFP::T(point[k].y());
        }
        uint8_t z = 0x20;
        if (normal[i].z() > 0) {
            z += RFP::T(normal[i].z() * 0xDF);
        }
        FB::Color c({0x00, z, 0x00, 0x00});
        fb.fill(c, num, poly_point);
    }
    // draw hidden faces
    for (int i = 0; i < cube.num_poly; ++i) {
        if (visible[i]) continue;
        int num = cube.poly[i].num_points;
        for (int j = 0; j < num; ++j) {
            int k = cube.poly[i].point[j];
            int n = cube.poly[i].point[(j + 1) % num];
            fb.draw_line(PFP::T(point[k].x()),
                         PFP::T(point[k].y()),
                         PFP::T(point[n].x()),
                         PFP::T(point[n].y()),
                         FB::GREY);
        }
    }
    // draw visible faces
    for (int i = 0; i < cube.num_poly; ++i) {
        if (!visible[i]) continue;
        int num = cube.poly[i].num_points;
        for (int j = 0; j < num; ++j) {
            int k = cube.poly[i].point[j];
            int n = cube.poly[i].point[(j + 1) % num];
            fb.draw_line(PFP::T(point[k].x()),
                         PFP::T(point[k].y()),
                         PFP::T(point[n].x()),
                         PFP::T(point[n].y()),
                         FB::WHITE);
        }
    }
    // draw points
    for (int i = 0; i < cube.num_points; ++i) {
        fb.at(PFP::T(point[i].x()), PFP::T(point[i].y())) = FB::RED;
    }
    /**/
/*
    for (size_t x = 0; x < fb.width(); x += 10) {
        fb.line(x, 0, fb.width() - 1 - x, fb.height() - 1, FB::WHITE);
    }
    for (size_t y = 0; y < fb.height(); y += 10) {
        fb.line(0, y, fb.width() - 1, fb.height() - 1 - y, FB::WHITE);
    }
*/
/*
    constexpr int x1 = 10;
    constexpr int y1 = 10;
    constexpr int x2 = 20;
    constexpr int y2 = 19;
    fb.line(x1, y1, x2, y2, FB::RED);
    fb.line(x2, y2, x1, y1, FB::GREEN);
    fb.at(x1, y1) = FB::WHITE;
    fb.at(x2, y2) = FB::WHITE;
*/
    fb.save("screenshot.pnm", "screenshot.pnm.tmp");
    fb.clear();
    sleep(1);
    }
}
