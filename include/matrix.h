#ifndef __AFFINE_MATRIX_H__
#define __AFFINE_MATRIX_H__

#include "vectortypes.h"
#include "vec3.h"
#include "vec4.h"

//
// 4x4 affine matrix. Always assumed to be of the form
//
//  [ R T ]
//  [ 0 1 ]
//
//  Where R is a rotation matrix, and T is a translation vector.
//  The last row is always implied
//
struct AffineMatrix 
{
	inline AffineMatrix() = default;
	inline AffineMatrix(float d00, float d01, float d02, float d03,
                        float d10, float d11, float d12, float d13,
                        float d20, float d21, float d22, float d23);
	inline ~AffineMatrix() = default;

    // Special transform factories
    static inline AffineMatrix translation(const vec3 & d);
    static inline AffineMatrix scale(float xs, float ys, float zs);
    static inline AffineMatrix rotation(const vec3 & axis, float angle);
    static inline AffineMatrix rotation(float angle, const vec3 & axis);
	
	void identity();

    std::string string() const;
	
	inline float & at(int r, int c) { return data[r * 4 + c]; }
	inline float at(int r, int c) const { return data[r * 4 + c]; }
    
	float data[12];
};

// Computes the matrix R = A * B
inline void mult(const AffineMatrix & A, const AffineMatrix & B, AffineMatrix & R);
inline AffineMatrix mult(const AffineMatrix & A, const AffineMatrix & B);

// Multiplies a matrix by a vector. Assumes that matrices multiply on the left of vectors.
// r = A * v
inline vec4 mult(const AffineMatrix & A, const vec4 & v);

inline Position3 mult(const AffineMatrix & A, const Position3 & p);
inline Position3 operator*(const AffineMatrix & A, const Position3 & p) { return mult(A, p); }

inline Direction3 mult(const AffineMatrix & A, const Direction3 & d);
inline Direction3 operator*(const AffineMatrix & A, const Direction3 & d) { return mult(A, d); }

// Multiplies the transpose of the matrix by the direction
inline Direction3 multTranspose(const AffineMatrix & A, const Direction3 & d);

inline void mult(const AffineMatrix & A, const vec4 & v, vec4 & r);
inline void mult(const AffineMatrix & A, const Position3 & p, Position3 & r);
inline void mult(const AffineMatrix & A, const Direction3 & d, Direction3 & r);

inline void scale(const AffineMatrix & A, float s, AffineMatrix & R);
inline AffineMatrix scale(const AffineMatrix & A, float s);

inline void inverse(const AffineMatrix & A, AffineMatrix & R);
inline AffineMatrix inverse(const AffineMatrix & A);

#include "matrix.hpp"

#endif
