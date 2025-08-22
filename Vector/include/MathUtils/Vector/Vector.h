#pragma once

#include <cstdlib>
#include <array>
#include <cassert>
#include <iostream>

// Define this macro to try to force the compiler to unroll the for loops.
#ifndef MATHUTILS_VECTOR_FORCE_FOR_LOOP_UNROLL
    #define MATHUTILS_VECTOR_FOR_LOOP_UNROLL
#endif

#ifndef MATHUTILS_VECTOR_FOR_LOOP_UNROLL
    #if defined(__clang__) || defined(__GNUC__) || defined(__INTEL_COMPILER) || defined(__INTEL_LLVM_COMPILER) || defined(__PGI) || defined(__NVCC__)
        #define MATHUTILS_VECTOR_FOR_LOOP_UNROLL _Pragma("unroll")
    #elif defined(_MSC_VER)
        #define MATHUTILS_VECTOR_FOR_LOOP_UNROLL __pragma(unroll)
    #else
        #error "We do not know how to unroll loops for this compiler. Please define the macro MATHUTILS_VECTOR_FOR_LOOP_UNROLL to force the compiler to unroll the loops."
    #endif
#endif

namespace MathUtils
{

namespace detail
{
template<typename T, size_t N>
class VectorBase
{
public:
    union
    {
        std::array<T, N> data;
        struct
        {
            T x, y, z;
        };
    };
};

// Partial specialization for N = 1.
template<typename T>
class VectorBase<T, 1>
{
public:
    union
    {
        std::array<T, 1> data;
        T x;
    };
};

// Partial specialization for N = 2.
template<typename T>
class VectorBase<T, 2>
{
public:
    union
    {
        std::array<T, 2> data;
        struct
        {
            T x, y;
        };
    };
};
}


template<typename T, size_t N>
class Vector : public detail::VectorBase<T, N>
{

    static_assert(std::is_arithmetic<T>::value,
                  "Vector's template parameter T must be a numerical type.");
    static_assert(N > 0, "Vector's size N must be greater than 0.");

public:

    // Constructors
    consteval Vector() : detail::VectorBase<T, N>()
    {}

    template<typename... Args>
    constexpr explicit Vector(Args &&... args) : detail::VectorBase<T, N>()
    {
        static_assert(sizeof...(args) == N, "Incorrect number of arguments for Vector construction.");
        T temp_data[] = {static_cast<T>(std::forward<Args>(args))...};
        MATHUTILS_VECTOR_FOR_LOOP_UNROLL
        for (size_t i = 0; i < N; ++i) {
            this->data[i] = temp_data[i];
        }
    }

    constexpr Vector(const Vector &other) : detail::VectorBase<T, N>()
    {
        this->data = other.data;
    }

    template<size_t M, typename = std::enable_if_t<(N >= M)>>
    constexpr explicit Vector(const Vector<T, M> &other) : detail::VectorBase<T, N>()
    {
        MATHUTILS_VECTOR_FOR_LOOP_UNROLL
        for (size_t i = 0; i < M; ++i) {
            this->data[i] = other.data[i];
        }
    }

    constexpr Vector(Vector &&other) noexcept: detail::VectorBase<T, N>()
    {
        this->data = std::move(other.data);
    }

    constexpr explicit Vector(const T &value) : detail::VectorBase<T, N>()
    {
        MATHUTILS_VECTOR_FOR_LOOP_UNROLL
        for (size_t i = 0; i < N; ++i) {
            this->data[i] = value;
        }
    }

    constexpr explicit Vector(const std::array<T, N> &data)
    {
        this->data = data;
    }

    constexpr explicit Vector(std::array<T, N> &&data)
    {
        this->data = std::move(data);
    }

    constexpr explicit Vector(const T (&data)[N])
    {
        for (size_t i = 0; i < N; ++i) {
            this->data[i] = data[i];
        }
    }

    constexpr explicit Vector(T (&&data)[N])
    {
        for (size_t i = 0; i < N; ++i) {
            this->data[i] = std::move(data[i]);
        }
    }

