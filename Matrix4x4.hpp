#ifndef Matrix4x4_hpp
#define Matrix4x4_hpp

#include <cmath>
#include <cstring>
#include <iostream>
#include <cassert>

#ifdef near
#undef near
#endif

#ifdef far
#undef far
#endif

template<typename T> struct Matrix4x4
{
    T values[16]; // Assumes column-major ordering.

    Matrix4x4<T>& operator*=(Matrix4x4<T> multiplier);

    inline operator T*() { return values; }
    inline operator const T*() const { return values; }
};

template<typename T> bool operator==(
    const Matrix4x4<T>& a,
    const Matrix4x4<T>& b)
{
    return !memcmp(&a, &b, sizeof(a));
}

template<typename T> bool operator!=(
    const Matrix4x4<T>& a,
    const Matrix4x4<T>& b)
{
    return memcmp(&a, &b, sizeof(a));
}

template<typename T> constexpr Matrix4x4<T> operator*(
    Matrix4x4<T> a, Matrix4x4<T> b)
{
    return {{
        a.values[0] * b.values[0] +
            a.values[4] * b.values[1] +
            a.values[8] * b.values[2] +
            a.values[12] * b.values[3],
        a.values[1] * b.values[0] +
            a.values[5] * b.values[1] +
            a.values[9] * b.values[2] +
            a.values[13] * b.values[3],
        a.values[2] * b.values[0] +
            a.values[6] * b.values[1] +
            a.values[10] * b.values[2] +
            a.values[14] * b.values[3],
        a.values[3] * b.values[0] +
            a.values[7] * b.values[1] +
            a.values[11] * b.values[2] +
            a.values[15] * b.values[3],
        a.values[0] * b.values[4] +
            a.values[4] * b.values[5] +
            a.values[8] * b.values[6] +
            a.values[12] * b.values[7],
        a.values[1] * b.values[4] +
            a.values[5] * b.values[5] +
            a.values[9] * b.values[6] +
            a.values[13] * b.values[7],
        a.values[2] * b.values[4] +
            a.values[6] * b.values[5] +
            a.values[10] * b.values[6] +
            a.values[14] * b.values[7],
        a.values[3] * b.values[4] +
            a.values[7] * b.values[5] +
            a.values[11] * b.values[6] +
            a.values[15] * b.values[7],
        a.values[0] * b.values[8] +
            a.values[4] * b.values[9] +
            a.values[8] * b.values[10] +
            a.values[12] * b.values[11],
        a.values[1] * b.values[8] +
            a.values[5] * b.values[9] +
            a.values[9] * b.values[10] +
            a.values[13] * b.values[11],
        a.values[2] * b.values[8] +
            a.values[6] * b.values[9] +
            a.values[10] * b.values[10] +
            a.values[14] * b.values[11],
        a.values[3] * b.values[8] +
            a.values[7] * b.values[9] +
            a.values[11] * b.values[10] +
            a.values[15] * b.values[11],
        a.values[0] * b.values[12] +
            a.values[4] * b.values[13] +
            a.values[8] * b.values[14] +
            a.values[12] * b.values[15],
        a.values[1] * b.values[12] +
            a.values[5] * b.values[13] +
            a.values[9] * b.values[14] +
            a.values[13] * b.values[15],
        a.values[2] * b.values[12] +
            a.values[6] * b.values[13] +
            a.values[10] * b.values[14] +
            a.values[14] * b.values[15],
        a.values[3] * b.values[12] +
            a.values[7] * b.values[13] +
            a.values[11] * b.values[14] +
            a.values[15] * b.values[15]}};
}

template<typename T> Matrix4x4<T> LoopMultiply(
    Matrix4x4<T> a, Matrix4x4<T> b)
{
    Matrix4x4<T> result;

    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 16; j += 4)
        {
            int index = j + i;
            result.values[index] = 0;

            for (int k = 0; k < 4; ++k)
            {
                result.values[index] +=
                    a.values[k * 4 + i] * b.values[j + k];
            }
        }
    }

    return result;
}

template<typename T> Matrix4x4<T>& Matrix4x4<T>::operator*=(
    Matrix4x4<T> multiplier)
{
    return *this = *this * multiplier;
}

template<typename T> std::ostream& operator<<(
    std::ostream& stream,
    Matrix4x4<T> matrix)
{
    for (int i = 0; i < 4; ++i)
    {
        for (int j = i; j < 16; j += 4)
        {
            stream << '[' << matrix.values[j] << ']';
        }

        stream << '\n';
    }

    return stream;
}

template<typename T> constexpr Matrix4x4<T> Identity4x4()
{
    return {{
        1,0,0,0,
        0,1,0,0,
        0,0,1,0,
        0,0,0,1}};
}

template<typename T> constexpr Matrix4x4<T> Scale(T x, T y, T z)
{
    return {{
        x,0,0,0,
        0,y,0,0,
        0,0,z,0,
        0,0,0,1}};
}

