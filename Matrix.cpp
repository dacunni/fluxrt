#include <math.h>
#include <stdio.h>
#include <sstream>

#include "Matrix.h"

AffineMatrix::AffineMatrix(float d00, float d01, float d02, float d03,
                           float d10, float d11, float d12, float d13,
                           float d20, float d21, float d22, float d23) 
{
    at(0, 0) = d00; at(0, 1) = d01; at(0, 2) = d02; at(0, 3) = d03;
    at(1, 0) = d10; at(1, 1) = d11; at(1, 2) = d12; at(1, 3) = d13;
    at(2, 0) = d20; at(2, 1) = d21; at(2, 2) = d22; at(2, 3) = d23;
}

void AffineMatrix::identity() 
{
    at(0, 0) = 1.0f; at(0, 1) = 0.0f; at(0, 2) = 0.0f; at(0, 3) = 0.0f;
    at(1, 0) = 0.0f; at(1, 1) = 1.0f; at(1, 2) = 0.0f; at(1, 3) = 0.0f;
    at(2, 0) = 0.0f; at(2, 1) = 0.0f; at(2, 2) = 1.0f; at(2, 3) = 0.0f;
}

void AffineMatrix::print() const
{
    const char * fmt = "M%d| %6.6f %6.6f %6.6f %6.6f |\n";
    for(int r = 0; r < 3; r++) {
        printf(fmt, r, at(r, 0), at(r, 1), at(r, 2), at(r, 3));
    }
    printf(fmt, 3, 0.0f, 0.0f, 0.0f, 1.0f); // implicit last row
}

std::string AffineMatrix::toJSON() const
{
    std::stringstream ss;

    ss << "["
        << "[" << at(0,0) << "," << at(0,1) << "," << at(0,2) << "," << at(0,3) << "],"
        << "[" << at(1,0) << "," << at(1,1) << "," << at(1,2) << "," << at(1,3) << "],"
        << "[" << at(2,0) << "," << at(2,1) << "," << at(2,2) << "," << at(2,3) << "],"
        << "[" << 0.0f << "," << 0.0f << "," << 0.0f << "," << 1.0f << "]"
        << "]";

    return ss.str();
}

