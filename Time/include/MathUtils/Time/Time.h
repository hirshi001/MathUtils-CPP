//
// Created by Hrishikesh Ingle on 8/21/25.
//

#pragma once

#include <cstdlib>
#include <cassert>
#include <chrono>

namespace MathUtils
{

// Note: Each TimeUnit should differ by a factor of 1000 from the previous one.
enum TimeUnit : uint8_t
{
    Attosecond = 0,
    Femtosecond,
    Picosecond,
    Nanosecond,
    Microsecond,
    Millisecond,
    Second
};

using DefaultTimePrecision = double;
constexpr TimeUnit DefaultTimeUnit = TimeUnit::Second;


namespace detail
{

template<TimeUnit Unit>
consteval auto map_unit()
{
    if constexpr (Unit == TimeUnit::Second) {
        return std::chrono::seconds();
    } else if constexpr (Unit == TimeUnit::Millisecond) {
        return std::chrono::milliseconds();
    } else if constexpr (Unit == TimeUnit::Microsecond) {
        return std::chrono::microseconds();
    } else if constexpr (Unit == TimeUnit::Nanosecond) {
        return std::chrono::nanoseconds();
    } else if constexpr (Unit == TimeUnit::Picosecond) {
        return std::chrono::duration<uint64_t, std::pico>();
    } else if constexpr (Unit == TimeUnit::Femtosecond) {
        return std::chrono::duration<uint64_t, std::femto>();
    } else if constexpr (Unit == TimeUnit::Attosecond) {
        return std::chrono::duration<uint64_t, std::atto>();
    } else {
        static_assert(Unit >= TimeUnit::Second, "Invalid TimeUnit specified.");
    }
}

template<TimeUnit fromUnit, TimeUnit toUnit>
consteval uint64_t conversionFactor()
{
    static_assert(fromUnit >= toUnit,
                  "Conversion factor can only be calculated for units that are smaller in length compared to the fromUnit.");

    if constexpr (toUnit == fromUnit) {
        return 1;
    } else {
        return 1000ULL * conversionFactor<static_cast<TimeUnit>(fromUnit - 1), toUnit>();
    }
}


// Additional methods for conversion, arithmetic, etc. can be added here.
template<TimeUnit fromUnit, TimeUnit toUnit, typename T>
constexpr T convert(T value)
{
    static_assert(std::is_arithmetic<T>::value, "Time's template parameter T must be a numerical type.");
    if constexpr (fromUnit == toUnit) {
        return value;
    }

    if constexpr (toUnit > fromUnit) {
        return value / conversionFactor<toUnit, fromUnit>();
    } else {
        return value * conversionFactor<fromUnit, toUnit>();
    }

    assert(false && "Invalid TimeUnit for conversion to seconds.");
    return value;
}

}

template<TimeUnit Unit = DefaultTimeUnit, typename T = DefaultTimePrecision>
class Interval
{

    static_assert(std::is_arithmetic<T>::value, "Time's template parameter T must be a numerical type.");

private:
    T duration;

public:
    constexpr Interval() : duration(0)
    {}

    constexpr explicit Interval(T value) : duration(value)
    {}

    template<typename Rep, typename Period>
    constexpr explicit Interval(const std::chrono::duration<Rep, Period> &d)
    {
        // Convert the incoming duration to our specific unit.
        auto converted_duration = std::chrono::duration_cast<decltype(detail::map_unit<Unit>())>(d);

        // Assign the converted duration's count to our duration member.
        duration = static_cast<T>(converted_duration.count());
    }

    [[nodiscard]] constexpr T getDuration() const
    {
        return duration;
    }

    [[nodiscard]] constexpr TimeUnit getUnit() const
    {
        return Unit;
    }

    // convert to std::chrono::duration
    [[nodiscard]] constexpr explicit operator std::chrono::duration<T, decltype(detail::map_unit<Unit>())>() const
    {
        // Assert that the value type T is compatible with the underlying duration type.
        static_assert(std::is_convertible_v<T, typename decltype(detail::map_unit<Unit>())::rep>,
                      "Conversion from Time's value type to std::chrono's rep type is not possible.");

        // Return a time_point from the duration relative to the system clock's epoch.
        return std::chrono::time_point<std::chrono::system_clock, decltype(detail::map_unit<Unit>())>(duration);
    }

