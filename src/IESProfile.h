#ifndef __IES_PROFILE_H__
#define __IES_PROFILE_H__

#include <string>
#include <vector>
#include <memory>

// IES photometric light profile (IESNA LM-63)
//
// Stores the angular distribution of luminous intensity (candela) from
// a luminaire as a 2D table: candela[horizIdx][vertIdx].
//
// Vertical angles:   0 = nadir (straight down), 90 = horizontal, 180 = zenith
// Horizontal angles: 0..360 degrees (C-plane photometry, type 1)
//
// References:
//   https://ieslibrary.com
//   IESNA LM-63-2002 standard
struct IESProfile
{
    std::vector<float> verticalAngles;    // degrees
    std::vector<float> horizontalAngles;  // degrees
    // candela[h * numVert + v]
    std::vector<float> candela;

    float candelaMultiplier = 1.0f;

    // Sample intensity (in candela) for a direction given as vertical and
    // horizontal angles in degrees. Bilinearly interpolated.
    float sample(float vertDeg, float horizDeg) const;

    // Peak candela value (useful for normalizing)
    float peak() const;
};

// Load an IES profile from a file. Returns nullptr on failure.
std::shared_ptr<IESProfile> loadIESProfile(const std::string & filename);

#endif
