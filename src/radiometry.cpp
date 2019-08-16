#include <cmath>
#include "radiometry.h"
#include "constants.h"

namespace radiometry {

using namespace constants;

Irradiance irradianceAtDistanceFromPointLight(Power power, float radius)
{
    return power / (4.0 * PI * radius * radius);
}

}; // radiometry

