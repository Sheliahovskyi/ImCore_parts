#include "kni.h"

KNI::KNI()
{

}

/*!
 * \brief KNI::resize конструктор класса КНИ
 * Расширяет поля до необходимого размера
 */

void KNI::resize()
{
   NLDPZ = myAssembly->myCore->NLDPZ;

   Tok.resize(NLDPZ);
   Tok_pr.resize(NLDPZ);
   I.resize(NLDPZ);
   I_pr.resize(NLDPZ);
   TPl.resize(NLDPZ);
   SED.resize(NLDPZ);

   TPl_displ.resize(NLDPZ);
   SL_displ.resize(NLDPZ);
   WD_displ.resize(NLDPZ);
   KC_displ.resize(NLDPZ);
   F1_displ.resize(NLDPZ);
   F2_displ.resize(NLDPZ);
   WT_displ.resize(NLDPZ);
   XE_displ.resize(NLDPZ);
   SM_displ.resize(NLDPZ);
   FT_displ.resize(NLDPZ);

   k1_pr.resize(NLDPZ, 0);
   k2_pr.resize(NLDPZ, 0);
   y_pr.resize(NLDPZ, 1);
   reliability.resize(NLDPZ,false);

   KV_dpz.resize(NLDPZ, 1);
}



/*!
 * \brief Pow_int   Функция для возведения числа в целую степень
 * \param arg Число для возведения в степень
 * \param pow Степень чила
 * \return Возвращает число arg в степени pow
 */

double Pow_int (double arg, int pow)
{
    double res = 1;
    for (unsigned int i = 0; i < pow; ++i)
        res *= arg;
    return res;
}

/*!
 * \brief KNI::current_conv Функция перевода токов в энерговыделения по ДПЗ
 * \author Шеляговский Дмитрий, Шлапак Игорь
 * Ипользуется методика Цимбалова для получения энерговыделений.
 * Значения энерговыделений рассчитываются в центрах проэктного расположения ДПЗ
 */
void KNI::current_conv()
{

    double s1, s2, s3, s4, s5, s6;
    vector<double> TPl_tmp(7,0);
    // Первод токов в энерговыделения по ДПЗ
    double k_dol = 8.9286E-05 * myAssembly->myCore->TEFF + 1.0;
    //double k_dol = 1;
    for(int h = 0; h < positions.size(); h++)
    {
        s1 = 1 + CIMB[1] * SL_displ[h] + CIMB[2] * Pow_int(SL_displ[h], 2) + CIMB[3] * Pow_int(SL_displ[h], 3) + CIMB[4] * Pow_int(SL_displ[h], 4);
        s2 = 1 + CIMB[5] * (1 + CIMB[6] * SL_displ[h]) * (WD_displ[h]*1e3 - CIMB[17]);
        s3 = 1 + CIMB[7] * (1 + CIMB[8] * SL_displ[h]) * (myAssembly->myCore->CB * 10.821 / 61.821  - CIMB[18]);
        s4 = 1 + (CIMB[9] + CIMB[10] * SL_displ[h]) * (TPl_displ[h] * 1e4 / 312.0  / CIMB[19] - 1);
        s5 = 1 + CIMB[11] * (h+1-1);
        //s5 = 1 + CIMB[11] * (4-(h+1));
        //s6 = (1 + CIMB[12] * myAssembly->SLp_7[h] + CIMB[13] * (SL_displ[h] - myAssembly->SLp_7[h])* myAssembly->SLp_7[h]) * (1 - D) + (1 + CIMB[14] + CIMB[15] * myAssembly->SLp_7[h] + CIMB[16] * Pow(myAssembly->SLp_7[h], 2)) * D;

        SED[h] = k_dol * CIMB[0] * s1 * s2 * s3 * s4 * s5 *  CIMB[20]* (myAssembly->myCore->NFPIN * Ka) / (Emitter_length * KC_displ[h]) / pow((1 - NU * ZE[h]), BETA);
        //SED[h] = CIMB[0] * s1 * s2 * s3 * s4 * s5 *  CIMB[20]* (myAssembly->myCore->NFPIN * Ka) / (Emitter_length * KC_displ[h]) / (1.0 - 3.8126e-3 * ZE[h]);
        //SED[h] = CIMB[0] * s1 * s2 * s3 * s4 * s5 *  CIMB[20]* (myAssembly->myCore->NFPIN) / (Emitter_length * myAssembly->KC_7[h]) / pow((1 - NU * ZE[h]), BETA);

                //--- Power calculation ---
        TPl_tmp[h] = SED[h] * I[h];

        //TPl[h] = SED[h] * I[h]  ;

//        if ( number == 1 && h == 3) {
//            printf("power = %g, s1 = %g, s2 = %g, s3 = %g, s4 = %g, s5 = %g, F(ZE) = %g\n", myAssembly->myCore->HEAT_POWER, s1,s2,s3,s4,s5, pow((1 - NU * ZE[h]), BETA));
//        }
//        cout << myAssembly->type << " ";
//        for (int i = 0; i < 21; i++) {
//            cout << CIMB[i] << " ";
//        }
//        cout << endl;


    }

    // блок учета смещения датчиков
    vector<double> x;
    vector<double> y;

    // насчитываем отклонение ЭВ по датчикам от расчитанного энерговыделения
    // в местах смещения, для апроксимации этого отклонения в места семислойного поля
    for(unsigned int h = 0; h < NLDPZ; h++)
    {
        if (TPl_tmp[h] > 0)
        {
            y.push_back(TPl_tmp[h] / TPl_displ[h]);
            x.push_back(positions[h] + displacement);
        }
    }
    // если достоверных ДПЗ в КНИ меньше четырех - отбрасываем весь канал
    if(y.size() < 4)
    {
        for(unsigned int h = 0; h < NLDPZ; h++)
            TPl[h] = 0;
    }
    else
    {
        //апроксимируем кубическим сплайном расчитанные поправки
        cubic_spline my_spline;
        my_spline.build_spline(x,y);
        // расчитываем ЭВ по ДПЗ в местах семислойного поля
        // используя профиль по высоте рассчитанных значений с корректировкой ДПЗ
        for(unsigned int h = 0; h < NLDPZ; h++)
            TPl[h] = myAssembly->TPl_7[h] * my_spline.f(myAssembly->Z7[h]);
    }

}


