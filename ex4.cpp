#include "vec3.hpp"
#include <cmath>
#include <iostream>
#include "geometry.hpp"

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