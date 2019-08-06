#pragma once

#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <ctime>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <ctime>
#include <iomanip>
#include <omp.h>
#include <sstream>
#include "xs.h"
#include "assembly.h"
#include "suz.h"
#include "crod.h"
#include "aprox_polynom.h"
#include "time_imcore.h"
#include <sys/stat.h>
#include <unistd.h>
#include "cstring"
#include <thread>
#include "test_writer.h"
#include "archive.h"


#include "alglibinternal.h"
#include "ap.h"
#include "interpolation.h"



using namespace std;

class ASSEMBLY;
class XS;
class KNI;
class ARCHIVE;

class CORE
{

    // Starting_functions
    void init();
    void formation_core();
    void find_adj_assembly();
    void checkSuzInfoInXsData();

    // Calculation_functions
    void core_power();
    void burnout();
    void absorbers();
    void pin();
    double internal_iteration();
    void external_iteration();
    void normalization();
    void update();
    void boron_crit();
    void power_crit();
    void rods_crit();
    void suz_position();
    void eigen_internal();
    void poisoning_xe_sm();
    void calc_NED();


    // Reading_funtions
    void read_inp_file();
    void read_PATH();
    void read_map();
    void read_fuel_base();
    void read_albedo();
    void read_kni();
    void read_put_sl();
    void read_put_sm();
    void read_put_xe();
    void read_put_pm();
    void read_put_i();
    void read_put_sl_pin();
    void read_put_rod_parts();
    void read_flow();
    void read_thermocouple();

    

    //Reactivity_functions_block
    void TP_reactivity_coef(double);
    void WT_reactivity_coef(double);
    void FT_reactivity_coef(double);
    void WD_reactivity_coef(double);
    void CB_reactivity_coef(double);
    double subcritical_param(double);
    void reactivity_reserv(double);
    void full_EP_efficiency(double);
    void conserv_EP_efficiency(double);
    void groupes_efficiency(double);
    void rod_efficiency(int, double);
    void rods_sorting();
    void base_param_setting();
    void base_param_writing();
    void xe_poisoning_calc(double);
    void sm_poisoning_calc(double);
    void RC_results_writing();

    //Reload_functions_block
    vector<vector<int>> read_fuel_reload_algorithm( vector<string>);
    void assembly_filling(int, int);
    void new_assembly(int, string);

    //Writing_inner_dynamic_fields
    void updateSlFile();
    void updateXeFile();
    void updateSmFile();
    void updateIFile();
    void updatePmFile();
    void updateZeFile();
    void updateSlPinFile();


public:

    vector<ASSEMBLY> FA;
    XS * xs_data;
    SUZ suz;
    TIME_IMCORE time;
    test_writer * t_w;

    time_t last_mod = 0;
    vector<KNI> kni;

    string CoreName;
    string HOMEDIR;

    vector<double> alb_top_11;
    vector<double> alb_top_12;
    vector<double> alb_top_22;
    vector<double> alb_bottom_11;
    vector<double> alb_bottom_12;
    vector<double> alb_bottom_22;
    vector<double> alb_side_11;
    vector<double> alb_side_12;
    vector<double> alb_side_22;

    vector<vector<double>> aver_of_orb;

    vector<double> AFT;

    double AO;
    vector<double> assembly_AO;
    vector<double> DPZ_AO;
    vector<double> KQ;
    double KQ_max;
    double KQ_max_cas;
    vector<vector<double>> KV;
    vector<double> KV_max_assembly;
    double KV_max;
    double KV_max_cas;
    double KV_max_lay;
    vector<vector<vector<double>>> QL;
    vector<double> QL_max_assembly;
    double QL_max;
    double QL_max_cas;
    double QL_max_lay;
    double QL_max_tvel;
    vector<vector<vector<double>>> KK;
    vector<vector<vector<double>>> KO;
    vector<double> KK_max;
    vector<double> KO_max;
    vector<vector<double>> KV_7;
    double KV_7_max;
    double KV_7_max_cas;
    double KV_7_max_lay;
    vector<vector<double>> TPl;
    double XE_aver;
    double XE_top_aver;
    double XE_bottom_aver;
    double SM_aver;
    double SM_top_aver;
    double SM_bottom_aver;
    vector<vector<double>> Krik;
    vector<double> Kri;
    vector<double> Kri_max;
    double Kr_max;
    vector<vector<double>> SL_tvel;
    vector<int> KQ_12;
    vector<int> KV_12;
    vector<int> KO_12;
    vector<int> WT_out_12;
    vector<int> dT_12;
    vector<int> TP_12;
    vector<int> QL_12;
    vector<int> SL_12;

    vector<int> max_rods_efficiency;
    vector<double> integral_eff_10;
    vector<double> diff_eff_10;



    alglib::real_2d_array field_lay;  ///< двумерный вектор библиотеки alglib для бикубической сплайн апроксимации
    alglib::idwinterpolant z1;        ///< результат бикубической апроксимации
    alglib::real_1d_array x;          ///< одномерный вектор библиотеки alglib используется при бикубической апроксимации

    vector<int> spt;                  ///< Вектор признаков работы петель, 4 значения (1/0)
    vector<double> txn;                ///< Значения температуры теплоносителя на входе по четырем петлям, град С


    double EP_efficiency;
    double EP_without_rod;
    double NED;

    double ro_XE = 0;
    double ro_SM = 0;
    double ro_XE_eq = 0;
    double ro_SM_eq = 0;
    double V = 0;


    vector<string> PATH;

