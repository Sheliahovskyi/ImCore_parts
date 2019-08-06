#ifndef CROD_H
#define CROD_H

#include <sstream>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

class CROD
{

public:
    CROD();

    vector<double> type_length;
    vector<double> type;

    unsigned int assembly;
    unsigned int group;
    double position;

};

#endif // CROD_H
