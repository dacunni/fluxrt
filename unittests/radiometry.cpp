#include <gtest/gtest.h>
#include <cmath>
#include "radiometry.h"

namespace {

TEST(IrradianceCalculation, IrradianceScalesWithDistance) {
    Power power = 100.0f;
    // typical distances
    {
        float distance1 = 20.0f;
        Irradiance irr1 = irradianceAtDistanceFromPointLight(power, distance1);
        Irradiance irr2 = irradianceAtDistanceFromPointLight(power, 2.0f * distance1);
        EXPECT_NEAR(irr1 / 4.0f, irr2, 0.0001);
    }
    {
        float distance1 = 20.0f;
        Irradiance irr1 = irradianceAtDistanceFromPointLight(power, distance1);
        Irradiance irr2 = irradianceAtDistanceFromPointLight(power, 10.0f * distance1);
        EXPECT_NEAR(irr1 / 100.0f, irr2, 0.0001);
    }
    // far
    {
        float distance1 = 1.0e6f;
        Irradiance irr1 = irradianceAtDistanceFromPointLight(power, distance1);
        Irradiance irr2 = irradianceAtDistanceFromPointLight(power, 2.0f * distance1);
        EXPECT_NEAR(irr1 / 4.0f, irr2, 0.0001);
    }
    // near
    {
        float distance1 = 1.0e-6f;
        Irradiance irr1 = irradianceAtDistanceFromPointLight(power, distance1);
        Irradiance irr2 = irradianceAtDistanceFromPointLight(power, 2.0f * distance1);
        EXPECT_NEAR(irr1 / 4.0f, irr2, 1.0e-10);
    }
}

} // namespace

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

