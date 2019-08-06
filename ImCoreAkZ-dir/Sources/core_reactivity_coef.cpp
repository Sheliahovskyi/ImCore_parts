#include "core.h"

vector<vector<vector<double>>> TP_b, NP_b, F1, F2;
vector<vector<double>> WT, FT, WD, CP, P, XE, SM;
vector<double> WT_in_b, rods_base_position;
double CB_b, HEAT_POWER_b;

/*!
 * \brief CORE::reactivity_coef Расчет коэффициентов реактивности и эффективности стержней ОР СУЗ
 * \author Шеляговский Дмитрий
 */
void CORE::reactivity_coef()
{
    vector<vector<vector<double>>> tmp1(NFA, vector<vector<double>>(NL, vector<double>(DIV, 0)));
    vector<vector<double>> tmp2(NFA, vector<double>(NL,0));
    vector<vector<double>> tmp3(10,  vector<double>(10,0));

    TP_b = NP_b = F1 = F2 = tmp1;
    WT = FT = WD = CP = P = XE = SM = tmp2;
    WT_in_b.resize(NFA);
    rods_base_position.resize(suz.RODS.size());
    rods_efficiency.resize(suz.RODS.size());
    heaviest_rods.resize(suz.RODS.size());
    integral_eff = diff_eff = tmp3;

    double base_state_K;
    // Flags
    fCrit = Cb;
    dTIME = 0;

    double t = clock();

    //**** STATE 0 - Base state ****
    diffusion_equetion();
    base_state_K = K;

    base_param_writing();

    TP_reactivity_coef(base_state_K);
    WT_reactivity_coef(base_state_K);
    FT_reactivity_coef(base_state_K);
    WD_reactivity_coef(base_state_K);
    CB_reactivity_coef(base_state_K);
    //CB99 = subcritical_param(0.99);
    //CB98 = subcritical_param(0.98);
    //CB95 = subcritical_param(0.95);
    //xe_poisoning_calc(base_state_K);
    //sm_poisoning_calc(base_state_K);
    //reactivity_reserv(base_state_K);
    //full_EP_efficiency(base_state_K);
    //conserv_EP_efficiency(base_state_K);
    //groupes_efficiency(base_state_K);


    // *** Console OUT result block ***
    cout << "--Reactivity coefficients running time: " << (clock() - t) / CLOCKS_PER_SEC << " sec." << endl;

    cout << "--Integral efficiency of n-th group: \n";
    for (int n = 9; n < 10; n++)
    {
        for (int h = 0; h < 10; h++)
            cout << integral_eff[n][h] * 1e2 << endl;
        cout << endl;
    }
    cout << endl;
    cout << "--Differential; efficiency of n-th group: \n";
    for (int n = 9; n < 10; n++)
    {
        for (int h = 9; h >= 0; h--)
            cout << diff_eff[n][h] * 1e5 << endl;
        cout << endl;
    }
    cout << endl;
    printf("--CB:                            %.3f g/kg;\n",                CB_b);
    printf("--Power reactivity coef.:        %.3f 1/proc.;\n",             TP_RC * 1e5 * 30);
    printf("--Temperature reactivity coef.:  %.2f 1/K;\n",                 WT_RC * 1e5);
    printf("--Boric reactivity coef.:        %.1f 1/(g/m3);\n",            CB_RC * 1e5);
    printf("--Fuel reactivity coef.:         %.3f 1/K;\n",                 FT_RC * 1e5);
    printf("--Density reactivity coef.:      %.2f 1/(g/cm3);\n",           WD_RC * 1e2);
    printf("--Subcritical 0.99|0.98|0.95 CB: %.3f | %.3f | %.3f g/kg;\n",  CB99, CB98, CB95);
    printf("--XE poisoning:                  %.3f g/kg;\n",                XE_poisoning);
    printf("--SM poisoning:                  %.3f proc.;\n",               SM_poisoning);
    printf("--full EP efficiency:            %.3f proc.;\n",               full_EP * 1e2);
    printf("--Conserv. EP efficiency:        %.3f proc.;\n",               conserv_EP * 1e2);
    printf("--Reactivity reserve:            %.3f proc.;\n",               reactivity_reserve * 1e2);

    //*** Writing results to *.out file ***
    RC_results_writing();

    // Initial parametr seting back
    base_param_setting();
    fAbsorb = ON;
}

