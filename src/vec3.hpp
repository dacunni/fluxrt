
#include "optics.h"

inline vec3 vec3::normalized() const
{
    float magsq = magnitude_sq();
    if(magsq != 0.0f) {
        float mag = std::sqrt(magsq);
        float invmag = 1.0f / mag;
        return vec3(x * invmag, y * invmag, z * invmag);
    }
    return *this;
}

inline vec3 cross(const vec3 & a, const vec3 & b)
{
    return vec3(a.y * b.z - a.z * b.y,
                a.z * b.x - a.x * b.z,
                a.x * b.y - a.y * b.x);
}

inline vec3 blend(const vec3 & a, float s, const vec3 & b, float t)
{
    return add(scale(a, s), scale(b, t));
}

// Given vector A pointing away from surface and normal N, computes the mirror direction R as
//
//  R = 2 * dot(A,N) * N - A
//
inline vec3 mirror(const vec3 & a, const vec3 & n)
{
    float twoAdotN = 2.0f * dot(a, n);
    vec3 NtwoAdotN = scale(n, twoAdotN);
    vec3 r = subtract(NtwoAdotN, a);
    return r;
}

// Given vector A pointing away from surface and normal N, as well as the
// indices of refraction of the original material (n1) and incident material
// (n2), computes the refraction direction R.
//
// Assuming A and N are proper directions (length = 1)
//
// Reference:
//   http://steve.hollasch.net/cgindex/render/refraction.txt
//   https://www.scratchapixel.com/lessons/3d-basic-rendering/introduction-to-shading/reflection-refraction-fresnel
//
inline vec3 refract(const vec3 & a, const vec3 & n, float n1, float n2)
{
#if 1
    // Baseline implementation from first principals

    float angleI = std::acos(dot(a, n));
    float angleT = optics::snellsLawAngle(n1, angleI, n2);

    if(std::isnan(angleT)) { // Total internal reflection
        return vec3::zero();
    }

    float cosT = std::cos(angleT);
    float sinT = std::sin(angleT);

    vec3 pivot = cross(a, n);
    vec3 axis = cross(pivot, n).normalized();

    vec3 refr = cosT * -n + sinT * axis;

    return refr.normalized();

#else
    // FIXME: This doesn't seem to work right

    //float eta = n1 / n2;
    float eta = std::min(n1, n2) / std::max(n1, n2);
    float c1 = dot(a, n);                           // cos(theta1)
    float c2sq = 1.0f - sq(eta) * (1.0f - sq(c1));  // cos(theta2)

    printf("c2sq %f\n", c2sq); // TEMP

    if(c2sq < 0.0f) { // Total internal reflection
        return vec3::zero();
    }

    return blend(a.negated(), eta,
                 n, eta * c1 - std::sqrt(c2sq)
                 ).normalized();
#endif
}

inline vec3 interp(const vec3 & a, const vec3 & b, const float alpha)
{
    const float oma = 1.0f - alpha;
    return vec3(oma * a.x + alpha * b.x,
                oma * a.y + alpha * b.y,
                oma * a.z + alpha * b.z);
}

