#include <cmath>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <limits>
#include <sstream>
#include <string>

struct Settings {
    double dpi        = 800.0;
    double cm360      = 35.0;
    double fov        = 103.0;                 // horizontal, degrees
    float  bgColor[3] = {0.10f, 0.10f, 0.12f}; // r, g, b in [0, 1]
    float  brightness = 1.0f;                  // 0 = black, 1 = normal, up to 2
};

bool inRange(double v, double lo, double hi) { return v >= lo && v <= hi; }

bool saveSettings(const Settings& s, const std::string& path) {
    std::ofstream f(path);
    if (!f.is_open()) {
        return false;
    }
    f << std::setprecision(std::numeric_limits<double>::max_digits10);
    f << "dpi=" << s.dpi << "\n";
    f << "cm360=" << s.cm360 << "\n";
    f << "fov=" << s.fov << "\n";
    f << std::setprecision(std::numeric_limits<float>::max_digits10);
    f << "bg_r=" << s.bgColor[0] << "\n";
    f << "bg_g=" << s.bgColor[1] << "\n";
    f << "bg_b=" << s.bgColor[2] << "\n";
    f << "brightness=" << s.brightness << "\n";
    f.close();
    return !f.fail();
}


Settings loadSettings(const std::string& path){
    Settings s;
    std::ifstream f(path);
    if (!f.is_open()) {
        return s; // return default settings if file cannot be opened
    }
    std::string line;
    while (std::getline(f, line)) {
        // Remove any trailing carriage return (for Windows line endings)
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        // Skip blank lines and comments
        if (line.empty() || line[0] == '#') {
            continue;
        }
        // Parse key-value pairs
        std::string::size_type pos = line.find('=');
        if (pos == std::string::npos) {
            continue;
        }
        std::string key = line.substr(0, pos);
        std::string value = line.substr(pos + 1);
        // Remove leading and trailing whitespace from key and value
        key.erase(0, key.find_first_not_of(" \t"));
        key.erase(key.find_last_not_of(" \t") + 1);
        value.erase(0, value.find_first_not_of(" \t"));
        value.erase(value.find_last_not_of(" \t") + 1);
        // Convert value to number and update settings
        std::istringstream iss(value);
        double num = 0;
        if (!(iss >> num)) continue; // skip if conversion fails
        std::string extra;
        if (iss >> extra) continue; // skip if there are extra characters after the number

        if (key == "dpi") {
            if (inRange(num, 100, 32000)) {
                s.dpi = num;
            }
        } else if (key == "cm360") {
            if (num > 0 && num <= 1000){
                s.cm360 = num;
            }
        } else if (key == "fov") {
            if (inRange(num, 30, 150)) {
                s.fov = num;
            }
        } else if (key == "bg_r") {
            if (inRange(num, 0, 1)) {
                s.bgColor[0] = static_cast<float>(num);
            }
        } else if (key == "bg_g") {
            if (inRange(num, 0, 1)) {
                s.bgColor[1] = static_cast<float>(num);
            }
        } else if (key == "bg_b") {
            if (inRange(num, 0, 1)) {
                s.bgColor[2] = static_cast<float>(num);
            }
        } else if (key == "brightness") {
            if (inRange(num, 0, 2)) {
                s.brightness = static_cast<float>(num);
            }
        }
    }
    return s;
}

// ---- tests ----
static int g_failures = 0;

void check(bool ok, const char* name) {
    std::cout << (ok ? "PASS  " : "FAIL  ") << name << "\n";
    if (!ok) {
        ++g_failures;
    }
}


bool approxEqual(double a, double b, double tol = 1e-9) {
    return std::fabs(a - b) < tol;
}

bool writeFile(const std::string& path, const std::string& text) {
    std::ofstream f(path);
    f << text;
    f.close();
    return !f.fail();
}

bool equalSettings(const Settings& a, const Settings& b) {
    return approxEqual(a.dpi, b.dpi, 1e-12) &&
           approxEqual(a.cm360, b.cm360, 1e-12) &&
           approxEqual(a.fov, b.fov, 1e-12) &&
           a.bgColor[0] == b.bgColor[0] &&
           a.bgColor[1] == b.bgColor[1] &&
           a.bgColor[2] == b.bgColor[2] &&
           a.brightness == b.brightness;
}