    consteval size_t size() {
        return N;
    }



    // Assignment operators
    /**
     * Copy assignment operator.
     * @param other The vector to copy from.
     * @return A reference to this vector.
     */
    constexpr Vector &operator=(const Vector &other)
    {
        if (this != &other) {
            this->data = other.data;
        }
        return *this;
    }

    /**
     * Move assignment operator.
     * @param other The vector to move from.
     * @return A reference to this vector.
     */
    constexpr Vector &operator=(Vector &&other) noexcept
    {
        if (this != &other) {
            this->data = std::move(other.data);
        }
        return *this;
    }

    // accessor methods
    /**
     * Accessor for the vector's data.
     * @return A reference to the underlying data array.
     */
    constexpr T &operator[](size_t index)
    {
        assert(index < N && "Index out of bounds.");
        return this->data[index];
    }

    /** Const accessor for the vector's data.
     * @return A const reference to the underlying data array.
     */
    constexpr const T &operator[](size_t index) const
    {
        assert(index < N && "Index out of bounds.");
        return this->data[index];
    }

    // Unary  operators.
    /**
     * Unary minus operator.
     * @return A new vector with all elements negated.
     */
    constexpr Vector<T, N> operator-() const
    {
        static_assert(std::is_signed<T>::value, "Vector's element type must be a signed type.");
        Vector<T, N> result;
        MATHUTILS_VECTOR_FOR_LOOP_UNROLL
        for (size_t i = 0; i < N; ++i) {
            result.data[i] = -this->data[i];
        }
        return result;
    }

    /**
     * Unary plus operator.
     * @return A new vector that is a copy of this vector.
     */
    constexpr Vector<T, N> operator+() const
    {
        return *this;
    }

    // Vector Vector Math operators
    /**
     * Vector addition operator. If the sizes of the vectors are different, the smaller one is implicitly padded with 0's before the addition.
     * @tparam M The size of the other vector.
     * @param other The vector to add.
     * @return A new vector that is the sum of this vector and the other vector.
     */
    template<size_t M>
    constexpr auto operator+(const Vector<T, M> &other) const
    {
        if constexpr (N >= M) {
            Vector<T, N> result = *this;
            MATHUTILS_VECTOR_FOR_LOOP_UNROLL
            for (size_t i = 0; i < M; ++i) {
                result.data[i] += other.data[i];
            }
            return result;
        } else {
            Vector<T, M> result = other;
            MATHUTILS_VECTOR_FOR_LOOP_UNROLL
            for (size_t i = 0; i < N; ++i) {
                result.data[i] += this->data[i];
            }
            return result;
        }
    }

    /**
     * Vector subtract operator. If the sizes of the vectors are different, the smaller one is implicitly padded with 0's before the subtraction.
     * @tparam M The size of the other vector.
     * @param other The vector to subtract from this.
     * @return A new vector that is the difference between this vector and the other vector.
     */
    template<size_t M>
    constexpr auto operator-(const Vector<T, M> &other) const
    {
        if constexpr (N >= M) {
            Vector<T, N> result = *this;
            MATHUTILS_VECTOR_FOR_LOOP_UNROLL
            for (size_t i = 0; i < M; ++i) {
                result.data[i] -= other.data[i];
            }
            return result;
        } else {
            Vector<T, M> result = -other;
            MATHUTILS_VECTOR_FOR_LOOP_UNROLL
            for (size_t i = 0; i < N; ++i) {
                result.data[i] += this->data[i];
            }
            return result;
        }
    }

    /**
     * Vector multiplication operator. If the sizes of the vectors are different, the smaller one is implicitly padded with 0's before the multiplication.
     * @tparam M The size of the other vector.
     * @param other The vector to multiply with this.
     * @return A new vector that is the element-wise product of this vector and the other vector.
     */
    template<size_t M>
    constexpr auto operator*(const Vector<T, M> &other) const
    {
        if constexpr (N >= M) {
            Vector<T, N> result;
            MATHUTILS_VECTOR_FOR_LOOP_UNROLL
            for (size_t i = 0; i < M; ++i) {
                result.data[i] = this->data[i] * other.data[i];
            }
            return result;
        } else {
            Vector<T, M> result;
            MATHUTILS_VECTOR_FOR_LOOP_UNROLL
            for (size_t i = 0; i < N; ++i) {
                result.data[i] = this->data[i] * other.data[i];
            }
            return result;
        }
    }

