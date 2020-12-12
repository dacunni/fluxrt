#include "rng.h"

RNG::RNG()
  : engine(device()),
    distribution(0.0f, 1.0f),
    normalDistribution(0.0f, 1.0f)
{
}

