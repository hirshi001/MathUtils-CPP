#define USING_INT64_VECTOR_TYPES

#include <gtest/gtest.h>
#include "MathUtils/Vector/Vector.h"

using namespace MathUtils;

class VectorTest : public ::testing::Test
{
protected:
    void SetUp() override
    {}

    void TearDown() override
    {}
};


TEST_F(VectorTest, Equality)
{
    Vec2I64 v1_a(1, 2);
    Vec2I64 v1_b(1, 2);
    Vec3I64 v1_c(1, 2, 0);

    EXPECT_EQ(v1_a, v1_b);
    EXPECT_EQ(v1_a, v1_c);
    EXPECT_EQ(v1_c, v1_a);
}

TEST_F(VectorTest, Inequality)
{
    Vec2I64 v1_a(1, 2);
    Vec3I64 v1_b(1, 2, 0);
    Vec2I64 v2(3, 4);

    EXPECT_NE(v1_a, v2);
    EXPECT_NE(v1_b, v2);
}


TEST_F(VectorTest, Addition)
{
    Vec3I64 expected(4, 6, 5);

    Vec2I64 v1(1, 2);
    Vec3I64 v2(3, 4, 5);

    Vec3I64 r1 = v1 + v2;
    EXPECT_EQ(r1, expected);

    Vec3I64 r2 = v2 + v1;
    EXPECT_EQ(r2, expected);
}

TEST_F(VectorTest, Subtraction)
{
    Vec3I64 expected(2, 2, 5);

    Vec2I64 v1(1, 2);
    Vec3I64 v2(3, 4, 5);

    Vec3I64 r1 = v2 - v1;
    EXPECT_EQ(r1, expected);

    Vec3I64 r2 = v1 - v2;
    EXPECT_EQ(r2, -expected);
}

TEST_F(VectorTest, Multiplication)
{
    Vec3I64 expected(3, 8, 0);

    Vec2I64 v1(1, 2);
    Vec3I64 v2(3, 4, 5);

    Vec3I64 r1 = v1 * v2;
    EXPECT_EQ(r1, expected);

    Vec3I64 r2 = v2 * v1;
    EXPECT_EQ(r2, expected);
}

TEST_F(VectorTest, Division)
{
    Vec3I64 expected(2, 10, 0);

    Vec2I64 v1(10, 20);
    Vec3I64 v2(5, 2, 535);

    Vec3I64 r1 = v1 / v2;
    EXPECT_EQ(r1, expected);
}

TEST_F(VectorTest, ScalarAddition)
{
    Vec3I64 expected(4, 5, 6);

    Vec3I64 v(3, 4, 5);

    Vec3I64 result = v + 1;
    EXPECT_EQ(result, expected);
}

TEST_F(VectorTest, ScalarSubtraction)
{
    Vec3I64 expected(2, 3, 4);

    Vec3I64 v(3, 4, 5);

    Vec3I64 result = v - 1;
    EXPECT_EQ(result, expected);
}

TEST_F(VectorTest, ScalarMultiplication)
{
    Vec3I64 expected(6, 8, 10);

    Vec3I64 v(3, 4, 5);

    Vec3I64 result = v * 2;
    EXPECT_EQ(result, expected);
}

TEST_F(VectorTest, ScalarDivision)
{
    Vec3I64 expected(1, 2, 2);

    Vec3I64 v(3, 6, 8);

    Vec3I64 result = v / 3;
    EXPECT_EQ(result, expected);
}

TEST_F(VectorTest, AdditionAssignment)
{
    Vec2I64 expected(4, 6);

    Vec2I64 v1(1, 2);
    Vec2I64 v2(3, 4);

    v2 += v1;
    EXPECT_EQ(v2, expected);
}

TEST_F(VectorTest, AdditionAssignmentBigger)
{
    Vec3I64 expected(5, 7, 3);

    Vec3I64 v1(1, 2, 3);
    Vec2I64 v2(4, 5);

    v1 += v2;
    EXPECT_EQ(v1, expected);
}

TEST_F(VectorTest, SubtractionAssignment)
{
    Vec2I64 expected(2, 2);

    Vec2I64 v1(5, 6);
    Vec2I64 v2(3, 4);

    v1 -= v2;
    EXPECT_EQ(v1, expected);
}