template<typename T> constexpr Matrix4x4<T> ScaleX(T n)
{
    return {{
        n,0,0,0,
        0,1,0,0,
        0,0,1,0,
        0,0,0,1}};
}

template<typename T> constexpr Matrix4x4<T> ScaleY(T n)
{
    return {{
        1,0,0,0,
        0,n,0,0,
        0,0,1,0,
        0,0,0,1}};
}

template<typename T> constexpr Matrix4x4<T> ScaleZ(T n)
{
    return {{
        1,0,0,0,
        0,1,0,0,
        0,0,n,0,
        0,0,0,1}};
}

template<typename T> constexpr Matrix4x4<T> Translate(T x, T y, T z)
{
    return {{
        1,0,0,0,
        0,1,0,0,
        0,0,1,0,
        x,y,z,1}};
}

template<typename T> Matrix4x4<T> Rotate(T radians, T x, T y, T z)
{
    T c = cos(radians);
    T ci = T(1) - c;
    T s = sin(radians);

    return {{
        x * x * ci + c, y * x * ci + (z * s), x * z * ci - (y * s), 0,
        x * y * ci - (z * s), y * y * ci + c, y * z * ci + (x * s), 0,
        x * z * ci + (y * s), y * z * ci - (x * s), z * z * ci + c, 0,
        0, 0, 0, 1}};
}

template<typename T> Matrix4x4<T> RotateX(T radians)
{
    T c = cos(radians);
    T s = sin(radians);

    return {{
        1,0,0,0,
        0,c,s,0,
        0,-s,c,0,
        0,0,0,1}};
}

template<typename T> Matrix4x4<T> RotateY(T radians)
{
    T c = cos(radians);
    T s = sin(radians);

    return {{
        c,0,-s,0,
        0,1,0,0,
        s,0,c,0,
        0,0,0,1}};
}

template<typename T> Matrix4x4<T> RotateZ(T radians)
{
    T c = cos(radians);
    T s = sin(radians);

    return {{
        c,s,0,0,
        -s,c,0,0,
        0,0,1,0,
        0,0,0,1}};
}

template<typename T> Matrix4x4<T> Orthographic(
    T left, T right, T bottom, T top, T near, T far)
{
    T rpl = right + left;
    T rml = right - left;
    T tpb = top + bottom;
    T tmb = top - bottom;
    T fpn = far + near;
    T fmn = far - near;

    return {{
        T(2) / rml,0,0,0,
        0,T(2) / tmb,0,0,
        0,0,T(-2) / fmn,0,
        rpl / rml, tpb / tmb, fpn / fmn, 1}};
}

template<typename T> Matrix4x4<T> Orthographic(T range, T ratio)
{
    return range < T(1) ?
        Orthographic<T>(
            -range,
            range,
            -range / ratio,
            range / ratio,
            range,
            -range) :
        Orthographic<T>(
            -range * ratio,
            range * ratio,
            -range,
            range,
            range,
            -range);
}

template<typename T> Matrix4x4<T> Perspective(
    T fieldOfViewRadians,
    T ratio,
    T near,
    T far,
    bool autoAdjust = false)
{
    /// Adaptation of gluPerspective.
    /// http://www.opengl.org/sdk/docs/man/xhtml/gluPerspective.xml

    T r = fieldOfViewRadians;
    if (autoAdjust && ratio < T(1)) r /= ratio;

    T f = T(1) / tan(r / T(2));
    T nmf = near - far;
    T npf = near + far;

    return {{
        f / ratio,0,0,0,
        0,f,0,0,
        0,0,npf / nmf,-1,
        0,0,(T(2) * far * near) / nmf,0}};
}

using Matrix4x4F = Matrix4x4<float>;

template<typename T> struct Vector4
{
    T values[4];

    inline operator T*() { return values; }
    inline operator const T*() const { return values; }
};

template<typename T> Vector4<T> operator*(
    Matrix4x4<T> matrix,
    Vector4<T> vector)
{
    Vector4<T> result = {};

    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
            result.values[i] += matrix.values[i + j * 4] * vector.values[j];
    }

    return result;
}

template<typename T> Vector4<T> Divided(Vector4<T> vector)
{
    assert(vector.values[3] != 0.0f);
    return {{
        vector.values[0] / vector.values[3],
        vector.values[1] / vector.values[3],
        vector.values[2] / vector.values[3],
        T(1)}};
}

template<typename T> std::ostream& operator<<(
    std::ostream& stream,
    Vector4<T> vector)
{
    return stream
        << '('
        << vector.values[0]
        << ", "
        << vector.values[1]
        << ", "
        << vector.values[2]
        << ", "
        << vector.values[3]
        << ')';
}

using Vector4F = Vector4<float>;

template<typename T> constexpr T Pi()
{
    return 3.14159265358979323846264338327950288419716939937510582097494459230;
}

template<typename T> constexpr T Tau()
{
    return Pi<T>() * T(2);
}

#endif
