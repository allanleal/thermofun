#ifndef GASSTP
#define GASSTP

#include "Common/ScalarTypes.hpp"

namespace ThermoFun {

struct Substance;
struct ThermoPropertiesSubstance;

auto thermoPropertiesGasSTP(Reaktoro_::Temperature t, Reaktoro_::Pressure p, Substance subst, ThermoPropertiesSubstance tps) -> ThermoPropertiesSubstance;

}


#endif // GASSTP

