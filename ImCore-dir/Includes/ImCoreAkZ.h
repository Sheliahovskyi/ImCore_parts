#ifndef IMCOREAKZ_H
#define IMCOREAKZ_H

#include "core.h"
#include "ImCoreMain.h"


class bufer
{

    int NTC;
    int NKNI;
    int NSUZ;
    int NGSUZ;
    int NPT;
    int NLDPZ;
    int NL;
    int DIV;
    int NFA;

    std::vector<std::vector<double>> I;                     ///< ���� �����
    std::vector<double> TC;                                 ///< ���� ��������� ��������
    std::vector<double> suz;                                ///< ���� ��������� �� ���
    std::vector<double> gsuz;                               ///< ���� ��������� ����� ���


    double power = 0;                                       ///< �������� �������� ��, ���
    double flow;                                            ///< ������ �������������, �/�
    double P_out;                                           ///< �������� ��� ���, ���

    TIME_IMCORE time_read;                                  ///< ����� ���������� plant.read

    int npt;                                                ///< ���������� ���������� ������
    std::vector<int> spt;                                   ///< ������ ��������� ������ ������, 4 �������� (1/0)

    std::vector<double> txn;                                ///< �������� ����������� ������������� �� ����� �� ������� ������, ���� �

    enum PR                                                 ///< ������ � ������
    {
        close,                                              ///< ������ ������
        new_info,                                           ///< �������� ����� ������
        old_info,                                           ///< ��������� ������ ���� �������
        no_data                                             ///< ��� ������
    };

    unsigned int fPR;                                       ///< ������� ������ plant.read
    unsigned int fDU;                                       ///< ������� ������ ���������� ������� ��

    std::vector<std::vector<double>> KC_dipl;               ///< ��������� ���� ��������������� ����������� ������ � ���� �����
    std::vector<std::vector<double>> TPl_dipl;              ///< ��������� �������� �������� �������� � ���� �����
    std::vector<std::vector<double>> WD_dipl;               ///< ��������� ��������� ������������� � ���� �����
    std::vector<std::vector<double>> SL_dipl;               ///< ��������� ����� � ���� �����

    std::vector<std::vector<double>> ZE;                    ///< ����������� ������

    double CB;                                              ///< ������������ ������ ������� �/��

    std::vector<std::vector<double>> TPl_DPZ;               ///< �������� �������� �������� �� ��� � ���� �����
    std::vector<std::vector<double>> TPl7_AS;               ///< �������� �������� �������� ��� � ���� �����
    std::vector<std::vector<std::vector<double>>> TP_ND;    ///< �������� �������� ��������� ���
    std::vector<std::vector<std::vector<double>>> NP_ND;    ///< ���������� �������� ��������� ���
    std::vector<std::vector<double>> TP_PR;                 ///< �������� �������� �����
    std::vector<std::vector<double>> NP_PR;                 ///< ���������� �������� �����
    std::vector<double> TP_AS;                              ///< �������� �������� ���
    std::vector<double> NP_AS;                              ///< ���������� �������� ���
    double TP_CO;                                           ///< �������� �������� ����
    double NP_CO;                                           ///< ���������� �������� ����
    double ro_XE;                                           ///< ���������� ��������
    double ro_SM;                                           ///< ���������� ��������




public:
    bufer(CORE *);

    void get_plant_read(CORE *);
    void set_ZE(CORE *);

    void set_DU_results(CORE *);
    void get_DU_results(CORE *);


    void writeInputToFile(CORE *);
    void getPlantData(InputBuff* pDataBuff);


};

class ImCoreAkZ {
    CORE * pCore1;
    CORE * pCore2;
    bufer * pBufer;
    XS * pXS;

public:
    std::string homeDir;

    ImCoreAkZ(char* homeDir);
    ~ImCoreAkZ();

    void init();
    void initCalc();
    void threadDU();
    void recovery();
    void getScanData(InputBuff* pDataBuff);
    void putRecoveryResult();
    void putDiffCalcResult();
};



#endif
