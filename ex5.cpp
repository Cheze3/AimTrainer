#include <algorithm>
#include <chrono>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <string>
#include <thread>
#include "vec3.hpp"

struct MovingTarget {
    Vec3 pos;
    double speed;   // units per second
    double dirX;    // +1 or -1
};

constexpr double MAX_X = 5.0;

void updateTarget(MovingTarget& t, double dt) {
    t.pos.x += t.speed * dt * t.dirX;
    if (t.pos.x > MAX_X) {
        double overshoot = t.pos.x - MAX_X;
        t.pos.x = MAX_X - overshoot;
        t.dirX *= -1;
    } else if (t.pos.x < -MAX_X) {
        double overshoot = -MAX_X - t.pos.x;
        t.pos.x = -MAX_X + overshoot;
        t.dirX *= -1;
    }
}

void check(bool ok, const char* name) {
    std::cout << (ok ? "PASS  " : "FAIL  ") << name << "\n";
}

bool approxEqual(double a, double b, double tol = 1e-9) {
    return std::fabs(a - b) < tol;
}

MovingTarget simulate(MovingTarget initial, double totalTime, int steps) {
    MovingTarget t = initial;
    double dt = totalTime / steps;
    for (int i = 0; i < steps; ++i) {
        updateTarget(t, dt);
    }
    return t;
}

void runTests() {
    // 1. One step moves speed * dt
    MovingTarget a{{0, 0, -10}, 4.0, 1.0};
    updateTarget(a, 1.0);
    check(approxEqual(a.pos.x, 4.0), "move right 1s");

    // 2. Frame-rate independence: 1 second in 1, 10 and 100 steps, no wall reached
    MovingTarget start{{0, 0, -10}, 4.0, 1.0};
    double x1   = simulate(start, 1.0, 1).pos.x;
    double x10  = simulate(start, 1.0, 10).pos.x;
    double x100 = simulate(start, 1.0, 100).pos.x;
    check(approxEqual(x1, 4.0, 1e-6) && approxEqual(x10, 4.0, 1e-6) && approxEqual(x100, 4.0, 1e-6),
          "same distance for 1, 10, 100 steps");

    // 3. Bounce off the right wall: 4.5 + 2.0 = 6.5, overshoot 1.5, x = 3.5
    MovingTarget r{{4.5, 0, -10}, 4.0, 1.0};
    updateTarget(r, 0.5);
    check(approxEqual(r.pos.x, 3.5) && approxEqual(r.dirX, -1.0), "bounce right wall");

    // 4. Bounce off the left wall: -4.5 - 2.0 = -6.5, overshoot 1.5, x = -3.5
    MovingTarget l{{-4.5, 0, -10}, 4.0, -1.0};
    updateTarget(l, 0.5);
    check(approxEqual(l.pos.x, -3.5) && approxEqual(l.dirX, 1.0), "bounce left wall");

    // 5. dt = 0 changes nothing
    MovingTarget z{{1.25, 0, -10}, 4.0, -1.0};
    updateTarget(z, 0.0);
    check(approxEqual(z.pos.x, 1.25) && approxEqual(z.dirX, -1.0), "dt = 0 leaves target unchanged");
}

int main(int argc, char** argv) {
    runTests();

    // dt    : real delta time, no sleep
    // sleep : real delta time, 1 ms sleep per frame
    // fixed : 1 ms sleep, ignores real time and moves a fixed step per frame
    std::string mode = (argc > 1) ? argv[1] : "dt";
    if (mode != "dt" && mode != "sleep" && mode != "fixed") {
        std::cout << "usage: ex5 [dt|sleep|fixed]\n";
        return 1;
    }
    bool useSleep = (mode == "sleep" || mode == "fixed");
    bool ignoreDt = (mode == "fixed");

    using Clock = std::chrono::steady_clock;
    MovingTarget target{{0, 0, -10}, 4.0, 1.0};

    auto start = Clock::now();
    auto previous = start;
    auto lastReport = start;
    int framesThisSecond = 0;
    long long totalFrames = 0;
    int bounces = 0;

    while (true) {
        auto now = Clock::now();
        double dt = std::chrono::duration<double>(now - previous).count();
        dt = std::min(dt, 0.1);  // a stalled frame must not teleport the target
        previous = now;

        double stepDt = ignoreDt ? 0.01 : dt;
        double dirBefore = target.dirX;
        updateTarget(target, stepDt);
        if (target.dirX != dirBefore) ++bounces;  // dirX is only ever +1 or -1, exact compare is fine
        ++framesThisSecond;
        ++totalFrames;

        double sinceReport = std::chrono::duration<double>(now - lastReport).count();
        if (sinceReport >= 1.0) {
            double elapsed = std::chrono::duration<double>(now - start).count();
            std::cout << std::fixed
                      << "Elapsed: " << std::setprecision(1) << elapsed
                      << "s, FPS: " << framesThisSecond
                      << ", Target X: " << std::setprecision(2) << target.pos.x
                      << "\n";
            framesThisSecond = 0;
            lastReport = now;
        }

        if (std::chrono::duration<double>(now - start).count() >= 10.0) break;

        if (useSleep) std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    std::cout << "mode " << mode << ": " << totalFrames << " frames, "
              << bounces << " bounces\n";
    return 0;
}