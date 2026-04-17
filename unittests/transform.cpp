#include <gtest/gtest.h>
#include <cmath>
#include "transform.h"
#include "vec3.h"

namespace {

// Helper function for comparing floats with tolerance
constexpr float EPSILON = 1e-5f;

bool floatEquals(float a, float b, float epsilon = EPSILON) {
    return std::abs(a - b) < epsilon;
}

// Helper function to check if a matrix is approximately identity
bool isIdentity(const AffineMatrix & m) {
    // Identity matrix for 3x4 affine:
    // [1 0 0 0]
    // [0 1 0 0]
    // [0 0 1 0]
    return floatEquals(m.at(0, 0), 1.0f) && floatEquals(m.at(0, 1), 0.0f) && floatEquals(m.at(0, 2), 0.0f) && floatEquals(m.at(0, 3), 0.0f)
        && floatEquals(m.at(1, 0), 0.0f) && floatEquals(m.at(1, 1), 1.0f) && floatEquals(m.at(1, 2), 0.0f) && floatEquals(m.at(1, 3), 0.0f)
        && floatEquals(m.at(2, 0), 0.0f) && floatEquals(m.at(2, 1), 0.0f) && floatEquals(m.at(2, 2), 1.0f) && floatEquals(m.at(2, 3), 0.0f);
}

// Helper function to check if rotation matrix is orthogonal (R^T * R = I)
bool isOrthogonal(const AffineMatrix & m) {
    // For a 3x3 rotation part, check that R^T * R = I
    // We'll do this by checking that columns are unit vectors and perpendicular
    for(int i = 0; i < 3; i++) {
        // Check column i is unit vector
        float col_sq = m.at(0, i) * m.at(0, i) + m.at(1, i) * m.at(1, i) + m.at(2, i) * m.at(2, i);
        if(!floatEquals(col_sq, 1.0f)) return false;
    }

    // Check columns are perpendicular
    for(int i = 0; i < 3; i++) {
        for(int j = i+1; j < 3; j++) {
            float dot = m.at(0, i) * m.at(0, j) + m.at(1, i) * m.at(1, j) + m.at(2, i) * m.at(2, j);
            if(!floatEquals(dot, 0.0f)) return false;
        }
    }
    return true;
}

// Helper function to check if matrices are approximately equal
bool matrixEquals(const AffineMatrix & a, const AffineMatrix & b) {
    for(int i = 0; i < 3; i++) {
        for(int j = 0; j < 4; j++) {
            if(!floatEquals(a.at(i, j), b.at(i, j))) return false;
        }
    }
    return true;
}

// ============================================================================
// DEFAULT CONSTRUCTION
// ============================================================================

TEST(TransformTest, DefaultConstruction) {
    Transform t;
    EXPECT_TRUE(isIdentity(t.fwd));
    EXPECT_TRUE(isIdentity(t.rev));
}

// ============================================================================
// EXPLICIT CONSTRUCTION
// ============================================================================

TEST(TransformTest, ExplicitConstruction) {
    AffineMatrix fwd;
    fwd.at(0, 0) = 2.0f; fwd.at(0, 1) = 0.0f; fwd.at(0, 2) = 0.0f; fwd.at(0, 3) = 5.0f;
    fwd.at(1, 0) = 0.0f; fwd.at(1, 1) = 3.0f; fwd.at(1, 2) = 0.0f; fwd.at(1, 3) = 6.0f;
    fwd.at(2, 0) = 0.0f; fwd.at(2, 1) = 0.0f; fwd.at(2, 2) = 4.0f; fwd.at(2, 3) = 7.0f;

    AffineMatrix rev;
    rev.at(0, 0) = 0.5f; rev.at(0, 1) = 0.0f; rev.at(0, 2) = 0.0f; rev.at(0, 3) = -2.5f;
    rev.at(1, 0) = 0.0f; rev.at(1, 1) = 0.333f; rev.at(1, 2) = 0.0f; rev.at(1, 3) = -2.0f;
    rev.at(2, 0) = 0.0f; rev.at(2, 1) = 0.0f; rev.at(2, 2) = 0.25f; rev.at(2, 3) = -1.75f;

    Transform t(fwd, rev);
    EXPECT_TRUE(matrixEquals(t.fwd, fwd));
    EXPECT_TRUE(matrixEquals(t.rev, rev));
}

// ============================================================================
// TRANSLATION FACTORY
// ============================================================================

TEST(TransformTest, TranslationFactory) {
    vec3 d(3.0f, 4.0f, 5.0f);
    Transform t = Transform::translation(d);

    // Forward should translate by d
    EXPECT_TRUE(floatEquals(t.fwd.at(0, 3), 3.0f));
    EXPECT_TRUE(floatEquals(t.fwd.at(1, 3), 4.0f));
    EXPECT_TRUE(floatEquals(t.fwd.at(2, 3), 5.0f));

    // Rotation part should be identity
    EXPECT_TRUE(floatEquals(t.fwd.at(0, 0), 1.0f) && floatEquals(t.fwd.at(1, 1), 1.0f) && floatEquals(t.fwd.at(2, 2), 1.0f));

    // Reverse should translate by -d
    EXPECT_TRUE(floatEquals(t.rev.at(0, 3), -3.0f));
    EXPECT_TRUE(floatEquals(t.rev.at(1, 3), -4.0f));
    EXPECT_TRUE(floatEquals(t.rev.at(2, 3), -5.0f));
}

TEST(TransformTest, TranslationPointTransform) {
    Transform t = Transform::translation(vec3(2.0f, 3.0f, 4.0f));
    Position3 p(1.0f, 1.0f, 1.0f);
    Position3 result = t.fwd * p;

    EXPECT_TRUE(floatEquals(result.x, 3.0f));
    EXPECT_TRUE(floatEquals(result.y, 4.0f));
    EXPECT_TRUE(floatEquals(result.z, 5.0f));
}

// ============================================================================
// SCALE FACTORY
// ============================================================================

TEST(TransformTest, UniformScale) {
    Transform t = Transform::scale(2.0f);

    // Forward should scale by 2
    EXPECT_TRUE(floatEquals(t.fwd.at(0, 0), 2.0f));
    EXPECT_TRUE(floatEquals(t.fwd.at(1, 1), 2.0f));
    EXPECT_TRUE(floatEquals(t.fwd.at(2, 2), 2.0f));

    // Reverse should scale by 0.5
    EXPECT_TRUE(floatEquals(t.rev.at(0, 0), 0.5f));
    EXPECT_TRUE(floatEquals(t.rev.at(1, 1), 0.5f));
    EXPECT_TRUE(floatEquals(t.rev.at(2, 2), 0.5f));
}

TEST(TransformTest, NonUniformScale) {
    Transform t = Transform::scale(2.0f, 3.0f, 4.0f);

    // Forward should scale by (2, 3, 4)
    EXPECT_TRUE(floatEquals(t.fwd.at(0, 0), 2.0f));
    EXPECT_TRUE(floatEquals(t.fwd.at(1, 1), 3.0f));
    EXPECT_TRUE(floatEquals(t.fwd.at(2, 2), 4.0f));

    // Reverse should scale by (0.5, 1/3, 0.25)
    EXPECT_TRUE(floatEquals(t.rev.at(0, 0), 0.5f));
    EXPECT_TRUE(floatEquals(t.rev.at(1, 1), 1.0f/3.0f));
    EXPECT_TRUE(floatEquals(t.rev.at(2, 2), 0.25f));
}

TEST(TransformTest, ScalePointTransform) {
    Transform t = Transform::scale(2.0f, 3.0f, 4.0f);
    Position3 p(1.0f, 1.0f, 1.0f);
    Position3 result = t.fwd * p;

    EXPECT_TRUE(floatEquals(result.x, 2.0f));
    EXPECT_TRUE(floatEquals(result.y, 3.0f));
    EXPECT_TRUE(floatEquals(result.z, 4.0f));
}

// ============================================================================
// ROTATION FACTORY
// ============================================================================

TEST(TransformTest, RotationFactory90DegreesX) {
    // 90-degree rotation around X axis
    Transform t = Transform::rotation(vec3(1.0f, 0.0f, 0.0f), M_PI_2);

    // Rotation part should be orthogonal
    EXPECT_TRUE(isOrthogonal(t.fwd));
    EXPECT_TRUE(isOrthogonal(t.rev));
}

TEST(TransformTest, RotationFactory90DegreesY) {
    // 90-degree rotation around Y axis
    Transform t = Transform::rotation(vec3(0.0f, 1.0f, 0.0f), M_PI_2);

    // Rotation part should be orthogonal
    EXPECT_TRUE(isOrthogonal(t.fwd));
    EXPECT_TRUE(isOrthogonal(t.rev));
}

TEST(TransformTest, RotationFactory90DegreesZ) {
    // 90-degree rotation around Z axis
    Transform t = Transform::rotation(vec3(0.0f, 0.0f, 1.0f), M_PI_2);

    // Rotation part should be orthogonal
    EXPECT_TRUE(isOrthogonal(t.fwd));
    EXPECT_TRUE(isOrthogonal(t.rev));
}

TEST(TransformTest, RotationAlternativeSignature) {
    // Test both signatures produce the same result
    Transform t1 = Transform::rotation(vec3(1.0f, 0.0f, 0.0f), M_PI_4);
    Transform t2 = Transform::rotation(M_PI_4, vec3(1.0f, 0.0f, 0.0f));

    EXPECT_TRUE(matrixEquals(t1.fwd, t2.fwd));
    EXPECT_TRUE(matrixEquals(t1.rev, t2.rev));
}

TEST(TransformTest, RotationDirectionTransform) {
    // Rotation should preserve direction magnitude
    Transform t = Transform::rotation(vec3(0.0f, 0.0f, 1.0f), M_PI_2);
    Direction3 d(1.0f, 0.0f, 0.0f);
    Direction3 result = t.fwd * d;

    // After 90-degree rotation around Z, (1,0,0) should map to approximately (0,1,0)
    EXPECT_TRUE(floatEquals(result.x, 0.0f));
    EXPECT_TRUE(floatEquals(result.y, 1.0f));
    EXPECT_TRUE(floatEquals(result.z, 0.0f));
}

// ============================================================================
// INVERSE METHOD (returns new Transform)
// ============================================================================

TEST(TransformTest, InverseReturnsNewTransform) {
    Transform t = Transform::translation(vec3(3.0f, 4.0f, 5.0f));
    Transform inv = t.inverse();

    // Original should be unchanged
    EXPECT_TRUE(floatEquals(t.fwd.at(0, 3), 3.0f));
    EXPECT_TRUE(floatEquals(t.fwd.at(1, 3), 4.0f));
    EXPECT_TRUE(floatEquals(t.fwd.at(2, 3), 5.0f));

    // Inverse should have swapped fwd/rev
    EXPECT_TRUE(matrixEquals(inv.fwd, t.rev));
    EXPECT_TRUE(matrixEquals(inv.rev, t.fwd));
}

// ============================================================================
// INVERT METHOD (in-place swap)
// ============================================================================

TEST(TransformTest, InvertInPlace) {
    Transform t = Transform::translation(vec3(3.0f, 4.0f, 5.0f));
    AffineMatrix originalFwd = t.fwd;
    AffineMatrix originalRev = t.rev;

    t.invert();

    // After invert, fwd and rev should be swapped
    EXPECT_TRUE(matrixEquals(t.fwd, originalRev));
    EXPECT_TRUE(matrixEquals(t.rev, originalFwd));
}

// ============================================================================
// ROUND-TRIP: T * T.inverse() ≈ Identity
// ============================================================================

TEST(TransformTest, RoundTripTranslation) {
    Transform t = Transform::translation(vec3(3.5f, 4.2f, 5.1f));
    Transform inv = t.inverse();
    Transform identity = compose(t, inv);

    EXPECT_TRUE(isIdentity(identity.fwd));
    EXPECT_TRUE(isIdentity(identity.rev));
}

TEST(TransformTest, RoundTripUniformScale) {
    Transform t = Transform::scale(2.5f);
    Transform inv = t.inverse();
    Transform identity = compose(t, inv);

    EXPECT_TRUE(isIdentity(identity.fwd));
    EXPECT_TRUE(isIdentity(identity.rev));
}

TEST(TransformTest, RoundTripNonUniformScale) {
    Transform t = Transform::scale(2.0f, 3.0f, 4.0f);
    Transform inv = t.inverse();
    Transform identity = compose(t, inv);

    EXPECT_TRUE(isIdentity(identity.fwd));
    EXPECT_TRUE(isIdentity(identity.rev));
}

TEST(TransformTest, RoundTripRotation) {
    Transform t = Transform::rotation(vec3(1.0f, 1.0f, 1.0f).normalized(), M_PI / 3.0f);
    Transform inv = t.inverse();
    Transform identity = compose(t, inv);

    EXPECT_TRUE(isIdentity(identity.fwd));
    EXPECT_TRUE(isIdentity(identity.rev));
}

// ============================================================================
// COMPOSITION - 2 TRANSFORMS
// ============================================================================

TEST(TransformTest, ComposeIdentity) {
    Transform t1;
    Transform t2;
    Transform result = compose(t1, t2);

    EXPECT_TRUE(isIdentity(result.fwd));
    EXPECT_TRUE(isIdentity(result.rev));
}

TEST(TransformTest, ComposeWithIdentity) {
    Transform translation = Transform::translation(vec3(2.0f, 3.0f, 4.0f));
    Transform identity;

    Transform result = compose(translation, identity);
    EXPECT_TRUE(matrixEquals(result.fwd, translation.fwd));
    EXPECT_TRUE(matrixEquals(result.rev, translation.rev));
}

TEST(TransformTest, ComposeOrder) {
    // Test that compose(T1, T2) applies T2 first, then T1
    // Transform a point through T2, then T1
    Transform t1 = Transform::translation(vec3(1.0f, 0.0f, 0.0f));
    Transform t2 = Transform::scale(2.0f);

    Transform composed = compose(t1, t2);

    // Apply to point (1, 0, 0)
    // First: scale by 2 -> (2, 0, 0)
    // Then: translate by (1, 0, 0) -> (3, 0, 0)
    Position3 p(1.0f, 0.0f, 0.0f);
    Position3 result = composed.fwd * p;

    EXPECT_TRUE(floatEquals(result.x, 3.0f));
    EXPECT_TRUE(floatEquals(result.y, 0.0f));
    EXPECT_TRUE(floatEquals(result.z, 0.0f));
}

TEST(TransformTest, ComposeTranslationAndScale) {
    Transform translation = Transform::translation(vec3(5.0f, 0.0f, 0.0f));
    Transform scale = Transform::scale(2.0f);

    Transform composed = compose(translation, scale);

    Position3 p(1.0f, 1.0f, 1.0f);
    Position3 result = composed.fwd * p;

    // First scale: (1, 1, 1) * 2 = (2, 2, 2)
    // Then translate: (2, 2, 2) + (5, 0, 0) = (7, 2, 2)
    EXPECT_TRUE(floatEquals(result.x, 7.0f));
    EXPECT_TRUE(floatEquals(result.y, 2.0f));
    EXPECT_TRUE(floatEquals(result.z, 2.0f));
}

// ============================================================================
// COMPOSITION - SINGLE ARGUMENT
// ============================================================================

TEST(TransformTest, ComposeSingleArgument) {
    Transform t = Transform::translation(vec3(1.0f, 2.0f, 3.0f));
    Transform result = compose(t);

    EXPECT_TRUE(matrixEquals(result.fwd, t.fwd));
    EXPECT_TRUE(matrixEquals(result.rev, t.rev));
}

// ============================================================================
// COMPOSITION - VARIADIC (3+ TRANSFORMS)
// ============================================================================

TEST(TransformTest, ComposeThreeTransforms) {
    Transform t1 = Transform::translation(vec3(1.0f, 0.0f, 0.0f));
    Transform t2 = Transform::scale(2.0f);
    Transform t3 = Transform::translation(vec3(0.0f, 1.0f, 0.0f));

    Transform composed = compose(t1, t2, t3);

    // Apply in order: t3, then t2, then t1
    // (1, 0, 0) -> translate by (0, 1, 0) -> (1, 1, 0)
    // (1, 1, 0) -> scale by 2 -> (2, 2, 0)
    // (2, 2, 0) -> translate by (1, 0, 0) -> (3, 2, 0)
    Position3 p(1.0f, 0.0f, 0.0f);
    Position3 result = composed.fwd * p;

    EXPECT_TRUE(floatEquals(result.x, 3.0f));
    EXPECT_TRUE(floatEquals(result.y, 2.0f));
    EXPECT_TRUE(floatEquals(result.z, 0.0f));
}

TEST(TransformTest, ComposeFourTransforms) {
    Transform t1 = Transform::translation(vec3(1.0f, 0.0f, 0.0f));
    Transform t2 = Transform::scale(2.0f);
    Transform t3 = Transform::translation(vec3(0.0f, 1.0f, 0.0f));
    Transform t4 = Transform::scale(0.5f);

    Transform composed = compose(t1, t2, t3, t4);

    // Apply: t4, t3, t2, t1
    // (2, 0, 0) -> scale 0.5 -> (1, 0, 0)
    // (1, 0, 0) -> translate (0, 1, 0) -> (1, 1, 0)
    // (1, 1, 0) -> scale 2 -> (2, 2, 0)
    // (2, 2, 0) -> translate (1, 0, 0) -> (3, 2, 0)
    Position3 p(2.0f, 0.0f, 0.0f);
    Position3 result = composed.fwd * p;

    EXPECT_TRUE(floatEquals(result.x, 3.0f));
    EXPECT_TRUE(floatEquals(result.y, 2.0f));
    EXPECT_TRUE(floatEquals(result.z, 0.0f));
}

// ============================================================================
// NORMAL TRANSFORMATION (inverse-transpose via rev)
// ============================================================================

TEST(TransformTest, NormalTransformationUniformScale) {
    // Uniform scale inversely affects normals to maintain perpendicularity
    Transform t = Transform::scale(2.0f);
    Direction3 normal(1.0f, 0.0f, 0.0f);
    Direction3 result = multTranspose(t.rev, normal);

    // Normal is scaled inversely: scale(2) -> normals scaled by 0.5
    EXPECT_TRUE(floatEquals(result.x, 0.5f));
    EXPECT_TRUE(floatEquals(result.y, 0.0f));
    EXPECT_TRUE(floatEquals(result.z, 0.0f));
}

TEST(TransformTest, NormalTransformationNonUniformScale) {
    // Non-uniform scale requires inverse-transpose for normals
    Transform t = Transform::scale(2.0f, 1.0f, 1.0f);
    Direction3 normal(1.0f, 0.0f, 0.0f);
    Direction3 result = multTranspose(t.rev, normal);

    // The rev matrix is the inverse, so multTranspose computes (T^-1)^T * n
    // For scale(2, 1, 1), inverse is scale(0.5, 1, 1)
    // Transpose doesn't change diagonal, so (0.5, 1, 1) * (1, 0, 0) = (0.5, 0, 0)
    EXPECT_TRUE(floatEquals(result.x, 0.5f));
    EXPECT_TRUE(floatEquals(result.y, 0.0f));
    EXPECT_TRUE(floatEquals(result.z, 0.0f));
}

TEST(TransformTest, NormalTransformationRotation) {
    // Pure rotation: normals transform the same as directions
    Transform t = Transform::rotation(vec3(0.0f, 0.0f, 1.0f), M_PI_2);
    Direction3 normal(1.0f, 0.0f, 0.0f);
    Direction3 result = multTranspose(t.rev, normal);

    // 90-degree rotation: (1, 0, 0) -> (0, 1, 0)
    EXPECT_TRUE(floatEquals(result.x, 0.0f));
    EXPECT_TRUE(floatEquals(result.y, 1.0f));
    EXPECT_TRUE(floatEquals(result.z, 0.0f));
}

} // namespace

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