/*!
 * \brief Запись рассчитанных коэффициентов реативности в файл
 */

void CORE::RC_results_writing()
{
    ofstream RC("x//res//RC.out" ,ios_base::app);

    char out[1000];
    int data;

    data = sprintf(out, "%.3f\t%.3f\t%.1f\t%.2f\t%.2f\t%.3f \n", CB_b, TP_RC*1e5*30, CB_RC*1e5, WT_RC*1e5, WD_RC*1e2, FT_RC*1e5);
    //data = sprintf(out, "%.3f\t%.3f\t%.1f\t%.2f\t%.2f\t%.3f \n", CB_b, WD_RC*1e2, WT_RC*1e5, FT_RC*1e5, TP_RC*1e5, CB_RC*1e2);

    string str(out);
    RC << str;

    RC.close();
}

/*!
 * \brief Запись полей базового сотояния во временные массивы
 */

void CORE::base_param_writing()
{
    //***BASE parameters writing***
    CB_b = CB;
    HEAT_POWER_b = HEAT_POWER;
    for (int n = 0; n < NFA; n++)
    {
        WT_in_b[n] = FA[n].WT_in;
        for (int h = 0; h < NL; h++)
        {
            WT[n][h] = FA[n].PRI[h].WT;
            FT[n][h] = FA[n].PRI[h].FT;
            WD[n][h] = FA[n].PRI[h].WD;
            CP[n][h] = FA[n].PRI[h].CP;
            P[n][h]  = FA[n].PRI[h].P ;
            XE[n][h] = FA[n].PRI[h].XE;
            SM[n][h] = FA[n].PRI[h].SM;
            for (int i = 0; i < DIV; i++)
            {
                TP_b[n][h][i] = FA[n].PRI[h].ND[i].TP;
                NP_b[n][h][i] = FA[n].PRI[h].ND[i].NP;
                F1[n][h][i] = FA[n].PRI[h].ND[i].F1;
                F2[n][h][i] = FA[n].PRI[h].ND[i].F2;
            }
        }
    }
    for (int n = 0; n < suz.RODS.size(); n++)
        rods_base_position[n] = suz.RODS[n].position;
}

/*!
 * \brief Переход к параметрам базового состояния после расчета коэффициента реактивности
 */

void CORE::base_param_setting()
{
    //***Fields recovering***
    dTIME = 0;
    Keff = 1.0;
    fTH = ON;
    fWT = ON;
    fFT = ON;
    fCrit = off;
    fAbsorb = OFF;

    HEAT_POWER = HEAT_POWER_b;
    CB = CB_b;
    for (int n = 0; n < NFA; n++)
    {
        FA[n].WT_in = WT_in_b[n];
        for (int h = 0; h < NL; h++)
        {
            FA[n].PRI[h].WT = WT[n][h];
            FA[n].PRI[h].FT = FT[n][h];
            FA[n].PRI[h].WD = WD[n][h];
            FA[n].PRI[h].CP = CP[n][h];
            FA[n].PRI[h].P  = P[n][h] ;
            FA[n].PRI[h].XE = XE[n][h];
            FA[n].PRI[h].SM = SM[n][h];
            for (int i = 0; i < DIV; i++)
            {
                FA[n].PRI[h].ND[i].TP = TP_b[n][h][i];
                FA[n].PRI[h].ND[i].NP = NP_b[n][h][i];
                FA[n].PRI[h].ND[i].F1 = F1[n][h][i];
                FA[n].PRI[h].ND[i].F2 = F2[n][h][i];
            }
        }
    }
    //*** OR SUZ position recovering ***
    for (int n = 0; n < suz.RODS.size(); n++)
        suz.RODS[n].position = rods_base_position[n];
}

/*!
 * \brief Функция расчета мощностного коэффициента реактивности
 * \param[in] base_K Значение Кэфф в базовом состоянии
 */

