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

#ifndef FST_M1_RDV_SNOWMAN_OBJECT_H
#define FST_M1_RDV_SNOWMAN_OBJECT_H


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
        float y = std::abs(paba - baba * 0.5) - baba * 0.5;
        float x2 = x * x;
        float y2 = y * y * baba;
        float d = (std::max(x, y) < 0.0) ? -std::min(x2, y2) : (((x > 0.0) ? x2 : 0.0) + ((y > 0.0) ? y2 : 0.0));
        return sign(d) * std::sqrt(std::abs(d)) / baba;
    }
};


#endif //FST_M1_RDV_SNOWMAN_OBJECT_H