// TODO - handle R being the same as one of A or B
void mult(const AffineMatrix & A, const AffineMatrix & B, AffineMatrix & R)
{
    const float B_row3[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
    for(int r = 0; r < 3; r++) {
        for(int c = 0; c < 4; c++) {
            R.at(r, c) = A.at(r, 0) * B.at(0, c)
                       + A.at(r, 1) * B.at(1, c)
                       + A.at(r, 2) * B.at(2, c)
                       + A.at(r, 3) * B_row3[c];
        }
    }
}

AffineMatrix mult(const AffineMatrix & A, const AffineMatrix & B)
{
    AffineMatrix R;
    mult(A, B, R);
    return R;
}

vec4 mult(const AffineMatrix & A, const vec4 & v)
{
    vec4 r;
    r.x = A.at(0, 0) * v.x + A.at(0, 1) * v.y + A.at(0, 2) * v.z + A.at(0, 3) * v.w;
    r.y = A.at(1, 0) * v.x + A.at(1, 1) * v.y + A.at(1, 2) * v.z + A.at(1, 3) * v.w;
    r.z = A.at(2, 0) * v.x + A.at(2, 1) * v.y + A.at(2, 2) * v.z + A.at(2, 3) * v.w;
    r.w = v.w;
    return r;
}

Position3 mult(const AffineMatrix & A, const Position3 & p)
{
    Position3 r; // implicit w=1
    r.x = A.at(0, 0) * p.x + A.at(0, 1) * p.y + A.at(0, 2) * p.z + A.at(0, 3);
    r.y = A.at(1, 0) * p.x + A.at(1, 1) * p.y + A.at(1, 2) * p.z + A.at(1, 3);
    r.z = A.at(2, 0) * p.x + A.at(2, 1) * p.y + A.at(2, 2) * p.z + A.at(2, 3);
    return r;
}

Direction3 mult(const AffineMatrix & A, const Direction3 & d)
{
    Direction3 r; // implicit w=0
    r.x = A.at(0, 0) * d.x + A.at(0, 1) * d.y + A.at(0, 2) * d.z;
    r.y = A.at(1, 0) * d.x + A.at(1, 1) * d.y + A.at(1, 2) * d.z;
    r.z = A.at(2, 0) * d.x + A.at(2, 1) * d.y + A.at(2, 2) * d.z;
    return r;
}

void mult(const AffineMatrix & A, const vec4 & v, vec4 & r)
{
    r.x = A.at(0, 0) * v.x + A.at(0, 1) * v.y + A.at(0, 2) * v.z + A.at(0, 3) * v.w;
    r.y = A.at(1, 0) * v.x + A.at(1, 1) * v.y + A.at(1, 2) * v.z + A.at(1, 3) * v.w;
    r.z = A.at(2, 0) * v.x + A.at(2, 1) * v.y + A.at(2, 2) * v.z + A.at(2, 3) * v.w;
    r.w = v.w;
}

void mult(const AffineMatrix & A, const Position3 & p, Position3 & r)
{
    r.x = A.at(0, 0) * p.x + A.at(0, 1) * p.y + A.at(0, 2) * p.z + A.at(0, 3);
    r.y = A.at(1, 0) * p.x + A.at(1, 1) * p.y + A.at(1, 2) * p.z + A.at(1, 3);
    r.z = A.at(2, 0) * p.x + A.at(2, 1) * p.y + A.at(2, 2) * p.z + A.at(2, 3);
    // implicit w=1
}

void mult(const AffineMatrix & A, const Direction3 & d, Direction3 & r)
{
    r.x = A.at(0, 0) * d.x + A.at(0, 1) * d.y + A.at(0, 2) * d.z;
    r.y = A.at(1, 0) * d.x + A.at(1, 1) * d.y + A.at(1, 2) * d.z;
    r.z = A.at(2, 0) * d.x + A.at(2, 1) * d.y + A.at(2, 2) * d.z;
    // implicit w=0
}


//
// Fast matrix inversion 
//
// Based on the fast matrix inversion tutortial here:
//		http://content.gpwiki.org/index.php/MathGem:Fast_Matrix_Inversion
//
void inverse(const AffineMatrix & A, AffineMatrix & R)
{
    // Transpose the 3x3 rotation submatrix
    R.at(0, 0) = A.at(0, 0); R.at(0, 1) = A.at(1, 0); R.at(0, 2) = A.at(2, 0);
    R.at(1, 0) = A.at(0, 1); R.at(1, 1) = A.at(1, 1); R.at(1, 2) = A.at(2, 1);
    R.at(2, 0) = A.at(0, 2); R.at(2, 1) = A.at(1, 2); R.at(2, 2) = A.at(2, 2);

    // Last column translation vector
    //   Each element is the negative of a column vector of the rotation submatrix dotted with the translation vector
    R.at(0, 3) = - (A.at(0, 0) * A.at(0, 3) + A.at(1, 0) * A.at(1, 3) + A.at(2, 0) * A.at(2, 3));
    R.at(1, 3) = - (A.at(0, 1) * A.at(0, 3) + A.at(1, 1) * A.at(1, 3) + A.at(2, 1) * A.at(2, 3));
    R.at(2, 3) = - (A.at(0, 2) * A.at(0, 3) + A.at(1, 2) * A.at(1, 3) + A.at(2, 2) * A.at(2, 3));
}

AffineMatrix inverse(const AffineMatrix & A)
{
    AffineMatrix R;
    inverse(A, R);
    return R;
}

AffineMatrix makeTranslationAffine(const vec3 & d)
{
    return AffineMatrix(1.0, 0.0, 0.0, d.x,
                        0.0, 1.0, 0.0, d.y,
                        0.0, 0.0, 1.0, d.z);
}

AffineMatrix makeScaleAffine(float xs, float ys, float zs)
{
    return AffineMatrix( xs, 0.0, 0.0, 0.0,
                        0.0,  ys, 0.0, 0.0,
                        0.0, 0.0,  zs, 0.0);
}

AffineMatrix makeRotationAffine(const vec3 & axis, float angle)
{
    float ca = std::cos( angle );
    float omca = 1.0f - ca;
    float sa = std::sin( angle );
    auto u = axis.normalized();
    
    return AffineMatrix(      ca + u.x * u.x * omca, u.x * u.y * omca - u.z * sa, u.x * u.z * omca + u.y * sa, 0.0,
                        u.y * u.x * omca + u.z * sa,       ca + u.y * u.y * omca, u.y * u.z * omca - u.x * sa, 0.0,
                        u.z * u.x * omca - u.y * sa, u.z * u.y * omca + u.x * sa,       ca + u.z * u.z * omca, 0.0);
}

AffineMatrix makeRotationAffine(float angle, const vec3 & axis)
{
    return makeRotationAffine(axis, angle);
}


