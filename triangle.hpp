
inline bool intersectsTriangle(const Ray & ray,
                               const vec3 & v0, const vec3 & v1, const vec3 & v2,
                               float minDistance)
{
    const float epsilon = 1.0e-6;

    // Compute edge vectors
    auto e1 = v1 - v0;
    auto e2 = v2 - v0;

    // Compute determinant
    auto P = cross(ray.direction, e2);
    auto det = dot(e1, P);

    // If determinant zero, the ray does not intersect the plane of the triangle
    // Note, we're not culling backfaces.
    if(std::fabs(det) < epsilon)
        return false;   // no intersection
    float inv_det = 1.0f / det;

    // Calculate vector from V0 to ray origin
    auto T = ray.origin - v0;

    // Calculate u coordinate and test whether the intersection lies within the valid range of u
    float u = inv_det * dot(T, P);
    if(u < 0.0f || u > 1.0f)
        return false;   // intersection out of valid u range

    // Calculate v coordinate and test whether the intersection lies within the valid range of v
    auto Q = cross(T, e1);
    float v = inv_det * dot(ray.direction, Q);
    if(v < 0.0f || u + v > 1.0f)
        return false;   // intersection out of u/v range

    float t = inv_det * dot(e2, Q);
    return t > minDistance;
}