    /**
     * Vector division operator. If the numerator is smaller than the denominator, the numerator is implicitly padded with 0's before the division. Does not support the case where the denominator is smaller than the numerator.
     * @tparam M The size of the other vector.
     * @param other The vector to divide this by.
     * @return A new vector that is the element-wise division of this vector by the other vector.
     */
    template<size_t M, typename = std::enable_if_t<(N <= M)>>
    constexpr Vector<T, M> operator/(const Vector<T, M> &other) const
    {
        Vector<T, M> result;
        MATHUTILS_VECTOR_FOR_LOOP_UNROLL
        for (size_t i = 0; i < N; ++i) {
            result.data[i] = this->data[i] / other.data[i];
        }
        return result;
    }

    // Vector Vector Math Assignment operators
    /**
     * Vector addition assignment operator. If the other vector is smaller than this vector, the other vector is implicitly padded with 0's before the addition.
     * @tparam M The size of the other vector.
     * @param other The vector to add to this.
     * @return A reference to this vector after the addition.
     */
    template<size_t M, typename = std::enable_if_t<(N >= M)>>
    constexpr Vector<T, N> &operator+=(const Vector<T, M> &other)
    {
        MATHUTILS_VECTOR_FOR_LOOP_UNROLL
        for (size_t i = 0; i < M; ++i) {
            this->data[i] += other.data[i];
        }
        return *this;
    }

    /**
     * Vector subtraction assignment operator. If the other vector is smaller than this vector, the other vector is implicitly padded with 0's before the subtraction.
     * @tparam M The size of the other vector.
     * @param other The vector to subtract from this.
     * @return A reference to this vector after the subtraction.
     */
    template<size_t M, typename = std::enable_if_t<(N >= M)>>
    constexpr Vector<T, N> &operator-=(const Vector<T, M> &other)
    {
        MATHUTILS_VECTOR_FOR_LOOP_UNROLL
        for (size_t i = 0; i < M; ++i) {
            this->data[i] -= other.data[i];
        }
        return *this;
    }

    /**
     * Vector multiplication assignment operator. If the other vector is smaller than this vector, the other vector is implicitly padded with 0's before the multiplication.
     * @tparam M The size of the other vector.
     * @param other The vector to multiply with this.
     * @return A reference to this vector after the multiplication.
     */
    template<size_t M, typename = std::enable_if_t<(N >= M)>>
    constexpr Vector<T, N> &operator*=(const Vector<T, M> &other)
    {
        MATHUTILS_VECTOR_FOR_LOOP_UNROLL
        for (size_t i = 0; i < M; ++i) {
            this->data[i] *= other.data[i];
        }
        return *this;
    }

    /**
     * Vector division assignment operator. Only supports the case where both vectors are of the same size.
     * @param other The vector to divide this by.
     * @return A reference to this vector after the division.
     */
    constexpr Vector<T, N> &operator/=(const Vector<T, N> &other)
    {
        MATHUTILS_VECTOR_FOR_LOOP_UNROLL
        for (size_t i = 0; i < N; ++i) {
            assert(other[i] != T() && "Division by zero.");
            this->data[i] /= other[i];
        }
        return *this;
    }

    // Scalar Vector Math operators
    /**
     * Scalar addition operator. Adds a scalar value to each element of the vector.
     * @param scalar The scalar value to add.
     * @return A new vector that is the sum of this vector and the scalar.
     */
    constexpr Vector<T, N> operator+(const T &scalar) const
    {
        Vector<T, N> result = *this;
        MATHUTILS_VECTOR_FOR_LOOP_UNROLL
        for (size_t i = 0; i < N; ++i) {
            result.data[i] += scalar;
        }
        return result;
    }

