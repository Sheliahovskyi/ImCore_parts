#include "time_imcore.h"

TIME_IMCORE::TIME_IMCORE()
{
    time_t rawtime;
    struct tm * timeinfo;

    time (&rawtime);
    timeinfo = localtime (&rawtime);
    tm_isdst = timeinfo->tm_isdst;
}

TIME_IMCORE::TIME_IMCORE(string text)
{
    time_t rawtime;
    struct tm * timeinfo;

    time (&rawtime);
    timeinfo = localtime (&rawtime);
    tm_isdst = timeinfo->tm_isdst;
    read(text);

}

TIME_IMCORE::TIME_IMCORE(tm Header) {
    time_t rawtime;
    struct tm * timeinfo;

    time (&rawtime);
    timeinfo = localtime (&rawtime);
    tm_isdst = timeinfo->tm_isdst;

    tm_hour = Header.tm_hour;
    tm_min = Header.tm_min;
    tm_sec = Header.tm_sec;
    tm_mon = Header.tm_mon;
    tm_mday = Header.tm_mday;
    tm_year = Header.tm_year;

    tm_msec = 0;
}

void TIME_IMCORE::show()
{
    cout << ((tm_hour <10) ? "0" : "") << tm_hour << ":";
    cout << (tm_min <10 ? "0" : "") << tm_min << ":";
    cout << (tm_sec <10 ? "0" : "") << tm_sec << ".";
    cout << (tm_msec <10 ? "00" : (tm_msec<100 ? "0" : "")) << tm_msec << " ";
    cout << (tm_mday <10 ? "0" : "") << tm_mday << ".";
    cout << (tm_mon <10 ? "0" : "") << tm_mon << ".";
    cout << tm_year + 1900 << endl;
}

void TIME_IMCORE::add(unsigned int msec)
{
    int sec = msec / 1000;
    int left_ms = msec %1000 + tm_msec;
    if (left_ms >= 1000)
    {
        sec++;
        left_ms = left_ms % 1000;

    }

    time_t loc_sec = mktime(this);

    loc_sec += sec;
    tm * tTm = gmtime(&loc_sec);
    tm_year = tTm->tm_year;
    tm_mon  = tTm->tm_mon;
    tm_mday = tTm->tm_mday;
    tm_hour = tTm->tm_hour;
    tm_min  = tTm->tm_min;
    tm_sec  = tTm->tm_sec ;

    tm_msec = left_ms;

}
void TIME_IMCORE::read(string text)
{

    tm_hour = atoi(text.substr(0, 2).c_str());
    tm_min = atoi(text.substr(2, 2).c_str());
    tm_sec = atoi(text.substr(4, 2).c_str());
    tm_mon = atoi(text.substr(6, 2).c_str())-1;
    tm_mday = atoi(text.substr(8, 2).c_str());
    tm_year = atoi(text.substr(10, 2).c_str())+100;
    tm_msec = 0;
}

string TIME_IMCORE::givedate()
{

    string date;
    date += ((tm_hour <10) ? "0" : "") + to_string(tm_hour);
    date += (tm_min <10 ? "0" : "") + to_string(tm_min);
    date += (tm_sec <10 ? "0" : "") + to_string(tm_sec);
    date += (tm_mon <9 ? "0" : "") + to_string(tm_mon+1);
    date += (tm_mday <10 ? "0" : "") + to_string(tm_mday);
    date += to_string(tm_year - 100);

    return date;
}

void TIME_IMCORE::read(string stime, string sdate)
{

    tm_hour = atoi(stime.substr(0, 2).c_str());
    tm_min = atoi(stime.substr(3, 2).c_str());
    tm_sec = atoi(stime.substr(6, 2).c_str());
    tm_msec = atoi(stime.substr(9, 3).c_str());
    tm_mday = atoi(sdate.substr(0, 2).c_str());
    tm_mon = atoi(sdate.substr(3, 2).c_str())-1;
    tm_year = atoi(sdate.substr(6, 4).c_str())-1900;
}

double TIME_IMCORE::diff(TIME_IMCORE * tm)
{

    double res = mktime(tm) - mktime(this);

    if (tm_msec > tm->tm_msec)
    {
        res--;
        res = res * 1000 + (tm->tm_msec + 1000 - tm_msec);
    }
    else
    {
        res = res * 1000 + (tm->tm_msec - tm_msec);
    }
    return res;
}

string TIME_IMCORE::givetxt()
{
    string date;
    date += to_string(tm_year + 1900) + "_";
    date += (tm_mon <9 ? "0" : "") + to_string(tm_mon+1) + "_";
    date += (tm_mday <10 ? "0" : "") + to_string(tm_mday) + "_";
    date += ((tm_hour <10) ? "0" : "") + to_string(tm_hour) + "_";
    date += (tm_min <10 ? "0" : "") + to_string(tm_min) + "_";
    date += (tm_sec <10 ? "0" : "") + to_string(tm_sec) + ".";
    date += (tm_msec <10 ? "00" : (tm_msec<100 ? "0" : "")) + to_string(tm_msec);

    return date;
}

int TIME_IMCORE::give_sec_from_1970()
{
    long sec = mktime(this);
    return sec;
}