// Writes `text` to a temp file, loads it, deletes the file.
Settings loadFromText(const std::string& text) {
    const std::string path = "test_settings.tmp";
    if (!writeFile(path, text)) {
        check(false, "failed to write temp file");
        return Settings();
    }
    Settings s = loadSettings(path);
    std::remove(path.c_str());
    return s;
}

void runTests() {
    const Settings defaults;

    Settings in;
    in.dpi = 1600;
    in.cm360 = 51.95123456789;
    in.fov = 90.5;
    in.bgColor[0]= 0.25f; in.bgColor[1]= 0.5f; in.bgColor[2]= 0.75f;
    in.brightness = 1.5f;
    const std::string path = "test_roundtrip.tmp";
    bool saved = saveSettings(in, path);
    Settings out = loadSettings(path);
    std::remove(path.c_str());
    check(saved && equalSettings(in, out), "round trip save/load");

    check(equalSettings(loadSettings("nonexistent.tmp"), Settings()), "missing file gives defaults");
    check(equalSettings(loadFromText(""), defaults), "empty file gives defaults");

    Settings expected = defaults;
    expected.dpi = 1600;
    check(equalSettings(loadFromText("dpi=1600\n"), expected), "partial file");

    expected = defaults;
    expected.cm360 = 40;
    check(equalSettings(loadFromText("garbage\ndpi=abc\nfov=\n=5\ncm360=40\n"), expected), "default except cm360=40");

    expected = defaults;
    expected.dpi = 1600;
    check(equalSettings(loadFromText("colour=5\ndpi=1600\n"), expected), "default except dpi=1600");
    

    expected = defaults;
    expected.fov = 90;
    expected.bgColor[0] = 0.5f; expected.bgColor[1] = 0.6f; expected.bgColor[2] = 0.7f;
    expected.brightness = 1.5f;
    expected.dpi = 1600;
    expected.cm360 = 40;
    check(equalSettings(loadFromText("dpi=1600\ncm360=40\nfov=90\nbg_r=0.5\nbg_g=0.6\nbg_b=0.7\nbrightness=1.5\n"), expected), "all valid values");

    check(equalSettings(loadFromText("dpi=-5\nfov=999\nbrightness=7\nbg_r=1.5\ncm360=0\n"), defaults), "all invalid values");

    expected = defaults;
    expected.dpi = 1200;
    expected.cm360 = 40;
    check(equalSettings(loadFromText("  dpi = 1200  \n# a comment\n\ncm360=40\n"), expected),
          "whitespace, comment and blank line");

    // Windows line endings
    check(equalSettings(loadFromText("dpi=1200\r\ncm360=40\r\n"), expected), "CRLF line endings");

    // Trailing junk makes the line malformed
    check(equalSettings(loadFromText("dpi=1200abc\n"), defaults), "trailing letters rejected");
    check(equalSettings(loadFromText("dpi=12 00\n"), defaults), "space inside number rejected");

    // A bad later line must not erase an earlier good value
    expected = defaults;
    expected.dpi = 1600;
    check(equalSettings(loadFromText("dpi=1600\ndpi=abc\n"), expected), "bad line keeps earlier good value");

    // Unwritable path
    check(!saveSettings(in, "/nonexistent_dir/x.txt"), "save to bad path returns false");

    // Boundary values are accepted
    expected = defaults;
    expected.dpi = 100;
    expected.fov = 150;
    expected.cm360 = 1000;
    expected.brightness = 0;
    expected.bgColor[1] = 1;
    check(equalSettings(loadFromText("dpi=100\nfov=150\ncm360=1000\nbrightness=0\nbg_g=1\n"), expected),
          "boundary values accepted");

    // Just outside the boundaries is rejected
    check(equalSettings(loadFromText("dpi=99.9\nfov=29.9\ncm360=1000.1\nbrightness=2.1\nbg_b=-0.1\n"), defaults),
          "just outside boundaries rejected");

    // nan and inf are rejected
    check(equalSettings(loadFromText("dpi=nan\nfov=inf\n"), defaults), "nan and inf rejected");
    
}   

int main() {
    runTests();
    std::cout << (g_failures == 0 ? "all tests passed\n"
                                  : std::to_string(g_failures) + " tests failed\n");
    return g_failures == 0 ? 0 : 1;
}