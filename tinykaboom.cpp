#define _USE_MATH_DEFINES
#include <cmath>
#include <algorithm>
#include <limits>
#include <iostream>
#include <fstream>
#include <vector>
#include "Object.h"

template <typename T> inline T lerp(const T &v0, const T &v1, float t) {
    return v0 + (v1-v0)*std::max(0.f, std::min(1.f, t));
}

float hash(const float n) {
    float x = sin(n)*43758.5453f;
    return x-floor(x);
}

float noise(const Vec3f &x) {
    Vec3f p(floor(x.x), floor(x.y), floor(x.z));
    Vec3f f(x.x-p.x, x.y-p.y, x.z-p.z);
    f = f*(f*(Vec3f(3.f, 3.f, 3.f)-f*2.f));
    float n = p*Vec3f(1.f, 57.f, 113.f);
    return lerp(lerp(
                     lerp(hash(n +  0.f), hash(n +  1.f), f.x),
                     lerp(hash(n + 57.f), hash(n + 58.f), f.x), f.y),
                lerp(
                    lerp(hash(n + 113.f), hash(n + 114.f), f.x),
                    lerp(hash(n + 170.f), hash(n + 171.f), f.x), f.y), f.z);
}

Vec3f rotate(const Vec3f &v) {
    return Vec3f(Vec3f(0.00,  0.80,  0.60)*v, Vec3f(-0.80,  0.36, -0.48)*v, Vec3f(-0.60, -0.48,  0.64)*v);
}

float fractal_brownian_motion(const Vec3f &x) { // this is a bad noise function with lots of artifacts. TODO: find a better one
    Vec3f p = rotate(x);
    float f = 0;
    f += 0.5000f*noise(p); p = p*2.32;
    f += 0.2500f*noise(p); p = p*3.03;
    f += 0.1250f*noise(p); p = p*2.61;
    f += 0.0625f*noise(p);
    return f/0.9375;
}

struct Shape {
    const std::vector<Object*> objects;
    const Vec3f color;
    const float noise_amplitude; // amount of noise applied to the sphere (towards the center)
    const float rFusionLisse;

    explicit Shape(const std::vector<Object*> &objects, const Vec3f &color = Vec3f(1, 1, 1),
                   const float &noise_amplitude = 0., const float &rFusionLisse = 0.)
            : objects(objects), color(color), noise_amplitude(noise_amplitude), rFusionLisse(rFusionLisse) {}

    [[nodiscard]] float getDistance(const Vec3f &p) const {
        float displacement = -fractal_brownian_motion(p*3.4)*noise_amplitude;

        // ----------------- Fusion lisse -----------------
        // à partir du code du dernier TP d'Infographie de l'année dernière
        // Distance initiale
        float res = !objects.empty() ? objects[0]->getDistance(p) : std::numeric_limits<float>::max();
        // Parcours des autres sphères
        for (size_t i = 1; i < objects.size(); ++i) {
            float dst = objects[i]->getDistance(p);
            res = smin(res, dst, rFusionLisse);
        }
        // -------------------------------------------------

        return res + displacement;
    }

private :
    // à partir du code du dernier TP d'Infographie de l'année dernière
    static float smin(float dA, float dB, float r) {
        float c = std::clamp(0.5f * (1.0f + (dB - dA) / r), 0.0f, 1.0f);
        return lerp(dB, dA, c) - r * c * (1.0f - c);
    }
};

