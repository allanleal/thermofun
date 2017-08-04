#include "Substances/Gases/GasSTP.h"
#include "Substances/Gases/s_solmod_.h"
#include "Substance.h"
#include "ThermoProperties.h"
#include "ThermoParameters.h"

namespace ThermoFun {

auto thermoPropertiesGasSTP(Reaktoro_::Temperature t, Reaktoro_::Pressure p, Substance subst, ThermoPropertiesSubstance tps) -> ThermoPropertiesSubstance
{
    double FugProps[6];
    char Eos_Code;

    if (subst.formula() == "CO2") Eos_Code = 'C';
    if (subst.formula() == "H2O") Eos_Code = 'V';

    solmod::TSTPcalc mySTP( 1, p.val, (t.val+273.15), Eos_Code );  // modified 05.11.2010 (TW)
    double TClow = subst.thermoParameters().temperature_intervals[0][0];
    double * CPg = new double[7];
    for (unsigned int i = 0; i < 7; i++)
    {
        CPg[i] = subst.thermoParameters().critical_parameters[i];
    }

    mySTP.STPCalcFugPure( (TClow/*+273.15*/), (CPg), FugProps );

    // increment thermodynamic properties
    tps.gibbs_energy += R_CONSTANT * (t+273.15) * log( FugProps[0] );
    tps.enthalpy     += FugProps[2];
    tps.entropy      += FugProps[3];
    tps.volume        = FugProps[4];
    auto Fug = FugProps[0] * (p);
    tps.gibbs_energy -= R_CONSTANT * (t+273.15) * log(Fug/p);

    subst.checkCalcMethodBounds("STP Sterner-Pitzer fluid model", t.val, p.val, tps);

    return tps;
}
}