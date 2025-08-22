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
    // We use an anonymous union here. Its members (data and x)
    // become direct members of the containing class (VectorBase and Vector).
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
        }; // Anonymous struct inside the anonymous union.
    };
};
}


template<typename T, size_t N>
class Vector : public detail::VectorBase<T, N>
{

    static_assert(std::is_arithmetic<T>::value,
                  "Vector's template parameter T must be a numerical type.");

public:

    // Constructors
    Vector() : detail::VectorBase<T, N>()
    {}

    template<typename... Args>
    explicit Vector(Args &&... args) : detail::VectorBase<T, N>()
    {
        static_assert(sizeof...(args) == N, "Incorrect number of arguments for Vector construction.");
        // Use an initializer list to populate the data array.
        T temp_data[] = {static_cast<T>(std::forward<Args>(args))...};
        for (size_t i = 0; i < N; ++i) {
            this->data[i] = temp_data[i];
        }
    }

    Vector(const Vector &other) : detail::VectorBase<T, N>()
    {
        this->data = other.data;
    }

    template<size_t M, typename = std::enable_if_t<(N >= M)>>
    explicit Vector(const Vector<T, M> &other) : detail::VectorBase<T, N>()
    {
        for (size_t i = 0; i < M; ++i) {
            this->data[i] = other.data[i];
        }
    }

    Vector(Vector &&other) noexcept: detail::VectorBase<T, N>()
    {
        this->data = std::move(other.data);
    }

    explicit Vector(const T &value) : detail::VectorBase<T, N>()
    {
        for (size_t i = 0; i < N; ++i) {
            this->data[i] = value;
        }
    }

    // Assignment operators
    /**
     * Copy assignment operator.
     * @param other The vector to copy from.
     * @return A reference to this vector.
     */
    Vector &operator=(const Vector &other)
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
    Vector &operator=(Vector &&other) noexcept
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
    T &operator[](size_t index)
    {
        assert(index < N && "Index out of bounds.");
        return this->data[index];
    }

    /** Const accessor for the vector's data.
     * @return A const reference to the underlying data array.
     */
    const T &operator[](size_t index) const
    {
        assert(index < N && "Index out of bounds.");
        return this->data[index];
    }

