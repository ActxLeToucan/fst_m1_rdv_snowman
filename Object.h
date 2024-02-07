#ifndef TINYKABOOM_OBJECT_H
#define TINYKABOOM_OBJECT_H


#include "geometry.h"

class Object {
public:
    [[nodiscard]] virtual float getDistance(const Vec3f &p) const = 0;
};

class Sphere : public Object {
public:
    const Vec3f orig;
    const float radius;

    Sphere(const Vec3f &o, const float &r) : orig(o), radius(r) {}

    [[nodiscard]] float getDistance(const Vec3f &p) const override {
        return (p - orig).norm() - radius;
    }
};

class Cylinder : public Object {
public:
    const Vec3f orig, dest;
    const float radius;

    Cylinder(const Vec3f &o, const Vec3f &d, const float &r) : orig(o), dest(d), radius(r) {}

    // A partir de 'Arbitrary Capped Cylinder' par Inigo Quilez :
    // https://iquilezles.org/articles/distfunctions/ ou https://www.shadertoy.com/view/wdXGDr
    [[nodiscard]] float getDistance(const Vec3f &p) const override {
        Vec3f ba = dest - orig;
        Vec3f pa = p - orig;
        float baba = dot(ba, ba);
        float paba = dot(pa, ba);
        float x = length(pa * baba - ba * paba) - radius * baba;
        float y = abs(paba - baba * 0.5) - baba * 0.5;
        float x2 = x * x;
        float y2 = y * y * baba;
        float d = (std::max(x, y) < 0.0) ? -std::min(x2, y2) : (((x > 0.0) ? x2 : 0.0) + ((y > 0.0) ? y2 : 0.0));
        return sign(d) * sqrt(abs(d)) / baba;
    }
};


#endif //TINYKABOOM_OBJECT_H
