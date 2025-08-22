//
// Created by Hrishikesh Ingle on 8/21/25.
//

#pragma once

#include <cassert>
#include <array>
#include "Vector.h"

namespace MathUtils
{
template<typename T, size_t N, size_t M = N>
class Matrix
{
    static_assert(N > 0 && M > 0, "Matrix dimensions must be greater than zero.");
    static_assert(std::is_arithmetic<T>::value, "Matrix's template parameter T must be a numerical type.");

private:
    Vector<T, M> data[N];
public:

    // Default constructor initializes all elements to zero.
    consteval Matrix()
    {
        for (size_t i = 0; i < N; ++i) {
            data[i] = Vector<T, M>();
        }
    }

    using Array = std::array<std::array<T, M>, N>;

    explicit constexpr Matrix(const Array &data)
    {
        for (size_t i = 0; i < N; i++) {
            this->data[i] = Vector<T, M>(data[i]);
        }
    }

    explicit constexpr Matrix(const T (&data)[N][M])
    {
        for (size_t i = 0; i < N; i++) {
            this->data[i] = Vector<T, M>(data[i]);
        }
    }

    explicit constexpr Matrix(T (&&data)[N][M])
    {
        for (size_t i = 0; i < N; i++) {
            this->data[i] = Vector<T, M>(std::move(data[i]));
        }
    }

    // Access operator
    constexpr T &operator()(size_t row, size_t col)
    {
        assert(row < N && col < M && "Matrix index out of bounds.");
        return data[row][col];
    }

    constexpr const T &operator()(size_t row, size_t col) const
    {
        assert(row < N && col < M && "Matrix index out of bounds.");
        return data[row][col];
    }

    constexpr const Vector<T, M> &operator[](size_t row) const
    {
        assert(row < N && "Matrix row index out of bounds.");
        return data[row];
    }

    constexpr Vector<T, M> &operator[](size_t row)
    {
        assert(row < N && "Matrix row index out of bounds.");
        return data[row];
    }

    // Get number of rows
    [[nodiscard]] constexpr size_t rows() const
    { return N; }

    // Get number of columns
    [[nodiscard]] constexpr size_t cols() const
    { return M; }

    // Matrix addition
    constexpr Matrix<T, N, M> operator+(const Matrix<T, N, M> &other) const
    {
        Matrix<T, N, M> result;
        for (size_t i = 0; i < N; ++i) {
            result[i] = (*this)[i] + other[i];
        }
        return result;
    }

    // Matrix subtraction
    constexpr Matrix<T, N, M> operator-(const Matrix<T, N, M> &other) const
    {
        Matrix<T, N, M> result;
        for (size_t i = 0; i < N; ++i) {
            result[i] = (*this)[i] - other[i];
        }
    }

    // Matrix element multiplication
    constexpr Matrix<T, N, M> operator*(const Matrix<T, N, M> &other) const
    {
        Matrix<T, N, M> result;
        for (size_t i = 0; i < N; ++i) {
            result[i] = (*this)[i] * other[i];
        }
    }

    // Matrix multiplication
    template<size_t P>
    constexpr Matrix<T, N, P> matMult(const Matrix<T, M, P> &other) const
    {
        Matrix<T, N, P> result;
        for (size_t i = 0; i < N; ++i) {
            for (size_t j = 0; j < P; ++j) {
                result(i, j) = T();
                for (size_t k = 0; k < M; ++k) {
                    result(i, j) += (*this)[i][k] * other[k][j];
                }
            }
        }
        return result;
    }

    constexpr Matrix<T, N, 1> matMult(const MathUtils::Vector<T, M> &other) const
    {
        Matrix<T, N, 1> result;
        for (size_t i = 0; i < N; ++i) {
            result(i, 0) = T();
            for (size_t j = 0; j < M; ++j) {
                result(i, 0) += (*this)[i][j] * other[j];
            }
        }
        return result;
    }

    // Scalar addition
    constexpr Matrix<T, N, M> operator+(const T &scalar) const
    {
        Matrix<T, N, M> result;
        for (size_t i = 0; i < N; ++i) {
            for (size_t j = 0; j < M; ++j) {
                result(i, j) = (*this)[i][j] + scalar;
            }
        }
        return result;
    }

    // Scalar subtraction
    constexpr Matrix<T, N, M> operator-(const T &scalar) const
    {
        Matrix<T, N, M> result;
        for (size_t i = 0; i < N; ++i) {
            for (size_t j = 0; j < M; ++j) {
                result(i, j) = (*this)[i][j] - scalar;
            }
        }
        return result;
    }

    // Scalar multiplication
    constexpr Matrix<T, N, M> operator*(const T &scalar) const
    {
        Matrix<T, N, M> result;
        for (size_t i = 0; i < N; ++i) {
            for (size_t j = 0; j < M; ++j) {
                result(i, j) = (*this)[i][j] * scalar;
            }
        }
        return result;
    }

    // Scalar division
    constexpr Matrix<T, N, M> operator/(const T &scalar) const
    {
        assert(scalar != T() && "Division by zero in Matrix.");
        Matrix<T, N, M> result;
        for (size_t i = 0; i < N; ++i) {
            for (size_t j = 0; j < M; ++j) {
                result(i, j) = (*this)[i][j] / scalar;
            }
        }
        return result;
    }

