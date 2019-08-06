#ifndef TIME_IMCORE_H
#define TIME_IMCORE_H
#include <string>
#include <ctime>
#include <iostream>

using namespace std;

struct TIME_IMCORE : tm
{
    int tm_msec = 0;

    TIME_IMCORE();
    TIME_IMCORE(tm Header);
    TIME_IMCORE(string text);

    void show();

    double diff(TIME_IMCORE * );
    void add(unsigned int);
    string givetxt();
    int give_sec_from_1970();
    void read(string);
    void read(string time, string date);
    string givedate();

};
#endif // TIME_IMCORE_H