void CORE::TP_reactivity_coef(double base_K)
{
    base_param_setting();
    double base_power;
    //**** STATE 1 - +-10% nominal power ****
    base_power = HEAT_POWER;

    if (base_power >= 0.5 * NOMINAL_POWER)
        HEAT_POWER = 0.9 * base_power;
    else
        HEAT_POWER = 1.1 * base_power;

    for (int n = 0; n < NFA; n++)
        FA[n].WT_in -= (base_power - HEAT_POWER) * (dTin_power/NOMINAL_POWER);

    diffusion_equetion();

    //**** Power reactivity coefficient calculating (1/MW) ****
    TP_RC = (K - base_K) / -(base_power - HEAT_POWER);
}

/*!
 * \brief Функция расчета коэффициента реактивности по температуре теплоносителя
 * \param[in] base_K Значение Кэфф в базовом состоянии
 */

void CORE::WT_reactivity_coef(double base_K)
{
    base_param_setting();
    ////**** STATE 3 - -5K inlet temperature ****
    //double delta_T = 5;
    //for (int n = 0; n < NFA; n++)
    //    FA[n].WT_in -= delta_T;
    //diffusion_equetion();
    //
    ////**** Temperature reactivity coefficient calculating (1/K) ****
    //WT_RC = (K - base_K) / -delta_T;


    //**** STATE 3 - -5K inlet temperature ****
    fWT = OFF;
    double delta_T = 5;
    for (int n = 0; n < NFA; n++)
        for (int h = 0; h < NL; h++)
            FA[n].PRI[h].WT -= delta_T;
    diffusion_equetion();

    //**** Temperature reactivity coefficient calculating (1/K) ****
    WT_RC = (K - base_K) / -delta_T;
}

/*!
 * \brief Функция расчета коэффициента реактивности по температуре топлива
 * \param[in] base_K Значение Кэфф в базовом состоянии
 */

void CORE::FT_reactivity_coef(double base_K)
{
    base_param_setting();
    double base_power = 0;
    vector<double> aver_FT(2, 0);
    //**** STATE 2 - +-5% nominal power ****
    fWT = OFF;
    base_power = HEAT_POWER;
    for(int n = 0; n < NFA; n++)
        for (int h = 0; h < NL; h++)
             aver_FT[0] += FA[n].PRI[h].FT / NFA / NL;
    if (base_power >= 0.5 * NOMINAL_POWER)
        HEAT_POWER = 0.98 * base_power;
    else
        HEAT_POWER = 1.02 * base_power;
    diffusion_equetion();
    for(int n = 0; n < NFA; n++)
        for (int h = 0; h < NL; h++)
             aver_FT[1] += FA[n].PRI[h].FT / NFA / NL;

    //**** FT reactivity coefficient calculating (1/K) ****
    FT_RC = (K - base_K) / -(aver_FT[0]-aver_FT[1]);


    //vector<double> aver_FT(2, 0);
    //double delta_FT = 10;
    //fTH = OFF;
    ////**** STATE 2 - +-5% nominal power ****
    //fWT = OFF;
    //for(int n = 0; n < NFA; n++)
    //    for (int h = 0; h < NL; h++)
    //         aver_FT[0] += FA[n].PRI[h].FT / NFA / NL;
    //
    //for(int n = 0; n < NFA; n++)
    //    for (int h = 0; h < NL; h++)
    //        FA[n].PRI[h].FT -= FA[n].PRI[h].FT/ aver_FT[0] * delta_FT;
    //diffusion_equetion();
    //
    ////**** FT reactivity coefficient calculating (1/K) ****
    //FT_RC = (K - base_K) / -delta_FT;
}

/*!
 * \brief Функция расчета коэффициента реактивности по плотности теплоносителя
 * \param[in] base_K Значение Кэфф в базовом состоянии
 */

