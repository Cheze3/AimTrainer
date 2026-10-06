#pragma once
#include <cmath>
#include <algorithm>
#include "vec3.hpp"

constexpr double PI = 3.14159265358979323846;
constexpr double CM_PER_INCH = 2.54;
constexpr double MAX_PITCH_DEG = 89.0;

struct Camera {
    double yawDeg = 0.0;        // wrapped to [0, 360)
    double pitchDeg = 0.0;      // clamped to [-89, 89]
    double totalYawDeg = 0.0;   // never wrapped, used only to verify calibration
};

// Degrees the view turns per raw mouse count.
// One full turn takes (cm360 / 2.54) * dpi counts, so degPerCount = 360 / that.
inline double degPerCount(double cm360, double dpi) {
    if (cm360 <= 0 || dpi <= 0) return 0.0;
    return 360.0 * CM_PER_INCH / (cm360 * dpi);
}

// TODO: dx > 0 (mouse right) increases yaw. dy > 0 (mouse down) decreases pitch.
// Wrap yawDeg into [0, 360) (std::fmod returns negative values for negative
// input, so fix that case). Clamp pitchDeg to +-MAX_PITCH_DEG.
// Add the unwrapped yaw change to totalYawDeg.
inline void applyMouseDelta(Camera& cam, double dx, double dy, double degPerCountValue){
    cam.yawDeg += dx * degPerCountValue;
    cam.pitchDeg -= dy * degPerCountValue;
    cam.totalYawDeg += dx * degPerCountValue;
    // Wrap yawDeg to [0, 360)
    cam.yawDeg = std::fmod(cam.yawDeg, 360.0);
    if (cam.yawDeg < 0) cam.yawDeg += 360.0;
    // Clamp pitchDeg to [-MAX_PITCH_DEG, MAX_PITCH_DEG]
    cam.pitchDeg = std::clamp(cam.pitchDeg, -MAX_PITCH_DEG, MAX_PITCH_DEG);
}

// TODO: unit vector the camera looks along. With yaw and pitch in radians:
//   x =  sin(yaw) * cos(pitch)
//   y =  sin(pitch)
//   z = -cos(yaw) * cos(pitch)
inline Vec3 forward(const Camera& cam){
    double yawRad = cam.yawDeg * PI / 180.0;
    double pitchRad = cam.pitchDeg * PI / 180.0;
    double x = std::sin(yawRad) * std::cos(pitchRad);
    double y = std::sin(pitchRad);
    double z = -std::cos(yawRad) * std::cos(pitchRad);
    return {x, y, z};
}