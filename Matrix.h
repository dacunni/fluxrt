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
class AffineMatrix 
{
public:
	AffineMatrix() {}
	AffineMatrix(float d00, float d01, float d02, float d03,
                 float d10, float d11, float d12, float d13,
                 float d20, float d21, float d22, float d23);
	~AffineMatrix() {}
	
	inline float & at(int r, int c) { return data[r * 4 + c]; }
	inline float at(int r, int c) const { return data[r * 4 + c]; }
    
	void identity();
	
    void glProjectionSymmetric(float width, float height, float near, float far);
    void glProjection(float left, float right, 
                       float bottom, float top, 
                       float near, float far);

	void print() const;
    std::string toJSON() const;
	
	float data[12];
};

// Computes the matrix R = A * B
void      mult(const AffineMatrix & A, const AffineMatrix & B, AffineMatrix & R);
AffineMatrix mult(const AffineMatrix & A, const AffineMatrix & B);

// Multiplies a matrix by a vector. Assumes that matrices multiply on the left of vectors.
// r = A * v
vec4 mult(const AffineMatrix & A, const vec4 & v);
Position3 mult(const AffineMatrix & A, const Position3 & p);
Direction3 mult(const AffineMatrix & A, const Direction3 & d);

void mult(const AffineMatrix & A, const vec4 & v, vec4 & r);
void mult(const AffineMatrix & A, const Position3 & p, Position3 & r);
void mult(const AffineMatrix & A, const Direction3 & d, Direction3 & r);

void scale(const AffineMatrix & A, float s, AffineMatrix & R);
AffineMatrix scale(const AffineMatrix & A, float s);

void inverse(const AffineMatrix & A, AffineMatrix & R);
AffineMatrix inverse(const AffineMatrix & A);

// Transformations
AffineMatrix makeTranslationAffine(const vec3 & d);
AffineMatrix makeScaleAffine(float xs, float ys, float zs);
AffineMatrix makeRotationAffine(const vec3 & axis, float angle);
AffineMatrix makeRotationAffine(float angle, const vec3 & axis);


#endif
