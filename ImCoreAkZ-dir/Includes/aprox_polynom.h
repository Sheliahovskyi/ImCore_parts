#ifndef APROX_POLYNOM_H
#define APROX_POLYNOM_H
#include <iostream>
#include <vector>

using namespace std;

class APROX_POLYNOM
{
    vector<double> coef_of_poly;


    double Power(double x, int pow);

public:
    APROX_POLYNOM();
    APROX_POLYNOM(vector<double> x, vector<double> y, int pow);
    void make_coef(vector<double> x, vector<double> y, int pow);
    vector<double> calc_value(vector<double> x);
    vector<double> give_coef();
};

#endif // APROX_POLYNOM_H
