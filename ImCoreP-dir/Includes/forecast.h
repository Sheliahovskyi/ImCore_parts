#pragma once
#include "core.h"
#include "ImCorePMain.h"

/*!
 * \author Шеляговский Дима
 * \date 08.04.2019
 * \brief Класс выполняющий прогноз состояния АкЗ
 *
 * Выполняет расчет текущего прогноза, сценариев запросного прогноза, расчет водообмена при выходе на МКУ
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

enum ForecastType {         ///< Типы этапа прогнозного расчета
    CurrentForecast,        ///< Текущий прогноз
    RequestForecast,        ///< Прогноз на управляющее воздействие
    Generator,              ///< Генератор сценария (ИПОРМ)
    MKU,                    ///< Расчет водообмена при выходе РУ на МКУ
    CompanyCalc             ///< Расчёт компании
};

enum FlagStatus {
    OFF,                ///< Выкл.
    ON                  ///< Вкл.
};

enum AOstrategy {
    AO_off,             ///< Нет стратегии
    AO_fixed,           ///< Поддержание заданного АО
    AO_fixed_2,         ///< Поддержание заданного АО ? 2%
    AO_fixed_3,         ///< Поддержание заданного АО ? 3%
    AO_fixed_4,         ///< Поддержание заданного АО ? 4%
    AO_fixed_5,         ///< Поддержание заданного АО ? 5%
    AO_perm,            ///< Поддержание АО в допустимых границах
    AO_perm_12h         ///< Поддержание АО с разрешением выхода за допустимые границы на время не более 12 часов
};

enum SUZstrategy {
    SUZ_off,            ///< Нет стратегиии
    SUZ_all,            ///< Поддержание всех групп в регламентном положении
    SUZ_7,              ///< Поддержание групп c 1 по 7 в регламентном положении
    SUZ_9,              ///< Поддержание групп c 1 по 9 в регламентном положении
    SUZ_SUPRESS         ///< Подавление колебаний
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

    int fCrit;                          ///< Флаг достижения критичности
    int fAOStrategy;                    ///< Флаг стратегии по АО
    int fSUZStrategy;                   ///< Флаг стратегии по СУЗам
    int fInterception;                  ///< Флаг перехвата СУЗов

    std::string FC_Name;                ///< название прогноза
    int FC_Type;                        ///< тип прогнозного расчета
    int EtapsNumber;                    ///< количество этапов прогнозного расчета
    int StepsNumber;                    ///< колличество шагов в этапе прогноза
    int CurrentEtapNumber;              ///< Номер текущего расчетного этапа
    TIME_IMCORE EtapDuration;           ///< Продолжительность текущего этапа
    int EtapStatus;                     ///< Статус текущего этапа ( рассчитать/пропустить )

    double Gfeed;                       ///< Расход подпитки
    double Gblow;                       ///< Расход продувки
    double CBfeed;                      ///< Концентрация борной кислоты в подпитке, г/кг


    std::string StartDate;
    long long StartTime1970;

    std::vector< EtapData > FC_List;            ///< Cценарий прогнозного расчета

    void ReadForecastInfo( CPredictRequestHeader* pHeader, LST* pEtapLst );
    void ReadStartCoreState( CoreState* core );
    void ZeroEtapParamSetting( EtapData* ZeroState );

    void SetStateParam( EtapData* CoreState, int EtapNumber, int Steps, int StepNumber );
    void StateParamCalc( EtapData*  LastState );

    void Forecast();
    void WriteStateResult( ResultData* answer );


    //double MKU_time;                ///< Время выхода на МКУ в процессе водообмена (сек)
    //double V_1k;                    ///< Объем теплоносителя в первом контуре (м3)
    //double V_KO;                    ///< Объем теплоносителя в КО (м3)
    //double S_KO;                    ///< Площадь поверхности теплоносителя в КО (м2)
    //double H_KO;                    ///< Уровень теплоносителя в КО (м)
    //double CB_KO;                   ///< Концентрация БК в теплоносителя КО (г/см3)
    //double Feed_rate;               ///< Величина подпитки 1к.
    //double Blow_rate;               ///< Величина продувки 1к.
    //double Feed_CB;                 ///< Концентрация БК в подпитке

};
