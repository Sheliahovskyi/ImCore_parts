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

    std::vector<std::vector<double>> I;                     ///< Поле токов
    std::vector<double> TC;                                 ///< Поле показаний термопар
    std::vector<double> suz;                                ///< Поле положений ОР СУЗ
    std::vector<double> gsuz;                               ///< Поле положений групп СУЗ


    double power = 0;                                       ///< Тепловая мощность РУ, МВт
    double flow;                                            ///< Расход теплоносителя, т/ч
    double P_out;                                           ///< Давление над АкЗ, МПа

    TIME_IMCORE time_read;                                  ///< Время считанного plant.read

    int npt;                                                ///< Количество работающих петель
    std::vector<int> spt;                                   ///< Вектор признаков работы петель, 4 значения (1/0)

    std::vector<double> txn;                                ///< Значения температуры теплоносителя на входе по четырем петлям, град С

    enum PR                                                 ///< Доступ к данным
    {
        close,                                              ///< Доступ закрыт
        new_info,                                           ///< Доступны новые данные
        old_info,                                           ///< Последние данные были считаны
        no_data                                             ///< Нет данных
    };

    unsigned int fPR;                                       ///< Признак данных plant.read
    unsigned int fDU;                                       ///< Признак данных результата решения ДУ

    std::vector<std::vector<double>> KC_dipl;               ///< Смещенные коэф неравномерности центральных твэлов в семи слоях
    std::vector<std::vector<double>> TPl_dipl;              ///< Смещенные линейная тепловая мощность в семи слоях
    std::vector<std::vector<double>> WD_dipl;               ///< Смещенные плотность теплоносителя в семи слоях
    std::vector<std::vector<double>> SL_dipl;               ///< Смещенные шлаки в семи слоях

    std::vector<std::vector<double>> ZE;                    ///< Накопленные заряды

    double CB;                                              ///< Концентрация борной кислоты г/кг

    std::vector<std::vector<double>> TPl_DPZ;               ///< Линейная тепловая мощность по ДПЗ в семи слоях
    std::vector<std::vector<double>> TPl7_AS;               ///< Линейная тепловая мощность ТВС в семи слоях
    std::vector<std::vector<std::vector<double>>> TP_ND;    ///< Тепловая мощность расчетных нод
    std::vector<std::vector<std::vector<double>>> NP_ND;    ///< Нейтронная мощность расчетных нод
    std::vector<std::vector<double>> TP_PR;                 ///< Тепловая мощность призм
    std::vector<std::vector<double>> NP_PR;                 ///< Нейтронная мощность призм
    std::vector<double> TP_AS;                              ///< Тепловая мощность ТВС
    std::vector<double> NP_AS;                              ///< Нейтронная мощность ТВС
    double TP_CO;                                           ///< Тепловая мощность зоны
    double NP_CO;                                           ///< Нейтронная мощность зоны
    double ro_XE;                                           ///< Отравление ксеноном
    double ro_SM;                                           ///< Отравление самарием




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
