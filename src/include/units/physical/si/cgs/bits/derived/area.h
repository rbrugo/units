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

#pragma once

#ifndef MP_UNITS_SYSTEM_SI_CGS

#error "Please do not include this file directly. Use `units/physical/si/cgs/cgs.h` to prevent potential ODR violation issues."

#endif

#include <units/physical/dimensions.h>
#include <units/physical/si/cgs/base/length.h>
#include <units/physical/si/si.h>
#include <units/quantity.h>

namespace units::physical::si::cgs {

using si::square_centimetre;

struct dim_area : physical::dim_area<dim_area, square_centimetre, dim_length> {};

template<Unit U, ScalableNumber Rep = double>
using area = quantity<dim_area, U, Rep>;

inline namespace literals {

// cm2
constexpr auto operator"" _q_cm2(unsigned long long l) { return area<square_centimetre, std::int64_t>(l); }
constexpr auto operator"" _q_cm2(long double l) { return area<square_centimetre, long double>(l); }

}

}  // namespace units::physical::si::cgs