const Shape body = Shape({
    new Sphere(Vec3f(0, -1.5, 0), 1.),
    new Sphere(Vec3f(0, -0.1, 0), 0.66),
    new Sphere(Vec3f(0, 0.85, 0), 0.35)
}, Vec3f(1, 1, 1), 0.15);
const Shape carrot = Shape({
    new Sphere(Vec3f(0, 0.80, 0.35), 0.08),
    new Sphere(Vec3f(0, 0.83, 0.55), 0.06),
    new Sphere(Vec3f(0, 0.86, 0.69), 0.03),
    new Sphere(Vec3f(0, 0.89, 0.83), 0.01),
}, Vec3f(0.89, 0.38, 0.12), 0.04, 0.17);
const Shape eye = Shape({
    new Sphere(Vec3f(0.19, 0.90, 0.32), 0.005),
    new Sphere(Vec3f(0.19, 0.99, 0.32), 0.005),
    new Sphere(Vec3f(-0.19, 0.90, 0.32), 0.005),
    new Sphere(Vec3f(-0.19, 0.99, 0.32), 0.005),
}, Vec3f(0.228, 0.278, 0.278), 0.09, 0.12);
const Shape stone = Shape({
    new Sphere(Vec3f(0, -1.5, 0.98), 0.045),
    new Sphere(Vec3f(0, -0.85, 0.75), 0.045),
    new Sphere(Vec3f(0, -0.1, 0.64), 0.045),
}, Vec3f(0.328, 0.328, 0.328), 0.15);
const Shape scarf_part1 = Shape({
    new Sphere(Vec3f(0, 0.52, 0), 0.44)
}, Vec3f(0.67, 0.11, 0.12), 0.05, 0);
const Shape scarf_part2 = Shape({
    new Sphere(Vec3f(0.18, 0.61, 0.28), 0.15),
    new Sphere(Vec3f(0.32, 0.24, 0.5), 0.09),
    new Sphere(Vec3f(0.2, -0.1, 0.64), 0.09),
}, Vec3f(0.67, 0.11, 0.12), 0.05, 0.5);
const Shape hat = Shape({
    new Cylinder(Vec3f(0, 1.2, 0), Vec3f(0, 1.7, 0), 0.3),
    new Cylinder(Vec3f(0, 1.15, 0), Vec3f(0, 1.2, 0), 0.5),
}, Vec3f(0.2, 0.2, 0.2));

const std::vector<Shape> shapes = {body, carrot, eye, stone, scarf_part1, scarf_part2, hat};

float signed_distance(const Vec3f &p, Vec3f *color = nullptr) { // this function defines the implicit surface we render
    float d = std::numeric_limits<float>::max();
    for (const Shape &shape : shapes) {
        float distance = shape.getDistance(p);
        if (distance < d) {
            d = distance;
            if (color) *color = shape.color;
        }
    }
    return d;
}

bool sphere_trace(const Vec3f &orig, const Vec3f &dir, Vec3f &pos, Vec3f &color) {
    pos = orig;
    for (size_t i=0; i<128; i++) {
        float d = signed_distance(pos, &color);
        if (d < 0) return true;
        pos = pos + dir*std::max(d*0.1f, .01f); // note that the step depends on the current distance, if we are far from the surface, we can do big steps
    }
    return false;
}

Vec3f distance_field_normal(const Vec3f &pos) { // simple finite differences, very sensitive to the choice of the eps constant
    const float eps = 0.1;
    float d = signed_distance(pos);
    float nx = signed_distance(pos + Vec3f(eps, 0, 0)) - d;
    float ny = signed_distance(pos + Vec3f(0, eps, 0)) - d;
    float nz = signed_distance(pos + Vec3f(0, 0, eps)) - d;
    return Vec3f(nx, ny, nz).normalize();
}

int main(int argc, char **argv) {
    float factor = 1;
    if (argc > 2) {
        std::cerr << "Usage: " << argv[0] << " [factor]" << std::endl;
        return 1;
    } else if (argc == 2) {
        std::string arg = argv[1];
        factor = std::stof(arg);
    }

    const int   width    = 640*factor; // image width
    const int   height   = 480*factor; // image height
    const float fov      = M_PI/3.; // field of view angle
    std::vector<Vec3f> framebuffer(width*height);

#pragma omp parallel for
    for (size_t j = 0; j<height; j++) { // actual rendering loop
        for (size_t i = 0; i<width; i++) {
            float dir_x =  (i + 0.5) -  width/2.;
            float dir_y = -(j + 0.5) + height/2.;    // this flips the image at the same time
            float dir_z = -height/(2.*tan(fov/2.));
            Vec3f hit, color;
            if (sphere_trace(Vec3f(0, 0, 5), Vec3f(dir_x, dir_y, dir_z).normalize(), hit, color)) { // the camera is placed to (0,0,3) and it looks along the -z axis
                Vec3f light_dir = (Vec3f(10, 10, 10) - hit).normalize();                     // one light is placed to (10,10,10)
                float light_intensity  = std::max(0.4f, light_dir*distance_field_normal(hit));
                framebuffer[i+j*width] = color * light_intensity;
            } else {
                framebuffer[i+j*width] = Vec3f(0.2, 0.7, 0.8); // background color
            }
        }
    }

    std::ofstream ofs("./out.ppm", std::ios::binary); // save the framebuffer to file
    ofs << "P6\n" << width << " " << height << "\n255\n";
    for (size_t i = 0; i < height*width; ++i) {
        for (size_t j = 0; j<3; j++) {
            ofs << (char)(std::max(0, std::min(255, static_cast<int>(255*framebuffer[i][j]))));
        }
    }
    ofs.close();

    return 0;
}

