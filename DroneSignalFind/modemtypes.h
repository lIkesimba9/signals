// в этом файле находятся описания типов данных и отладочных функций ввода-вывода

#ifndef MODDEMODTYPES_H
#define MODDEMODTYPES_H

#include <complex>
#include <vector>
#include <deque>
#include <sstream>
#include <string>
#include <iostream>

using namespace std;

static const double M_PI = 3.14159265358979323846;
static const double _2M_PI = M_PI * 2;

typedef complex<int32_t> cplx_int32;
typedef complex<int16_t> cplx_int16;
typedef complex<float> cplx_fl;
typedef complex<double> cplx_dbl;

typedef vector<int32_t> v_int32;
typedef vector<float> v_fl;
typedef vector<double> v_dbl;
typedef vector<cplx_int32> v_cplx_int32;
typedef vector<cplx_int16> v_cplx_int16;
typedef vector<cplx_fl> v_cplx_fl;
typedef vector<cplx_dbl> v_cplx_dbl;

typedef deque<cplx_fl> d_cplx_fl;
typedef deque<cplx_dbl> d_cplx_dbl;
typedef deque<cplx_int32> d_cplx_int32;
typedef deque<cplx_int16> d_cplx_int16;
typedef deque<float> d_fl;
typedef deque<int32_t> d_int32;
typedef deque<uint32_t> d_uint32;

static cplx_fl _im_j_fl(0,1);     // мнимая единица (0+1j)
static cplx_int32 _im_j_int32(0,1);

static const cplx_fl CPLX_FL_0(0.0,0.0);

namespace modemtypes {
template <typename T> struct real_t
{
    typedef T type;
};
template <typename T> struct real_t< std::complex<T> >
{
    typedef T type;
};
template <typename T> struct type
{
    typedef typename real_t<T>::type real_t;
};
}

template<typename T>
complex<T> make_complex(T magnitude, T arg) {
    return complex<T>(magnitude * cos(arg), magnitude * sin(arg));
}

template<typename T>
istream& operator>>(istream& in, complex<T>& data) {
    string s;
    in >> s;
    if (s == "0") {
        data = complex<T>(0,0);
    }
    else {
        stringstream ss(s);
        T re, im;
        ss >> re >> im;
        data = complex<T>(re,im);
    }
    return in;
}

template<typename T>
istream& operator>>(istream& in, vector<T>& data) {
    T d;
    while (in >> d) {
        data.push_back(d);
    }
    return in;
}

template<typename T>
ostream& operator<<(ostream& out, const complex<T>& data) {
    if (data == complex<T>(0,0))
        out << 0;
    else {
        out << data.real();
        if (data.imag() >= 0)
            out << '+';
        out << data.imag() << 'i';
    }
    return out;
}

template<typename T>
ostream& operator<<(ostream& out, const vector<T>& data) {
    for (const T& d : data)
        out << d << endl;
    return out;
}

template<typename T>
inline cplx_fl cplx_to_cplx_fl(complex<T> d) {
    return cplx_fl(d.real(), d.imag());
}

template<typename T>
inline cplx_int32 cplx_to_cplx_int32(complex<T> d) {
    return cplx_int32(static_cast<int32_t>(d.real()), static_cast<int32_t>(d.imag()));
}

#endif // MODDEMODTYPES_H
