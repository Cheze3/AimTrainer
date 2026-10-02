#include <algorithm>
#include <iostream>
#include <random>
#include <vector>
#include "vec3.hpp"

struct Target {
    Vec3 pos;
    double radius;
    bool alive;
};

constexpr double WALL_Z = -10.0;
constexpr double WALL_HALF_WIDTH = 5.0;   // x in [-5, 5]
constexpr double WALL_HALF_HEIGHT = 3.0;  // y in [-3, 3]
constexpr double TARGET_RADIUS = 0.3;

std::vector<Target> spawnTargets(int count, std::mt19937& rng){
    std::vector<Target> targets;
    std::uniform_real_distribution<double> xDist(-WALL_HALF_WIDTH, WALL_HALF_WIDTH);
    std::uniform_real_distribution<double> yDist(-WALL_HALF_HEIGHT, WALL_HALF_HEIGHT);

    for (int i = 0; i < count; ++i) {
        Target t;
        t.pos.x = xDist(rng);
        t.pos.y = yDist(rng);
        t.pos.z = WALL_Z;
        t.radius = TARGET_RADIUS;
        t.alive = true;
        targets.push_back(t);
    }

    return targets;
}

bool removeAt(std::vector<Target>& targets, std::size_t index) {
    if (index >= targets.size()) {
        return false;
    }
    targets.erase(targets.begin() + index);
    return true;
}

void removeDead(std::vector<Target>& targets) {
    targets.erase(std::remove_if(targets.begin(), targets.end(), [](const Target& t) { return !t.alive; }), targets.end());
}

int countAlive(const std::vector<Target>& targets) {
    return std::count_if(targets.begin(), targets.end(), [](const Target& t) { return t.alive; });
}

// ---- tests ----
void check(bool ok, const char* name) {
    std::cout << (ok ? "PASS  " : "FAIL  ") << name << "\n";
}

int main() {
    std::mt19937 rng(12345);   // fixed seed: same numbers every run, so tests are repeatable

    std::vector<Target> targets = spawnTargets(10, rng);
    check(targets.size() == 10, "spawn count");

    bool inBounds = true;
    for (const Target& t : targets) {
        if (std::fabs(t.pos.x) > WALL_HALF_WIDTH ||
            std::fabs(t.pos.y) > WALL_HALF_HEIGHT ||
            t.pos.z != WALL_Z || !t.alive) {
            inBounds = false;
        }
    }
    check(inBounds, "spawn positions in bounds");

    // TODO: more checks (see below)
}