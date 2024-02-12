// Copyright ou © ou Copr. Antoine Contoux, Guillaume Renard, (2024)
//
// contoux1u@etu.univ-lorraine.fr
// renard124u@etu.univ-lorraine.fr
//
// Ce logiciel est un programme informatique servant à dessiner un
// bonhomme de neige.
//
// Ce logiciel est régi par la licence CeCILL-B soumise au droit français et
// respectant les principes de diffusion des logiciels libres. Vous pouvez
// utiliser, modifier et/ou redistribuer ce programme sous les conditions
// de la licence CeCILL-B telle que diffusée par le CEA, le CNRS et INRIA
// sur le site "http://cecill.info".
//
// En contrepartie de l'accessibilité au code source et des droits de copie,
// de modification et de redistribution accordés par cette licence, il n'est
// offert aux utilisateurs qu'une garantie limitée. Pour les mêmes raisons,
// seule une responsabilité restreinte pèse sur l'auteur du programme, le
// titulaire des droits patrimoniaux et les concédants successifs.
//
// À cet égard, l'attention de l'utilisateur est attirée sur les risques
// associés au chargement, à l'utilisation, à la modification et/ou au
// développement et à la reproduction du logiciel par l'utilisateur, étant
// donné sa spécificité de logiciel libre, qui peut le rendre complexe à
// manipuler et qui le réserve donc à des développeurs et des professionnels
// avertis possédant des connaissances informatiques approfondies. Les
// utilisateurs sont donc invités à charger et à tester l'adéquation du
// logiciel à leurs besoins dans des conditions permettant d'assurer la
// sécurité de leurs systèmes et/ou de leurs données et, plus généralement,
// à l'utiliser et à l'exploiter dans les mêmes conditions de sécurité.
//
// Le fait que vous puissiez accéder à cet en-tête signifie que vous avez
// pris connaissance de la licence CeCILL-B, et que vous en avez accepté les
// termes.

#define _USE_MATH_DEFINES
#include <cmath>
#include <algorithm>
#include <limits>
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include <vector>
#include "Object.h"

std::vector<Vec3f> envmap;
int envmap_width, envmap_height;

template <typename T> inline T lerp(const T &v0, const T &v1, float t) {
    return v0 + (v1-v0)*std::max(0.f, std::min(1.f, t));
}

float hash(const float n) {
    float x = std::sin(n)*43758.5453f;
    return x-std::floor(x);
}

