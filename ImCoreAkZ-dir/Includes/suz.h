#ifndef SUZ_H
#define SUZ_H

#include <sstream>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include "crod.h"

using namespace std;

class SUZ
{


public:
    vector<CROD> RODS;
    vector<double> GROUPS;

    SUZ();

    void read_suz_data(string);
    vector<double> suz_position(double, double, int);
    void move_group(int, double);
    void emergency_protection(double);
    double give_group_position(int group);

};

#endif // SUZ_H
