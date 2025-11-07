#ifndef __VALUEARRAY_H__
#define __VALUEARRAY_H__

#include <algorithm>
#include <string>
#include <functional>

struct ValueArrayInitZero {
    static constexpr float value() { return 0.0f; }
};

struct ValueArrayInitOne {
    static constexpr float value() { return 1.0f; }
};

template <typename SubType, unsigned int N, typename INIT = ValueArrayInitZero>
struct ValueArray
{
    ValueArray() {
        for(auto & v: values) {
            v = INIT::value();
        }
    }
    // Copy constructor
    explicit ValueArray(const ValueArray & a) {
        std::copy(a.values, a.values + N, values);
    }
    // Array constructor
    ValueArray(float v[N]) {
        std::copy(v, v + N, values);
    }
    // Other value array type
    template <typename S>
    explicit ValueArray(const S & s) {
        std::copy(std::begin(s), std::end(s), values);
    }

    SubType residual() const;

    bool hasNonZeroComponent() const;

    float values[N];
    static constexpr unsigned int NumElements = N;
};

// Method Implementations
template <typename SubType, unsigned int N, typename INIT>
SubType ValueArray<SubType, N, INIT>::residual() const {
    ValueArray<SubType, N> a = *this;
    for(auto & v : a.values) {
        v = 1.0f - v; 
    } 
    return a;
}

template <typename SubType, unsigned int N, typename INIT>
bool ValueArray<SubType, N, INIT>::hasNonZeroComponent() const {
    for(auto v : values) {
        if(v > 0.0f) {
            return true;
        }
    }
    return false;
}

// Operators
template<class Op, typename ReturnType, typename LeftType, typename RightType>
inline ReturnType opValueArrays(Op op, const LeftType & left, const RightType & right)
{
    ReturnType prod;
    for(unsigned int i = 0; i < ReturnType::NumElements; ++i) {
        prod.values[i] = op(left[i], right[i]);
    }    
    return prod;
}
template<typename ReturnType, typename LeftType, typename RightType>
inline ReturnType addValueArrays(const LeftType & left, const RightType & right)
{
    return opValueArrays<ReturnType>(std::plus<float>(), left, right);
}
template<typename ReturnType, typename LeftType, typename RightType>
inline ReturnType multiplyValueArrays(const LeftType & left, const RightType & right)
{
    return opValueArrays<ReturnType>(std::multiplies<float>(), left, right);
}
template<typename ReturnType, typename LeftType, typename RightType>
inline ReturnType divideValueArrays(const LeftType & left, const RightType & right)
{
    return opValueArrays<ReturnType>(std::divides<float>(), left, right);
}

template<class Op, typename ReturnType, typename LeftType>
inline ReturnType opValueArrayScalar(Op op, const LeftType & left, float right)
{
    ReturnType prod;
    for(unsigned int i = 0; i < ReturnType::NumElements; ++i) {
        prod.values[i] = op(left[i], right);
    }    
    return prod;
}
template<typename ReturnType, typename LeftType>
inline ReturnType multiplyValueArrayScalar(const LeftType & left, float right)
{
    return opValueArrayScalar<ReturnType>(std::multiplies<float>(), left, right);
}

template<typename ReturnType, typename LeftType>
inline ReturnType divideValueArrayScalar(const LeftType & left, float right)
{
    return opValueArrayScalar<ReturnType>(std::divides<float>(), left, right);
}

// Subtypes

template<unsigned int N>
struct ColorArray : public ValueArray<ColorArray<N>, N, ValueArrayInitZero>
{
    ColorArray() = default;
    ColorArray(float v[N]) : ValueArray<ColorArray<N>, N>() {}

    std::string string() const;
};

template<unsigned int N>
struct RadianceArray : public ValueArray<RadianceArray<N>, N, ValueArrayInitZero>
{
    RadianceArray() = default;
    RadianceArray(float v[N]) : ValueArray<RadianceArray<N>, N>() {}
};

template<unsigned int N>
struct ReflectanceArray : public ValueArray<ReflectanceArray<N>, N, ValueArrayInitOne>
{
    ReflectanceArray() = default;
    ReflectanceArray(float v[N]) : ValueArray<ReflectanceArray<N>, N>() {}
    explicit ReflectanceArray(const ColorArray<N> & c) : ValueArray<ReflectanceArray<N>, N>(c) {}
};

template<unsigned int N>
inline RadianceArray<N> operator*(const ReflectanceArray<N> & ref,
                                  const RadianceArray<N> & rad);

template<unsigned int N>
struct ParameterArray : public ValueArray<ParameterArray<N>, N, ValueArrayInitZero>
{
    ParameterArray() = default;
    ParameterArray(float v[N]) : ValueArray<ParameterArray<N>, N>() {}
    explicit ParameterArray(const ColorArray<N> & c) : ValueArray<ParameterArray<N>, N>(c) {}
};

// Inline implementations

template<unsigned int N>
inline RadianceArray<N> operator+(const RadianceArray<N> & a, const RadianceArray<N> & b)
{
    return addValueArrays<RadianceArray<N>>(a, b);
}

template<unsigned int N>
inline RadianceArray<N> operator+=(RadianceArray<N> & a, const RadianceArray<N> & b)
{
    a = a + b;
    return a;
}

template<unsigned int N>
inline RadianceArray<N> operator*(const ReflectanceArray<N> & ref, const RadianceArray<N> & rad)
{
    return multiplyValueArrays<RadianceArray<N>>(ref, rad);
}

template<unsigned int N>
inline RadianceArray<N> operator*(const RadianceArray<N> & r, float s)
{
    return multiplyValueArrayScalar<RadianceArray<N>>(r, s);
}
template<unsigned int N>
inline RadianceArray<N> operator*(float s, const RadianceArray<N> & r)
{
    return operator*(r, s);
}

template<unsigned int N>
inline RadianceArray<N> operator*=(RadianceArray<N> & r, float s)
{
    r = r * s;
    return r;
}

template<unsigned int N>
inline RadianceArray<N> operator/(const RadianceArray<N> & r, float s)
{
    return divideValueArrayScalar<RadianceArray<N>>(r, s);
}

template<unsigned int N>
inline RadianceArray<N> operator/=(RadianceArray<N> & r, float s)
{
    r = r / s;
    return r;
}

template<unsigned int N>
inline RadianceArray<N> operator*(const ParameterArray<N> & param, const RadianceArray<N> & rad)
{
    return multiplyValueArrays<RadianceArray<N>>(param, rad);
}

template<unsigned int N>
inline RadianceArray<N> operator*(const RadianceArray<N> & rad, const ParameterArray<N> & param)
{
    return operator*(param, rad);
}

#endif // __VALUEARRAY_H__
