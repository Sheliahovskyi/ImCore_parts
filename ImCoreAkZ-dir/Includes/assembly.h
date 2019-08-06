#pragma once
#include "prism.h"
#include "kni.h"
#include "core.h"
#include "cubic_spline.h"


class CORE;
class KNI;

class ASSEMBLY
{

public:
    std::vector<ASSEMBLY*> ADJ;
    std::vector<PRISM> PRI;
    std::vector<vector<double>> PRI_parts;
    CORE * myCore = 0;
    KNI * kni = 0;
    vector<double> DPZ_current;
    vector<double> DPZ_power;
    vector<double> SL_7;
    vector<double> KC_7;
    vector<double> WD_7;
    vector<double> TPl_7;
    vector<double> KDPZ7;
    vector<double> KDPZN;
    vector<double> ZN;
    vector<double> Z7;
    vector<double> parts_length;
    vector<int> parts_type;


    double S;
    double height;
    double V;
    double TP;
    double NP;
    double MU;
    double  SL_f;
    double  XE_f;
    double  SM_f;
    double  PM_f;
    double  I_f;
    double XE_eq;
    double SM_eq;
    double PM_eq;
    double I_eq;
    double flow;
    double WT_in;
    double WT_out;
    double thermocouple;
    double KTECON;
    double dT;

    double SL;
    double XE;
    double SM;
    double PM;
    double I;

    double ro_XE = 0;
    double ro_SM = 0;
    double ro_XE_eq = 0;
    double ro_SM_eq = 0;

    std::string type;
    unsigned int num_adj;
    int n;
    unsigned int NL;
    int X;
    int Y;
    unsigned int orb;
    unsigned int NTC = 0;

    ASSEMBLY();
    ~ASSEMBLY();
    void find_adj();
    void init(CORE *);
    void division();
    void power();
    void burnout();
    void absorbers();
    void albedo_put();
    void set_begin_value();
    double internal_iteration();
    void normalization();
    void wt_calc();
    void ft_calc();
    void recovery();
    void interpolation_7_lays();
    void poisoning_xe_sm();






};

