#ifndef __BARYCENTRIC_H__
#define __BARYCENTRIC_H__

struct Position3;
struct Direction3;
struct TextureCoordinate;

// Barycentric coordinates using the convension that u + v + w = 1
struct BarycentricCoordinate
{
    inline BarycentricCoordinate() : u(0.0f), v(0.0f), w(1.0f) {}
    inline BarycentricCoordinate(float u, float v) : u(u), v(v), w(1.0f - u - v) {}
    inline BarycentricCoordinate(float u, float v, float w) : u(u), v(v), w(w) {}
    inline ~BarycentricCoordinate() = default;

    float u, v, w;
};

// Finds the barycentric coordinate of a point in a triangle. Assumes the
// point is inside the triangle (in the same plane).
BarycentricCoordinate barycentricForPointInTriangle(const Position3 & P,
                                                    const Position3 & tA,
                                                    const Position3 & tB,
                                                    const Position3 & tC);

// Calculates the weighted average of three points, using the barycentric
// coordinate as the weights.
Position3 interpolate(const Position3 & v0,
                      const Position3 & v1,
                      const Position3 & v2,
                      const BarycentricCoordinate & bary);

// Calculates the weighted average of three directions, using the barycentric
// coordinate as the weights.
Direction3 interpolate(const Direction3 & v0,
                       const Direction3 & v1,
                       const Direction3 & v2,
                       const BarycentricCoordinate & bary);

// Calculates the weighted average of three texture coordinates, using the barycentric
// coordinate as the weights.
TextureCoordinate interpolate(const TextureCoordinate & tc0,
                              const TextureCoordinate & tc1,
                              const TextureCoordinate & tc2,
                              const BarycentricCoordinate & bary);

// Calculates the weighted average of three values, using the barycentric
// coordinate as the weights.
float interpolate(const float v0, const float v1, const float v2,
                  const BarycentricCoordinate & bary);

#endif
