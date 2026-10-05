#include "vec3.hpp"
#include <cmath>
#include <iostream>

bool rayHitsSphere(const Vec3& origin, const Vec3& dir, const Vec3& center, double radius, double& tHit) {
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

void check(bool ok, const char* name) {
    std::cout << (ok ? "PASS  " : "FAIL  ") << name << "\n";
}

bool approxEqual(double a, double b, double tol = 1e-9) { return std::fabs(a - b) < tol; }


int main() {
    Vec3 origin = {0, 0, 0};
    Vec3 dir = {0, 0, -1};
    double tHit;

    check(rayHitsSphere(origin, dir, {0, 0, -10}, 0.3, tHit) && approxEqual(tHit, 9.7), "direct hit");
    check(!rayHitsSphere(origin, dir, {0, 5, -10}, 0.3, tHit), "clean miss");
    check(rayHitsSphere(origin, dir, {1, 0, -10}, 1.0, tHit) && approxEqual(tHit, 10.0), "tangent");
    check(!rayHitsSphere(origin, dir, {0, 0, 10}, 1.0, tHit), "sphere behind");
    check(rayHitsSphere(origin, dir, {0, 0, 0}, 1.0, tHit) && approxEqual(tHit, 1.0), "origin inside");
    check(rayHitsSphere(origin, {0, 0, -2}, {0, 0, -10}, 0.3, tHit) && approxEqual(tHit, 4.85), "unnormalized dir");
    check(rayHitsSphere(origin, {1, 0, -10}, {1, 0, -10}, 0.3, tHit)
      && approxEqual(tHit, 1.0 - 0.3 / std::sqrt(101.0)), "off-axis dir through center");
}