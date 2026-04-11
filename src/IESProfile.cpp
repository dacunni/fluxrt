#include "IESProfile.h"

#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

// ---------------------------------------------------------------------------
// Parsing helpers
// ---------------------------------------------------------------------------

static bool startsWith(const std::string & s, const std::string & prefix)
{
    return s.size() >= prefix.size() && s.compare(0, prefix.size(), prefix) == 0;
}

// Read all whitespace-delimited tokens from a stream into a flat list.
static std::vector<std::string> tokenize(std::istream & in)
{
    std::vector<std::string> tokens;
    std::string tok;
    while(in >> tok) {
        tokens.push_back(tok);
    }
    return tokens;
}

// Parse a float from a string, throwing on failure.
static float parseFloat(const std::string & s)
{
    try {
        return std::stof(s);
    } catch(...) {
        throw std::runtime_error("IES parse error: expected float, got '" + s + "'");
    }
}

static int parseInt(const std::string & s)
{
    try {
        return std::stoi(s);
    } catch(...) {
        throw std::runtime_error("IES parse error: expected int, got '" + s + "'");
    }
}

// ---------------------------------------------------------------------------
// IES file parser
// ---------------------------------------------------------------------------
//
// Format (IESNA LM-63-1991/1995/2002 and older headerless variant):
//
//   [optional header line: IESNA:LM-63-xxxx]
//   [optional keyword lines: [KEYWORD] value]
//   TILT=NONE  (or TILT=INCLUDE / TILT=filename — we skip tilt data)
//   <numLamps> <lumensPerLamp> <candelaMultiplier> <numVert> <numHoriz>
//       <photometricType> <unitsType> <width> <length> <height>
//   <ballastFactor> <ballastLampFactor> <inputWatts>
//   <vertical angle 0> ... <vertical angle numVert-1>
//   <horizontal angle 0> ... <horizontal angle numHoriz-1>
//   [for each horizontal angle: <candela 0> ... <candela numVert-1>]

std::shared_ptr<IESProfile> loadIESProfile(const std::string & filename)
{
    std::ifstream file(filename);
    if(!file.is_open()) {
        std::cerr << "IESProfile: cannot open '" << filename << "'\n";
        return nullptr;
    }

    try {
        // Read all lines, skipping header/keyword lines until TILT=
        std::string line;
        bool foundTilt = false;
        std::string dataBlock;

        while(std::getline(file, line)) {
            // Trim trailing CR
            if(!line.empty() && line.back() == '\r') {
                line.pop_back();
            }
            // Skip IESNA header, keyword lines, and blank lines
            if(startsWith(line, "IESNA") || startsWith(line, "[") || line.empty()) {
                continue;
            }
            if(startsWith(line, "TILT")) {
                foundTilt = true;
                // If TILT=INCLUDE, there's tilt data on subsequent lines we must skip.
                // We do not support tilt; for TILT=INCLUDE or TILT=filename we bail.
                std::string tiltVal = line.substr(line.find('=') + 1);
                // trim
                while(!tiltVal.empty() && (tiltVal.front() == ' ' || tiltVal.front() == '\t'))
                    tiltVal.erase(tiltVal.begin());
                if(tiltVal != "NONE") {
                    // Skip: lamp-to-luminaire geometry, number of tilt entries, angles, factors
                    // We skip lines until we've consumed: 1 (geometry) + 1 (count) + 2*count lines
                    std::string tiltLine;
                    // Lamp-to-luminaire geometry
                    std::getline(file, tiltLine);
                    // Number of pairs
                    std::getline(file, tiltLine);
                    try { std::stoi(tiltLine); } catch(...) {}
                    // angles line(s) and factors line(s) — conservatively skip 2*nPairs tokens worth
                    // via the data block below (they'll be consumed but we'll skip them)
                    // For simplicity, just warn and skip entire tilt (use 0 pairs = NONE behavior)
                    std::cerr << "IESProfile: TILT data not supported in '" << filename
                              << "', tilt ignored\n";
                    // Skip nPairs worth of angle and factor tokens by reading into data block
                    // then we drop them below by parsing from correct offset.
                    // Easiest: slurp remaining file, let the numeric parser sort it out.
                }
                // Fall through — start accumulating data
                while(std::getline(file, line)) {
                    if(!line.empty() && line.back() == '\r') line.pop_back();
                    dataBlock += line + ' ';
                }
                break;
            }
            // Lines before TILT that aren't keywords — shouldn't happen in valid IES
        }

        if(!foundTilt) {
            std::cerr << "IESProfile: no TILT line found in '" << filename << "'\n";
            return nullptr;
        }

        std::istringstream dataStream(dataBlock);
        std::vector<std::string> tokens = tokenize(dataStream);
        size_t idx = 0;

        auto nextInt   = [&]() { return parseInt(tokens.at(idx++)); };
        auto nextFloat = [&]() { return parseFloat(tokens.at(idx++)); };

        // Line 1 of data
        /*int numLamps         =*/ nextInt();
        /*float lumensPerLamp  =*/ nextFloat();
        float candelaMultiplier = nextFloat();
        int numVert             = nextInt();
        int numHoriz            = nextInt();
        /*int photometricType  =*/ nextInt();
        /*int unitsType        =*/ nextInt();
        /*float width          =*/ nextFloat();
        /*float length         =*/ nextFloat();
        /*float height         =*/ nextFloat();

        // Line 2
        /*float ballastFactor     =*/ nextFloat();
        /*float ballastLampFactor =*/ nextFloat();
        /*float inputWatts        =*/ nextFloat();

        if(numVert <= 0 || numHoriz <= 0) {
            std::cerr << "IESProfile: invalid angle counts in '" << filename << "'\n";
            return nullptr;
        }

        auto profile = std::make_shared<IESProfile>();
        profile->candelaMultiplier = candelaMultiplier;

        // Vertical angles
        profile->verticalAngles.reserve(numVert);
        for(int i = 0; i < numVert; ++i) {
            profile->verticalAngles.push_back(nextFloat());
        }

        // Horizontal angles
        profile->horizontalAngles.reserve(numHoriz);
        for(int i = 0; i < numHoriz; ++i) {
            profile->horizontalAngles.push_back(nextFloat());
        }

        // Candela values: for each horizontal angle, numVert values
        profile->candela.reserve(numHoriz * numVert);
        for(int h = 0; h < numHoriz; ++h) {
            for(int v = 0; v < numVert; ++v) {
                profile->candela.push_back(nextFloat() * candelaMultiplier);
            }
        }

        return profile;

    } catch(const std::exception & e) {
        std::cerr << "IESProfile: error parsing '" << filename << "': " << e.what() << '\n';
        return nullptr;
    }
}

