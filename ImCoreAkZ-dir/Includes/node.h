#pragma once
 
#include <vector>
#include "prism.h"
#include "core.h"

#include <vector>
#include "prism.h"
#include "core.h"

class CORE;
class PRISM;

struct PreparedData {
    // данные для подготовки к дифф расчету

    std::vector<double> DDR1;
    std::vector<double> DDR2;

    std::vector<double> DpRNei1;
    std::vector<double> DpRNei2;

    double DDA1;
    double DDA2;

    double hRad;
    double hAxi;

    double BR1;
    double BR2;
    double BR3;
    double BR4;
    double BR5;

    double BA1;
    double BA2;
    double BA3;
    double BA4;
    double BA5;

    double DpR1;
    double DpR2;

    double aD1 = 0;
    double aD2 = 0;

    double S_rem1 = 0;
    double S_rem2 = 0;
};

class NODE
{
public:

    PreparedData preDif;

    std::vector<NODE*> ADJ_R_NODE;
    std::vector<double> contact_r;
    std::vector<double> range_r;
    std::vector<NODE*> ADJ_A_NODE;
    std::vector<double> contact_a;
    std::vector<double> range_a;


    CORE * myCore;
    PRISM * myPrism;

    double X;
    double Y;
    double Z;
    double S;
    double height;
    double V;
    double TP;
    double NP;
    double F1;
    double F2;
    double F1p;
    double F2p;
    double alb_side_11 = -1;
    double alb_side_12 = -1;
    double alb_side_22 = -1;
    double alb_top_11 = -1;
    double alb_top_12 = -1;
    double alb_top_22 = -1;
    double alb_bottom_11 = -1;
    double alb_bottom_12 = -1;
    double alb_bottom_22 = -1;

    unsigned int d;
    unsigned int my_number;
    int numRadNei;
    int numAxiNei;



    NODE();
    void power();
    double internal_iteration();
    double internal_iteration2();
    double internal_iteration3();
    double internal_iterationO();

};
