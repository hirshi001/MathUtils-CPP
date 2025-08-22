#include <gtest/gtest.h>

#include "MathUtils/Time/Time.h"
#include <thread>

using namespace MathUtils;

class TimeTest : public ::testing::Test
{
protected:
    void SetUp() override
    {}

    void TearDown() override
    {}
};

TEST_F(TimeTest, DefaultConstructor)
{
    Time time;
    EXPECT_EQ(time.getValue(), 0);

    Interval interval;
    EXPECT_EQ(interval.getDuration(), 0);
}

TEST_F(TimeTest, ConstructorWithValue)
{
    Time time(5);
    EXPECT_EQ(time.getValue(), 5);

    Interval interval(10.0);
    EXPECT_EQ(interval.getDuration(), 10);
}

TEST_F(TimeTest, TimeArithmetic)
{
    Time time1(5);
    Time time2(3);

    Interval interval = time1 - time2;
    EXPECT_EQ(interval.getDuration(), 2);
    EXPECT_EQ(time2 + interval, time1);
}

TEST_F(TimeTest, Waiting)
{
    Time start = TimeUtils::now<Second>();
    double delta = 0.1;
    long time = 1;
    Interval exactInterval( (double(time)) );
    Interval low = exactInterval - Interval(delta);
    Interval high = exactInterval + Interval(delta);

    // Simulate waiting
    std::this_thread::sleep_for(std::chrono::seconds(time));

    Time end = TimeUtils::now();
    Interval elapsed = end - start;

    EXPECT_GE(elapsed.getDuration(), low.getDuration());
    EXPECT_LE(elapsed.getDuration(), high.getDuration());
}

TEST_F(TimeTest, Conversion)
{
    Time time(5.0);
    Time<Millisecond> convertedTime1 = time.as<Millisecond>();
    EXPECT_EQ(convertedTime1.getValue(), 5000.0);

    Time<Millisecond> time2(5.0);
    Time convertedTime2 = time2.as<Second>();
    EXPECT_EQ(convertedTime2.getValue(), 0.005);

    Interval interval(2.0);
    Interval<Millisecond> convertedInterval1 = interval.as<Millisecond>();
    EXPECT_EQ(convertedInterval1.getDuration(), 2000.0);

    Interval<Millisecond> interval2(2000.0);
    Interval convertedInterval2 = interval2.as<Second>();
    EXPECT_EQ(convertedInterval2.getDuration(), 2.0);
}

TEST_F(TimeTest, Comparison)
{
    Time time1(5.0);
    Time time2(3.0);

    EXPECT_TRUE(time1 > time2);
    EXPECT_FALSE(time1 < time2);
    EXPECT_TRUE(time1 >= time2);
    EXPECT_FALSE(time1 <= time2);

    Interval interval1(5.0);
    Interval interval2(3.0);

    EXPECT_TRUE(interval1 > interval2);
    EXPECT_FALSE(interval1 < interval2);
    EXPECT_TRUE(interval1 >= interval2);
    EXPECT_FALSE(interval1 <= interval2);
}

TEST_F(TimeTest, Equality)
{
    Time time1(5.0);
    Time time2(5.0);
    Time time3(3.0);

    EXPECT_TRUE(time1 == time2);
    EXPECT_FALSE(time1 == time3);
    EXPECT_FALSE(time1 != time2);
    EXPECT_TRUE(time1 != time3);

    Interval interval1(5.0);
    Interval interval2(5.0);
    Interval interval3(3.0);

    EXPECT_TRUE(interval1 == interval2);
    EXPECT_FALSE(interval1 == interval3);
    EXPECT_FALSE(interval1 != interval2);
    EXPECT_TRUE(interval1 != interval3);
}

