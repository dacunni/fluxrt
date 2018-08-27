
inline float vec4::magnitude_sq() const
{
    if(w == 0.0f) return sq(x) + sq(y) + sq(z);
    else return sq(x/w) + sq(y/w) + sq(z/w);
}

inline float vec4::magnitude() const
{
    return std::sqrt(magnitude_sq());
}

inline void vec4::normalize()
{
    float magsq = magnitude_sq();
    if(magsq != 0.0f) {
        float mag = std::sqrt(magsq);
        float invmag = 1.0f / mag;
        x *= invmag; y *= invmag; z *= invmag;
    }
}

inline vec4 vec4::normalized() const
{
    float magsq = magnitude_sq();
    if(magsq != 0.0f) {
        float mag = std::sqrt(magsq);
        float invmag = 1.0f / mag;
        return vec4(x * invmag, y * invmag, z * invmag, w);
    }
    return *this;
}

inline vec4 scale(const vec4 & a, float s)
{
    return vec4(a.x * s, a.y * s, a.z * s, a.w);
}

