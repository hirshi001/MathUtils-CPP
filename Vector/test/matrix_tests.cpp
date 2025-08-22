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

using Vec2I = Vector<int64_t, 2>;
using Vec3I = Vector<int64_t, 3>;
using Vec4I = Vector<int64_t, 4>;

using Mat4x4I = Matrix<int64_t, 4, 4>;

TEST_F(MatrixTest, Equality)
{

    Mat4x4I m1_a;
    Mat4x4I m1_b;
    Mat4x4I m1_c;

    m1_a(0, 0) = 1; m1_a(0, 1) = 2; m1_a(0, 2) = 3; m1_a(0, 3) = 4;
    m1_a(1, 0) = 5; m1_a(1, 1) = 6; m1_a(1, 2) = 7; m1_a(1, 3) = 8;
    m1_a(2, 0) = 9; m1_a(2, 1) = 10; m1_a(2, 2) = 11; m1_a(2, 3) = 12;
    m1_a(3, 0) = 13; m1_a(3, 1) = 14; m1_a(3, 2) = 15; m1_a(3, 3) = 16;

    m1_b = m1_a;
    EXPECT_EQ(m1_a, m1_b);

    // Test with a different matrix
    m1_c(0, 0) = -m1_a(0, 0);
    EXPECT_NE(m1_a, m1_c);
}

TEST_F(MatrixTest, Inequality)
{
    Mat4x4I m1_a;

    m1_a(0, 0) = 1; m1_a(0, 1) = 2; m1_a(0, 2) = 3; m1_a(0, 3) = 4;
    m1_a(1, 0) = 5; m1_a(1, 1) = 6; m1_a(1, 2) = 7; m1_a(1, 3) = 8;
    m1_a(2, 0) = 9; m1_a(2, 1) = 10; m1_a(2, 2) = 11; m1_a(2, 3) = 12;
    m1_a(3, 0) = 13; m1_a(3, 1) = 14; m1_a(3, 2) = 15; m1_a(3, 3) = 16;

    // Test with a different matrix
    m1_b(0, 0) = -m1_a(0, 0);
    EXPECT_NE(m1_a, m1_b);
}