    // Additional methods for arithmetic operations, comparisons, etc. can be added here.
    template<TimeUnit toUnit>
    [[nodiscard]] constexpr Interval<toUnit, T> as() const
    {
        return Interval<toUnit, T>(detail::convert<Unit, toUnit, T>(duration));
    }

    constexpr Interval &operator=(const Interval<Unit, T> &other) = default;

    constexpr Interval &operator=(Interval<Unit, T> &&other) noexcept = default;

    template<TimeUnit OtherUnit>
    constexpr bool operator==(const Interval<OtherUnit, T> &other) const
    {
        return duration == detail::convert<Unit, OtherUnit, T>(other.duration);
    }

    template<TimeUnit OtherUnit>
    constexpr bool operator!=(const Interval<OtherUnit, T> &other) const
    {
        return duration != detail::convert<Unit, OtherUnit, T>(other.duration);
    }

    template<TimeUnit OtherUnit>
    constexpr bool operator<(const Interval<OtherUnit, T> &other) const
    {
        return duration < detail::convert<Unit, OtherUnit, T>(other.duration);
    }

    template<TimeUnit OtherUnit>
    constexpr bool operator<=(const Interval<OtherUnit, T> &other) const
    {
        return duration <= detail::convert<Unit, OtherUnit, T>(other.duration);
    }

    template<TimeUnit OtherUnit>
    constexpr bool operator>(const Interval<OtherUnit, T> &other) const
    {
        return duration > detail::convert<Unit, OtherUnit, T>(other.duration);
    }

    template<TimeUnit OtherUnit>
    constexpr bool operator>=(const Interval<OtherUnit, T> &other) const
    {
        return duration >= detail::convert<Unit, OtherUnit, T>(other.duration);
    }

    constexpr Interval operator+(const Interval &other) const
    {
        return Interval(duration + other.duration);
    }

    constexpr Interval operator-(const Interval &other) const
    {
        return Interval(duration - other.duration);
    }

    constexpr Interval operator*(T value) const
    {
        return Interval(duration * value);
    }

    constexpr Interval operator/(T value) const
    {
        assert(value != 0 && "Division by zero in Interval.");
        return Interval(duration / value);
    }

    constexpr Interval &operator+=(const Interval &other)
    {
        duration += other.duration;
        return *this;
    }

    constexpr Interval &operator-=(const Interval &other)
    {
        duration -= other.duration;
        return *this;
    }

    constexpr Interval &operator*=(T value)
    {
        duration *= value;
        return *this;
    }

    constexpr Interval &operator/=(T value)
    {
        assert(value != 0 && "Division by zero in Interval.");
        duration /= value;
        return *this;
    }
};

template<TimeUnit Unit = DefaultTimeUnit, typename T = DefaultTimePrecision>
class Time
{

    static_assert(std::is_arithmetic<T>::value, "Time's template parameter T must be a numerical type.");

private:
    T timePoint;

public:
    constexpr Time() : timePoint(0)
    {}

    constexpr explicit Time(T value) : timePoint(value)
    {}

    constexpr explicit Time(std::chrono::time_point<std::chrono::system_clock, decltype(detail::map_unit<Unit>())> tp)
    {
        // Assert that the value type T is compatible with the underlying duration type.
        static_assert(std::is_convertible_v<T, typename decltype(detail::map_unit<Unit>())::rep>,
                      "Conversion from Time's value type to std::chrono's rep type is not possible.");

        // Convert the time_point to our timePoint value.
        timePoint = static_cast<T>(tp.time_since_epoch().count());
    }

    [[nodiscard]] constexpr T getValue() const
    {
        return timePoint;
    }

    [[nodiscard]] constexpr TimeUnit getUnit() const
    {
        return Unit;
    }

