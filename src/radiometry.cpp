#include <cmath>
#include "radiometry.h"

namespace radiometry {

Irradiance irradianceAtDistanceFromPointLight(Power power, float radius)
{
    return power / (4.0 * M_PI * radius * radius);
}

}; // radiometry

