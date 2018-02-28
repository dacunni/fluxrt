#ifndef __position3_H__
#define __position3_H__

#include "vec3.h"

struct Position3 : public vec3
{
    explicit Position3() : vec3() {}
    explicit Position3(const vec3 & a) : vec3(a) {}
	explicit Position3(float xn, float yn, float zn) : vec3(xn, yn, zn) {}
};

struct Direction3 : public vec3
{
    explicit Direction3() : vec3() {}
    explicit Direction3(const vec3 & a) : vec3(a) {}
	explicit Direction3(float xn, float yn, float zn) : vec3(xn, yn, zn) {}
};

#if 0
static Position3 foo(vec3 a) {
    return Position3(a);
}

static Position3 bar(Direction3 a) {
    //return a;
    return Position3(a);
}

static Direction3 bar(Position3 a) {
    //return a;
    return Direction3(a);
}
#endif

#if 0
static Position3 foo(const Position3 & a, const Position3 & b) {
    return Position3(add(a, b));
}

static Direction3 foo(const Direction3 & a, const Direction3 & b) {
    return Direction3(add(a, b));
}
#endif


#endif