    // convert to std::chrono::time_point
    // A conversion operator that allows implicit conversion from MathTime to std::chrono::time_point.
    [[nodiscard]] constexpr explicit operator std::chrono::time_point<std::chrono::system_clock, decltype(detail::map_unit<Unit>())>() const
    {
        // Assert that the value type T is compatible with the underlying duration type.
        static_assert(std::is_convertible_v<T, typename decltype(detail::map_unit<Unit>())::rep>,
                      "Conversion from Time's value type to std::chrono's rep type is not possible.");

        // Create the duration from our timePoint value.
        auto duration = decltype(detail::map_unit<Unit>())(timePoint);

        // Return a time_point from the duration relative to the system clock's epoch.
        return std::chrono::time_point<std::chrono::system_clock, decltype(detail::map_unit<Unit>())>(duration);
    }

    // Additional methods for conversion, arithmetic, etc. can be added here.
    template<TimeUnit toUnit>
    [[nodiscard]] constexpr Time<toUnit, T> as() const
    {
        return Time<toUnit, T>(detail::convert<Unit, toUnit, T>(timePoint));
    }

    constexpr Time &operator=(const Time<Unit, T> &other) = default;

    constexpr Time &operator=(Time<Unit, T> &&other) noexcept = default;

    template<TimeUnit OtherUnit>
    constexpr bool operator==(const Time<OtherUnit, T> &other) const
    {
        return timePoint == detail::convert<Unit, OtherUnit, T>(other.timePoint);
    }

    template<TimeUnit OtherUnit>
    constexpr bool operator!=(const Time<OtherUnit, T> &other) const
    {
        return timePoint != detail::convert<Unit, OtherUnit, T>(other.timePoint);
    }

    template<TimeUnit OtherUnit>
    constexpr bool operator<(const Time<OtherUnit, T> &other) const
    {
        return timePoint < detail::convert<Unit, OtherUnit, T>(other.timePoint);
    }

    template<TimeUnit OtherUnit>
    constexpr bool operator<=(const Time<OtherUnit, T> &other) const
    {
        return timePoint <= detail::convert<Unit, OtherUnit, T>(other.timePoint);
    }

    template<TimeUnit OtherUnit>
    constexpr bool operator>(const Time<OtherUnit, T> &other) const
    {
        return timePoint > detail::convert<Unit, OtherUnit, T>(other.timePoint);
    }

    template<TimeUnit OtherUnit>
    constexpr bool operator>=(const Time<OtherUnit, T> &other) const
    {
        return timePoint >= detail::convert<Unit, OtherUnit, T>(other.timePoint);
    }

    constexpr Time operator+(const Interval<Unit, T> &other) const
    {
        return Time(timePoint + other.getDuration());
    }

    constexpr Time operator-(const Interval<Unit, T> &other) const
    {
        return Time(timePoint - other.getDuration());
    }

    constexpr Time &operator+=(const Interval<Unit, T> &other)
    {
        timePoint += other.getDuration();
        return *this;
    }

    constexpr Time &operator-=(const Interval<Unit, T> &other)
    {
        timePoint -= other.getDuration();
        return *this;
    }

    constexpr Interval<Unit, T> operator-(const Time &other) const
    {
        return Interval(timePoint - other.timePoint);
    }
};


namespace TimeUtils
{

template<TimeUnit AsUnit = DefaultTimeUnit, TimeUnit FromUnit = DefaultTimeUnit, typename T = DefaultTimePrecision>
constexpr Interval<AsUnit, T> interval(T value)
{
    static_assert(std::is_arithmetic<T>::value, "Time template parameter T must be a numerical type.");
    return Interval<AsUnit, T>(MathUtils::detail::convert<FromUnit, AsUnit, T>(value));
}

template<TimeUnit AsUnit = DefaultTimeUnit, typename T = DefaultTimePrecision>
Time<AsUnit, T> now()
{
    static_assert(std::is_arithmetic<T>::value, "Time template parameter T must be a numerical type.");

    // Get the current time point with the highest possible resolution.
    auto currentTimePoint = std::chrono::high_resolution_clock::now();

    // Use duration_cast to convert the time point's duration to the target unit.
    // We use our helper function to get the correct std::chrono::duration type.
    auto duration = std::chrono::duration_cast<decltype(MathUtils::detail::map_unit<AsUnit>())>(
            currentTimePoint.time_since_epoch());

    // Return a new MathTime object, initialized with the count of the duration.
    return Time<AsUnit, T>(static_cast<T>(duration.count()));
}

}
}