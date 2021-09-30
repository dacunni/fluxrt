#include <sstream>
#include "color.h"

namespace color {

namespace channel {

template<> float minValue() { return 0.0f; }
template<> float maxValue() { return 1.0f; }

template<> double minValue() { return 0.0; }
template<> double maxValue() { return 1.0; }

}; // namespace channel

}; // namespace color