    /**
     * Scalar subtraction operator. Subtracts a scalar value from each element of the vector.
     * @param scalar The scalar value to subtract.
     * @return A new vector that is the difference between this vector and the scalar.
     */
    constexpr Vector<T, N> operator-(const T &scalar) const
    {
        Vector<T, N> result = *this;
        MATHUTILS_VECTOR_FOR_LOOP_UNROLL
        for (size_t i = 0; i < N; ++i) {
            result.data[i] -= scalar;
        }
        return result;
    }

    /**
     * Scalar multiplication operator. Multiplies each element of the vector by a scalar value.
     * @param scalar The scalar value to multiply with.
     * @return A new vector that is the product of this vector and the scalar.
     */
    constexpr Vector<T, N> operator*(const T &scalar) const
    {
        Vector<T, N> result = *this;
        MATHUTILS_VECTOR_FOR_LOOP_UNROLL
        for (size_t i = 0; i < N; ++i) {
            result.data[i] *= scalar;
        }
        return result;
    }

    /**
     * Scalar division operator. Divides each element of the vector by a scalar value. Throws an assertion if the scalar is zero.
     * @param scalar The scalar value to divide by.
     * @return A new vector that is the result of dividing this vector by the scalar.
     */
    constexpr Vector<T, N> operator/(const T &scalar) const
    {
        assert(scalar != T() && "Division by zero.");
        Vector<T, N> result = *this;
        MATHUTILS_VECTOR_FOR_LOOP_UNROLL
        for (size_t i = 0; i < N; ++i) {
            result.data[i] /= scalar;
        }
        return result;
    }

    // Scalar Vector Math Assignment operators
    /**
     * Scalar addition assignment operator. Adds a scalar value to each element of the vector.
     * @param scalar The scalar value to add.
     * @return A reference to this vector after the addition.
     */
    constexpr Vector<T, N> &operator+=(const T &scalar)
    {
        MATHUTILS_VECTOR_FOR_LOOP_UNROLL
        for (size_t i = 0; i < N; ++i) {
            this->data[i] += scalar;
        }
        return *this;
    }

    /**
     * Scalar subtraction assignment operator. Subtracts a scalar value from each element of the vector.
     * @param scalar The scalar value to subtract.
     * @return A reference to this vector after the subtraction.
     */
    constexpr Vector<T, N> &operator-=(const T &scalar)
    {
        MATHUTILS_VECTOR_FOR_LOOP_UNROLL
        for (size_t i = 0; i < N; ++i) {
            this->data[i] -= scalar;
        }
        return *this;
    }

    /**
     * Scalar multiplication assignment operator. Multiplies each element of the vector by a scalar value.
     * @param scalar The scalar value to multiply.
     * @return A reference to this vector after the multiplication.
     */
    constexpr Vector<T, N> &operator*=(const T &scalar)
    {
        MATHUTILS_VECTOR_FOR_LOOP_UNROLL
        for (size_t i = 0; i < N; ++i) {
            this->data[i] *= scalar;
        }
        return *this;
    }

    /**
     * Scalar division assignment operator. Divides each element of the vector by a scalar value.
     * @param scalar The scalar value to divide by.
     * @return A reference to this vector after the division.
     */
    constexpr Vector<T, N> &operator/=(const T &scalar)
    {
        assert(scalar != T() && "Division by zero.");
        MATHUTILS_VECTOR_FOR_LOOP_UNROLL
        for (size_t i = 0; i < N; ++i) {
            this->data[i] /= scalar;
        }
        return *this;
    }

    // Vector functions
    /**
     * Dot product function. Computes the dot product of this vector with another vector.
     * @param other The other vector to compute the dot product with.
     * @return The dot product of this vector and the other vector.
     */
    constexpr T dot(const Vector<T, N> &other) const
    {
        T result = T(); // Initialize with a zero value
        MATHUTILS_VECTOR_FOR_LOOP_UNROLL
        for (size_t i = 0; i < N; ++i) {
            result += this->data[i] * other.data[i];
        }
        return result;
    }

