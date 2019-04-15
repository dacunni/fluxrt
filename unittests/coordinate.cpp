#include <gtest/gtest.h>
#include <cmath>
#include "coordinate.h"

namespace {

// Euclidean <-> Polar unit axes
//
TEST(CoordinateTransformTest, EuclideanToPolar_UnitAxes) {
    float theta = 0.0f, phi = 0.0f, radius = 0.0f;

    coordinate::euclideanToPolar(1, 0, 0, theta, phi, radius);
    EXPECT_FLOAT_EQ(theta,  0.5 * M_PI);
    EXPECT_FLOAT_EQ(phi,    0.0f);
    EXPECT_FLOAT_EQ(radius, 1.0f);

    coordinate::euclideanToPolar(0, 1, 0, theta, phi, radius);
    EXPECT_FLOAT_EQ(theta,  0.5 * M_PI);
    EXPECT_FLOAT_EQ(phi,    0.5 * M_PI);
    EXPECT_FLOAT_EQ(radius, 1.0f);

    coordinate::euclideanToPolar(-1, 0, 0, theta, phi, radius);
    EXPECT_FLOAT_EQ(theta,  0.5 * M_PI);
    EXPECT_FLOAT_EQ(phi,    M_PI);
    EXPECT_FLOAT_EQ(radius, 1.0f);

    coordinate::euclideanToPolar(0, -1, 0, theta, phi, radius);
    EXPECT_FLOAT_EQ(theta,  0.5 * M_PI);
    EXPECT_FLOAT_EQ(phi,   -0.5 * M_PI);
    EXPECT_FLOAT_EQ(radius, 1.0f);

    coordinate::euclideanToPolar(0, 0, 1, theta, phi, radius);
    EXPECT_FLOAT_EQ(theta,  0.0);
    //EXPECT_FLOAT_EQ(phi,    0.0); // degenerate
    EXPECT_FLOAT_EQ(radius, 1.0f);

    coordinate::euclideanToPolar(0, 0, -1, theta, phi, radius);
    EXPECT_FLOAT_EQ(theta,  M_PI);
    //EXPECT_FLOAT_EQ(phi,    0.0); // degenerate
    EXPECT_FLOAT_EQ(radius, 1.0f);
}

TEST(CoordinateTransformTest, PolarToEuclidean_UnitAxes) {
    float x = 0.0f, y = 0.0f, z = 0.0f;

    const float closeness = 1.0e-6;

    coordinate::polarToEuclidean(0.5 * M_PI, 0.0f, 1.0f, x, y, z);
    EXPECT_NEAR(x, 1.0f, closeness);
    EXPECT_NEAR(y, 0.0f, closeness);
    EXPECT_NEAR(z, 0.0f, closeness);

    coordinate::polarToEuclidean(0.5 * M_PI, 0.5 * M_PI, 1.0f, x, y, z);
    EXPECT_NEAR(x, 0.0f, closeness);
    EXPECT_NEAR(y, 1.0f, closeness);
    EXPECT_NEAR(z, 0.0f, closeness);

    coordinate::polarToEuclidean(0.5 * M_PI, M_PI, 1.0f, x, y, z);
    EXPECT_NEAR(x, -1.0f, closeness);
    EXPECT_NEAR(y, 0.0f, closeness);
    EXPECT_NEAR(z, 0.0f, closeness);

    coordinate::polarToEuclidean(0.5 * M_PI, 1.5 * M_PI, 1.0f, x, y, z);
    EXPECT_NEAR(x, 0.0f, closeness);
    EXPECT_NEAR(y, -1.0f, closeness);
    EXPECT_NEAR(z, 0.0f, closeness);

    coordinate::polarToEuclidean(0.0, 0.0f, 1.0f, x, y, z);
    //EXPECT_NEAR(x, 0.0f, closeness); // degenerate
    //EXPECT_NEAR(y, 0.0f, closeness); // degenerate
    EXPECT_NEAR(z, 1.0f, closeness);

    coordinate::polarToEuclidean(M_PI, 0.0f, 1.0f, x, y, z);
    //EXPECT_NEAR(x, 0.0f, closeness); // degenerate
    //EXPECT_NEAR(y, 0.0f, closeness); // degenerate
    EXPECT_NEAR(z, -1.0f, closeness);
}

// Euclidean <-> Polar scaled axes
//
TEST(CoordinateTransformTest, EuclideanToPolar_ScaledAxes) {
    float theta = 0.0f, phi = 0.0f, radius = 0.0f;
    const float R = 7.5f;

    coordinate::euclideanToPolar(R, 0, 0, theta, phi, radius);
    EXPECT_FLOAT_EQ(theta,  0.5 * M_PI);
    EXPECT_FLOAT_EQ(phi,    0.0f);
    EXPECT_FLOAT_EQ(radius, R);

    coordinate::euclideanToPolar(0, R, 0, theta, phi, radius);
    EXPECT_FLOAT_EQ(theta,  0.5 * M_PI);
    EXPECT_FLOAT_EQ(phi,    0.5 * M_PI);
    EXPECT_FLOAT_EQ(radius, R);

    coordinate::euclideanToPolar(-R, 0, 0, theta, phi, radius);
    EXPECT_FLOAT_EQ(theta,  0.5 * M_PI);
    EXPECT_FLOAT_EQ(phi,    M_PI);
    EXPECT_FLOAT_EQ(radius, R);

    coordinate::euclideanToPolar(0, -R, 0, theta, phi, radius);
    EXPECT_FLOAT_EQ(theta,  0.5 * M_PI);
    EXPECT_FLOAT_EQ(phi,   -0.5 * M_PI);
    EXPECT_FLOAT_EQ(radius, R);

    coordinate::euclideanToPolar(0, 0, R, theta, phi, radius);
    EXPECT_FLOAT_EQ(theta,  0.0);
    //EXPECT_FLOAT_EQ(phi,    0.0); // degenerate
    EXPECT_FLOAT_EQ(radius, R);

    coordinate::euclideanToPolar(0, 0, -R, theta, phi, radius);
    EXPECT_FLOAT_EQ(theta,  M_PI);
    //EXPECT_FLOAT_EQ(phi,    0.0); // degenerate
    EXPECT_FLOAT_EQ(radius, R);
}

TEST(CoordinateTransformTest, PolarToEuclidean_ScaledAxes) {
    float x = 0.0f, y = 0.0f, z = 0.0f;

    const float closeness = 1.0e-6;
    const float R = 8.2;

    coordinate::polarToEuclidean(0.5 * M_PI, 0.0f, R, x, y, z);
    EXPECT_NEAR(x, R, closeness);
    EXPECT_NEAR(y, 0.0f, closeness);
    EXPECT_NEAR(z, 0.0f, closeness);

    coordinate::polarToEuclidean(0.5 * M_PI, 0.5 * M_PI, R, x, y, z);
    EXPECT_NEAR(x, 0.0f, closeness);
    EXPECT_NEAR(y, R, closeness);
    EXPECT_NEAR(z, 0.0f, closeness);

    coordinate::polarToEuclidean(0.5 * M_PI, M_PI, R, x, y, z);
    EXPECT_NEAR(x, -R, closeness);
    EXPECT_NEAR(y, 0.0f, closeness);
    EXPECT_NEAR(z, 0.0f, closeness);

    coordinate::polarToEuclidean(0.5 * M_PI, 1.5 * M_PI, R, x, y, z);
    EXPECT_NEAR(x, 0.0f, closeness);
    EXPECT_NEAR(y, -R, closeness);
    EXPECT_NEAR(z, 0.0f, closeness);

    coordinate::polarToEuclidean(0.0, 0.0f, R, x, y, z);
    //EXPECT_NEAR(x, 0.0f, closeness); // degenerate
    //EXPECT_NEAR(y, 0.0f, closeness); // degenerate
    EXPECT_NEAR(z, R, closeness);

    coordinate::polarToEuclidean(M_PI, 0.0f, R, x, y, z);
    //EXPECT_NEAR(x, 0.0f, closeness); // degenerate
    //EXPECT_NEAR(y, 0.0f, closeness); // degenerate
    EXPECT_NEAR(z, -R, closeness);
}

} // namespace

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

