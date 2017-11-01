#define IPMGEMPLUGIN
#define NOPARTICLEARRAY

#include <iostream>
#include <fstream>
#include <sys/time.h>
// ThermoFun include
#include "ThermoFun.h"
#include "Common/OutputToCSV.h"
// GEMS4R include
#include "node.h"

#pragma GCC diagnostic ignored "-fpermissive"


using namespace std;
using namespace ThermoFun;

auto compare (ThermoPropertiesSubstance result_gems, ThermoPropertiesSubstance result_tcorrpt, string symbol, double T, double P ) -> int
{
    int i = 1; double d;
    double tolerance = 1e-03;

    d = (fabs(result_gems.gibbs_energy.val - result_tcorrpt.gibbs_energy.val) / fabs(result_gems.gibbs_energy.val));
    if ( d > tolerance)
    {
        cout << "At T:"<< T << "C and P:" << P << "bar" << endl <<
                "For substance " << symbol << " the Gibbs energy (G0) calculated in GEMS (" << result_gems.gibbs_energy << "J/mol)\n"
                "is not the same as claulcated with the TCorrPT library (" << result_tcorrpt.gibbs_energy << "J/mol)\n"
                "d% = " << d*100 << endl << endl;
        i = -1;
    }

    d = (fabs(result_gems.entropy.val - result_tcorrpt.entropy.val) / fabs(result_gems.entropy.val));
    if ( d > tolerance)
    {
        cout << "At T:"<< T << "C and P:" << P << "bar" << endl <<
                "For substance " << symbol << " the entropy (S0) calculated in GEMS (" << result_gems.entropy << "J/mol)\n"
                "is not the same as claulcated with the TCorrPT library (" << result_tcorrpt.entropy << "J/mol)\n"
                "d% = " << d*100 << endl << endl;
        i = -1;
    }

    d = (fabs(result_gems.enthalpy.val - result_tcorrpt.enthalpy.val) / fabs(result_gems.enthalpy.val));
    if ( d > tolerance)
    {
        cout << "At T:"<< T << "C and P:" << P << "bar" << endl <<
                "For substance " << symbol << " the enthalpy (H0) calculated in GEMS (" << result_gems.enthalpy << "J/mol)\n"
                "is not the same as claulcated with the TCorrPT library (" << result_tcorrpt.enthalpy << "J/mol)\n"
                "d% = " << d*100 << endl << endl;
        i = -1;
    }

    d = (fabs(result_gems.heat_capacity_cp.val - result_tcorrpt.heat_capacity_cp.val) / fabs(result_gems.heat_capacity_cp.val));
    if ( d > tolerance)
    {
        cout << "At T:"<< T << "C and P:" << P << "bar" << endl <<
                "For substance " << symbol << " the heat capacity (Cp0) calculated in GEMS (" << result_gems.heat_capacity_cp << "J/mol K)\n"
                "is not the same as claulcated with the TCorrPT library (" << result_tcorrpt.heat_capacity_cp << "J/mol K)\n"
                "d% = " << d*100 << endl << endl;
        i = -1;
    }

    d = (fabs(result_gems.volume.val - result_tcorrpt.volume.val) / fabs(result_gems.volume.val));
    if ( d > tolerance)
    {
        cout << "At T:"<< T << "C and P:" << P << "bar" << endl <<
                "For substance " << symbol << " the volume (V0) calculated in GEMS (" << result_gems.volume << "J/bar)\n"
                "is not the same as claulcated with the TCorrPT library (" << result_tcorrpt.volume << "J/bar)\n"
                "d% = " << d*100 << endl << endl;
        i = -1;
    }
    return i;
}

