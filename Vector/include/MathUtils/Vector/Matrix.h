//
// Created by Hrishikesh Ingle on 8/21/25.
//

#pragma once

#include <cassert>
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
    Matrix()
    {
        for (size_t i = 0; i < N; ++i) {
            data[i] = Vector<T, M>();
        }
    }

    // Access operator
    T &operator()(size_t row, size_t col)
    {
        assert(row < N && col < M && "Matrix index out of bounds.");
        return data[row][col];
    }

    const T &operator()(size_t row, size_t col) const
    {
        assert(row < N && col < M && "Matrix index out of bounds.");
        return data[row][col];
    }

    const Vector<T, M>& operator[](size_t row) const
    {
        assert(row < N && "Matrix row index out of bounds.");
        return data[row];
    }

    Vector<T, M>& operator[](size_t row)
    {
        assert(row < N && "Matrix row index out of bounds.");
        return data[row];
    }

    // Get number of rows
    [[nodiscard]] size_t rows() const
    { return N; }

    // Get number of columns
    [[nodiscard]] size_t cols() const
    { return M; }

    // Matrix addition
    Matrix<T, N, M> operator+(const Matrix<T, N, M> &other) const
    {
        Matrix<T, N, M> result;
        for (size_t i = 0; i < N; ++i) {
            result[i] = this->data[i] + other.data[i];
        }
        return result;
    }

    // Matrix subtraction
    Matrix<T, N, M> operator-(const Matrix<T, N, M> &other) const
    {
        Matrix<T, N, M> result;
        for (size_t i = 0; i < N; ++i) {
            result[i] = this->data[i] - other.data[i];
        }
    }

    // Matrix element multiplication
    Matrix<T, N, M> operator*(const Matrix<T, N, M> &other) const
    {
        Matrix<T, N, M> result;
        for (size_t i = 0; i < N; ++i) {
            result[i] = this->data[i] * other.data[i];
        }
    }

    // Matrix multiplication
    template<size_t P>
    Matrix<T, N, P> operator$(const Matrix<T, M, P> &other) const
    {
        static_assert(M == P,
                      "Matrix multiplication requires the number of columns in the first matrix to match the number of rows in the second matrix.");
        Matrix<T, N, P> result;
        for (size_t i = 0; i < N; ++i) {
            for (size_t j = 0; j < P; ++j) {
                result(i, j) = T();
                for (size_t k = 0; k < M; ++k) {
                    result(i, j) += this->data[i][k] * other.data[k][j];
                }
            }
        }
        return result;
    }

    Matrix<T, N, 1> operator$(const MathUtils::Vector<T, M> &other) const
    {
        static_assert(M == other.size(),
                      "Matrix multiplication requires the number of columns in the matrix to match the size of the vector.");
        Matrix<T, N, 1> result;
        for (size_t i = 0; i < N; ++i) {
            result(i, 0) = T();
            for (size_t j = 0; j < M; ++j) {
                result(i, 0) += this->data[i][j] * other[j];
            }
        }
        return result;
    }

    // Scalar addition
    Matrix<T, N, M> operator+(const T &scalar) const
    {
        Matrix<T, N, M> result;
        for (size_t i = 0; i < N; ++i) {
            for (size_t j = 0; j < M; ++j) {
                result(i, j) = this->data[i][j] + scalar;
            }
        }
        return result;
    }

    // Scalar subtraction
    Matrix<T, N, M> operator-(const T &scalar) const
    {
        Matrix<T, N, M> result;
        for (size_t i = 0; i < N; ++i) {
            for (size_t j = 0; j < M; ++j) {
                result(i, j) = this->data[i][j] - scalar;
            }
        }
        return result;
    }

    // Scalar multiplication
    Matrix<T, N, M> operator*(const T &scalar) const
    {
        Matrix<T, N, M> result;
        for (size_t i = 0; i < N; ++i) {
            for (size_t j = 0; j < M; ++j) {
                result(i, j) = this->data[i][j] * scalar;
            }
        }
        return result;
    }

    // Scalar division
    Matrix<T, N, M> operator/(const T &scalar) const
    {
        assert(scalar != T() && "Division by zero in Matrix.");
        Matrix<T, N, M> result;
        for (size_t i = 0; i < N; ++i) {
            for (size_t j = 0; j < M; ++j) {
                result(i, j) = this->data[i][j] / scalar;
            }
        }
        return result;
    }

    // Scalar addition assignment
    Matrix<T, N, M> &operator+=(const T &scalar)
    {
        for (size_t i = 0; i < N; ++i) {
            for (size_t j = 0; j < M; ++j) {
                this->data[i][j] += scalar;
            }
        }
        return *this;
    }

    // Scalar subtraction assignment
    Matrix<T, N, M> &operator-=(const T &scalar)
    {
        for (size_t i = 0; i < N; ++i) {
            for (size_t j = 0; j < M; ++j) {
                this->data[i][j] -= scalar;
            }
        }
        return *this;
    }

    // Scalar multiplication assignment
    Matrix<T, N, M> &operator*=(const T &scalar)
    {
        for (size_t i = 0; i < N; ++i) {
            for (size_t j = 0; j < M; ++j) {
                this->data[i][j] *= scalar;
            }
        }
        return *this;
    }

    // Scalar division assignment
    Matrix<T, N, M> &operator/=(const T &scalar)
    {
        assert(scalar != T() && "Division by zero in Matrix.");
        for (size_t i = 0; i < N; ++i) {
            for (size_t j = 0; j < M; ++j) {
                this->data[i][j] /= scalar;
            }
        }
        return *this;
    }

    // Equality operator
    bool operator==(const Matrix<T, N, M> &other) const
    {
        for (size_t i = 0; i < N; ++i) {
            for (size_t j = 0; j < M; ++j) {
                if (this->data[i][j] != other.data[i][j]) {
                    return false;
                }
            }
        }
        return true;
    }
};
}