TEST_F(VectorTest, SubtractionAssignmentBigger)
{
    Vec3I64 expected(2, 2, 3);

    Vec3I64 v1(5, 6, 3);
    Vec2I64 v2(3, 4);

    v1 -= v2;
    EXPECT_EQ(v1, expected);
}

TEST_F(VectorTest, MultiplicationAssignment)
{
    Vec2I64 expected(3, 8);

    Vec2I64 v1(1, 2);
    Vec2I64 v2(3, 4);

    v1 *= v2;
    EXPECT_EQ(v1, expected);
}

TEST_F(VectorTest, MultiplicationAssignmentBigger)
{
    Vec3I64 expected(3, 8, 0);

    Vec3I64 v1(1, 2, 0);
    Vec2I64 v2(3, 4);

    v1 *= v2;
    EXPECT_EQ(v1, expected);
}

TEST_F(VectorTest, DivisionAssignment)
{
    Vec2I64 expected(1, 2);

    Vec2I64 v1(3, 4);
    Vec2I64 v2(3, 2);

    v1 /= v2;
    EXPECT_EQ(v1, expected);
}

TEST_F(VectorTest, ScalarAdditionAssignment)
{
    Vec3I64 expected(4, 5, 6);

    Vec3I64 v(3, 4, 5);

    v += 1;
    EXPECT_EQ(v, expected);
}

TEST_F(VectorTest, ScalarSubtractionAssignment)
{
    Vec3I64 expected(2, 3, 4);

    Vec3I64 v(3, 4, 5);

    v -= 1;
    EXPECT_EQ(v, expected);
}

TEST_F(VectorTest, ScalarMultiplicationAssignment)
{
    Vec3I64 expected(6, 8, 10);

    Vec3I64 v(3, 4, 5);

    v *= 2;
    EXPECT_EQ(v, expected);
}

TEST_F(VectorTest, ScalarDivisionAssignment)
{
    Vec3I64 expected(1, 2, 2);

    Vec3I64 v(3, 6, 8);

    v /= 3;
    EXPECT_EQ(v, expected);
}

TEST_F(VectorTest, DotProduct)
{
    Vec3I64 v1(1, 2, 3);
    Vec3I64 v2(4, 5, 6);

    int64_t expected = 32; // 1*4 + 2*5 + 3*6
    int64_t result = v1.dot(v2);
    EXPECT_EQ(result, expected);
}

TEST_F(VectorTest, CrossProduct)
{
    Vec3I64 v1(1, 2, 3);
    Vec3I64 v2(4, 5, 6);

    Vec3I64 expected(-3, 6, -3); // Cross product result
    Vec3I64 result = v1.cross(v2);
    EXPECT_EQ(result, expected);
}

TEST_F(VectorTest, Swap)
{
    Vec3I64 v(1, 2, 3);
    Vec3I64 expected(2, 1, 3);

    v.swap(0, 1);
    EXPECT_EQ(v, expected);

    const Vec3I64 v_const(1, 2, 3);
    Vec3I64 result = v_const.swap(0, 1);
}

TEST_F(VectorTest, LessThan)
{
    Vec3I64 v1(1, 2, 3);
    Vec3I64 v2(4, 5, 6);

    EXPECT_TRUE(v1 < v2);
    EXPECT_FALSE(v2 < v1);

    EXPECT_TRUE(v1 < Vec4I64(1, 2, 3, 4));
    EXPECT_FALSE(v1 < Vec4I64(1, 2, 3, 0));

    EXPECT_FALSE(v1 < Vec2I64(1, 2));

    EXPECT_TRUE(Vec4I64(1, 2, 3, -1) < v1);
}

TEST_F(VectorTest, LessThanOrEqual)
{
    Vec3I64 v1(1, 2, 3);
    Vec3I64 v2(4, 5, 6);

    EXPECT_TRUE(v1 <= v2);
    EXPECT_FALSE(v2 <= v1);

    EXPECT_TRUE(v1 <= Vec4I64(1, 2, 3, 4));
    EXPECT_TRUE(v1 <= Vec4I64(1, 2, 3, 0));

    EXPECT_FALSE(v1 <= Vec2I64(1, 2));
}