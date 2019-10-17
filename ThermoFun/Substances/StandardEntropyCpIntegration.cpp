#include "StandardEntropyCpIntegration.h"
#include "ThermoProperties.h"
#include "Substance.h"

namespace ThermoFun {

auto thermoPropertiesEntropyCpIntegration(Reaktoro_::Temperature TK, Reaktoro_::Pressure /*Pbar*/, Substance substance) -> ThermoPropertiesSubstance
{
    ThermoPropertiesSubstance   thermo_properties_PT   = substance.thermoProperties();
    ThermoPropertiesSubstance   thermo_properties_PrTr = substance.thermoReferenceProperties();
    SubstanceClass::type        substance_class        = substance.substanceClass();

    auto TK_=TK;
    auto TrK = substance.referenceT()/* + C_to_K*/;

    auto S = thermo_properties_PrTr.entropy;
    auto G = thermo_properties_PrTr.gibbs_energy;
    auto H = thermo_properties_PrTr.enthalpy;
    auto Cp = thermo_properties_PrTr.heat_capacity_cp;

    G -= S * (TK - TrK);
    S += (Cp * log(TK/TrK) );
    G -= (Cp * ( TK * log(TK/TrK) - (TK - TrK) ) );
    H += (Cp * (TK - TrK) );

    thermo_properties_PT.heat_capacity_cp   = Cp;
    thermo_properties_PT.gibbs_energy       = G;
    thermo_properties_PT.enthalpy           = H;
    thermo_properties_PT.entropy            = S;

    return thermo_properties_PT;
}
} // namespace ThermoFun