void CORE::WD_reactivity_coef(double base_K)
{
    base_param_setting();
    fWT = OFF;
    fFT = OFF;
    vector<double> aver_WD(2, 0);
    ////**** STATE 5 - +0.015 g/cm3 WD -> P = 15.7 - 25 MPa;
    //P_in += 7;
    //P_out += 7;
    //
    //for (int n = 0; n < NFA; n++)
    //    for (int h = 0; h < NL; h++)
    //        FA[n].PRI[h].P += 7;
    //
    //for(int n = 0; n < NFA; n++)
    //    for (int h = 0; h < NL; h++)
    //         aver_WD[0] += FA[n].PRI[h].WD / NFA / NL;
    //
    //diffusion_equetion();
    //
    //for(int n = 0; n < NFA; n++)
    //    for (int h = 0; h < NL; h++)
    //         aver_WD[1] += FA[n].PRI[h].WD / NFA / NL;
    //
    ////**** WD reactivity coefficient calculating (1/(g/cm3)) ****
    //WD_RC = (K - base_K) / -(aver_WD[0]-aver_WD[1]);


    //**** STATE 5 - +0.015 g/cm3 WD -> P = 15.7 - 25 MPa;
    fTH = 0;
    double delta_WD = 0.15;

    for(int n = 0; n < NFA; n++)
        for (int h = 0; h < NL; h++)
             aver_WD[0] += FA[n].PRI[h].WD / NFA / NL;

    for (int n = 0; n < NFA; n++)
        for (int h = 0; h < NL; h++)
            FA[n].PRI[h].WD += FA[n].PRI[h].WD/aver_WD[0] * delta_WD;

    diffusion_equetion();

    //**** WD reactivity coefficient calculating (1/(g/cm3)) ****
    WD_RC = (K - base_K) / delta_WD;
}

/*!
 * \brief Функция расчета коэффициента реактивности по концентрации борной кислоты
 * \param[in] base_K Значение Кэфф в базовом состоянии
 */

void CORE::CB_reactivity_coef(double base_K)
{
    base_param_setting();
    double base_CB = CB;
    //**** STATE 5 - +-0.1 g/kg of Bor concentration ****
    if (base_CB >= 2.86)
        CB -= 0.572;
    else CB += 0.572;
    diffusion_equetion();

    //**** Boric reactivity coefficient calculating (1/(g/kg)) ****
    if (base_CB >= 2.86)
        CB_RC = (K - base_K) / -0.572; // 0.572 g/kg boric acid = 0.1 g/kg bor concentration;
    else
        CB_RC = (K - base_K) / 0.572; // 0.572 g/kg boric acid = 0.1 g/kg bor concentration;

}

/*!
 * \brief Функция расчета концентрации борной кислоты для обеспечения заданого уровня подкритичности
 * \param[in] K_sub Уровень подкритичности (значение Кэфф)
 */

double CORE::subcritical_param(double K_sub)
{
    base_param_setting();
    //***** STATE 6-8 - Subcritical CB concentration calc *****
    fCrit = Cb;
    Keff = K_sub;
    // T_in|XE|SM concentration setting
    for (int n = 0; n < NFA; n++)
    {
        FA[n].WT_in -= HEAT_POWER * (dTin_power / NOMINAL_POWER);
        for (int h = 0; h < NL; h++)
            FA[n].PRI[h].XE = 0;
    }
    // BKB OR SUZ position setting
    for (int n = 1; n <= 10; n++)
        suz.move_group(n, HCORE);

    diffusion_equetion();

    return CB;
}

/*!
 * \brief Функция расчета запаса рективности
 * \param[in] base_K Значение Кэфф в базовом состоянии
 */

void CORE::reactivity_reserv(double base_K)
{
    base_param_setting();
    //***** STATE 9 - Cb = 0; OR SUZ position - BKB *****
    CB = 0;
    for (int n = 1; n <= 10; n++)
        suz.move_group(n, HCORE);

    diffusion_equetion();

    //***Reactivity reserve calculating (%)***
    reactivity_reserve = (K - base_K);
}

/*!
 * \brief Функция расчета полной эффективности аварийной защиты
 * \param[in] base_K Значение Кэфф в базовом состоянии
 */

void CORE::full_EP_efficiency(double base_K)
{
    base_param_setting();
    //***** STATE 10 - full EP efficiency *****
    suz.emergency_protection(NJU);

    diffusion_equetion();

    //**** Full EP efficiency calculating (%) ****
    full_EP = K - base_K;
}

/*!
 * \brief Функция расчета эффективности аварийной защиты без одного наиболее эффективного ОР СУЗ
 * \param[in] base_K Значение Кэфф в базовом состоянии
 */

