#ifndef __TONEMAPPING_H__
#define __TONEMAPPING_H__

namespace tonemapping {

inline float reinhard(float value);

inline float reinhardExtended(float value, float maxValue);



// Inline implementations

inline float reinhard(float value)
{
    return value / (1.0f + value);
}

inline float reinhardExtended(float value, float maxValue)
{
    float num = value * (1.0f + value / (maxValue * maxValue));
    float den = 1.0f + value;
    return num / den;
}

}; // namespace tonemapping


#endif
