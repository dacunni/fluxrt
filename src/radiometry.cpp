#include <cmath>
#include "radiometry.h"
#include "constants.h"

using namespace constants;

Irradiance irradianceAtDistanceFromPointLight(Power power, float radius)
{
    return power / (4.0 * PI * radius * radius);
}

