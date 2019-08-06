#pragma once
#include "core.h"
#include "ImCorePMain.h"

/*!
 * \author ����������� ����
 * \date 08.04.2019
 * \brief ����� ����������� ������� ��������� ���
 *
 * ��������� ������ �������� ��������, ��������� ���������� ��������, ������ ���������� ��� ������ �� ���
 *
 */

struct EtapData
{
    int EtapNumber;
    CTime EtapDuration;
    int status;

    float iPOWER;
    float iH10;
    float iH9;
    float iH8;
    float iH7;
    float iH6;
    float iH5;
    float iH4;
    float iH3;
    float iH2;
    float iH1;
    float iCB;
    float iTIN;
    float iGV1K;
    float iP1K;
    float iOFSTS;
    int iSGCN1;
    int iSGCN2;
    int iSGCN3;
    int iSGCN4;
    float iGpodp;
    float iCBpodp;
    std::vector<float> iSZ;
    int iCritParam;
    int iAOStrategy;
    int iSZStrategy;
    int iSZInterception;
    int iKV_SIZE;
    int iDNBR_SIZE;
    int iQVMF_SIZE;
};

struct ResultData
{
    int 	Etap_number;
    int		AnswerType;
    CTime	StartTime;
    CTime	PointTime;
    float	POWER;
    float	AO;
    float	AOvrg;
    float	AOnrg;
    float	VCB;
    float	VWATER;
    float	XE;
    float	SM;
    float	CB;
    float	H1;
    float	H2;
    float	H3;
    float	H4;
    float	H5;
    float	H6;
    float	H7;
    float	H8;
    float	H9;
    float	H10;
    float	H8vrg;
    float	H8nrg;
    float	H9vrg;
    float	H9nrg;
    float	H10vrg;
    float	H10nrg;
    std::vector<vector<float>>	KV;
    std::vector<vector<float>>	KVvrg;
    std::vector<float>	KQ;
    std::vector<float>	KQvrg;
    std::vector<vector<float>>	QVMF;
    std::vector<vector<float>>	QVMFvrg;
    std::vector<vector<float>>	DNBR;
    std::vector<vector<float>>	DNBRnrg;
    std::vector<float>	DNBT;
    std::vector<float>	DNBTnrg;
};

enum ForecastType {         ///< ���� ����� ����������� �������
    CurrentForecast,        ///< ������� �������
    RequestForecast,        ///< ������� �� ����������� �����������
    Generator,              ///< ��������� �������� (�����)
    MKU,                    ///< ������ ���������� ��� ������ �� �� ���
    CompanyCalc             ///< ���ޣ� ��������
};

enum FlagStatus {
    OFF,                ///< ����.
    ON                  ///< ���.
};

enum AOstrategy {
    AO_off,             ///< ��� ���������
    AO_fixed,           ///< ����������� ��������� ��
    AO_fixed_2,         ///< ����������� ��������� �� ? 2%
    AO_fixed_3,         ///< ����������� ��������� �� ? 3%
    AO_fixed_4,         ///< ����������� ��������� �� ? 4%
    AO_fixed_5,         ///< ����������� ��������� �� ? 5%
    AO_perm,            ///< ����������� �� � ���������� ��������
    AO_perm_12h         ///< ����������� �� � ����������� ������ �� ���������� ������� �� ����� �� ����� 12 �����
};

enum SUZstrategy {
    SUZ_off,            ///< ��� ����������
    SUZ_all,            ///< ����������� ���� ����� � ������������ ���������
    SUZ_7,              ///< ����������� ����� c 1 �� 7 � ������������ ���������
    SUZ_9,              ///< ����������� ����� c 1 �� 9 � ������������ ���������
    SUZ_SUPRESS         ///< ���������� ���������
};

class FORECAST
{
    void phase_param_setting(int);
    void step_param_setting(int);
    void MKU_time_calc(double);
    void power_mode_calc();
    void company_mode_calc();
    void mku_mode_calc();

public:
    CORE*    pCORE;
    XS*      pXS;
    ARCHIVE* pARCH;

    FORECAST(char* homeDir);
    ~FORECAST();

    int fCrit;                          ///< ���� ���������� �����������
    int fAOStrategy;                    ///< ���� ��������� �� ��
    int fSUZStrategy;                   ///< ���� ��������� �� �����
    int fInterception;                  ///< ���� ��������� �����

    std::string FC_Name;                ///< �������� ��������
    int FC_Type;                        ///< ��� ����������� �������
    int EtapsNumber;                    ///< ���������� ������ ����������� �������
    int StepsNumber;                    ///< ����������� ����� � ����� ��������
    int CurrentEtapNumber;              ///< ����� �������� ���������� �����
    TIME_IMCORE EtapDuration;           ///< ����������������� �������� �����
    int EtapStatus;                     ///< ������ �������� ����� ( ����������/���������� )

    double Gfeed;                       ///< ������ ��������
    double Gblow;                       ///< ������ ��������
    double CBfeed;                      ///< ������������ ������ ������� � ��������, �/��


    std::string StartDate;
    long long StartTime1970;

    std::vector< EtapData > FC_List;            ///< C������� ����������� �������

    void ReadForecastInfo( CPredictRequestHeader* pHeader, LST* pEtapLst );
    void ReadStartCoreState( CoreState* core );
    void ZeroEtapParamSetting( EtapData* ZeroState );

    void SetStateParam( EtapData* CoreState, int EtapNumber, int Steps, int StepNumber );
    void StateParamCalc( EtapData*  LastState );

    void Forecast();
    void WriteStateResult( ResultData* answer );


    //double MKU_time;                ///< ����� ������ �� ��� � �������� ���������� (���)
    //double V_1k;                    ///< ����� ������������� � ������ ������� (�3)
    //double V_KO;                    ///< ����� ������������� � �� (�3)
    //double S_KO;                    ///< ������� ����������� ������������� � �� (�2)
    //double H_KO;                    ///< ������� ������������� � �� (�)
    //double CB_KO;                   ///< ������������ �� � ������������� �� (�/��3)
    //double Feed_rate;               ///< �������� �������� 1�.
    //double Blow_rate;               ///< �������� �������� 1�.
    //double Feed_CB;                 ///< ������������ �� � ��������

};