    /**
     * Cross product function. Computes the cross product of this vector with another 3D vector.
     * @param other The other 3D vector to compute the cross product with.
     * @return A new vector that is the cross product of this vector and the other vector.
     */
    constexpr Vector<T, 3> cross(const Vector<T, 3> &other) const
    {
        static_assert(N == 3, "Cross product is only defined for 3D vectors.");
        return Vector<T, 3>(
                this->y * other.z - this->z * other.y,
                this->z * other.x - this->x * other.z,
                this->x * other.y - this->y * other.x
        );
    }

    /**
     * Swap function. Swaps the elements at the specified indices in the vector.
     * @param index1 The first index of the vector to swap.
     * @param index2 The second index of the vector to swap.
     * @return A reference to this vector after the swap.
     */
    constexpr Vector<T, N> &swap(size_t index1, size_t index2)
    {
        assert(index1 < N && index2 < N && "Index out of bounds.");
        std::swap(this->data[index1], this->data[index2]);
        return *this;
    }

    /**
     * Swap function. Swaps the elements at the specified indices in the vector and returns a new vector.
     * @param index1 The first index of the vector to swap.
     * @param index2 The second index of the vector to swap.
     * @return A new vector that is a copy of this vector with the specified elements swapped.
     */
    constexpr Vector<T, N> swap(size_t index1, size_t index2) const
    {
        assert(index1 < N && index2 < N && "Index out of bounds.");
        Vector<T, N> result = *this;
        std::swap(result.data[index1], result.data[index2]);
        return result;
    }

    /**
     * Equality operator. Compares this vector with another vector of the same or different size. The shorter vector is implicitly padded with 0's before the comparison.
     * @tparam M The size of the other vector.
     * @param other The vector to compare with this.
     * @return True if the vectors are equal, false otherwise.
     */
    template<size_t M>
    constexpr bool operator==(const Vector<T, M> &other) const
    {
        constexpr
        size_t smallest_size = std::min(N, M);
        MATHUTILS_VECTOR_FOR_LOOP_UNROLL
        for (size_t i = 0; i < smallest_size; ++i) {
            if (this->data[i] != other.data[i]) {
                return false;
            }
        }
        if constexpr (N > M) {
            MATHUTILS_VECTOR_FOR_LOOP_UNROLL
            for (size_t i = M; i < N; ++i) {
                if (this->data[i] != T()) { // Assuming T() is the default value for T
                    return false;
                }
            }
        } else if constexpr (M > N) {
            MATHUTILS_VECTOR_FOR_LOOP_UNROLL
            for (size_t i = N; i < M; ++i) {
                if (other.data[i] != T()) { // Assuming T() is the default value for T
                    return false;
                }
            }
        }
        return true;
    }

    /**
     * Inequality operator. Compares this vector with another vector of the same or different size. The shorter vector is implicitly padded with 0's before the comparison.
     * @tparam M The size of the other vector.
     * @param other The vector to compare with this.
     * @return True if the vectors are not equal, false otherwise.
     */
    template<size_t M>
    constexpr bool operator!=(const Vector<T, M> &other) const
    {
        constexpr
        size_t smallest_size = std::min(N, M);
        MATHUTILS_VECTOR_FOR_LOOP_UNROLL
        for (size_t i = 0; i < smallest_size; ++i) {
            if (this->data[i] != other.data[i]) {
                return true;
            }
        }
        if constexpr (N > M) {
            MATHUTILS_VECTOR_FOR_LOOP_UNROLL
            for (size_t i = M; i < N; ++i) {
                if (this->data[i] != T()) { // Assuming T() is the default value for T
                    return true;
                }
            }
        } else if constexpr (M > N) {
            MATHUTILS_VECTOR_FOR_LOOP_UNROLL
            for (size_t i = N; i < M; ++i) {
                if (other.data[i] != T()) { // Assuming T() is the default value for T
                    return true;
                }
            }
        }
        return false;
    }