    // Unary  operators.
    /**
     * Unary minus operator.
     * @return A new vector with all elements negated.
     */
    Vector<T, N> operator-() const
    {
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
    Vector<T, N> operator+() const
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
    Vector<T, N> operator+(const Vector<T, M> &other) const
    {
        if constexpr (N >= M) {
            Vector<T, N> result = *this;
            MATHUTILS_VECTOR_FOR_LOOP_UNROLL
            for (size_t i = 0; i < M; ++i) {
                result.data[i] += other.data[i];
            }
            return result;
        }else {
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
    template<size_t M, typename = std::enable_if_t<(N >= M)>>
    Vector<T, N> operator-(const Vector<T, M> &other) const
    {
        if constexpr(N >= M) {
            Vector<T, N> result = *this;
            MATHUTILS_VECTOR_FOR_LOOP_UNROLL
            for (size_t i = 0; i < M; ++i) {
                result.data[i] -= other.data[i];
            }
            return result;
        }
        else {
            Vector<T, M> result = -other;
            MATHUTILS_VECTOR_FOR_LOOP_UNROLL
            for (size_t i = 0; i < N; ++i) {
                result.data[i] += this->data[i];
            }
            return result;
        }
    }

    template<size_t M, typename = std::enable_if_t<(N >= M)>>
    Vector<T, N> operator*(const Vector<T, M> &other) const
    {
        Vector<T, N> result;
        MATHUTILS_VECTOR_FOR_LOOP_UNROLL
        for (size_t i = 0; i < M; ++i) {
            result.data[i] = this->data[i] * other.data[i];
        }
        return result;
    }

    template<size_t M, typename = std::enable_if_t<(N < M)>>
    Vector<T, M> operator*(const Vector<T, M> &other) const
    {
        Vector<T, M> result;
        MATHUTILS_VECTOR_FOR_LOOP_UNROLL
        for (size_t i = 0; i < N; ++i) {
            result.data[i] = this->data[i] * other.data[i];
        }
        return result;
    }

    template<size_t M, typename = std::enable_if_t<(N <= M)>>
    Vector<T, M> operator/(const Vector<T, M> &other) const
    {
        Vector<T, M> result;
        MATHUTILS_VECTOR_FOR_LOOP_UNROLL
        for (size_t i = 0; i < N; ++i) {
            result.data[i] = this->data[i] / other.data[i];
        }
        return result;
    }

    // Vector Vector Math Assignment operators
    template<size_t M, typename = std::enable_if_t<(N >= M)>>
    Vector<T, N> &operator+=(const Vector<T, M> &other)
    {
        MATHUTILS_VECTOR_FOR_LOOP_UNROLL
        for (size_t i = 0; i < M; ++i) {
            this->data[i] += other.data[i];
        }
        return *this;
    }

    template<size_t M, typename = std::enable_if_t<(N >= M)>>
    Vector<T, N> &operator-=(const Vector<T, M> &other)
    {
        MATHUTILS_VECTOR_FOR_LOOP_UNROLL
        for (size_t i = 0; i < M; ++i) {
            this->data[i] -= other.data[i];
        }
        return *this;
    }

    template<size_t M, typename = std::enable_if_t<(N >= M)>>
    Vector<T, N> &operator*=(const Vector<T, M> &other)
    {
        MATHUTILS_VECTOR_FOR_LOOP_UNROLL
        for (size_t i = 0; i < M; ++i) {
            this->data[i] *= other.data[i];
        }
        return *this;
    }

    Vector<T, N> &operator/=(const Vector<T, N> &other)
    {
        MATHUTILS_VECTOR_FOR_LOOP_UNROLL
        for (size_t i = 0; i < N; ++i) {
            this->data[i] /= other[i];
        }
        return *this;
    }

    // Scalar Vector Math operators
    Vector<T, N> operator+(const T &scalar) const
    {
        Vector<T, N> result = *this;
        MATHUTILS_VECTOR_FOR_LOOP_UNROLL
        for (size_t i = 0; i < N; ++i) {
            result.data[i] += scalar;
        }
        return result;
    }

    Vector<T, N> operator-(const T &scalar) const
    {
        Vector<T, N> result = *this;
        MATHUTILS_VECTOR_FOR_LOOP_UNROLL
        for (size_t i = 0; i < N; ++i) {
            result.data[i] -= scalar;
        }
        return result;
    }

    Vector<T, N> operator*(const T &scalar) const
    {
        Vector<T, N> result = *this;
        MATHUTILS_VECTOR_FOR_LOOP_UNROLL
        for (size_t i = 0; i < N; ++i) {
            result.data[i] *= scalar;
        }
        return result;
    }

    Vector<T, N> operator/(const T &scalar) const
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
    Vector<T, N> &operator+=(const T &scalar)
    {
        MATHUTILS_VECTOR_FOR_LOOP_UNROLL
        for (size_t i = 0; i < N; ++i) {
            this->data[i] += scalar;
        }
        return *this;
    }

    Vector<T, N> &operator-=(const T &scalar)
    {
        MATHUTILS_VECTOR_FOR_LOOP_UNROLL
        for (size_t i = 0; i < N; ++i) {
            this->data[i] -= scalar;
        }
        return *this;
    }

    Vector<T, N> &operator*=(const T &scalar)
    {
        MATHUTILS_VECTOR_FOR_LOOP_UNROLL
        for (size_t i = 0; i < N; ++i) {
            this->data[i] *= scalar;
        }
        return *this;
    }

    Vector<T, N> &operator/=(const T &scalar)
    {
        assert(scalar != T() && "Division by zero.");
        MATHUTILS_VECTOR_FOR_LOOP_UNROLL
        for (size_t i = 0; i < N; ++i) {
            this->data[i] /= scalar;
        }
        return *this;
    }

    // Vector functions
    // Dot product function. This should return a scalar value.
    template<size_t M, typename = std::enable_if_t<(N == M)>>
    T dot(const Vector<T, M> &other) const
    {
        T result = T(); // Initialize with a zero value
        MATHUTILS_VECTOR_FOR_LOOP_UNROLL
        for (size_t i = 0; i < N; ++i) {
            result += this->data[i] * other.data[i];
        }
        return result;
    }

    // Cross product function. Only defined for 3D vectors.
    Vector<T, 3> cross(const Vector<T, 3> &other) const
    {
        static_assert(N == 3, "Cross product is only defined for 3D or 7D vectors.");
        return Vector<T, 3>(
                this->y * other.z - this->z * other.y,
                this->z * other.x - this->x * other.z,
                this->x * other.y - this->y * other.x
        );
    }


    Vector<T, N> &swap(size_t index1, size_t index2)
    {
        assert(index1 < N && index2 < N && "Index out of bounds.");
        std::swap(this->data[index1], this->data[index2]);
        return *this;
    }

    Vector<T, N> swap(size_t index1, size_t index2) const
    {
        assert(index1 < N && index2 < N && "Index out of bounds.");
        Vector<T, N> result = *this;
        std::swap(result.data[index1], result.data[index2]);
        return result;
    }

    // equality
    template<size_t M>
    bool operator==(const Vector<T, M> &other) const
    {
        constexpr size_t smallest_size = std::min(N, M);
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

    // inequality
    template<size_t M>
    bool operator!=(const Vector<T, M> &other) const
    {
        constexpr size_t smallest_size = std::min(N, M);
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

    // less than
    template<size_t M>
    bool operator<(const Vector<T, M> &other) const
    {

        constexpr size_t min_dim = (N < M) ? N : M;

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

    // less than or equal to
    template<size_t M>
    bool operator<=(const Vector<T, M> &other) const
    {

        constexpr size_t min_dim = (N < M) ? N : M;

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

    // greater than
    template<size_t M>
    bool operator>(const Vector<T, M> &other) const
    {
        constexpr size_t min_dim = (N < M) ? N : M;

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

    // greater than or equal to
    template<size_t M>
    bool operator>=(const Vector<T, M> &other) const
    {
        constexpr size_t min_dim = (N < M) ? N : M;

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


}