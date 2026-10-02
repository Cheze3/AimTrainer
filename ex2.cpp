#include <cmath>
#include <iostream>

constexpr double CM_PER_INCH = 2.54;

double cm360FromSens(double sens, double dpi, double yaw){
    if (sens == 0 || dpi == 0 || yaw == 0) return 0;
    return (360.0/(sens * dpi * yaw))* CM_PER_INCH;
}
double sensFromCm360(double cm360, double dpi, double yaw){
    if (cm360 == 0 || dpi == 0 || yaw == 0) return 0;
    return (360.0/(cm360 * dpi * yaw)) * CM_PER_INCH;
}

bool approxEqual(double a, double b, double tol = 1e-9) {
    return std::fabs(a - b) < tol;
}

void check(bool ok, const char* name) {
    std::cout << (ok ? "PASS  " : "FAIL  ") << name << "\n";
}

int main() {
    constexpr double CS2_YAW = 0.022;
    constexpr double VAL_YAW = 0.07;

    // hand-calculated value from earlier
    check(approxEqual(cm360FromSens(1.0, 800, CS2_YAW), 51.95, 0.01), "cs2 known value");

    // round trip: converting there and back must return the original
    double cm = cm360FromSens(1.3, 1600, CS2_YAW);
    check(approxEqual(sensFromCm360(cm, 1600, CS2_YAW), 1.3), "round trip");

    // same physical sensitivity in two games
    // Valorant sens = CS2 sens * (0.022 / 0.07)
    double a = cm360FromSens(1.0, 800, CS2_YAW);
    double b = cm360FromSens(1.0 * CS2_YAW / VAL_YAW, 800, VAL_YAW);
    check(approxEqual(a, b), "cs2 to valorant equivalence");

    // doubling DPI halves cm/360
    check(approxEqual(cm360FromSens(1.0, 1600, CS2_YAW) * 2.0,
                      cm360FromSens(1.0, 800, CS2_YAW)), "dpi scaling");

    // zero input must not produce inf
    check(std::isfinite(cm360FromSens(0.0, 800, CS2_YAW)), "zero sens is handled");
}
