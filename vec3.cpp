#include <cmath>
#include <algorithm>
#include <sstream>
#include "vec3.h"

vec3 vec3::zero() { return vec3(0.0f, 0.0f, 0.0f); }

vec3::vec3()
    : x(0.0f), y(0.0f), z(0.0f)
{

}

vec3::vec3(const vec3 & a)
    : x(a.x), y(a.y), z(a.z)
{

}

vec3::vec3(float xn, float yn, float zn)
    : x(xn), y(yn), z(zn)
{

}

vec3::~vec3()
{

}

void vec3::set(float xn, float yn, float zn)
{
    x=xn; y=yn; z=zn;
}

vec3 vec3::negated() const
{
    return vec3(-x, -y, -z);
}

float vec3::magnitude_sq() const
{
    return sq(x) + sq(y) + sq(z);
}

float vec3::magnitude() const
{
    return std::sqrt(sq(x) + sq(y) + sq(z));
}

vec3 vec3::normalized() const
{
    float magsq = magnitude_sq();
    if(magsq != 0.0f) {
        float mag = std::sqrt(magsq);
        float invmag = 1.0f / mag;
        return vec3(x * invmag, y * invmag, z * invmag);
    }
    return *this;
}

void vec3::normalize()
{
    *this = this->normalized();
}

std::string vec3::string() const
{
    std::stringstream ss;
    const char * sep = ", ";
    ss << x << sep << y << sep << z;
    return ss.str();
}

//
// OPERATORS
//

bool operator==(const vec3 & a, const vec3 & b) { return a.x == b.x && a.y == b.y && a.z == b.z; }

vec3 add(const vec3 & a, const vec3 & b) { return vec3(a.x + b.x, a.y + b.y, a.z + b.z); }
vec3 operator+(const vec3 & a, const vec3 & b) { return add(a, b); }

vec3 subtract(const vec3 & a, const vec3 & b) { return vec3(a.x - b.x, a.y - b.y, a.z - b.z); }
vec3 operator-(const vec3 & a, const vec3 & b) { return subtract(a, b); }

vec3 scale(const vec3 & a, float s) { return vec3(a.x * s, a.y * s, a.z * s); }
vec3 operator*(const vec3 & a, float s) { return scale(a, s); }
vec3 operator*(float s, const vec3 & a) { return scale(a, s); }

float dot(const vec3 & a, const vec3 & b) { return a.x * b.x + a.y * b.y + a.z * b.z; }
float clampedDot(const vec3 & a, const vec3 & b) { return std::max(dot(a, b), 0.0f); }

vec3 cross(const vec3 & a, const vec3 & b) {
    return vec3(a.y * b.z - a.z * b.y,
                a.z * b.x - a.x * b.z,
                a.x * b.y - a.y * b.x);
}

vec3 blend(const vec3 & a, float s, const vec3 & b, float t) { return add(scale(a, s), scale(b, t)); }

// Given vector A pointing away from surface and normal N, computes the mirror direction R as
//
//  R = 2 * dot(A,N) * N - A
//
vec3 mirror(const vec3 & a, const vec3 & n) {
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
//
vec3 refract(const vec3 & a, const vec3 & n, float n1, float n2) {
    float eta = n1 / n2;
    float c1 = dot(a, n);                           // cos(theta1)
    float c2sq = 1.0f - sq(eta) * (1.0f - sq(c1));  // cos(theta2)

    if(c2sq < 0.0f) { // Total internal reflection
        return vec3::zero();
    }

    return blend(a.negated(), eta, n, eta * c1 - std::sqrt(c2sq)).normalized();
}

vec3 interp(const vec3 & a, const vec3 & b, const float alpha) {
    const float oma = 1.0f - alpha;
    return vec3(oma * a.x + alpha * b.x,
                oma * a.y + alpha * b.y,
                oma * a.z + alpha * b.z);
}

