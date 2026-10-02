#include <cmath>
#include <iostream>
#include "vec3.hpp"


bool approxEqual(double a, double b) { return std::fabs(a - b) < 1e-9; }

void check(bool ok, const char* name) {
    std::cout << (ok ? "PASS  " : "FAIL  ") << name << "\n";
}

int main() {
    Vec3 s = add({1, 2, 3}, {4, 5, 6});
    check(approxEqual(s.x, 5) && approxEqual(s.y, 7) && approxEqual(s.z, 9), "add");

    Vec3 d = sub({1, 2, 3}, {4, 5, 6});
    check(approxEqual(d.x, -3) && approxEqual(d.y, -3) && approxEqual(d.z, -3), "sub");
    Vec3 sc = scale({1, 2, 3}, 2);
    check(approxEqual(sc.x, 2) && approxEqual(sc.y, 4) && approxEqual(sc.z, 6), "scale");
    check(approxEqual(dot({1, 2, 3}, {4, 5, 6}), 32), "dot");
    check(approxEqual(length({3, 4, 0}), 5), "length");

    Vec3 n = normalize({0, 0, 5});
    check(approxEqual(n.x, 0) && approxEqual(n.y, 0) && approxEqual(n.z, 1), "normalize");
    check(approxEqual(length(normalize({2, -7, 4})), 1), "normalize gives length 1");

    Vec3 z = normalize({0, 0, 0});
    check(approxEqual(z.x, 0) && approxEqual(z.y, 0) && approxEqual(z.z, 0), "normalize zero vector");
}