void CORE::conserv_EP_efficiency(double base_K)
{
    base_param_setting();
    //*** All rods weighting and sorting ***
    for (int n = 0; n < suz.RODS.size(); n++)
        rod_efficiency(n, base_K);
    rods_sorting();

    base_param_setting();
    //***** STATE 11 - conservative EP efficiency *****
    suz.emergency_protection(NJU);
    suz.RODS[heaviest_rods[0]-1].position = rods_base_position[heaviest_rods[0]-1]; //heaviest rod base position setting

    diffusion_equetion();

    //**** Conservative EP efficiency calculating (%) ****
    conserv_EP = K - base_K;
}

/*!
 * \brief Функция расчета дифференциальной и интегральной эффективности рабочей группы
 * \param[in] base_K Значение Кэфф в базовом состоянии
 */

void CORE::groupes_efficiency(double base_K)
{
    double K_VKV = 0;
    double K_pr = 0;
    //*** STATE 14-83 DIFF. and INTEGRAL efficiency of each control group ***
    for (int n = 9; n < 10; n++)
    {
        base_param_setting();
        for(int h = 10; h >= 0; h--)
        {
            if (h == 10)
                suz.move_group(n, HCORE);
            else
                suz.move_group(n, (HCORE)*0.1*h + NKV);

            diffusion_equetion();

            if (h == 10)
                K_VKV = K;

            else
            {
                //**** INTEGRAL efficiency of n-th road group calculating (%) ****
                integral_eff[n][h] = -(K - base_K);
                //**** DIFFERENTIAL efficiency of 10-th road group calculating (%/cm) ****
                diff_eff[n][h] = -(K-K_pr) / (HCORE/10);
            }
            K_pr = K;
        }
    }
    fCrit = Cb;
    diffusion_equetion();
    cout << "CB*** = " << CB << endl;
}

/*!
 * \brief Функция расчета инегральной эффективности одного ОР СУЗ
 * \param[in] n Номер ОР СУЗ
 * \param[in] base_K Значение Кэфф в базовом состоянии
 */

void CORE::rod_efficiency(int n, double base_K)
{
    base_param_setting();
    //**** OR SUZ position setting ****
    suz.RODS[n].position = 0;

    diffusion_equetion();

    rods_efficiency[n] = (K - base_K);
}

/*!
 * \brief Функция нахождения наиболее эффективного (тяжелого) ОР СУЗ
 */

void CORE::rods_sorting()
{
    //*** Rods sorting ***
    vector<int>    x_rods(rods_efficiency.size(), 0);
    vector<double> y_rods(rods_efficiency.size(), 0);
    double tx, ty;
    int j = 0;
    for(int i = 0; i < rods_efficiency.size(); i++)
    {
        y_rods[i] = abs(rods_efficiency[i]);
        x_rods[i] = i+1;
    }
    for(int i=1; i< rods_efficiency.size(); i++)
    {
        tx = x_rods[i];
        ty = y_rods[i];
        j = i - 1;
        while(j >= 0 && y_rods[j] > ty)
        {
            x_rods[j+1] = x_rods[j];
            y_rods[j+1] = y_rods[j];
            j--;
        }
        x_rods[j+1] = tx;
        y_rods[j+1] = ty;
    }
    reverse(x_rods.begin(), x_rods.end());
    reverse(y_rods.begin(), y_rods.end());
    heaviest_rods = x_rods;
}

/*!
 * \brief Функция расчета величины отравления 135Xe
 * \param[in] base_K Значение Кэфф в базовом состоянии
 */

void CORE::xe_poisoning_calc(double base_K)
{
    base_param_setting();
    //***** STATE 12 - XE = 0 *****
    for (int n = 0; n < NFA; n++)
        for (int h = 0; h < NL; h++)
            FA[n].PRI[h].XE = 0;

    diffusion_equetion();

    //*** XE poisoning value calculating ***
    XE_poisoning = K - base_K;
}

/*!
 * \brief Функция расчета величины отравления 149Sm
 * \param[in] base_K Значение Кэфф в базовом состоянии
 */

void CORE::sm_poisoning_calc(double base_K)
{
    base_param_setting();
    //***** STATE 13 - SM = 0 *****
    for (int n = 0; n < NFA; n++)
        for (int h = 0; h < NL; h++)
            FA[n].PRI[h].SM = 0;

    diffusion_equetion();

    //*** SM poisoning value calculating ***
    SM_poisoning = K - base_K;
}
