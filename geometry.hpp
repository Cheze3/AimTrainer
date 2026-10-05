#pragma once
#include <cmath>
#include "vec3.hpp"

// On a miss, tHit is not modified. On a hit, tHit is set to the distance along the ray to the first intersection point.
inline bool rayHitsSphere(const Vec3& origin, const Vec3& dir, const Vec3& center, double radius, double& tHit) {
    Vec3 oc = sub(origin, center);
    double a = dot(dir, dir);
    if (a < 1e-12) return false; 
    double b = 2.0 * dot(oc, dir);
    double c = dot(oc, oc) - radius * radius;
    double disc = b * b - 4 * a * c;
    if (disc < 0) return false;

    double s = std::sqrt(disc);
    double t0 = (-b - s) / (2.0 * a);
    double t1 = (-b + s) / (2.0 * a);
    if (t0 >= 0) {
        tHit = t0;
        return true;
    }
    if (t1 >= 0) {
        tHit = t1;
        return true;
    }
    return false;
}
