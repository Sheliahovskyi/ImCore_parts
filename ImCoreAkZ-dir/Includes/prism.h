#pragma once

#include <cmath>
#include <vector>
#include "node.h"
#include "core.h"

using namespace std;

class CORE;

class ASSEMBLY;
class NODE;


class PRISM
{


public:
    CORE * myCore;
    ASSEMBLY * myAssembly;
    std::vector<NODE> ND;
    std::vector<double> pin_x;
    std::vector<double> pin_y;
    std::vector<double> pin_power;
    std::vector<double> pin_factors;
    std::vector<double> PinSl;

    std::vector<PRISM*> ADJ_PRISM;

    unsigned int type = 0;
    unsigned int n = 0;
    unsigned int h = 0;

    double X = 0;
    double Y = 0;
    double Z = 0;
    double S = 0;
    double V = 0;
    double height = 0;
    double part = 0;

    double KC = 0;
    double TP = 0;
    double NP = 0;
    double F1 = 0;
    double F2 = 0;
    double WT = 0;
    double FT = 0;
    double WD = 0;
    double CP = 0;
    double P = 0;
    double  SL_f = 0;
    double  XE_f = 0;
    double  SM_f = 0;
    double  PM_f = 0;
    double  I_f = 0;
    double XE_eq = 0;
    double SM_eq = 0;
    double PM_eq = 0;
    double I_eq = 0;
    double dXE = 0;
    double dSM = 0;
    double dPM = 0;
    double dI = 0;

    double SL = 0;
    double XE = 0;
    double SM = 0;
    double PM = 0;
    double I = 0;


    double D1 = 0;
    double D2 = 0;
    double Sa1 = 0;
    double Sa2 = 0;
    double nSf1 = 0;
    double nSf2 = 0;
    double Sf1 = 0;
    double Sf2 = 0;
    double S12 = 0;
    double E = 0;
    double gI = 0;
    double gXE = 0;
    double gPM = 0;
    double saX1 = 0;
    double saX2 = 0;
    double saS1 = 0;
    double saS2 = 0;

    double ro_XE = 0;
    double ro_SM = 0;
    double ro_XE_eq = 0;
    double ro_SM_eq = 0;
    std::vector<double> Rod_part;       // 0-empty; 1-bpel; 2-dysp;


    PRISM();
    ~PRISM();

    void division();
    void find_adj_node();
    void power();
    void burnout();
    void absorbers();
    double internal_iteration();
    void normalization();
    void poisoning_xe_sm();
    void prepareForDifCal();

};


