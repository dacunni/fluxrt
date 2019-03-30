#include "barycentric.h"
#include "vectortypes.h"

// Calculate the area of a triangle given its vertices
float triangleArea(const Position3 & A, const Position3 & B, const Position3 & C)
{
    // Area of the parallelogram with AB and AC as sides
    auto AB = B - A;
    auto AC = C - A;
    auto ABxAC = cross(AB, AC);
    float paraArea = ABxAC.magnitude();

    // The triangle is half of this parallegram
    return paraArea * 0.5;
}

// Finds the barycentric coordinate of a point in a triangle. Assumes the point
// is inside the triangle (in the same plane).
BarycentricCoordinate barycentricForPointInTriangle(const Position3 & P,
                                                    const Position3 & tA,
                                                    const Position3 & tB,
                                                    const Position3 & tC)
{
    float tArea = triangleArea(tA, tB, tC);

    if(tArea <= 0.0f) { // degenerate triangle
        return BarycentricCoordinate(0.0, 0.0);
    }

    return BarycentricCoordinate(triangleArea(tB, tC, P) / tArea,
                                 triangleArea(tC, tA, P) / tArea);
}

Position3 interpolate(const Position3 & v0,
                      const Position3 & v1,
                      const Position3 & v2,
                      const BarycentricCoordinate & bary)
{
    return Position3(v0 * bary.u + v1 * bary.v + v2 * bary.w);
}
Direction3 interpolate(const Direction3 & v0,
                       const Direction3 & v1,
                       const Direction3 & v2,
                       const BarycentricCoordinate & bary)
{
    return (v0 * bary.u + v1 * bary.v + v2 * bary.w).normalized();
}