/*!
 * \brief KNI::lag_elimination Устарнение запаздывания тока ДПЗ
 * \author Шеляговский Дмитрий
 */
void KNI::lag_elimination()
{
    // *** DPZ current lag elimination ***
    double QQ, PP, m1, m2, a1, a2, b1, b2, e1, e2, f1, y1, k1, k2, y;

    for (unsigned int h = 0; h < positions.size(); h++)
    {
        QQ = LRh104 * LRh104m * (1 + ALFA) / ALFA;
        PP = LRh104 + LRh104m + LRh104 * sRh104 / ALFA / (sRh104 + sRh104m);
        m1 = -PP / 2 + pow((PP*PP / 4 - QQ), 0.5);
        m2 = -PP / 2 - pow((PP*PP / 4 - QQ), 0.5);
        a1 = m2 * (m1 + LRh104) * (m1 + LRh104m) / LRh104 / LRh104m / (m1 - m2);
        a2 = m1 * (m2 + LRh104) * (m2 + LRh104m) / LRh104 / LRh104m / (m1 - m2);
        b1 = exp(m1 * myAssembly->myCore->dTIME / 2) * a1;
        b2 = exp(m2 * myAssembly->myCore->dTIME / 2) * a2;
        e1 = exp(m1 * myAssembly->myCore->dTIME);
        e2 = exp(m2 * myAssembly->myCore->dTIME);
        k1 = e1 * k1_pr[h] + b1 * (Tok[h] - Tok_pr[h]);
        k2 = e2 * k2_pr[h] + b2 * (Tok[h] - Tok_pr[h]);
        f1 = Tok[h] + k1 - k2;
        y1 = f1 / I_pr[h];
        y = y_pr[h] + g1 * (y1 - y_pr[h]);

        // DPZ current after lag elimination
        I[h] = I_pr[h] * y + h1 * (f1 - I_pr[h] * y);

        // New previous value
        Tok_pr[h] = Tok[h];
        I_pr[h] = I[h];
        y_pr[h] = y;
        k1_pr[h] = k1;
        k2_pr[h] = k2;
    }
}
/*!
 * \brief KNI::model_dpz_current Расчет модельных токов ДПЗ
 * \param h Номер слоя ДПЗ
 * \return Возвращает значение модельного тока ДПЗ
 * \author Шеляговский Дмитрий
 */
double KNI::model_dpz_current(int h)
{
    // *** Model DPZ current ***
    return TPl_displ[h] / SED[h] ;
}


/*!
 * \brief KNI::dpz_charge Расчет накопленных зарядов ДПЗ
 * \author Шеляговский Дмитрий
 */
void KNI::dpz_charge()
{
    double Current = 0;
    // *** DPZ charge (ZE) calculation ***
    for (unsigned int h = 0; h < positions.size(); h++)
    {
        if (I[h] > 0){
            ZE[h] += I[h]/1e6 * myAssembly->myCore->dTIME * 3600.0 * 24.0;

        }


        else
        {
           I[h] = model_dpz_current(h);
           ZE[h] +=I[h]/1e6 * myAssembly->myCore->dTIME * 3600.0 * 24.0;
        }
    }
}