int main(int argc, char *argv[])
{
    struct timeval start, now_gems, now_tfun, end;
    gettimeofday(&start, NULL);
    int xCH;

    cout << endl << "+++ Test started: comparing thermodynamic properties calculated by GEM4R with properties calculated by ThermoFun +++ " << endl << endl;

    string file = "Resources/test_multi_new.VertexSubstance.json";
    char config_json_file_path[256] = "Resources/GEMS4/TestMulti";

    TNode* node = new TNode();
    node->GEM_init(config_json_file_path);

    gettimeofday(&now_gems, NULL);
    double delta_gems = ((now_gems.tv_sec  - start.tv_sec) * 1000000u +
             now_gems.tv_usec - start.tv_usec) / 1.e6;

    Database tdb(file);
//    Database tdb;
    vector<Substance> vSubst = tdb.getSubstances();

    /// adding the names of the solvent to be used in the aqueous species properties
    /// calculations
//    for (int i = 0; i < vSubst.size(); i++)
//    {
////        if (vSubst[i].substanceClass() == SubstanceClass::type::AQSOLUTE)
////        vSubst[i].setSolventSymbol("H2O@");
//        tdb.addSubstance(vSubst[i]);
//    }

    double T;
    double P;

    int c = 0;

//    propertiesUnits["tmeperature"] = "K";

    /// Used to test different waters solvent models
//    Substance water;
//    water.setName("water");
//    water.setSymbol("H2O@_");
//    water.setFormula("H2O");
//    water.setSubstanceClass(SubstanceClass::type::AQSOLVENT);
//    water.setAggregateState(AggregateState::type::AQUEOUS);

//    water.setMethodGenEoS(MethodGenEoS_Thrift::type::CTPM_WJNR);

//    water.setMethod_T(MethodCorrT_Thrift::type::CTM_WWP);

//    tdb.addSubstance(water);

//    vector<Substance> vSubst = tdb.getSubstances();

//    for (int i = 0; i < vSubst.size(); i++)
//    {
//        if (vSubst[i].substanceClass() == SubstanceClass::type::AQSOLUTE)
//        vSubst[i].setSolventSymbol(water.symbol());

//        tdb2.addSubstance(vSubst[i]);
//    }

    Thermo thermo (tdb);
    ThermoPropertiesSubstance result_tcorrpt, tps_gems, result_tcorrpt_;
    thermo.setSolventSymbol("H2O@");

    OutputToCSV out (argv[0]);
    out.openThermoPropertiesSubstanceFile("CompareP0_T5_370.csv");

    P=1;
    gettimeofday(&now_tfun, NULL);
    double delta_tfun = ((now_tfun.tv_sec  - now_gems.tv_sec) * 1000000u +
             now_tfun.tv_usec - now_gems.tv_usec) / 1.e6;

    P = 5000; T = 25;
    // test PRSV
//    result_tcorrpt_ = thermo.thermoPropertiesSubstance(T,P,"Al+3");
//    xCH = node->DC_name_to_xCH("Al+3");
//    tps_gems.gibbs_energy     = node->DC_G0 (xCH, P*1e05, T+273.15, false);
//    tps_gems.enthalpy         = node->DC_H0 (xCH, P*1e05, T+273.15);
//    tps_gems.entropy          = node->DC_S0 (xCH, P*1e05, T+273.15);
//    tps_gems.heat_capacity_cp = node->DC_Cp0(xCH, P*1e05, T+273.15);
//    tps_gems.volume           = node->DC_V0 (xCH, P*1e05, T+273.15)*1e05;


    P = 0; T = 5;
    //
    do {

        for (int i = 0; i < vSubst.size(); i++)
        {
            result_tcorrpt = thermo.thermoPropertiesSubstance(T,P,vSubst[i].symbol());
            out.writeThermoPropertiesSubstance( vSubst[i].symbol() + "_TCorrPT", T, P, result_tcorrpt);

            xCH = node->DC_name_to_xCH(vSubst[i].symbol().c_str());
            tps_gems.gibbs_energy     = node->DC_G0 (xCH, 0, T+273.15, false);
            tps_gems.enthalpy         = node->DC_H0 (xCH, 0, T+273.15);
            tps_gems.entropy          = node->DC_S0 (xCH, 0, T+273.15);
            tps_gems.heat_capacity_cp = node->DC_Cp0(xCH, 0, T+273.15);
            tps_gems.volume           = node->DC_V0 (xCH, 0, T+273.15)*1e05;
            out.writeThermoPropertiesSubstance( vSubst[i].symbol() + "_Gems", T, P, tps_gems);

            compare(tps_gems, result_tcorrpt, vSubst[i].symbol().c_str(), T, P);

            c++;
            //            i = vSubst.size();
        }
        P = 0;
        T +=5;
    } while (T <= 354); // 370 coreections in g function
    out.closeThermoPropertiesSubstanceFile();

    P = 1; T = 5;
    out.openThermoPropertiesSubstanceFile("CompareP1_T5_200.csv");
    do {

        for (int i = 0; i < vSubst.size(); i++)
        {
            result_tcorrpt = thermo.thermoPropertiesSubstance(T,P,vSubst[i].symbol());
            out.writeThermoPropertiesSubstance( vSubst[i].symbol() + "_TCorrPT", T, P, result_tcorrpt);

            xCH = node->DC_name_to_xCH(vSubst[i].symbol().c_str());
            tps_gems.gibbs_energy     = node->DC_G0 (xCH, P*1e05, T+273.15, false);
            tps_gems.enthalpy         = node->DC_H0 (xCH, P*1e05, T+273.15);
            tps_gems.entropy          = node->DC_S0 (xCH, P*1e05, T+273.15);
            tps_gems.heat_capacity_cp = node->DC_Cp0(xCH, P*1e05, T+273.15);
            tps_gems.volume           = node->DC_V0 (xCH, P*1e05, T+273.15)*1e05;
            out.writeThermoPropertiesSubstance( vSubst[i].symbol() + "_Gems", T, P, tps_gems);

            compare(tps_gems, result_tcorrpt, vSubst[i].symbol().c_str(), T, P);

            c++;
            P = 1;
        }
        T +=5;
    } while (T <= 200);
    out.closeThermoPropertiesSubstanceFile();

//    P = 500; T = 25;
//    out.openThermoPropertiesSubstanceFile("CompareP500_T25_800.csv");
//    do {

//        for (int i = 0; i < vSubst.size(); i++)
//        {
//            result_tcorrpt = thermo.thermoPropertiesSubstance(T,P,vSubst[i].symbol());
//            out.writeThermoPropertiesSubstance( vSubst[i].symbol() + "_TCorrPT", T, P, result_tcorrpt);

//            xCH = node->DC_name_to_xCH(vSubst[i].symbol().c_str());
//            tps_gems.gibbs_energy     = node->DC_G0 (xCH, P*1e05, T+273.15, false);
//            tps_gems.enthalpy         = node->DC_H0 (xCH, P*1e05, T+273.15);
//            tps_gems.entropy          = node->DC_S0 (xCH, P*1e05, T+273.15);
//            tps_gems.heat_capacity_cp = node->DC_Cp0(xCH, P*1e05, T+273.15);
//            tps_gems.volume           = node->DC_V0 (xCH, P*1e05, T+273.15)*1e05;
//            out.writeThermoPropertiesSubstance( vSubst[i].symbol() + "_Gems", T, P, tps_gems);

//            compare(tps_gems, result_tcorrpt, vSubst[i].symbol().c_str(), T, P);

//            c++;
//            P = 500;
//        }
//        T +=25;
//    } while (T <= 800);
//    //
//    out.closeThermoPropertiesSubstanceFile();

    P = 1000; T = 25;
    out.openThermoPropertiesSubstanceFile("CompareP1000_T25_800.csv");
    do {

        for (int i = 0; i < vSubst.size(); i++)
        {
            result_tcorrpt = thermo.thermoPropertiesSubstance(T,P,vSubst[i].symbol());
            out.writeThermoPropertiesSubstance( vSubst[i].symbol() + "_TCorrPT", T, P, result_tcorrpt);

            xCH = node->DC_name_to_xCH(vSubst[i].symbol().c_str());
            tps_gems.gibbs_energy     = node->DC_G0 (xCH, P*1e05, T+273.15, false);
            tps_gems.enthalpy         = node->DC_H0 (xCH, P*1e05, T+273.15);
            tps_gems.entropy          = node->DC_S0 (xCH, P*1e05, T+273.15);
            tps_gems.heat_capacity_cp = node->DC_Cp0(xCH, P*1e05, T+273.15);
            tps_gems.volume           = node->DC_V0 (xCH, P*1e05, T+273.15)*1e05;
            out.writeThermoPropertiesSubstance( vSubst[i].symbol() + "_Gems", T, P, tps_gems);

            compare(tps_gems, result_tcorrpt, vSubst[i].symbol().c_str(), T, P);

            c++;
            P = 1000;
        }
        T +=25;
    } while (T <= 800);
    out.closeThermoPropertiesSubstanceFile();

    P = 2000; T = 25;
    out.openThermoPropertiesSubstanceFile("CompareP2000_T25_800.csv");
    do {

        for (int i = 0; i < vSubst.size(); i++)
        {
            result_tcorrpt = thermo.thermoPropertiesSubstance(T,P,vSubst[i].symbol());
            out.writeThermoPropertiesSubstance( vSubst[i].symbol() + "_TCorrPT", T, P, result_tcorrpt);

            xCH = node->DC_name_to_xCH(vSubst[i].symbol().c_str());
            tps_gems.gibbs_energy     = node->DC_G0 (xCH, P*1e05, T+273.15, false);
            tps_gems.enthalpy         = node->DC_H0 (xCH, P*1e05, T+273.15);
            tps_gems.entropy          = node->DC_S0 (xCH, P*1e05, T+273.15);
            tps_gems.heat_capacity_cp = node->DC_Cp0(xCH, P*1e05, T+273.15);
            tps_gems.volume           = node->DC_V0 (xCH, P*1e05, T+273.15)*1e05;
            out.writeThermoPropertiesSubstance( vSubst[i].symbol() + "_Gems", T, P, tps_gems);

            compare(tps_gems, result_tcorrpt, vSubst[i].symbol().c_str(), T, P);

            c++;
            P = 2000;
        }
        T +=25;
    } while (T <= 800);
    out.closeThermoPropertiesSubstanceFile();

    P = 5000; T = 25;
    out.openThermoPropertiesSubstanceFile("CompareP5000_T25_800.csv");
    do {

        for (int i = 0; i < vSubst.size(); i++)
        {
//            if( T == 800 && P == 5000 && vSubst[i].symbol() == "Quartz")
//            {
//                cout << "here" << endl;
//            }

            result_tcorrpt = thermo.thermoPropertiesSubstance(T,P,vSubst[i].symbol());
            out.writeThermoPropertiesSubstance( vSubst[i].symbol() + "_TCorrPT", T, P, result_tcorrpt);

            xCH = node->DC_name_to_xCH(vSubst[i].symbol().c_str());
            tps_gems.gibbs_energy     = node->DC_G0 (xCH, P*1e05, T+273.15, false);
            tps_gems.enthalpy         = node->DC_H0 (xCH, P*1e05, T+273.15);
            tps_gems.entropy          = node->DC_S0 (xCH, P*1e05, T+273.15);
            tps_gems.heat_capacity_cp = node->DC_Cp0(xCH, P*1e05, T+273.15);
            tps_gems.volume           = node->DC_V0 (xCH, P*1e05, T+273.15)*1e05;
            out.writeThermoPropertiesSubstance( vSubst[i].symbol() + "_Gems", T, P, tps_gems);

            compare(tps_gems, result_tcorrpt, vSubst[i].symbol().c_str(), T, P);

            c++;
            P = 5000;
        }
        T +=25;
    } while (T <= 800);
    out.closeThermoPropertiesSubstanceFile();

    gettimeofday(&end, NULL);
    double delta_calc = ((end.tv_sec  - now_tfun.tv_sec) * 1000000u +
             end.tv_usec - now_tfun.tv_usec) / 1.e6;
    cout << "==================================================================" << endl;
    cout << "+ Time for GEMS4R initialization: " << delta_gems << "s "<< endl;
    cout << "+ Time for ThermoFun initialization: " << delta_tfun << "s "<< endl;
    cout << "+ Time for "<< c << " T-P calculations: "<< delta_calc << "s " << endl;
    cout << "+ Total time: " << delta_gems + delta_tfun + delta_calc << "s "<< endl;
    cout << "==================================================================" << endl << endl;



    return 0;
}