    /**
     * Less than operator. Compares this vector with another vector of the same or different size. The shorter vector is implicitly padded with 0's before the comparison.
     * @tparam M The size of the other vector.
     * @param other The vector to compare with this.
     * @return True if this vector is less than the other vector, false otherwise.
     */
    template<size_t M>
    constexpr bool operator<(const Vector<T, M> &other) const
    {

        constexpr
        size_t min_dim = (N < M) ? N : M;

        MATHUTILS_VECTOR_FOR_LOOP_UNROLL
        for (size_t i = 0; i < min_dim; ++i) {
            if (this->data[i] < other.data[i]) {
                return true;
            } else if (this->data[i] > other.data[i]) {
                return false;
            }
        }
        if constexpr (N < M) {
            MATHUTILS_VECTOR_FOR_LOOP_UNROLL
            for (size_t i = N; i < M; ++i) {
                if (other.data[i] > T()) { // Assuming T() is the default value for T
                    return true;
                } else if (other.data[i] < T()) {
                    return false;
                }
            }
        } else if constexpr (N > M) {
            MATHUTILS_VECTOR_FOR_LOOP_UNROLL
            for (size_t i = M; i < N; ++i) {
                if (this->data[i] > T()) { // Assuming T() is the default value for T
                    return false;
                } else if (this->data[i] < T()) {
                    return true;
                }
            }
        }
        return false;
    }

    /**
     * Less than or equal to operator. Compares this vector with another vector of the same or different size. The shorter vector is implicitly padded with 0's before the comparison.
     * @tparam M The size of the other vector.
     * @param other The vector to compare with this.
     * @return True if this vector is less than or equal to the other vector, false otherwise.
     */
    template<size_t M>
    constexpr bool operator<=(const Vector<T, M> &other) const
    {

        constexpr
        size_t min_dim = (N < M) ? N : M;

        MATHUTILS_VECTOR_FOR_LOOP_UNROLL
        for (size_t i = 0; i < min_dim; ++i) {
            if (this->data[i] < other.data[i]) {
                return true;
            } else if (this->data[i] > other.data[i]) {
                return false;
            }
        }
        if constexpr (N < M) {
            MATHUTILS_VECTOR_FOR_LOOP_UNROLL
            for (size_t i = N; i < M; ++i) {
                if (other.data[i] > T()) { // Assuming T() is the default value for T
                    return true;
                } else if (other.data[i] < T()) {
                    return false;
                }
            }
        } else if constexpr (N > M) {
            MATHUTILS_VECTOR_FOR_LOOP_UNROLL
            for (size_t i = M; i < N; ++i) {
                if (this->data[i] > T()) { // Assuming T() is the default value for T
                    return false;
                } else if (this->data[i] < T()) {
                    return true;
                }
            }
        }
        return true;
    }

    /**
     * Greater than operator. Compares this vector with another vector of the same or different size. The shorter vector is implicitly padded with 0's before the comparison.
     * @tparam M The size of the other vector.
     * @param other The vector to compare with this.
     * @return True if this vector is greater than the other vector, false otherwise.
     */
    template<size_t M>
    constexpr bool operator>(const Vector<T, M> &other) const
    {
        constexpr
        size_t min_dim = (N < M) ? N : M;

        MATHUTILS_VECTOR_FOR_LOOP_UNROLL
        for (size_t i = 0; i < min_dim; ++i) {
            if (this->data[i] > other.data[i]) {
                return true;
            } else if (this->data[i] < other.data[i]) {
                return false;
            }
        }

        if constexpr (N < M) {
            MATHUTILS_VECTOR_FOR_LOOP_UNROLL
            for (size_t i = N; i < M; ++i) {
                if (other.data[i] > T()) { // Assuming T() is the default value for T
                    return false;
                } else if (other.data[i] < T()) {
                    return true;
                }
            }
        } else if constexpr (M < N) {
            MATHUTILS_VECTOR_FOR_LOOP_UNROLL
            for (size_t i = M; i < N; ++i) {
                if (this->data[i] > T()) { // Assuming T() is the default value for T
                    return true;
                } else if (this->data[i] < T()) {
                    return false;
                }
            }
        }
        return false;
    }