    // Scalar addition assignment
    constexpr Matrix<T, N, M> &operator+=(const T &scalar)
    {
        for (size_t i = 0; i < N; ++i) {
            for (size_t j = 0; j < M; ++j) {
                (*this)[i][j] += scalar;
            }
        }
        return *this;
    }

    // Scalar subtraction assignment
    constexpr Matrix<T, N, M> &operator-=(const T &scalar)
    {
        for (size_t i = 0; i < N; ++i) {
            for (size_t j = 0; j < M; ++j) {
                (*this)[i][j] -= scalar;
            }
        }
        return *this;
    }

    // Scalar multiplication assignment
    constexpr Matrix<T, N, M> &operator*=(const T &scalar)
    {
        for (size_t i = 0; i < N; ++i) {
            for (size_t j = 0; j < M; ++j) {
                (*this)[i][j] *= scalar;
            }
        }
        return *this;
    }

    // Scalar division assignment
    constexpr Matrix<T, N, M> &operator/=(const T &scalar)
    {
        assert(scalar != T() && "Division by zero in Matrix.");
        for (size_t i = 0; i < N; ++i) {
            for (size_t j = 0; j < M; ++j) {
                (*this)[i][j] /= scalar;
            }
        }
        return *this;
    }

    // Equality operator
    constexpr bool operator==(const Matrix<T, N, M> &other) const
    {
        for (size_t i = 0; i < N; ++i) {
            for (size_t j = 0; j < M; ++j) {
                if ((*this)[i][j] != other[i][j]) {
                    return false;
                }
            }
        }
        return true;
    }
};

#define USING_MATRIX(R, C, SUFFIX, TYPE) \
using Mat##R##x##C##SUFFIX = Matrix<TYPE, R, C>; \
USING_VECTOR(C, SUFFIX, TYPE)

#define MATRIX_ROW_1(C, SUFFIX, TYPE) \
    USING_MATRIX(1, C, SUFFIX, TYPE)

#define MATRIX_ROW_2(C, SUFFIX, TYPE) \
    MATRIX_ROW_1(C, SUFFIX, TYPE)     \
    USING_MATRIX(2, C, SUFFIX, TYPE)

#define MATRIX_ROW_3(C, SUFFIX, TYPE) \
    MATRIX_ROW_2(C, SUFFIX, TYPE)     \
    USING_MATRIX(3, C, SUFFIX, TYPE)

#define MATRIX_ROW_4(C, SUFFIX, TYPE) \
    MATRIX_ROW_3(C, SUFFIX, TYPE)     \
    USING_MATRIX(4, C, SUFFIX, TYPE)

#define MATRIX_ALL_1(SUFFIX, TYPE) \
    MATRIX_ROW_1(1, SUFFIX, TYPE)

#define MATRIX_ALL_2(SUFFIX, TYPE) \
    MATRIX_ALL_1(SUFFIX, TYPE)     \
    MATRIX_ROW_2(2, SUFFIX, TYPE)

#define MATRIX_ALL_3(SUFFIX, TYPE) \
    MATRIX_ALL_2(SUFFIX, TYPE)     \
    MATRIX_ROW_3(3, SUFFIX, TYPE)

#define MATRIX_ALL_4(SUFFIX, TYPE) \
    MATRIX_ALL_3(SUFFIX, TYPE)     \
    MATRIX_ROW_4(4, SUFFIX, TYPE)

#ifdef USING_ALL_MATRIX_TYPES
    #define USING_INT8_MATRIX_TYPES
    #define USING_UINT8_MATRIX_TYPES

    #define USING_INT16_MATRIX_TYPES
    #define USING_UINT16_MATRIX_TYPES

    #define USING_INT32_MATRIX_TYPES
    #define USING_UINT32_MATRIX_TYPES

    #define USING_INT64_MATRIX_TYPES
    #define USING_UINT64_MATRIX_TYPES

    #define USING_FLOATING_MATRIX_TYPES
    #define USING_DOUBLE_MATRIX_TYPES
#endif

#define MATRIX_ALL(SUFFIX, TYPE) MATRIX_ALL_4(SUFFIX, TYPE)

#ifdef USING_INT8_MATRIX_TYPES
MATRIX_ALL(I8, int8_t)
#endif

#ifdef USING_UINT8_MATRIX_TYPES
MATRIX_ALL(UI8, uint8_t)
#endif

#ifdef USING_INT16_MATRIX_TYPES
MATRIX_ALL(I16, int16_t)
#endif

#ifdef USING_UINT16_MATRIX_TYPES
MATRIX_ALL(UI16, uint16_t)
#endif

#ifdef USING_INT32_MATRIX_TYPES
MATRIX_ALL(I32, int32_t)
#endif

#ifdef USING_UINT32_MATRIX_TYPES
MATRIX_ALL(UI32, uint32_t)
#endif

#ifdef USING_INT64_MATRIX_TYPES
MATRIX_ALL(I64, int64_t)
#endif

#ifdef USING_UINT64_MATRIX_TYPES
MATRIX_ALL(UI64, uint64_t)
#endif

#ifdef USING_FLOATING_MATRIX_TYPES
MATRIX_ALL(F, float)
#endif

#ifdef USING_DOUBLE_MATRIX_TYPES
MATRIX_ALL(D, double)
#endif

}