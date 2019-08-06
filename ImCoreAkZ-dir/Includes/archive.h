#pragma once

#include "core.h"
#include <glob.h>
#include <map>
#include <cstring>


/*!
 * \date 16.07.2019
 * \brief ����� ��� ������ � ��������
 *
 * ���������� ������
 * ��������� ��������� ���� �� �������
 * �������� ������� ��������� ����
 * ��������� ����� ������� ������ �� ����
 *
 */

struct CoreState {                          ///< ���������, ���������� ����������� ���������� � ��������� ��� � ����� ������ �������
    ///< Date
    TIME_IMCORE CurrentDate;
    double Teff;
    ///< Geometry
    vector<vector<int>> CoreMap;
    vector<vector<double>> PrismHight;
    ///< Fields
    vector<vector<double>> SL;
    vector<vector<double>> SM;
    vector<vector<double>> PM;
    vector<vector<double>> XE;
    vector<vector<double>> I;
    vector<vector<vector<double>>> PinSL;
    vector<vector<double>> ZE;

    ///< Parameters
    vector<double> HSuz;
    double G;
    double CB;
    double Pout;
    double Pdrop;
    double Tin;
};

struct MemoryPoint {
    string date;
    long long TimeFrom1970;
    string ArchiveFileGlobalPath;
};

class ARCHIVE {

public:
    CORE * pCore;

    ARCHIVE( CORE* CorePointer );
    ~ARCHIVE();

    vector<MemoryPoint> ArchiveList;

    CoreState MakeCoreSlice();                      ///< ������������ ��������� CoreState ������� ��������� ��������� ���
    CoreState FindCoreArchive( long long TimeFrom1970 );               ///< ����� ��������� ����� �� ����������� ����
    void ArchiveListFilling();                      ///< ���������� ������ ��������� �������� ����� ��� ������
    void ResizeCoreSliceFields( CoreState * CoreState );      ///< Resize() ����� ������ CoreState ��� �� ��������� ����������

    void WriteCoreSlice();                          ///< ������ ��������� ��� � �������� ����
    void ReadCoreSlice( long long TimeFrom1970 );                      ///< ������ ��������� ��� �� ��������� �����

};
