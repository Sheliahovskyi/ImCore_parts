#pragma once

#include <sstream>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include "core.h"


using namespace std;

class CORE;
class PRISM;

class XS
{

public:
    CORE * myCore;

    vector<vector<string>> fuel_base;
    vector<vector<double>> CS;
    vector<vector<double>> PF;
    vector<vector<double>> CIMB_bank;
    vector<vector<double>> WD_bank;
    vector<vector<double>> CP_bank;
    vector<double> press_range;
    vector<double> temp_range;

    XS();
    ~XS();

    void read_cs_data(std::string);
    void read_therm_data(std::string, std::string);
    void read_pin_data(std::string);
    void read_cimb_data(std::string);
    void cs_update(PRISM &);
    void pin_update(PRISM &);
    void therm_update(PRISM &);
    double cp_calc(double T, double P);
    double wd_calc(double T, double P);

};

