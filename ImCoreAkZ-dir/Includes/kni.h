#pragma once

#include <sstream>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include "assembly.h"

class ASSEMBLY;
using namespace std;

class KNI
{

public:
    ASSEMBLY * myAssembly;
    vector<double> CIMB;

    vector<double> positions;
    vector<double> ZE;
    vector<double> I;
    vector<double> I_pr;
    vector<double> Tok;
    vector<double> Tok_pr;
    vector<double> TPl;
    vector<double> SED;
    vector<double> KV_dpz;
    vector<bool>   reliability;

    vector<double> SL_displ;
    vector<double> WD_displ;
    vector<double> KC_displ;
    vector<double> TPl_displ;
    vector<double> F1_displ;
    vector<double> F2_displ;
    vector<double> WT_displ;
    vector<double> XE_displ;
    vector<double> SM_displ;
    vector<double> FT_displ;

    vector<double> k1_pr;
    vector<double> k2_pr;
    vector<double> y_pr;

    vector<vector<double>> PRI_parts;



    unsigned int number;
    double displacement;
    double Emitter_length;
    double NU;
    double BETA;
    double Ka;
    unsigned int D;
    double LRh104;
    double LRh104m;
    double sRh104;
    double sRh104m;
    double ALFA;
    double h1;
    double g1;
    int orb;
    int NLDPZ;

    KNI();

    void current_conv();
    void lag_elimination();
    void dpz_charge();
    double model_dpz_current(int);
    void resize();

};


