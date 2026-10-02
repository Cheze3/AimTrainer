#pragma once
#include <cmath>

struct Vec3 {
    double x, y, z;
};

inline Vec3 add(const Vec3& a, const Vec3& b) {
    return {a.x + b.x, a.y + b.y, a.z + b.z};
}

inline Vec3 sub(const Vec3& a, const Vec3& b){
    return {a.x - b.x, a.y - b.y, a.z - b.z};
}

inline Vec3 scale(const Vec3& v, double s){
    return {v.x * s, v.y * s, v.z * s};
}

inline double dot(const Vec3& a, const Vec3& b){
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

inline double length(const Vec3& v) {
    return std::sqrt(dot(v, v));
}

inline Vec3 normalize(const Vec3& v){
    double len = length(v);
    if (len < 1e-12) return {0, 0, 0};
    return scale(v, 1.0 / len);
}