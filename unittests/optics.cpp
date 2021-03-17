#include <gtest/gtest.h>
#include <cmath>
#include "optics.h"

namespace {

TEST(OpticsSellmeierIor, General3Coeffs) {
    // Glass, BK7 Schott - https://refractiveindex.info/?shelf=glass&book=BK7&page=SCHOTT
    float B[3] = { 1.03961212, 0.231792344, 1.01046945 };
    float C[3] = { 0.00600069867, 0.0200179144, 103.560653 };
    EXPECT_NEAR(1.5392, optics::ior::sellmeier(0.35, B, C, 3), 0.0001);
    EXPECT_NEAR(1.4945, optics::ior::sellmeier(2.0, B, C, 3), 0.0001);
}

} // namespace

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

