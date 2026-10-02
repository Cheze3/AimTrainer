#include <cmath>
#include <iostream>

struct Vec3 {
    double x, y, z;
};

Vec3 add(const Vec3& a, const Vec3& b) {
    return {a.x + b.x, a.y + b.y, a.z + b.z};
}

Vec3 sub(const Vec3& a, const Vec3& b){
    return {a.x - b.x, a.y - b.y, a.z - b.z};
}

Vec3 scale(const Vec3& v, double s){
    return {v.x * s, v.y * s, v.z * s};
}

double dot(const Vec3& a, const Vec3& b){
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

double length(const Vec3& v) {
    return std::sqrt(dot(v, v));
}

Vec3 normalize(const Vec3& v){
    double len = length(v);
    if (len < 1e-12) return {0, 0, 0};
    return scale(v, 1.0 / len);
}

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
