
#pragma once

#include <units/physical/si/si.h>

namespace units{
namespace experimental{

namespace force {

template<typename Rep = double>
using N = units::physical::si::force<units::physical::si::newton, Rep>;

}

}} // units::experimental
