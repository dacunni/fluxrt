inline Transform::Transform()
{
    fwd.identity();
    rev.identity();
}

inline Transform::Transform(const AffineMatrix & f, const AffineMatrix & r)
: fwd(f), rev(r)
{ }

Transform Transform::translation(const vec3 & d)
{
    return Transform(AffineMatrix::translation(d),
                     AffineMatrix::translation(-d));
}

Transform Transform::scale(float xs, float ys, float zs)
{
    return Transform(AffineMatrix::scale(xs, ys, zs),
                     AffineMatrix::scale(1.0f/xs, 1.0f/ys, 1.0f/zs));
}

Transform Transform::scale(float s)
{
    return Transform(AffineMatrix::scale(s),
                     AffineMatrix::scale(1.0f/s));
}

Transform Transform::rotation(const vec3 & axis, float angle)
{
    return Transform(AffineMatrix::rotation(angle, axis),
                     AffineMatrix::rotation(-angle, axis));
}

Transform Transform::rotation(float angle, const vec3 & axis)
{
    return rotation(axis, angle);
}

Transform compose(const Transform & t1)
{
    return t1;
}

Transform compose(const Transform & t1, const Transform & t2)
{
    Transform c;
    mult(t1.fwd, t2.fwd, c.fwd);
    mult(t2.rev, t1.rev, c.rev);
    return c;
}

