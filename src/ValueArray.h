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

template <typename T, unsigned int N, typename INIT = ValueArrayInitZero>
struct ValueArray
{
    ValueArray() {
        for(auto & v: values) {
            v = INIT::value();
        }
    }
    ValueArray(float v[N]) {
        std::copy(begin(v), end(v), begin(values));
    }
    // Other value array type
    template <typename S>
    explicit ValueArray(const S & s) {
        std::copy(begin(s.values), end(s.values), begin(values));
    }
    // TODO: Add move/copy constructors and simplify operators below

    T residual() const {
        ValueArray<T, N> a = *this;
        for(auto & v : a.values) {
            v = 1.0f - v; 
        } 
        return a;
    }

    bool hasNonZeroComponent() const {
        for(auto v : values) {
            if(v > 0.0f) {
                return true;
            }
        }
        return false;
    }

    float values[N];
    static constexpr unsigned int NumElements = N;
};

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
    ColorArray(float v[N]) = default;

    std::string string() const;
};

template<unsigned int N>
struct RadianceArray : public ValueArray<RadianceArray<N>, N, ValueArrayInitZero>
{
    RadianceArray() = default;
    RadianceArray(float v[N]) = default;
};

template<unsigned int N>
struct ReflectanceArray : public ValueArray<ReflectanceArray<N>, N, ValueArrayInitOne>
{
    ReflectanceArray() = default;
    ReflectanceArray(float v[N]) = default;
    explicit ReflectanceArray(const ColorArray<N> & c) : ValueArray<ReflectanceArray<N>, N>(c) {}
};

template<unsigned int N>
inline RadianceArray<N> operator*(const ReflectanceArray<N> & ref,
                                  const RadianceArray<N> & rad);

template<unsigned int N>
struct ParameterArray : public ValueArray<ParameterArray<N>, N, ValueArrayInitZero>
{
    ParameterArray() = default;
    ParameterArray(float v[N]) = default;
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
