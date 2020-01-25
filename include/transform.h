#ifndef __TRANSFORM_H__
#define __TRANSFORM_H__

#include <functional>
#include "matrix.h"

class Transform
{
public:
    inline Transform();
    inline Transform(const AffineMatrix & f, const AffineMatrix & r);
    virtual ~Transform() = default;

    // Special transform factories
    static inline Transform translation(const vec3 & d);
    static inline Transform scale(float xs, float ys, float zs);
    static inline Transform rotation(const vec3 & axis, float angle);
    static inline Transform rotation(float angle, const vec3 & axis);

    virtual void updateAnim(float t) {}
    
    // Returns a new Transform that is the inverse of this Transform
    Transform inverse() const { return Transform(rev, fwd); }
        
    // Inverts this Transform
    void invert() { auto r = rev; rev = fwd; fwd = r; }

    // Members
    AffineMatrix fwd;      // Forward transformation
    AffineMatrix rev;      // Reverse transformation
};

// Composes multiple Transforms into one. Last transform is applied first.
inline Transform compose(const Transform & t1);
inline Transform compose(const Transform & t1, const Transform & t2);

// Variadic expansion of compose() to N transforms
template<typename... Args>
inline Transform compose(const Transform & t1, Args... args) {
    return compose(t1, compose(args...));
}

#include "transform.hpp"

#endif
