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
double convertSens(double sens, double dpi, double yawFrom, double yawTo){
    if (sens == 0 || dpi == 0 || yawFrom == 0 || yawTo == 0) return 0;
    double cm360 = cm360FromSens(sens, dpi, yawFrom);
    return sensFromCm360(cm360, dpi, yawTo);
}

enum class Game { CS2, Valorant };

double yawFor(Game g) {
    switch (g) {
        case Game::CS2:  return 0.022;
        case Game::Valorant: return 0.07;
    }
    return 0; 
}

void runTests() {
    // hand-calculated value from earlier
    check(approxEqual(cm360FromSens(1.0, 800, yawFor(Game::CS2)), 51.95, 0.01), "cs2 known value");

    // round trip: converting there and back must return the original
    double cm = cm360FromSens(1.3, 1600, yawFor(Game::CS2));
    check(approxEqual(sensFromCm360(cm, 1600, yawFor(Game::CS2)), 1.3), "round trip");

    // same physical sensitivity in two games
    // Valorant sens = CS2 sens * (0.022 / 0.07)
    double a = cm360FromSens(1.0, 800, yawFor(Game::CS2));
    double b = cm360FromSens(1.0 * yawFor(Game::CS2) / yawFor(Game::Valorant), 800, yawFor(Game::Valorant));
    check(approxEqual(a, b), "cs2 to valorant equivalence");

    // doubling DPI halves cm/360
    check(approxEqual(cm360FromSens(1.0, 1600, yawFor(Game::CS2)) * 2.0,
                      cm360FromSens(1.0, 800, yawFor(Game::CS2))), "dpi scaling");

    // zero input must not produce inf
    check(std::isfinite(cm360FromSens(0.0, 800, yawFor(Game::CS2))), "zero sens is handled");

    // CS2 -> Valorant: 1.0 * 0.022 / 0.07
    check(approxEqual(convertSens(1.0, 800, yawFor(Game::CS2), yawFor(Game::Valorant)),
                    0.022 / 0.07), "convertSens cs2 to valorant");

    // converting there and back returns the original
    double v = convertSens(1.3, 800, yawFor(Game::CS2), yawFor(Game::Valorant));
    check(approxEqual(convertSens(v, 800, yawFor(Game::Valorant), yawFor(Game::CS2)), 1.3),
        "convertSens round trip");

    // negative input is rejected
    check(convertSens(-1.0, 800, 0.022, 0.07) == 0, "negative sens rejected");
}

double readPositiveDouble(const char* prompt) {
    double value = 0;
    while (true) {
        std::cout << prompt;
        bool ok = static_cast<bool>(std::cin >> value);
        if (std::cin.eof()) std::exit(1);   // input stream closed, avoid infinite loop
        std::cin.clear();                   // reset the failed state
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // drop the rest of the line
        if (ok && value > 0) return value;
        std::cout << "Enter a number greater than 0.\n";
    }
}

Game readGame(const char* prompt) {
    while (true) {
        std::cout << prompt << " (1: CS2/Apex, 2: Valorant): ";
        int choice = 0;
        bool ok = static_cast<bool>(std::cin >> choice);
        if (std::cin.eof()) std::exit(1);
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        if (ok && choice == 1) return Game::CS2;
        if (ok && choice == 2) return Game::Valorant;
        std::cout << "Enter 1 or 2.\n";
    }
}

int main() {
    runTests();

    double dpi  = readPositiveDouble("DPI: ");
    double sens = readPositiveDouble("Sensitivity: ");
    Game from   = readGame("Source game");
    Game to     = readGame("Target game");

    double result = convertSens(sens, dpi, yawFor(from), yawFor(to));
    std::cout << "Converted sensitivity: " << result << "\n";
}


