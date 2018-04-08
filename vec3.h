#ifndef __VEC3_H__
#define __VEC3_H__

#include <string>
#include "base.h"

struct vec3
{
    vec3();
    vec3(const vec3 & a);
	vec3(float xn, float yn, float zn);
    ~vec3();

	void set(float xn, float yn, float zn);
	vec3 negated() const;
	float magnitude_sq() const;
	float magnitude() const;
	vec3 normalized() const;
    void normalize();
    std::string string() const;

    static vec3 zero();

    float x, y, z;
};


bool operator==(const vec3 & a, const vec3 & b);

vec3 add(const vec3 & a, const vec3 & b);
vec3 operator+(const vec3 & a, const vec3 & b);

vec3 subtract(const vec3 & a, const vec3 & b);
vec3 operator-(const vec3 & a, const vec3 & b);

vec3 scale(const vec3 & a, float s);
vec3 operator*(const vec3 & a, float s);
vec3 operator*(float s, const vec3 & a);

float dot(const vec3 & a, const vec3 & b);
float clampedDot(const vec3 & a, const vec3 & b);

vec3 cross(const vec3 & a, const vec3 & b);

vec3 blend(const vec3 & a, float s, const vec3 & b, float t);
vec3 mirror(const vec3 & a, const vec3 & n);
vec3 refract(const vec3 & a, const vec3 & n, float n1, float n2);
vec3 interp(const vec3 & a, const vec3 & b, const float alpha);

#endif