float noise(const Vec3f &x) {
    Vec3f p(std::floor(x.x), std::floor(x.y), std::floor(x.z));
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
        // Parcours des autres objets
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
const Shape branchLeft = Shape({ 
    new Cylinder(Vec3f(-0.66, 0.1, 0.30), Vec3f(-1.2, 0.2, 0.30), 0.015),
    new Cylinder(Vec3f(-0.90, 0.18, 0.30), Vec3f(-1.09, 0.53, 0.28), 0.01),
    new Cylinder(Vec3f(-0.88, 0.10, 0.32), Vec3f(-1.22, -0.25, 0.36), 0.01)
}, Vec3f(0.554, 0.417, 0.35), 0.06, 0.03);
const Shape branchRight = Shape({ 
    new Cylinder(Vec3f(0.63, -0.05, 0.30), Vec3f(1.09, 0.15, 0.35), 0.025),
    new Cylinder(Vec3f(1.1, 0.17, 0.35), Vec3f(1.46, 0, 0.41), 0.01),
    new Cylinder(Vec3f(1.19, 0.20, 0.33), Vec3f(1.5, 0.41, 0.25), 0.022)
}, Vec3f(0.554, 0.417, 0.35), 0.06, 0.05);
const Shape rake = Shape({
    new Cylinder(Vec3f(1.5, -2.5, 0.35), Vec3f(1.25, 1, 0.35), 0.06), // tige du balai
    new Cylinder(Vec3f(0.80, 0.95, 0.35), Vec3f(1.70, 1.05, 0.35), 0.04),
    new Cylinder(Vec3f(0.80, 0.95, 0.35), Vec3f(0.75, 1.28, 0.37), 0.02),
    new Cylinder(Vec3f(0.87, 0.95, 0.35), Vec3f(0.82, 1.28, 0.37), 0.02),
    new Cylinder(Vec3f(0.95, 0.95, 0.35), Vec3f(0.90, 1.28, 0.37), 0.02),
    new Cylinder(Vec3f(1.02, 0.96, 0.35), Vec3f(0.97, 1.29, 0.37), 0.02),
    new Cylinder(Vec3f(1.1, 0.97, 0.35), Vec3f(1.05, 1.29, 0.37), 0.02),
    new Cylinder(Vec3f(1.17, 0.97, 0.35), Vec3f(1.12, 1.29, 0.37), 0.02),
    new Cylinder(Vec3f(1.25, 0.99, 0.35), Vec3f(1.20, 1.33, 0.37), 0.02),
    new Cylinder(Vec3f(1.32, 0.99, 0.35), Vec3f(1.27, 1.33, 0.37), 0.02),
    new Cylinder(Vec3f(1.40, 1.0, 0.35), Vec3f(1.35, 1.36, 0.37), 0.02),
    new Cylinder(Vec3f(1.47, 1.0, 0.35), Vec3f(1.42, 1.36, 0.37), 0.02),
    new Cylinder(Vec3f(1.55, 1.05, 0.35), Vec3f(1.50, 1.39, 0.37), 0.02),
    new Cylinder(Vec3f(1.62, 1.05, 0.35), Vec3f(1.57, 1.40, 0.37), 0.02),
    new Cylinder(Vec3f(1.70, 1.05, 0.35), Vec3f(1.65, 1.41, 0.37), 0.02)
}, Vec3f(0.554, 0.5, 0.35));

const std::vector<Shape> shapes = {body, carrot, eye, stone, scarf_part1, scarf_part2, hat, branchLeft, branchRight, rake};

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

bool loadEnvmap() {
    int n = -1;
    unsigned char *envmap_pixmap = stbi_load("../envmap.jpg", &envmap_width, &envmap_height, &n, 0);
    if (!envmap_pixmap || 3!=n) {
        std::cerr << "Error: can not load the environment map" << std::endl;
        return false;
    }

    envmap = std::vector<Vec3f>(envmap_width*envmap_height);
    for (int j = envmap_height - 1; j >= 0; j--) {
        for (int i = 0; i < envmap_width; i++) {
            envmap[i + j * envmap_width] = Vec3f(
                    envmap_pixmap[(i + j * envmap_width) * 3 + 0],
                    envmap_pixmap[(i + j * envmap_width) * 3 + 1],
                    envmap_pixmap[(i + j * envmap_width) * 3 + 2]
            ) * (1 / 255.);
        }
    }
    stbi_image_free(envmap_pixmap);
    return true;
}

int main(int argc, char **argv) {
    bool showProgress = false;
    float factor = 1;
    if (argc > 3) {
        std::cerr << "Usage: " << argv[0] << " [factor] [-v]" << std::endl;
        return 1;
    } else if (argc == 2) {
        if (std::string(argv[1]) == "-v") {
            showProgress = true;
        } else {
            std::string arg = argv[1];
            factor = std::stof(arg);
        }
    } else if (argc == 3) {
        std::string arg1 = argv[1];
        factor = std::stof(arg1);
        std::string arg2 = argv[2];
        if (arg2 == "-v") {
            showProgress = true;
        } else {
            std::cerr << "Usage: " << argv[0] << " [factor] [-v]" << std::endl;
            return 1;
        }
    }

    bool envmapLoaded = loadEnvmap();

    const int   width    = 640*factor; // image width
    const int   height   = 480*factor; // image height
    const float fov      = M_PI/3.; // field of view angle
    std::vector<Vec3f> framebuffer(width*height);

    int nbComputedPixels = 0;
#pragma omp parallel for
    for (int j = 0; j<height; j++) { // actual rendering loop
        for (int i = 0; i<width; i++) {
            float dir_x =  (i + 0.5) -  width/2.;
            float dir_y = -(j + 0.5) + height/2.;    // this flips the image at the same time
            float dir_z = -height/(2.*std::tan(fov/2.));
            Vec3f dir = Vec3f(dir_x, dir_y, dir_z).normalize();
            Vec3f hit, color;
            if (sphere_trace(Vec3f(0, 0, 5), dir, hit, color)) { // the camera is placed to (0,0,5) and it looks along the -z axis
                Vec3f light_dir = (Vec3f(10, 10, 10) - hit).normalize();                     // one light is placed to (10,10,10)
                float light_intensity  = std::max(0.4f, light_dir*distance_field_normal(hit));
                framebuffer[i+j*width] = color * light_intensity;
            } else {
                if (envmapLoaded) {
                    float u = (std::atan2(dir.z, dir.x) + M_PI) / (2*M_PI);
                    float v = std::acos(dir.y) / M_PI;
                    int x = std::min(envmap_width  - 1, int(u * envmap_width));
                    int y = std::min(envmap_height - 1, int(v * envmap_height));
                    framebuffer[i+j*width] = envmap[x + y * envmap_width];
                } else {
                    framebuffer[i+j*width] = Vec3f(0.2, 0.7, 0.8); // background color
                }
            }
            if (showProgress) {
                // certainement pas precis, mais ça donne une petite une idée de l'avancement
                nbComputedPixels++;
                if (nbComputedPixels % 10000 == 0) {
                    std::cout << "\033[H\033[2JRendering: " << (100. * nbComputedPixels / (width * height)) << "%" << std::endl;
                }
            }
        }
    }

    // save the framebuffer to file
    std::vector<unsigned char> pixmap(width*height*3);
    for (int i = 0; i < height*width; ++i) {
        Vec3f &c = framebuffer[i];
        float max = std::max(c[0], std::max(c[1], c[2]));
        if (max>1) c = c*(1./max);
        for (int j = 0; j<3; j++) {
            pixmap[i*3+j] = (unsigned char)(255 * std::max(0.f, std::min(1.f, framebuffer[i][j])));
        }
    }
    stbi_write_jpg("out.jpg", width, height, 3, pixmap.data(), 100);

    return 0;
}