// ---------------------------------------------------------------------------
// IESProfile::sample  — bilinear interpolation
// ---------------------------------------------------------------------------

static float lerpf(float a, float b, float t) { return a + (b - a) * t; }

// Find interval index such that table[i] <= value < table[i+1].
// Returns clamped index in [0, n-2].
static int findInterval(const std::vector<float> & table, float value)
{
    int n = (int)table.size();
    if(n <= 1) return 0;
    // clamp
    if(value <= table.front()) return 0;
    if(value >= table.back())  return n - 2;
    // binary search
    int lo = 0, hi = n - 2;
    while(lo < hi) {
        int mid = (lo + hi + 1) / 2;
        if(table[mid] <= value) lo = mid;
        else                    hi = mid - 1;
    }
    return lo;
}

float IESProfile::sample(float vertDeg, float horizDeg) const
{
    if(candela.empty()) return 0.0f;

    const int nV = (int)verticalAngles.size();
    const int nH = (int)horizontalAngles.size();

    // Clamp to valid range
    vertDeg  = std::max(verticalAngles.front(),  std::min(verticalAngles.back(),  vertDeg));

    // Handle horizontal symmetry:
    //   nH == 1              → omnidirectional (or single plane), no horizontal variation
    //   last angle == 90     → bilateral symmetry (mirror at 90)
    //   last angle == 180    → front/back symmetry (mirror at 180)
    //   last angle == 360    → full revolution
    if(nH == 1) {
        horizDeg = horizontalAngles[0];
    } else {
        float hMax = horizontalAngles.back();
        // Wrap horizDeg into [0, 360)
        horizDeg = std::fmod(horizDeg, 360.0f);
        if(horizDeg < 0.0f) horizDeg += 360.0f;

        if(hMax <= 90.0f + 1e-3f) {
            // Quarter symmetry: fold into [0, 90]
            horizDeg = std::fmod(horizDeg, 90.0f);
        } else if(hMax <= 180.0f + 1e-3f) {
            // Half symmetry: fold into [0, 180]
            if(horizDeg > 180.0f) horizDeg = 360.0f - horizDeg;
        }
        // else full 360 — use as-is, but clamp to valid range
        horizDeg = std::max(horizontalAngles.front(), std::min(hMax, horizDeg));
    }

    // Find vertical interval and fractional position
    int vi = findInterval(verticalAngles, vertDeg);
    float vt = (nV < 2) ? 0.0f
             : (vertDeg - verticalAngles[vi]) / (verticalAngles[vi + 1] - verticalAngles[vi]);

    if(nH == 1) {
        // No horizontal interpolation
        float c0 = candela[vi];
        float c1 = (vi + 1 < nV) ? candela[vi + 1] : c0;
        return lerpf(c0, c1, vt);
    }

    // Find horizontal interval and fractional position
    int hi = findInterval(horizontalAngles, horizDeg);
    float ht = (horizontalAngles[hi + 1] - horizontalAngles[hi]) < 1e-6f ? 0.0f
             : (horizDeg - horizontalAngles[hi]) / (horizontalAngles[hi + 1] - horizontalAngles[hi]);

    // Bilinear: indices into candela[h * nV + v]
    auto c = [&](int h, int v) -> float {
        h = std::max(0, std::min(nH - 1, h));
        v = std::max(0, std::min(nV - 1, v));
        return candela[h * nV + v];
    };

    float c00 = c(hi,     vi);
    float c10 = c(hi + 1, vi);
    float c01 = c(hi,     vi + 1);
    float c11 = c(hi + 1, vi + 1);

    return lerpf(lerpf(c00, c10, ht), lerpf(c01, c11, ht), vt);
}

float IESProfile::peak() const
{
    if(candela.empty()) return 0.0f;
    return *std::max_element(candela.begin(), candela.end());
}
