// The MIT License (MIT)
//
// Copyright (c) 2018 Mateusz Pusz
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "test_tools.h"
#include "units/math.h"
#include "units/physical/si/si.h"
#include "units/physical/si/us/us.h"
#include <chrono>
#include <utility>

namespace {

using namespace units;
using namespace units::physical::si;

// class invariants

template<typename DimLength>
concept invalid_types = requires
{
  !requires { typename quantity<DimLength, second, int>; };  // unit of a different dimension
  !requires { typename quantity<DimLength, metre, quantity<DimLength, metre, int>>; };  // quantity used as Rep
  !requires { typename quantity<metre, DimLength, double>; };  // reordered arguments
};

static_assert(invalid_types<dim_length>);

// member types

static_assert(is_same_v<length<metre, int>::rep, int>);
static_assert(is_same_v<length<metre, double>::rep, double>);
static_assert(is_same_v<length<metre, int>::unit, metre>);
static_assert(is_same_v<length<kilometre, int>::unit, kilometre>);

// constructors

static_assert(length<metre, int>().count() == 0);
constexpr length<metre, int> km{1000};
static_assert(km.count() == 1000);
static_assert(length<metre, int>(km).count() == km.count());

static_assert(length<metre, int>(1).count() == 1);
static_assert(!std::is_constructible_v<length<metre, int>, double>); // truncating conversion
static_assert(length<metre, double>(1.0).count() == 1.0);
static_assert(length<metre, double>(1).count() == 1.0);
static_assert(length<metre, double>(3.14).count() == 3.14);

static_assert(length<metre, int>(km).count() == 1000);
static_assert(!std::is_constructible_v<length<metre, int>,
                                       length<metre, double>>);  // truncating conversion
static_assert(length<metre, double>(1000.0_q_m).count() == 1000.0);
static_assert(length<metre, double>(km).count() == 1000.0);
static_assert(length<metre, int>(1_q_km).count() == 1000);
static_assert(!std::is_constructible_v<length<metre, int>,
                                       physical::si::time<second, int>>);  // different dimensions
static_assert(!std::is_constructible_v<length<kilometre, int>,
                                       length<metre, int>>);  // truncating conversion

// assignment operator

static_assert([]() { length<metre, int> l1(1), l2(2); return l2 = l1; }().count() == 1);

// static member functions

static_assert(length<metre, int>::zero().count() == 0);
static_assert(length<metre, int>::min().count() == std::numeric_limits<int>::lowest());
static_assert(length<metre, int>::max().count() == std::numeric_limits<int>::max());
static_assert(length<metre, double>::zero().count() == 0.0);
static_assert(length<metre, double>::min().count() == std::numeric_limits<double>::lowest());
static_assert(length<metre, double>::max().count() == std::numeric_limits<double>::max());

// unary member operators

static_assert((+km).count() == 1000);
static_assert((-km).count() == -1000);
static_assert((+(-km)).count() == -1000);
static_assert((-(-km)).count() == 1000);

static_assert([](auto v) {
  auto vv = v++;
  return std::pair(v, vv);
}(km) == std::pair(length<metre, int>(1001), length<metre, int>(1000)));
static_assert([](auto v) {
  auto vv = ++v;
  return std::pair(v, vv);
}(km) == std::pair(length<metre, int>(1001), length<metre, int>(1001)));
static_assert([](auto v) {
  auto vv = v--;
  return std::pair(v, vv);
}(km) == std::pair(length<metre, int>(999), length<metre, int>(1000)));
static_assert([](auto v) {
  auto vv = --v;
  return std::pair(v, vv);
}(km) == std::pair(length<metre, int>(999), length<metre, int>(999)));

// compound assignment

static_assert((1_q_m += 1_q_m).count() == 2);
static_assert((2_q_m -= 1_q_m).count() == 1);
static_assert((1_q_m *= 2).count() == 2);
static_assert((2_q_m /= 2).count() == 1);
static_assert((7_q_m %= 2).count() == 1);
static_assert((7_q_m %= 2_q_m).count() == 1);

static_assert((2.5_q_m += 3_q_m).count() == 5.5);
static_assert((2.5_q_m += 3.5_q_m).count() == 6);

static_assert((2.5_q_m *= 3).count() == 7.5);
static_assert((2.5_q_m *= 3.5).count() == 8.75);

// operations not allowed for the respective quantities
template<typename Metre>
concept invalid_compound_assignments = requires()
{
    !requires(length<Metre, double> l) { l %= 2.; };
    !requires(length<Metre, double> l) { l %= 2; };
    !requires(length<Metre, int> l) { l %= 2.; };
    !requires(length<Metre, double> l) { l %= 2._q_m; };
    !requires(length<Metre, double> l) { l %= 2_q_m; };
    !requires(length<Metre, int> l) { l %= 2._q_m; };
    !requires(length<Metre, int> l) { l += 3.5_q_m; };
    !requires(length<Metre, int> l) { l *= 3.5_q_m; };
};

static_assert(invalid_compound_assignments<metre>);

// non-member arithmetic operators

static_assert(compare<decltype(length<metre, int>() + length<metre, double>()), length<metre, double>>);
static_assert(compare<decltype(length<metre, int>() + length<metre, double>()), length<metre, double>>);
static_assert(compare<decltype(length<kilometre, int>() + length<metre, double>()), length<metre, double>>);
static_assert(compare<decltype(length<metre, double>() - length<metre, int>()), length<metre, double>>);
static_assert(compare<decltype(length<kilometre, double>() - length<metre, int>()), length<metre, double>>);
static_assert(compare<decltype(length<metre, int>() * 1.0), length<metre, double>>);
static_assert(compare<decltype(1.0 * length<metre, int>()), length<metre, double>>);
static_assert(
    compare<decltype(speed<metre_per_second, int>() * physical::si::time<second, int>()), length<metre, int>>);
static_assert(
    compare<decltype(speed<metre_per_second, int>() * physical::si::time<hour, int>()), length<scaled_unit<ratio(36, 1, 2), metre>, int>>);
static_assert(compare<decltype(length<metre>() * physical::si::time<minute>()),
              quantity<unknown_dimension<units::exponent<dim_length, 1>, units::exponent<dim_time, 1>>, scaled_unit<ratio(6, 1, 1), unknown_coherent_unit>>>);
static_assert(compare<decltype(1 / physical::si::time<second, int>()), frequency<hertz, int>>);
static_assert(compare<decltype(1 / physical::si::time<minute, int>()), frequency<scaled_unit<ratio(1, 6, -1), hertz>, int>>);
static_assert(compare<decltype(1 / frequency<hertz, int>()), physical::si::time<second, int>>);
static_assert(compare<decltype(1 / length<kilometre>()),
              quantity<unknown_dimension<units::exponent<dim_length, -1>>, scaled_unit<ratio(1, 1, -3), unknown_coherent_unit>>>);
static_assert(compare<decltype(length<metre, int>() / 1.0), length<metre, double>>);
static_assert(compare<decltype(length<metre, int>() / length<metre, double>()), dimensionless<one, double>>);
static_assert(compare<decltype(length<kilometre, int>() / length<metre, double>()), dimensionless<scaled_unit<ratio(1, 1, 3), one>, double>>);
static_assert(
    compare<decltype(length<metre, int>() / physical::si::time<second, int>()), speed<metre_per_second, int>>);
static_assert(
    compare<decltype(length<metre>() / physical::si::time<minute>()), speed<scaled_unit<ratio(1, 6, -1), metre_per_second>>>);
static_assert(compare<decltype(physical::si::time<minute>() / length<metre>()),
              quantity<unknown_dimension<units::exponent<dim_length, -1>, units::exponent<dim_time, 1>>, scaled_unit<ratio(6 ,1 , 1), unknown_coherent_unit>>>);
static_assert(compare<decltype(length<metre, int>() % short(1)), length<metre, int>>);
static_assert(compare<decltype(length<metre, int>() % length<metre, short>(1)), length<metre, int>>);

static_assert((1_q_m + km).count() == 1001);
static_assert((1_q_m + 1_q_km).count() == 1001);
static_assert((km - 1_q_m).count() == 999);
static_assert((1_q_km - 1_q_m).count() == 999);
static_assert((2_q_m * 2).count() == 4);
static_assert((3 * 3_q_m).count() == 9);
static_assert((4_q_m / 2).count() == 2);
static_assert((4_q_km / 2_q_m).count() == 2);
static_assert((4000_q_m / 2_q_m).count() == 2000);
static_assert((7_q_m % 2).count() == 1);
static_assert((7_q_m % 2_q_m).count() == 1);
static_assert((7_q_km % 2000_q_m).count() == 1000);

static_assert((10_q_km2 * 10_q_km2) / 50_q_km2 == 2_q_km2);

constexpr auto q1 = 10_q_km / 5_q_m;
static_assert(compare<decltype(q1), const dimensionless<scaled_unit<ratio(1, 1, 3), one>, std::int64_t>>);
static_assert(q1.count() == 2);

constexpr dimensionless<one> q2 = q1;
static_assert(q2.count() == 2000);

#if DOWNCAST_MODE == 0
static_assert(quantity_cast<dim_one, one>(q1).count() == 2000);
#else
static_assert(quantity_cast<one>(q1).count() == 2000);
#endif

constexpr auto q3 = 10_q_s * 2_q_kHz;
static_assert(compare<decltype(q3), const dimensionless<scaled_unit<ratio(1, 1, 3), one>, std::int64_t>>);
static_assert(q3.count() == 20);

// comparators

static_assert(2_q_m + 1_q_m == 3_q_m);
static_assert(!(2_q_m + 2_q_m == 3_q_m));
static_assert(2_q_m + 2_q_m != 3_q_m);
static_assert(!(2_q_m + 2_q_m != 4_q_m));
static_assert(2_q_m > 1_q_m);
static_assert(!(1_q_m > 1_q_m));
static_assert(1_q_m < 2_q_m);
static_assert(!(2_q_m < 2_q_m));
static_assert(2_q_m >= 1_q_m);
static_assert(2_q_m >= 2_q_m);
static_assert(!(2_q_m >= 3_q_m));
static_assert(1_q_m <= 2_q_m);
static_assert(2_q_m <= 2_q_m);
static_assert(!(3_q_m <= 2_q_m));

static_assert(3_q_m == 3.0_q_m);
static_assert(3_q_m != 3.14_q_m);
static_assert(2_q_m > 1.0_q_m);
static_assert(1.0_q_m < 2_q_m);
static_assert(2.0_q_m >= 1_q_m);
static_assert(1_q_m <= 2.0_q_m);

static_assert(1000_q_m == 1_q_km);
static_assert(1001_q_m != 1_q_km);
static_assert(1001_q_m > 1_q_km);
static_assert(999_q_m < 1_q_km);
static_assert(1000_q_m >= 1_q_km);
static_assert(1000_q_m <= 1_q_km);

// alias units

static_assert(2_q_l + 2_q_ml == 2002_q_ml);
static_assert(2_q_l + 2_q_ml == 2002_q_cm3);
static_assert(2_q_l + 2_q_cm3 == 2002_q_ml);
static_assert(2_q_dm3 + 2_q_cm3 == 2002_q_ml);

// is_quantity

static_assert(Quantity<length<millimetre, int>>);

// common_quantity

static_assert(compare<common_quantity<length<metre, int>, length<kilometre, int>>, length<metre, int>>);
static_assert(compare<common_quantity<length<kilometre, long long>, length<metre, int>>, length<metre, long long>>);
static_assert(
    compare<common_quantity<length<kilometre, long long>, length<millimetre, double>>, length<millimetre, double>>);

// common_type

using namespace units::physical::si::us::literals;

static_assert(std::equality_comparable<decltype(1_q_m)>);
static_assert(std::equality_comparable_with<decltype(1_q_m), decltype(1_q_cm)>);
static_assert(0_q_m == 0_q_ft_us);
static_assert(std::equality_comparable_with<decltype(1_q_m), decltype(1_q_ft_us)>);
static_assert(std::equality_comparable_with<dimensionless<one>, int>);
static_assert(std::equality_comparable_with<dimensionless<one>, double>);
static_assert(std::equality_comparable_with<dimensionless<one, int>, int>);
static_assert(!std::equality_comparable_with<dimensionless<one, int>, double>);

// quantity_cast

static_assert(compare<decltype(quantity_cast<scaled_unit<ratio(1), metre>>(2_q_km))::unit, metre>);

static_assert(quantity_cast<length<metre, int>>(2_q_km).count() == 2000);
static_assert(quantity_cast<length<kilometre, int>>(2000_q_m).count() == 2);
static_assert(quantity_cast<length<metre, int>>(1.23_q_m).count() == 1);
static_assert(quantity_cast<metre>(2_q_km).count() == 2000);
static_assert(quantity_cast<kilometre>(2000_q_m).count() == 2);
static_assert(quantity_cast<int>(1.23_q_m).count() == 1);
static_assert(quantity_cast<dim_speed, kilometre_per_hour>(2000.0_q_m / 3600.0_q_s).count() == 2);

// dimensionless

static_assert(std::is_convertible_v<double, dimensionless<one>>);
static_assert(std::is_convertible_v<float, dimensionless<one>>);
static_assert(!std::is_convertible_v<double, dimensionless<one, int>>);
static_assert(std::is_convertible_v<int, dimensionless<one>>);

static_assert(!std::is_convertible_v<double, dimensionless<scaled_unit<ratio(1, 1, 1), one>>>);
static_assert(std::is_constructible_v<dimensionless<scaled_unit<ratio(1, 1, 1), one>>, double>);

static_assert(dimensionless<one>(1.23) + dimensionless<one>(1.23) == dimensionless<one>(2.46));
static_assert(dimensionless<one>(1.23) + dimensionless<one>(1.23) == 2.46);
static_assert(dimensionless<one>(1.23) + 1.23 == 2.46);
static_assert(1.23 + dimensionless<one>(1.23) == 2.46);
static_assert(dimensionless<one>(1) + 1 == 2);
static_assert(dimensionless<one, int>(1) + 1 == 2);

template<typename Int>
concept invalid_dimensionless_operations = requires
{
    !requires(dimensionless<one, Int> d) { d + 1.23; };
    !requires(dimensionless<one, Int> d) { 1.23 + d; };
    !requires(dimensionless<scaled_unit<ratio(1, 1, 1), one>, Int> d) { 1 + d; };
    !requires(dimensionless<scaled_unit<ratio(1, 1, 1), one>, Int> d) { d + 1; };
};
static_assert(invalid_dimensionless_operations<int>);

static_assert(compare<decltype(10_q_km / 5_q_km), quantity<dim_one, one, std::int64_t>>);

#if DOWNCAST_MODE == 0
static_assert(quantity_cast<dim_one, percent>(50._q_m / 100._q_m).count() == 50);
#else
static_assert(quantity_cast<percent>(50._q_m / 100._q_m).count() == 50);
#endif
static_assert(50._q_m / 100._q_m == dimensionless<percent>(50));

static_assert(dimensionless<one>(dimensionless<percent>(50)).count() == 0.5);

// time

static_assert(1_q_h == 3600_q_s);

template<typename Metre>
concept no_crossdimensional_equality = !requires { 1_q_s == length<Metre, int>(1); };

static_assert(no_crossdimensional_equality<metre>);

// length

static_assert(1_q_km == 1000_q_m);
static_assert(1_q_km + 1_q_m == 1001_q_m);
static_assert(10_q_km / 5_q_km == 2);
static_assert(10_q_km / 2 == 5_q_km);

// speed

static_assert(10_q_m / 5_q_s == 2_q_m_per_s);
static_assert(10 / 5_q_s * 1_q_m == 2_q_m_per_s);
static_assert(1_q_km / 1_q_s == 1000_q_m_per_s);
static_assert(2_q_km_per_h * 2_q_h == 4_q_km);
static_assert(2_q_km / 2_q_km_per_h == 1_q_h);

static_assert(compare<decltype(pow<2>(2_q_m)), decltype(4_q_m2)>);

// downcasting

#if DOWNCAST_MODE == 0

static_assert(std::is_same_v<decltype(10_q_m / 5_q_s), quantity<unknown_dimension<units::exponent<dim_length, 1>, units::exponent<dim_time, -1>>, scaled_unit<ratio(1), unknown_coherent_unit>, std::int64_t>>);
static_assert(std::is_same_v<decltype(1_q_mm + 1_q_km), length<scaled_unit<ratio(1, 1, -3), metre>, std::int64_t>>);

#else

static_assert(std::is_same_v<decltype(10_q_m / 5_q_s), speed<metre_per_second, std::int64_t>>);
static_assert(std::is_same_v<decltype(1_q_mm + 1_q_km), length<millimetre, std::int64_t>>);

#endif

}  // namespace
