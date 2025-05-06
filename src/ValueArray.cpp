#include "ValueArray.h"

#include <sstream>


template<unsigned int N>
std::string ColorArray<N>::string() const
{
    std::stringstream ss;
    const char * sep = ", ";

    for(unsigned int i = 0; i < N; ++i) {
        if(i != 0) {
            ss << sep;
        }
        ss << this->values[i];
    } 

    return ss.str();
}