    /**
     * Greater than or equal to operator. Compares this vector with another vector of the same or different size. The shorter vector is implicitly padded with 0's before the comparison.
     * @tparam M The size of the other vector.
     * @param other The vector to compare with this.
     * @return True if this vector is greater than or equal to the other vector, false otherwise.
     */
    template<size_t M>
    constexpr bool operator>=(const Vector<T, M> &other) const
    {
        constexpr
        size_t min_dim = (N < M) ? N : M;

        MATHUTILS_VECTOR_FOR_LOOP_UNROLL
        for (size_t i = 0; i < min_dim; ++i) {
            if (this->data[i] > other.data[i]) {
                return true;
            } else if (this->data[i] < other.data[i]) {
                return false;
            }
        }

        if constexpr (N < M) {
            MATHUTILS_VECTOR_FOR_LOOP_UNROLL
            for (size_t i = N; i < M; ++i) {
                if (other.data[i] > T()) { // Assuming T() is the default value for T
                    return false;
                } else if (other.data[i] < T()) {
                    return true;
                }
            }
        } else if constexpr (M < N) {
            MATHUTILS_VECTOR_FOR_LOOP_UNROLL
            for (size_t i = M; i < N; ++i) {
                if (this->data[i] > T()) { // Assuming T() is the default value for T
                    return true;
                } else if (this->data[i] < T()) {
                    return false;
                }
            }
        }
        return true;
    }


    // formating
    friend std::ostream &operator<<(std::ostream &os, const Vector<T, N> &vec)
    {
        os << "(";
        for (size_t i = 0; i < N; ++i) {
            os << vec.data[i];
            if (i < N - 1) {
                os << ", ";
            }
        }
        os << ")";
        return os;
    }
};

#define USING_VECTOR(L, SUFFIX, TYPE) using Vec##L##SUFFIX = Vector<TYPE, L>;

#define VECTOR_ALL(SUFFIX, TYPE) \
USING_VECTOR(1, SUFFIX, TYPE)    \
USING_VECTOR(2, SUFFIX, TYPE)    \
USING_VECTOR(3, SUFFIX, TYPE)    \
USING_VECTOR(4, SUFFIX, TYPE)    \


#ifdef USING_ALL_VECTOR_TYPES
    #define USING_INT8_VECTOR_TYPES
    #define USING_UINT8_VECTOR_TYPES

    #define USING_INT16_VECTOR_TYPES
    #define USING_UINT16_VECTOR_TYPES

    #define USING_INT32_VECTOR_TYPES
    #define USING_UINT32_VECTOR_TYPES

    #define USING_INT64_VECTOR_TYPES
    #define USING_UINT64_VECTOR_TYPES

    #define USING_FLOATING_VECTOR_TYPES
    #define USING_DOUBLE_VECTOR_TYPES
#endif

#ifdef USING_INT8_VECTOR_TYPES
VECTOR_ALL(I8, int8_t)
#endif

#ifdef USING_UINT8_VECTOR_TYPES
VECTOR_ALL(UI8, uint8_t)
#endif

#ifdef USING_INT16_VECTOR_TYPES
VECTOR_ALL(I16, int16_t)
#endif

#ifdef USING_UINT16_VECTOR_TYPES
VECTOR_ALL(UI16, uint16_t)
#endif

#ifdef USING_INT32_VECTOR_TYPES
VECTOR_ALL(I32, int32_t)
#endif

#ifdef USING_UINT32_VECTOR_TYPES
VECTOR_ALL(UI32, uint32_t)
#endif

#ifdef USING_INT64_VECTOR_TYPES
VECTOR_ALL(I64, int64_t)
#endif

#ifdef USING_UINT64_VECTOR_TYPES
VECTOR_ALL(UI64, uint64_t)
#endif

#ifdef USING_FLOATING_VECTOR_TYPES
VECTOR_ALL(F, float)
#endif

#ifdef USING_DOUBLE_VECTOR_TYPES
VECTOR_ALL(D, double)
#endif

}