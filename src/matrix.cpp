#include <sstream>
#include "matrix.h"


std::string AffineMatrix::string() const
{
    std::stringstream ss;
    ss << '[';
    std::for_each(data, &data[12], [&](const float & v) {
                  ss << ' ' << v;
                  });
    ss << " ]";
    return ss.str();
}