    unsigned int NFA;
    unsigned int NL;
    unsigned int DIV;
    unsigned int NPIN;
    unsigned int NFPIN;
    unsigned int NKNI;
    unsigned int fRECOVERY;
    unsigned int fXe = current;
    unsigned int fSm = current;
    unsigned int fTH;
    unsigned int fCrit;             ///< Флаг выбора параметра достижения критичности при расчете
    unsigned int N_pot = 1;
    unsigned int NAREA;
    unsigned int ns;
    unsigned int nd;
    unsigned int nc;
    unsigned int ntc;
    unsigned int NLDPZ;
    unsigned int NTC;
    unsigned int NSUZ;
    unsigned int NGSUZ;
    unsigned int NPT;               ///< Сумарное количество петель первого контура
    unsigned int npt;               ///< Количество работающих петель первого контура
    unsigned int Operative_group;   ///< Номер рабочей группы СУЗ

    double TP_RC;                   ///< Коэффициент реактивности по мощности
    double WT_RC;                   ///< Коэффициент реактивности по температуре теплоносителя
    double FT_RC;                   ///< Коэффициент реактивности по температуре топлива
    double WD_RC;                   ///< Коэффициент реактивности по плотности теплоносителя
    double CB_RC;                   ///< Коэффициент реактивности по концентрации борной кислоты
    double CB99;                    ///< Концентрация борной кислоты обеспечивающая подкритичность -0.01
    double CB98;                    ///< Концентрация борной кислоты обеспечивающая подкритичность -0.02
    double CB95;                    ///< Концентрация борной кислоты обеспечивающая подкритичность -0.05
    double reactivity_reserve;      ///< Запас реактивности
    double full_EP;                 ///< Полная эффективность аварийной защиты
    double conserv_EP;              ///< Эффективность аварийной защиты без одного наиболее эффективного ОР СУЗ
    double XE_poisoning;            ///< Величина отравления 135Xe
    double SM_poisoning;            ///< Величина отравления 149Sm
    double NKV;                     ///< Высота нижнего концевого выключателя
    double NJU;                     ///< Высота нижнего жесткого упора
    double dTin_power;              ///< Изменение входной температуры т-н при изменении мощности от 0 до 100% от номинала
    double Keff = 1.0;              ///< Значение Кэфф
    double pin_step;                ///< шаг расположения твэлов
    double P_out;                   ///< давление на выходе из ТВС
    double P_drop;                  ///< Падение давления по высоте АкЗ


    vector<double> rods_efficiency;      ///< Эффективность стержней ОР СУЗ
    vector<double> groups_efficiency;    ///< Эффективность групп стержней ОР СУЗ
    vector<int> heaviest_rods;           ///< Номера стержней по уменьшению их интегральной эффективности при полном погружении в Ак.З.
    vector<vector<double>> integral_eff; ///< Интегральная эффективность стержней ОР СУЗ [n][h] n=0-9 номер группы; h=0-11 высота
    vector<vector<double>> diff_eff;     ///< Дифференциальная эффективность стержней ОР СУЗ [n][h] n=0-9 номер группы; h=0-11 высота


    unsigned int fWT;
    unsigned int fFT;
    unsigned int fRC = OFF;
    unsigned int fAbsorb;
    bool DBG_FLG = 0;

    double pin_time;
    double DU_time;
    double upd_time;
    double prep_DU_time;
    double inter_time;
    double ext_time;


    enum Flag_status        ///< Статус флага доступа
    {
        OFF,                ///< Выключенный
        ON                  ///< Включенный
    };

    enum Critical_param     ///< Вариации достижения критичности
    {
        off,                ///< Отключение достижения критичности при расчете
        Cb,                 ///< Назначение борной кислоты критическим параметром
        Power,              ///< Назначение мощности критическим параметром
        Rods                ///< Назначение положения рабочей группы критическим параметром
    };

    enum Poisoners_status   ///< Концентрации отравителей
    {
        null,               ///< Отсутствие отравителя при расчете
        eq,                 ///< Равновесное значение отравителя
        SMandPM,            ///< (!только для Sm) Pm + Sm
        SMandPM_eq,         ///< (!только для Sm) Pm_eq + Sm_eq
        current             ///< Текущее значение отравителя
    };

    enum Forecast_type      ///< Типы этапа прогнозного расчета
    {
        empty,              ///< Тип не задан
        MKU,                ///< Расчет водообмена при выходе РУ на МКУ
        power_mode,         ///< Расчет работы РУ на мощности
        company_mode             ///< Расчёт компании
    };

    enum FileNames {
        InitFile,
        XSFile,
        MapFile,
        SmFile,
        XeFile,
        SlFile,
        ZeFile,
        FlowFile,
        SuzFile,
        KniFile,
        AlbedoFile,
        WdFile,
        CpFile,
        FuelReloadFile,
        PinFactFile,
        TcFile,
        CimbFile,
        CompanyFile,
        LogFile,
        IodFile,
        PmFile,
        ForecastFile,
        FuelBaseFile,
        KniReloadFile,
        SlPinFile,
        RestFile,
        TestWriterFile,
        DebugDir,
        ArchiveDir
    };

    double HCORE,
          G,
          CB,
          FASTEP,
          HEAT_POWER,
          NOMINAL_POWER,
          TP,
          NP,
          DEX,
          DIN,
          WT_in,
          LXE,
          EVTOJ,
          SQRT3,
          BARN,
          TEFF,
          LI,
          LPM,
          LRh104,
          LRh104m,
          sRh104,
          sRh104m,
          ALFA,
          h1,
          g1,
          CtoK,
          K,
          dTIME;



    CORE();
    ~CORE();

    void start();
    void reactivity_coef();
    void field_functions();
    void write_to_log(string);
    void make_copy(CORE &);
    void reload(double);
    void write_info();
    void input_read();
    void diffusion_equetion();
    void zero_power_calculate();
    void update_files();
    void set_pressure();
    void test_dpz();
    void debug_write(string);
    void recovery();
};
