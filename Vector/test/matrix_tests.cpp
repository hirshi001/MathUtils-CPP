
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

using Vec2I = Vector<int64_t, 2>;
using Vec3I = Vector<int64_t, 3>;
using Vec4I = Vector<int64_t, 4>;


consteval Mat4x4I64 getMatrix() {
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