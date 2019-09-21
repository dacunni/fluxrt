
inline vec2 vec2::normalized() const
{
    float magsq = magnitude_sq();
    if(magsq != 0.0f) {
        float mag = std::sqrt(magsq);
        float invmag = 1.0f / mag;
        return vec2(x * invmag, y * invmag);
    }
    return *this;
}

inline vec2 blend(const vec2 & a, float s, const vec2 & b, float t)
{
    return add(scale(a, s), scale(b, t));
}

inline vec2 interp(const vec2 & a, const vec2 & b, const float alpha)
{
    const float oma = 1.0f - alpha;
    return vec2(oma * a.x + alpha * b.x,
                oma * a.y + alpha * b.y);
}

