
#define USING_INT64_MATRIX_TYPES

#include <gtest/gtest.h>
#include "MathUtils/Vector/Matrix.h"

using namespace MathUtils;

class MatrixTest : public ::testing::Test
{
protected:
    void SetUp() override
    {}

    void TearDown() override
    {}
};

consteval Mat4x4I64 getMatrix()
{
    Mat4x4I64 m;
    int value = 1;
    for (size_t i = 0; i < 4; ++i) {
        for (size_t j = 0; j < 4; ++j) {
            m(i, j) = value++;
        }
    }
    return m;
}


TEST_F(MatrixTest, Equality)
{
    Mat4x4I64 m1_a = getMatrix();
    Mat4x4I64 m1_b;
    Mat4x4I64 m1_c;

    m1_b = m1_a;
    EXPECT_EQ(m1_a, m1_b);

    // Test with a different matrix
    m1_c(0, 0) = -m1_a(0, 0);
    EXPECT_NE(m1_a, m1_c);
}

TEST_F(MatrixTest, Inequality)
{
    Mat4x4I64 m1_a = getMatrix();
    Mat4x4I64 m1_b = getMatrix();

    // Test with a different matrix
    m1_b(0, 0) = -m1_a(0, 0);
    EXPECT_NE(m1_a, m1_b);
}

TEST_F(MatrixTest, MatrixMultiply)
{

    int64_t data1[2][3] = {
            {1, 2, 3},
            {4, 5, 6}
    };
    Mat2x3I64 m1(data1);

    int64_t data2[3][4] = {
            {1, 2, 3, 4},
            {5, 6, 7, 8},
            {9, 10, 11, 12}
    };

    Mat3x4I64 m2(data2);

    Mat2x4I64 result = m1.matMult(m2);

    int64_t expectedData[2][4] = {
            {38, 44, 50, 56},
            {83, 98, 113, 128}
    };
    Mat2x4I64 expected(expectedData);

    EXPECT_EQ(result, expected);
    EXPECT_NE(result, Mat2x4I64());
}