#include <core.h>


struct DATA
{
    string name = "";
    double value = 0;
    int auth = -1;
};

double give(vector<DATA> data, string name)
{
    for (unsigned int i = 0; i < data.size(); ++i)
        if (data[i].name == name && data[i].auth == 0)
        {
            return data[i].value;
        }
    return 0;
}


CORE::CORE()
{
    HOMEDIR = "./";
    AFT.resize(5);
}

CORE::~CORE()
{

}

void CORE::start()
{

    init();
    cout << "in done" << std::endl;
    xs_data->myCore = this;
    cout << "CS readed" << std::endl;
    xs_data->read_cs_data(PATH[XSFile]);
    cout << "term " << std::endl;
    xs_data->read_therm_data(PATH[WdFile], PATH[CpFile]);
    cout << "pin" << std::endl;
    xs_data->read_pin_data(PATH[PinFactFile]);
    cout << "cimb" << std::endl;
    xs_data->read_cimb_data(PATH[CimbFile]);
    cout << "suz" << std::endl;
    suz.read_suz_data(PATH[SuzFile]);
    cout << "formation core" << std::endl;
    formation_core();
    cout << "thermocouple" << std::endl;
    read_thermocouple();
    cout << "check suz" << std::endl;
    checkSuzInfoInXsData();
    cout << "read kni" << std::endl;
    read_kni();
    cout << "read flow" << std::endl;
    read_flow();
    cout << "put sl" << std::endl;
    read_put_sl();
    cout << "sl pin" << std::endl;
    read_put_sl_pin();
    cout << "sm" << std::endl;
    read_put_sm();
    cout << "xe" << std::endl;
    read_put_xe();
    cout << "pm" << std::endl;
    read_put_pm();
    cout << "i" << std::endl;
    read_put_i();
    cout << "rod" << std::endl;
    read_put_rod_parts();
    cout << "suz pos" << std::endl;
    suz_position();
    cout << "Files readed" << std::endl;

    // Задание первоначальных значений
    G = 64 *1e9/3600;
    HEAT_POWER = NOMINAL_POWER;
    TP = NOMINAL_POWER;
    NP = 0;
    CB = 8.0;
    Operative_group = 10;
    set_pressure();
    for(int n = 0; n < NFA; ++n)
    {

        FA[n].set_begin_value();
        for (unsigned int h =0 ; h <NL; ++h)
            xs_data->cs_update(FA[n].PRI[h]);
    }
    core_power();
    write_to_log("start comleted");

}




void CORE::init()
{
    read_PATH();
    cout << "Path readed" << std::endl;
    read_inp_file();
    kni.resize(NKNI);

    assembly_AO.resize(NFA);
    DPZ_AO.resize(NKNI);
    KQ.resize(NFA);
    KV.resize(NFA, vector<double>(NL));
    TPl.resize(NFA, vector<double>(NL));
    KV_7.resize(NFA, vector<double>(NLDPZ));
    KV_max_assembly.resize(NFA);
    Kri_max.resize(NFA);

    QL.resize(NFA, vector<vector<double>>(NPIN, vector<double>(NL)));
    KK.resize(NFA, vector<vector<double>>(NPIN, vector<double>(NL)));
    KO.resize(NFA, vector<vector<double>>(NPIN, vector<double>(NL)));
    QL_max_assembly.resize(NFA);
    KK_max.resize(NFA);
    KO_max.resize(NFA);

    Krik.resize(NFA, vector<double>(NPIN));
    Kri.resize(NFA);

    txn.resize(NPT);
    spt.resize(NPT);


    read_albedo();
    SQRT3 = pow(3,0.5);
    aver_of_orb.resize(19, vector<double> (7, 0));
    cout << "Init done" << std::endl;

}

void CORE::core_power()
{
    TP = 0;
    NP = 0;
    for (unsigned int n = 0; n < NFA; n++)
    {
        FA[n].power();
        TP += FA[n].TP;
        NP += FA[n].NP;
        FA[n].interpolation_7_lays();
    }
}


void CORE::burnout()
{
    for (unsigned int n = 0; n < NFA; n++)
        FA[n].burnout();
}

void CORE::absorbers()
{
    for (unsigned int n = 0; n < NFA; n++)
        FA[n].absorbers();
}


void CORE::suz_position()
{
    for (unsigned int n = 0; n < suz.RODS.size(); n++)
    {
        int assemblyNamber = suz.RODS[n].assembly;
        for (unsigned int h = 0; h < NL; h++)
        {
            double centerPrism = FA[assemblyNamber].PRI[h].Z;
            double heightPrism = FA[assemblyNamber].PRI[h].height;

            FA[assemblyNamber].PRI[h].Rod_part = suz.suz_position(centerPrism, heightPrism, n);
        }
    }
}

void CORE::checkSuzInfoInXsData()
{
    for (unsigned int i = 0; i < suz.RODS.size(); i++)
    {
        if(suz.RODS[i].type_length.size()+1 != xs_data->CS[FA[suz.RODS[i].assembly].PRI[1].type][13])
        {
            write_to_log("Rod types nubmer does not mutch number of constant sets!");
        }
    }
}

/*!
 * \brief CORE::pin Расчет потвэльного энерговыделения
 * Расчитывает энерговыделение гомогенизированных облостей в местах расположения твэл
 * методом бикубической сплайн апроксимации по расчетным слоям активной зоны
 * полученные значению умножаются на неравномерность энерговыделения в ТВС бесконечной решетке (pin-факторы)
 */

void CORE::pin()
{
    //for (unsigned int n =0; n < NFA; ++n)
    //    FA[n].pin();


    double tBeg = 0;
    double t1 = 0;
    double t2 = 0;


    #pragma omp parallel for num_threads(N_pot)
    for (unsigned int h = 0; h < NL; h++)
    {
        double t4 = omp_get_wtime();
        alglib::real_2d_array field_lay;  ///< двумерный вектор библиотеки alglib для дикубической сплайн апроксимации
        alglib::idwinterpolant z1;        ///< результат бикубической апроксимации
        alglib::real_1d_array x;          ///< одномерный вектор библиотеки alglib используется при бикубической апроксимации
        x.setlength(2);

        field_lay.setlength(NFA*DIV,3);

        for (unsigned int n = 0 ; n < NFA; n++)
        {
            for (unsigned int p = 0; p < NPIN; ++p)
                FA[n].PRI[h].pin_power[p] = 0;

            xs_data->pin_update(FA[n].PRI[h]);

            for (unsigned int d = 0; d < DIV; d++ )
            {
                field_lay(n*DIV+d, 0) = FA[n].PRI[0].ND[d].X;
                field_lay(n*DIV+d, 1) = FA[n].PRI[0].ND[d].Y;
                field_lay(n*DIV+d,2) = FA[n].PRI[h].ND[d].TP ;
            }
        }

        int nx = 2;         // размерность пространства
        int d = 2;      // метод отклонения 2 - круто
        int nodes = DIV*NFA;  // кол-во узлов
        int nq = 4;    // коэффициенты 15
        int nw = 6;    // коэффициенты 25

        double t3 = omp_get_wtime();
        alglib::idwbuildmodifiedshepard(field_lay, nodes, nx, d, nq, nw, z1);
        t2 += omp_get_wtime() - t3;

        for (unsigned int n = 0 ; n < NFA; n++)
        {
            for (unsigned int p = 0; p < NPIN; ++p)
            {
                x(0) = FA[n].PRI[h].pin_x[p];
                x(1) = FA[n].PRI[h].pin_y[p];

                t3 = omp_get_wtime();
                FA[n].PRI[h].pin_power[p] = idwcalc(z1, x) * FA[n].PRI[h].pin_factors[p] * DIV/NFPIN;
                t2 += omp_get_wtime() - t3;
            }

            double sumKC = (FA[n].PRI[h].pin_power[144] + FA[n].PRI[h].pin_power[145]
                         + FA[n].PRI[h].pin_power[164] + FA[n].PRI[h].pin_power[166]
                         + FA[n].PRI[h].pin_power[185] + FA[n].PRI[h].pin_power[186])/6.0;
            double sum = 0;
            for (unsigned int p =0 ; p < NPIN; ++p)
                sum += FA[n].PRI[h].pin_power[p];
            FA[n].PRI[h].KC = sumKC / sum * NFPIN;
        }

        t1 += omp_get_wtime() - t4;
    }

    printf("Time pin all = %g\n", t1);
    printf("Time alglib  = %g, part = %g \n", t2, t2/t1 * 100);

}


/*!
 * \brief CORE::test_dpz Функция предназначена для отбраковки показаний ДПЗ
 * \author Шеляговский Дмитрий
 * Отбраковка проводится по отклонению показания ДПЗ от аксиального профиля всех ДПЗ на величину более 3 СКО
 * СКО расчитывается по отклонениям всех ДПЗ от среднего значения аксиального профиля
 */
void CORE::test_dpz()
{
    // *** DPZ reliability test ***
    vector<double> average(NKNI, 0);
    vector<vector<double>> Normilized_Tok(NKNI, vector<double>(NLDPZ, 0));
    vector<double> Kz (NLDPZ, 0);
    vector<double> SKO(NLDPZ, 0);
    vector<double> dev(NLDPZ, 0);
    // Each DPZ narmalization per average KNI value
    int cnt = 0;
    for (unsigned int k = 0; k < NKNI; k++)
    {
        int cnt = 0;
        for (unsigned int h = 0; h < NLDPZ; h++)
        {
            if(kni[k].I[h] > 1e-3)
            {
                kni[k].reliability[h] = true;
                average[k] += kni[k].I[h];
                cnt++;
            }
            else
                kni[k].reliability[h] = false;
        }
        average[k] /= cnt;
     }

    for (unsigned int k = 0; k < NKNI; k++)
        for (unsigned int h = 0; h < NLDPZ; h++)
        {
            if(kni[k].reliability[h])
                Normilized_Tok[k][h] = kni[k].I[h] / average[k];
            else
                Normilized_Tok[k][h] = 0;
        }

    // Middle value per each layer calculating (Kz)
    for (unsigned int h = 0; h < NLDPZ; h++)
    {
        cnt = 0;
        for (unsigned int k = 0; k < NKNI; k++)
            if (kni[k].reliability[h])
            {
                cnt++;
                Kz[h] += Normilized_Tok[k][h];
            }
        Kz[h] /= cnt;
    }

    // SKO claculating:
    for(int h = 0; h < NLDPZ; h++)
    {
        cnt = 0;
        for(int k = 0; k < NKNI; k++)
        {
            if(kni[k].reliability[h])
            {
                cnt++;
                dev[h] += pow((Normilized_Tok[k][h] - Kz[h]) / Kz[h], 2.0);
            }
        }
        SKO[h] = pow(dev[h]/ (cnt-1.0), 0.5);
    }

    for (unsigned int k = 0; k < NKNI; k++)
        for (unsigned int h = 0; h < NLDPZ; h++)
            if (abs(Normilized_Tok[k][h] - Kz[h])/Kz[h] > 3*SKO[h] )
            {
                kni[k].I[h] = kni[k].Tok[h] = 0.0;
                kni[k].reliability[h] = false;
            }

    for (unsigned int k = 0; k < NKNI; k++) {
        double nDPZon = 0;
        for (unsigned int h = 0; h < NLDPZ; h++) {
            if ( kni[k].reliability[h] ) {
                nDPZon += 1;
            }
        }

        if ( nDPZon < 4 ) {
            for (unsigned int h = 0; h < NLDPZ; h++) {
                kni[k].I[h] = kni[k].Tok[h] = 0.0;
                kni[k].reliability[h] = false;
            }
        }
    }

}


void CORE::write_to_log(string text)
{

    ofstream file(PATH[LogFile], ios_base::app);
    file << time.givetxt() << "   " << text << endl;
    cout << text << endl;
    file.close();

}

/*!
 * \brief CORE::set_pressure Функция для расчета давления в призмах по линейному распределению
 */
void CORE::set_pressure()
{
    for (int n = 0; n < NFA; n++ )
        for (int h = 0; h < NL; ++h)
            FA[n].PRI[h].P = P_out + P_drop / HCORE * (HCORE - FA[n].PRI[h].Z);
}

void CORE::updateSlFile() {
    int size = NFA * NL * 20;
    int current_size = 0;
    char * sBufer = new char[size];

    current_size += sprintf(&sBufer[current_size], "%-30s %-15.5f\n", time.givedate().c_str(), TEFF);
    for ( int n = 0; n < NFA; n++ ) {
        for ( int h = 0; h < NL; h++ ) {
            current_size += sprintf(&sBufer[current_size], "%-15.5f", FA[n].PRI[h].SL);
        }
        current_size += sprintf(&sBufer[current_size], "\n");
    }

    int index = SlFile;
    FILE * out = fopen((PATH[index]+".tmp").c_str(), "w");
    if ( out ) {
        fprintf(out, "%s", sBufer);
        fclose(out);
        remove((PATH[index]).c_str());
        if (rename((PATH[index]+".tmp").c_str(), PATH[index].c_str() )!=0)
        {
            write_to_log("Error to rename file " + PATH[index]+".tmp");
        }
        //remove((PATH[index]+".tmp").c_str());
    } else {
        write_to_log("Cannot open file for writing " + PATH[index]);
    }

    delete sBufer;
}

void CORE::updateXeFile() {
    int size = NFA * NL * 20;
    int current_size = 0;
    char * sBufer = new char[size];

    for ( int n = 0; n < NFA; n++ ) {
        for ( int h = 0; h < NL; h++ ) {
            current_size += sprintf(&sBufer[current_size], "%-15.5e", FA[n].PRI[h].XE);
        }
        current_size += sprintf(&sBufer[current_size], "\n");
    }

    int index = XeFile;
    FILE * out = fopen((PATH[index]+".tmp").c_str(), "w");
    if ( out ) {
        fprintf(out, "%s", sBufer);
        fclose(out);
        remove((PATH[index]).c_str());
        if (rename((PATH[index]+".tmp").c_str(), PATH[index].c_str() )!=0)
        {
            write_to_log("Error to rename file " + PATH[index]+".tmp");
        }
        //remove((PATH[index]+".tmp").c_str());
    } else {
        write_to_log("Cannot open file for writing " + PATH[index]);
    }

    delete sBufer;
}

void CORE::updateSmFile() {
    int size = NFA * NL * 20;
    int current_size = 0;
    char * sBufer = new char[size];

    for ( int n = 0; n < NFA; n++ ) {
        for ( int h = 0; h < NL; h++ ) {
            current_size += sprintf(&sBufer[current_size], "%-15.5e", FA[n].PRI[h].SM);
        }
        current_size += sprintf(&sBufer[current_size], "\n");
    }

    int index = SmFile;
    FILE * out = fopen((PATH[index]+".tmp").c_str(), "w");
    if ( out ) {
        fprintf(out, "%s", sBufer);
        fclose(out);
        remove((PATH[index]).c_str());
        if (rename((PATH[index]+".tmp").c_str(), PATH[index].c_str() )!=0)
        {
            write_to_log("Error to rename file " + PATH[index]+".tmp");
        }
        //remove((PATH[index]+".tmp").c_str());
    } else {
        write_to_log("Cannot open file for writing " + PATH[index]);
    }

    delete sBufer;
}

void CORE::updateIFile() {
    int size = NFA * NL * 20;
    int current_size = 0;
    char * sBufer = new char[size];

    for ( int n = 0; n < NFA; n++ ) {
        for ( int h = 0; h < NL; h++ ) {
            current_size += sprintf(&sBufer[current_size], "%-15.5e", FA[n].PRI[h].I);
        }
        current_size += sprintf(&sBufer[current_size], "\n");
    }

    int index = IodFile;
    FILE * out = fopen((PATH[index]+".tmp").c_str(), "w");
    if ( out ) {
        fprintf(out, "%s", sBufer);
        fclose(out);
        remove((PATH[index]).c_str());
        if (rename((PATH[index]+".tmp").c_str(), PATH[index].c_str() )!=0)
        {
            write_to_log("Error to rename file " + PATH[index]+".tmp");
        }
        //remove((PATH[index]+".tmp").c_str());
    } else {
        write_to_log("Cannot open file for writing " + PATH[index]);
    }

    delete sBufer;
}

void CORE::updatePmFile() {
    int size = NFA * NL * 20;
    int current_size = 0;
    char * sBufer = new char[size];

    for ( int n = 0; n < NFA; n++ ) {
        for ( int h = 0; h < NL; h++ ) {
            current_size += sprintf(&sBufer[current_size], "%-15.5e", FA[n].PRI[h].PM);
        }
        current_size += sprintf(&sBufer[current_size], "\n");
    }

    int index = PmFile;
    FILE * out = fopen((PATH[index]+".tmp").c_str(), "w");
    if ( out ) {
        fprintf(out, "%s", sBufer);
        fclose(out);
        remove((PATH[index]).c_str());
        if (rename((PATH[index]+".tmp").c_str(), PATH[index].c_str() )!=0)
        {
            write_to_log("Error to rename file " + PATH[index]+".tmp");
        }
        //remove((PATH[index]+".tmp").c_str());
    } else {
        write_to_log("Cannot open file for writing " + PATH[index]);
    }

    delete sBufer;
}

void CORE::updateZeFile() {
    int size = NKNI * NLDPZ * 20;
    int current_size = 0;
    char * sBufer = new char[size];

    for ( int n = 0; n < NKNI; n++ ) {
        for ( int h = 0; h < NLDPZ; h++ ) {
            current_size += sprintf(&sBufer[current_size], "%-15.5f", kni[n].ZE[h]);
        }
        current_size += sprintf(&sBufer[current_size], "\n");
    }

    int index = ZeFile;
    FILE * out = fopen((PATH[index]+".tmp").c_str(), "w");
    if ( out ) {
        fprintf(out, "%s", sBufer);
        fclose(out);
        remove((PATH[index]).c_str());
        if (rename((PATH[index]+".tmp").c_str(), PATH[index].c_str() )!=0)
        {
            write_to_log("Error to rename file " + PATH[index]+".tmp");
        }
        //remove((PATH[index]+".tmp").c_str());
    } else {
        write_to_log("Cannot open file for writing " + PATH[index]);
    }

    delete sBufer;
}

void CORE::updateSlPinFile() {
    int size = NFA * NL * NPIN * 20;
    int current_size = 0;
    char * sBufer = new char[size];

    for ( int n = 0; n < NFA; n++ ) {
        for ( int h = 0; h < NL; h++ ) {
            for (int p = 0; p < NPIN; p++ ) {
                current_size += sprintf(&sBufer[current_size], "%-15.5f", FA[n].PRI[h].PinSl[p]);
            }
            current_size += sprintf(&sBufer[current_size], "\n");
        }
    }

    int index = SlPinFile;
    FILE * out = fopen((PATH[index]+".tmp").c_str(), "w");
    if ( out ) {
        fprintf(out, "%s", sBufer);
        fclose(out);
        remove((PATH[index]).c_str());
        if (rename((PATH[index]+".tmp").c_str(), PATH[index].c_str() )!=0)
        {
            write_to_log("Error to rename file " + PATH[index]+".tmp");
        }
        //remove((PATH[index]+".tmp").c_str());
    } else {
        write_to_log("Cannot open file for writing " + PATH[index]);
    }

    delete sBufer;
}


/*!
 * \brief CORE::update_files Функция для обновления файлов шлаков, отравителей
 */

void CORE::update_files()
{

    // Обновление файла шлаков
    updateSlFile();

    // Обновление файла ксенона
    updateXeFile();

    // Обновление файла самария
    updateSmFile();

    // Обновление файла йода
    updateIFile();

    // Обновление файла прометия
    updatePmFile();

    // Обновление файла шлаков твэлов
    updateSlPinFile();

    // Обновление файла накопленных зарядов
    updateZeFile();

}

void CORE::boron_crit()
{
    if (fCrit == Cb)
    {
        double borD = 0.000001;


        if(K < Keff-1E-2)
            borD = -0.5;
        else if(K > Keff+1E-2)
            borD = -0.5;

        else if(K < Keff-1E-3)
            borD = -0.05;
        else if(K > Keff+1E-3)
            borD = -0.05;

        else if(K < Keff-1E-4)
            borD = -0.01;
        else if(K > Keff+1E-4)
            borD = -0.01;

        else if(K < Keff-5E-5)
            borD = -0.008;
        else if(K > Keff+5E-5)
            borD = -0.008;

        else if(K < Keff-1E-5)
            borD = -0.004;
        else if(K > Keff+1E-5)
            borD = -0.004;

        else if(K < Keff-1E-6)
            borD = -0.0007;
        else if(K > Keff+1E-6)
            borD = -0.0007;

        else if(K < Keff-1E-7)
            borD = -0.0001;
        else if(K > Keff+1E-7)
            borD = -0.0001;

        else if(K < Keff-1E-8)
            borD = -0.00002;
        else if(K > Keff+1E-8)
            borD = -0.00002;

        else if(K < Keff-1E-9)
            borD = -0.00001;
        else if(K > Keff+1E-9)
            borD = -0.00001;

        if(K - Keff > 0)
            borD *= -0.8;
        CB += borD;
    }
}

void CORE::power_crit()
{
    if(fCrit == Power)
    {
        double powerD = 0;


        if     (K < Keff-1E-2)
            powerD -= 1000;
        else if(K > Keff+1E-2)
            powerD += 1000 * 0.45;
        else if(K < Keff-5E-3)
            powerD -= 300;
        else if(K > Keff+5E-3)
            powerD += 300 * 0.45;
        else if(K < Keff-1E-3)
            powerD -= 100;
        else if(K > Keff+1E-3)
            powerD += 100 * 0.45;
        else if(K < Keff-5E-4)
            powerD -= 50;
        else if(K > Keff+5E-4)
            powerD += 50 * 0.45;
        else if(K < Keff-1E-4)
            powerD -= 10;
        else if(K > Keff+1E-4)
            powerD += 10 * 0.45;
        else if(K < Keff-1E-5)
            powerD -= 1;
        else if(K > Keff+1E-5)
            powerD += 1 * 0.45;
        else if(K < Keff-1E-6)
            powerD -= 0.2;
        else if(K > Keff+1E-6)
            powerD += 0.2 * 0.45;
        else if(K < Keff-5E-7)
            powerD -= 0.01;
        else if(K > Keff+5E-7)
            powerD += 0.01 * 0.45;
        else if(K < Keff-5E-8)
            powerD -= 0.005;
        else if(K > Keff+5E-8)
            powerD += 0.005 * 0.45;

        if(HEAT_POWER + powerD < 0 || HEAT_POWER +powerD > 3300)
        {
            cout << "Incorrect heat power value (0-3300 MW)";
        }


        HEAT_POWER += powerD;
    }
}

void CORE::rods_crit()
{
    if(fCrit == Rods)
    {
        double heightD = 0;
        double position = 0;

        if (K < Keff-1E-5)
            heightD -= 1;
        else if(K > Keff+1E-5)
            heightD += 1 * 0.45;
        else if (K < Keff-1E-6)
            heightD -= 0.5;
        else if(K > Keff+1E-6)
            heightD += 0.5 * 0.45;
        else if (K < Keff-1E-7)
            heightD -= 0.1;
        else if(K > Keff+1E-7)
            heightD += 0.1 * 0.45;
        else if (K < Keff-1E-8)
            heightD -= 0.05;
        else if(K > Keff+1E-8)
            heightD += 0.05 * 0.45;


        position = suz.give_group_position(Operative_group) - heightD;

        if(position > HCORE || position <= 248.5)
        {
            cout << "Incorrect operative group position (70-100 %)";
        }

        suz.move_group(Operative_group, position);
        suz_position();
    }
}

void CORE::debug_write(string file_name)
{

    if ( !DBG_FLG ) {
        return;
    }
    
    char * sBufer = new char [500000];
    int current_size = 0;

    current_size += sprintf(&sBufer[current_size], "%-15.5f %-15.5f\n\n", CB, AO);


    for (int n = 0; n < NFA; n++)
    {
        for (int h = 0; h < NL; h++)
            current_size += sprintf(&sBufer[current_size], "%-10.4f", KV[n][h]);
        current_size += sprintf(&sBufer[current_size], "\n");
    }
    current_size += sprintf(&sBufer[current_size], "\n");

    for (int n = 0; n < NFA; n++)
    {
        for (int h = 0; h < NLDPZ; h++)
            current_size += sprintf(&sBufer[current_size], "%-10.4f", KV_7[n][h]);
        current_size += sprintf(&sBufer[current_size], "\n");
    }

    sBufer[current_size++] = '\0';

    FILE * out = fopen((PATH[DebugDir]+"/res_"+file_name+".out").c_str(), "w");
    if(out)
    {
    	fprintf(out, "%s", sBufer);
    	fclose(out);
    } else {
        cout << PATH[DebugDir]+"/res_"+file_name+".out" << endl;
    }

    current_size = 0;
    current_size += sprintf(&sBufer[current_size], "%-20s", time.givedate().c_str());

    for (int i = 0; i < 10; i++)
        current_size += sprintf(&sBufer[current_size], "%15.5e", suz.GROUPS[i]);

    for (int i = 0; i < suz.RODS.size(); i++)
        current_size += sprintf(&sBufer[current_size], "%15.5e", suz.RODS[i].position);

    current_size += sprintf(&sBufer[current_size], "%15.5e", HEAT_POWER);

    for (int k = 0; k < NKNI; k++)
        for (int h = 0; h < NLDPZ; h++)
            current_size += sprintf(&sBufer[current_size], "%15.5e", kni[k].I[h]);

    current_size += sprintf(&sBufer[current_size], "%3i", npt);

    for (int i = 0; i < spt.size(); i++)
        current_size += sprintf(&sBufer[current_size], "%3i", spt[i]);

    current_size += sprintf(&sBufer[current_size], "%15.5e", P_out * 9.8);

    for (int i = 0; i < txn.size(); i++)
        current_size += sprintf(&sBufer[current_size], "%15.5e", txn[i]);

    for(unsigned int n = 0 ; n < NFA; n++)
    {
        if(FA[n].NTC)
        {
            current_size += sprintf(&sBufer[current_size], "%15.5e", FA[n].thermocouple);
        }
    }

    current_size += sprintf(&sBufer[current_size], "%15.3f\n", G*3600*1e-9);
    sBufer[current_size++] = '\0';
    out = fopen((PATH[DebugDir]+"/plant_reading.out").c_str(), "a");
    if(out) {
    	fprintf(out, "%s", sBufer);
		fclose(out);
    } else {
        cout << "Cannot open " + PATH[DebugDir] + "/plant_reading.out" << endl;
    }



    current_size = 0;

    //Запишем дату и Тэфф
    current_size += sprintf(&sBufer[current_size], "%-22s\n", time.givedate().c_str());
    current_size += sprintf(&sBufer[current_size], "%-15.5f\n", TEFF);

    //Запишем накопленные заряды
    for (unsigned int k = 0; k < NKNI; k++) {
        for (unsigned int h = 0; h < NLDPZ; h++) {
            current_size += sprintf(&sBufer[current_size], "%-15.5f", kni[k].ZE[h]);
        }
        current_size += sprintf(&sBufer[current_size], "\n");
    }
    current_size += sprintf(&sBufer[current_size], "\n");

    //Запишем высоты участков
    for (unsigned int h = 0; h < NL; h++ ) {
        current_size += sprintf(&sBufer[current_size], "%-15.5f", FA[0].PRI[h].height);
    }
    current_size += sprintf(&sBufer[current_size], "\n\n");

    //Запиишем шлаки
    for (unsigned int n = 0 ; n < NFA; ++n)
    {
        for (unsigned int h = 0; h < NL; ++h)
            current_size += sprintf(&sBufer[current_size], "%-15.5f", FA[n].PRI[h].SL);
        current_size += sprintf(&sBufer[current_size], "\n");
    }
    current_size += sprintf(&sBufer[current_size], "\n");

    //Запишем ксенон
    for (unsigned int n = 0 ; n < NFA; ++n)
    {
        for (unsigned int h = 0; h < NL; ++h)
            current_size += sprintf(&sBufer[current_size], "%-15.5e", FA[n].PRI[h].XE);
        current_size += sprintf(&sBufer[current_size], "\n");
    }
    current_size += sprintf(&sBufer[current_size], "\n");

    //Запишем самарий
    for (unsigned int n = 0 ; n < NFA; ++n)
    {
        for (unsigned int h = 0; h < NL; ++h)
            current_size += sprintf(&sBufer[current_size], "%-15.5e", FA[n].PRI[h].SM);
        current_size += sprintf(&sBufer[current_size], "\n");
    }
    current_size += sprintf(&sBufer[current_size], "\n");

    //Запишем йод
    for (unsigned int n = 0 ; n < NFA; ++n)
    {
        for (unsigned int h = 0; h < NL; ++h)
            current_size += sprintf(&sBufer[current_size], "%-15.5e", FA[n].PRI[h].I);
        current_size += sprintf(&sBufer[current_size], "\n");
    }
    current_size += sprintf(&sBufer[current_size], "\n");

    //Запишем прометий
    for (unsigned int n = 0 ; n < NFA; ++n)
    {
        for (unsigned int h = 0; h < NL; ++h)
            current_size += sprintf(&sBufer[current_size], "%-15.5e", FA[n].PRI[h].PM);
        current_size += sprintf(&sBufer[current_size], "\n");
    }
    current_size += sprintf(&sBufer[current_size], "\n");

    sBufer[current_size++] = '\0';
    out = fopen((PATH[DebugDir]+"/rest_"+file_name+".out").c_str(), "w");
    if(out) {
    	fprintf(out, "%s",sBufer);
		fclose(out);
	} else {
        cout << "Cannot open " + PATH[DebugDir]+"/rest_"+file_name+".out" << endl;
	}

    delete sBufer;

}


void CORE::diffusion_equetion()
{
    pin_time     = 0;
    DU_time      = 0;
    upd_time     = 0;
    prep_DU_time = 0;
    inter_time   = 0;
    ext_time     = 0;

    double t0 = omp_get_wtime();
    K = 1.01;
    double K_pr1 = 1.05, K_pr2 = 1.1;
    bool flag = true;
    double t = clock();
    suz_position();
    double t1 = omp_get_wtime();
    pin();
    pin_time = omp_get_wtime() - t1;
    int cnt = 0;

    t1 = omp_get_wtime();
    while (flag && cnt++ < 100)
    {
        if(fCrit != off)
        {
            if(abs(K - Keff) < DEX && abs(K_pr1 - Keff) < DEX )
                flag = false;
        }
        else
        {
            if(abs(K - K_pr1) < DEX && abs(K_pr1 - K_pr2) < DEX )
                flag = false;
        }
        K_pr2 = K_pr1;
        K_pr1 = K;
        external_iteration();
    }
    ext_time = omp_get_wtime() - t1;

    if (fRECOVERY == ON && TP/NOMINAL_POWER > 0.1)
            recovery();
    else
        normalization();
    burnout();

    for (unsigned int n = 0; n < NFA; ++n)
    {
        for (unsigned int h = 0; h < NL; ++h)
        {
            FA[n].PRI[h].XE += FA[n].PRI[h].dXE;
            FA[n].PRI[h].I  += FA[n].PRI[h].dI;
            FA[n].PRI[h].SM += FA[n].PRI[h].dSM;
            FA[n].PRI[h].PM += FA[n].PRI[h].dPM;
        }

    }

    poisoning_xe_sm();       //Расчет величины отравления
    field_functions();       //Расчет функционалов поля
    calc_NED();              //Расчет мощности по ДПЗ

    TEFF += dTIME * HEAT_POWER / NOMINAL_POWER;          //Расчет Тэфф

    DU_time = omp_get_wtime() - t0;

//    if ( true ) {
//        printf("ALL time   = %e\n", DU_time);
//        printf("Pin time   = %e, part = %f\n", pin_time,      pin_time / DU_time * 100.0);
//        printf("Prep time  = %e, part = %f\n", prep_DU_time,  prep_DU_time / DU_time * 100.0);
//        printf("Upd time   = %e, part = %f\n", upd_time,      upd_time / DU_time * 100.0);
//        printf("Int time   = %e, part = %f\n", inter_time,    inter_time / DU_time * 100.0);
//        printf("Ext other  = %e, part = %f\n", ext_time - prep_DU_time - upd_time - inter_time,    (ext_time - prep_DU_time - upd_time - inter_time) / DU_time * 100.0);
//        printf("DU  other  = %e, part = %f\n", DU_time - ext_time - pin_time,    (DU_time - ext_time - pin_time) / DU_time * 100.0);
//    }

}


void CORE::eigen_internal()
{
//    PRISM * prism;
//    NODE * node;
//    vector<Eigen::Triplet<double>> triplets;
//    triplets.reserve(NFA*DIV*NL*6);
//    int two = NFA*NL*DIV;
//
//    double aD1;
//    double aD2;
//
//    double a12;
//
//    double tmp1;
//    double tmp2;
//
//
//    double A11;
//    double A12;
//    double A22;
//
//    double h1;
//    double h2;
//
//    double  K1 ;
//    double  K1s;
//    double  K12;
//    double  K2 ;
//    double  K2s;
//
//    for(int n = 0; n < NFA; n++)
//    {
//        for(int h = 0; h < NL; h++)
//        {
//            prism = &(FA[n].PRI[h]);
//            for(int d = 0; d < DIV; d++)
//            {
//                node  = &(prism->ND[d]);
//
//                aD1  = 0;
//                aD2  = 0;
//
//                a12 = 0;
//
//                h1 = node->contact_r[0] / (node->range_r[0] * 2.0 ) * 2.0;
//                h2 = node->contact_a[0] / node->range_a[0];
//
//
//                for (unsigned int r = 0; r < node->ADJ_R_NODE.size(); ++r)
//                {
//                    tmp1 =  h1 * prism->D1 * node->ADJ_R_NODE[r]->myPrism->D1
//                            / (prism->D1 + node->ADJ_R_NODE[r]->myPrism->D1);
//
//                    tmp2 = h1 * prism->D2 * node->ADJ_R_NODE[r]->myPrism->D2
//                            / (prism->D2 + node->ADJ_R_NODE[r]->myPrism->D2);
//
//                    aD1  += tmp1;
//                    aD2  += tmp2;
//
//                    triplets.push_back(Eigen::Triplet<double>(
//                       node->my_number,
//                       node->ADJ_R_NODE[r]->my_number,
//                       -tmp1));
//
//                    triplets.push_back(Eigen::Triplet<double>(
//                       node->my_number+two,
//                       node->ADJ_R_NODE[r]->my_number + two,
//                       -tmp2));
//
//
//                }
//                if(node->ADJ_R_NODE.size() < 3)
//                {
//                    K1 = prism->D1 / (node->range_r[0] * 2.0) * (1 + node->alb_side_11)
//                            / (prism->D1 / (node->range_r[0] * 2.0) *(1 + node->alb_side_11) + (1 - node->alb_side_11) / 4.0 );
//
//                    K2 = prism->D2 / (node->range_r[0] * 2.0) * (1 + node->alb_side_22)
//                            / (prism->D2 / (node->range_r[0] * 2.0) *(1 + node->alb_side_22) + (1 - node->alb_side_22) / 4.0 );
//
//                    K12 = (node->alb_side_12 * K1 / 4.0 - prism->D1 / (node->range_r[0] * 2.0) * node->alb_side_12 * (K1-1.0))
//                            / (prism->D2 / (node->range_r[0] * 2.0) *(1 + node->alb_side_22) + (1 - node->alb_side_22) / 4.0 );
//
//                    aD1  += h1 * prism->D1 * (1 - K1);
//                    aD2  += h1 * prism->D2 * (1 - K2);
//
//                    a12 += K12 * h1 * prism->D2;
//
//                }
//
//                for (unsigned int a = 0; a < node->ADJ_A_NODE.size(); ++a)
//                {
//                    tmp1 = h2 * prism->D1;
//                    tmp2 = h2 * prism->D2;
//
//                    K1 = (prism->D1 / node->range_a[a]) / (prism->D1 / node->range_a[a] + node->ADJ_A_NODE[a]->myPrism->D1 / node->ADJ_A_NODE[a]->range_a[0]);
//                    K1s = (node->ADJ_A_NODE[a]->myPrism->D1 / node->ADJ_A_NODE[a]->range_a[0]) / (prism->D1 / node->range_a[a] + node->ADJ_A_NODE[a]->myPrism->D1 / node->ADJ_A_NODE[a]->range_a[0]);
//                    K2 = (prism->D2 / node->range_a[a]) / (prism->D2 / node->range_a[a] + node->ADJ_A_NODE[a]->myPrism->D2 / node->ADJ_A_NODE[a]->range_a[0]);
//                    K2s = (node->ADJ_A_NODE[a]->myPrism->D2 / node->ADJ_A_NODE[a]->range_a[0]) / (prism->D2 / node->range_a[a] + node->ADJ_A_NODE[a]->myPrism->D2 / node->ADJ_A_NODE[a]->range_a[0]);
//
//
//                    aD1  += tmp1 * (1.0 - K1);
//                    aD2  += tmp2 * (1.0 - K2);
//
//                    triplets.push_back(Eigen::Triplet<double>(
//                       node->my_number,
//                       node->ADJ_A_NODE[a]->my_number,
//                       -tmp1 * K1s));
//
//                    triplets.push_back(Eigen::Triplet<double>(
//                       node->my_number+two,
//                       node->ADJ_A_NODE[a]->my_number+two,
//                       -tmp2 * K2s));
//                }
//
//                if (node->ADJ_A_NODE.size() < 2)
//                {
//
//
//
//                    if (node->myPrism->h == node->myCore->NL-1)
//                    {
//                        A11 = node->alb_top_11;
//                        A12 = node->alb_top_12;
//                        A22 = node->alb_top_22;
//
//                    }
//                    else if (node->myPrism->h == 0)
//                    {
//                        A11 = node->alb_bottom_11;
//                        A12 = node->alb_bottom_12;
//                        A22 = node->alb_bottom_22;
//                    }
//
//                    K1 = prism->D1 / (node->range_a[0] * 2.0) * (1 + A11)
//                            / (prism->D1 / (node->range_a[0] * 2.0) *(1 + A11) + (1 - A11) / 4.0 );
//
//                    K2 = prism->D2 / (node->range_a[0] * 2.0) * (1 + A22)
//                            / (prism->D2 / (node->range_a[0] * 2.0) *(1 + A22) + (1 - A22) / 4.0 );
//
//                    K12 = (A12 * K1 / 4.0 - prism->D1 / (node->range_a[0] * 2.0) * A12 * (K1-1.0))
//                            / (prism->D2 / (node->range_a[0] * 2.0) *(1 + A22) + (1 - A22) / 4.0 );
//
//                    aD1  += h2 * prism->D1 * (1 - K1);
//                    aD2  += h2 * prism->D2 * (1 - K2);
//
//                    a12 += K12 * h2 * prism->D2;
//
//
//
//                }
//
//                a12 += prism->S12 * node->V;
//
//                aD1 += (prism->S12 + prism->Sa1) * node->V;
//
//                aD2 += prism->Sa2 * node->V;
//
//                triplets.push_back(Eigen::Triplet<double>(
//                   node->my_number + two,
//                   node->my_number,
//                   -a12));
//
//                triplets.push_back(Eigen::Triplet<double>(
//                   node->my_number,
//                   node->my_number,
//                   aD1));
//
//                triplets.push_back(Eigen::Triplet<double>(
//                   node->my_number + two,
//                   node->my_number + two,
//                   aD2));
//
//
//
//                flux[node->my_number] = node->F1;
//                flux[node->my_number + two] = node->F2;
//
//                neutron_power[node->my_number] = node->NP;
//                neutron_power[node->my_number + two] = 0;
//
//
//
//            }
//        }
//    }
//
//    sparse_matrix.setFromTriplets(triplets.begin(), triplets.end());
//
//    {
//      //Eigen::MINRES<Eigen::SparseMatrix<double>, Eigen::Lower|Eigen::Upper, Eigen::IdentityPreconditioner> bicg;
//      //Eigen::DGMRES<Eigen::SparseMatrix<double>, Eigen::IdentityPreconditioner> bicg;
//      //Eigen::GMRES<Eigen::SparseMatrix<double>, Eigen::IdentityPreconditioner> bicg;
//      //Eigen::LeastSquaresConjugateGradient<Eigen::SparseMatrix<double> >bicg;
//      //Eigen::ConjugateGradient<Eigen::SparseMatrix<double>, Eigen::Lower|Eigen::Upper, Eigen::IdentityPreconditioner>bicg;
//      Eigen::BiCGSTAB<Eigen::SparseMatrix<double>, Eigen::IdentityPreconditioner> bicg;
//      //Eigen::UmfPackLU<Eigen::SparseMatrix<double>> bicg;
//      bicg.compute(sparse_matrix);
//      bicg.setTolerance(1e-6);
//      flux = bicg.solveWithGuess(neutron_power,flux);
//
//    }
//
//    for(int n = 0; n < NFA; n++)
//    {
//        for(int h = 0; h < NL; h++)
//        {
//            for(int d = 0; d < DIV; d++)
//            {
//                node = &(FA[n].PRI[h].ND[d]);
//                node->F1 = flux[node->my_number];
//                node->F2 = flux[node->my_number + two];
//
//            }
//        }
//    }


}

void CORE::external_iteration()
{
    double t1 = omp_get_wtime();
    cout << "   ext iter, k-1 = " << K - 1.0<< "  ";



    write_info();
    normalization();
    write_info();

    t1 = omp_get_wtime();
    update();
    upd_time += omp_get_wtime() - t1;
    write_info();

    t1 = omp_get_wtime();
    for ( int n = 0; n < NFA; n++ ) {
        for ( int h = 0; h < NL; h++ ) {
            FA[n].PRI[h].prepareForDifCal();
        }
    }
    prep_DU_time += omp_get_wtime() - t1;


    t1 = omp_get_wtime();
    double dev = 1;
    int cnt = 0;
    while(dev > DIN)
    {
       cnt++;
       dev = internal_iteration();
    }
    inter_time += omp_get_wtime() - t1;
    cout << "amount INTERNAL IT = " << cnt << endl;





   //eigen_internal();

    write_info();
    double NP_pr = NP;
    core_power();
    write_info();





    K = NP / NP_pr;


    if (fRECOVERY == ON && TP/NOMINAL_POWER > 0.1)
        recovery();
    write_info();



    if(fCrit == Cb)
        boron_crit();
    if(fCrit == Power)
        power_crit();
    if(fCrit == Rods)
        rods_crit();
    write_info();
}


void CORE::field_functions()
{
    // *** Axial offset calculation ***
    double TP_top = 0, TP_bottom = 0;
    for(int n = 0; n < NFA; n++)
    {
        for(int h = 0; h < NL; h++)
        {
            if(FA[n].PRI[h].Z - FA[n].PRI[h].height/2 >= HCORE/2)
                TP_top += FA[n].PRI[h].TP;
            else if(FA[n].PRI[h].Z + FA[n].PRI[h].height/2 <= HCORE/2)
                TP_bottom += FA[n].PRI[h].TP;
            else
            {
                TP_top += FA[n].PRI[h].TP * ((FA[n].PRI[h].Z+FA[n].PRI[h].height/2 - HCORE/2)/FA[n].PRI[h].height);
                TP_bottom += FA[n].PRI[h].TP * ((HCORE/2 - (FA[n].PRI[h].Z-FA[n].PRI[h].height/2))/FA[n].PRI[h].height);
            }
        }
    }
    AO = (TP_top - TP_bottom) / TP * 100.0;

    // *** Axial offset per assembly calculation ***

    vector<double> TP_top_a(NFA,0);
    vector<double> TP_bottom_a(NFA,0);
    for(int n = 0; n < NFA; n++)
    {
        for(int h = 0; h < NL; h++)
        {
            if(FA[n].PRI[h].Z - FA[n].PRI[h].height/2 >= HCORE/2)
                TP_top_a[n] += FA[n].PRI[h].TP;
            else if(FA[n].PRI[h].Z + FA[n].PRI[h].height/2 <= HCORE/2)
                TP_bottom_a[n] += FA[n].PRI[h].TP;
            else
            {
                TP_top_a[n] += FA[n].PRI[h].TP * ((FA[n].PRI[h].Z+FA[n].PRI[h].height/2 - HCORE/2)/FA[n].PRI[h].height);
                TP_bottom_a[n] += FA[n].PRI[h].TP * ((HCORE/2 - (FA[n].PRI[h].Z-FA[n].PRI[h].height/2))/FA[n].PRI[h].height);
            }
        }
        assembly_AO[n] = (TP_top_a[n]-TP_bottom_a[n]) / (TP_top_a[n]+TP_bottom_a[n]);

    }

    // *** Axial offset per DPZ calculation ***

    vector<double> TP_top_kni(NKNI,0);
    vector<double> TP_bottom_kni(NKNI,0);
    for(int n = 0; n < NFA; n++)
    {
        for(int h = 0; h < FA[0].SL_7.size(); h++)
        {
            if(FA[n].kni != 0)
            {
                if(h < 3)
                    TP_bottom_kni[FA[n].kni->number-1] += FA[n].kni->Tok[h];
                else if (h > 3)
                    TP_top_kni[FA[n].kni->number-1] += FA[n].kni->Tok[h];
                else if (h == 3)
                {
                    TP_top_kni[FA[n].kni->number-1] += 0.5 * FA[n].kni->Tok[h];
                    TP_bottom_kni[FA[n].kni->number-1] += 0.5 * FA[n].kni->Tok[h];
                }
            }
        }
        if(FA[n].kni != 0)
            DPZ_AO[FA[n].kni->number-1] = (TP_top_kni[FA[n].kni->number-1] - TP_bottom_kni[FA[n].kni->number-1]) /
                                          (TP_top_kni[FA[n].kni->number-1] + TP_bottom_kni[FA[n].kni->number-1]);
    }

    // *** KQ calculation ***

    for(int n = 0; n < NFA; n++)
    {
        KQ[n] = FA[n].TP / (TP/NFA);
    }

    // *** KV_16 calculation ***
    for(int n = 0; n < NFA; n++)
    {
        for(int h = 0; h < NL; h++)
        {
            TPl[n][h] = FA[n].PRI[h].TP / FA[n].PRI[h].height;
            KV[n][h]  = TPl[n][h] / ((TP/HCORE)/NFA);
        }
    }

    // *** KV_max and KQ_max definition ***
    double max_kv = 0, max_kq = 0,
    max_kv_cas, max_kq_cas, max_kv_lay;
    vector<double> max_assembly_KV(NFA, 0);
    for(int n = 0; n < NFA; n++)
    {
        if(KQ[n] > max_kq)
        {
            max_kq = KQ[n];
            max_kq_cas = n+1;
        }
        for(int h = 0; h < NL; h++)
        {
            if(KV[n][h] > max_assembly_KV[n])
            {
                max_assembly_KV[n] = KV[n][h];
                if(max_assembly_KV[n] > max_kv)
                {
                    max_kv = max_assembly_KV[n];
                    max_kv_cas = n+1;
                    max_kv_lay = h+1;
                }
            }
        }
    }
    KQ_max = max_kq;
    KQ_max_cas = max_kq_cas;
    KV_max = max_kv;
    KV_max_assembly = max_assembly_KV;
    KV_max_cas = max_kv_cas;
    KV_max_lay = max_kv_lay;


    // *** KV_7 calculation ***

    for(int n = 0; n < NFA; n++)
    {

        for (unsigned int k = 0; k < 7; k++)
        {
            KV_7[n][k] = 0;
            for(int h = 0; h < NL; h++)
            {
                KV_7[n][k] += KV[n][h] * FA[n].PRI_parts[k][h];
            }
        }

    }

    // *** KV_7_max defininion ***
    double max_kv_7 = 0, max_kv_7_cas, max_kv_7_lay;
    for(int n = 0; n < NFA; n++)
    {
        for(int h = 0; h < FA[n].TPl_7.size(); h++)
        {
            if(KV_7[n][h] > max_kv_7)
            {
                max_kv_7 = KV_7[n][h];
                max_kv_7_cas = n+1;
                max_kv_7_lay = h+1;
            }
        }
    }
    KV_7_max = max_kv_7;
    KV_7_max_cas = max_kv_7_cas;
    KV_7_max_lay = max_kv_7_lay;



    // *** QL, KK and KO calculation ***
    for(int n = 0; n < NFA; n++)
    {
        for(int t = 0; t < NPIN; t++)
        {
            for(int h = 0; h < NL; h++)
            {
                QL[n][t][h] = FA[n].PRI[h].pin_power[t] / FA[n].PRI[h].height * 1e6; // W/cm
                KK[n][t][h] = QL[n][t][h] / (FA[n].PRI[h].TP *1e6 / NFPIN / FA[n].PRI[h].height);
                KO[n][t][h] = QL[n][t][h] / (TP*1e6/NFA/NFPIN/ HCORE);

                //cout << "QL = " << QL[n][t][h] <<" av = " << TP*1e6/NFA/NFPIN/ HCORE << endl;
            }
        }
    }


    // *** QL_max definition ***

    vector<double> max_QL_assembly(NFA, 0);
    double max_QL = 0, max_QL_cas, max_QL_lay, max_QL_tvel;
    for(int n = 0; n < NFA; n++)
    {
        for(int t = 0; t < NPIN; t++)
        {
            for(int h = 0; h < NL; h++)
            {
                if(QL[n][t][h] > max_QL_assembly[n])
                {
                    max_QL_assembly[n] = QL[n][t][h];
                    if(max_QL_assembly[n] > max_QL)
                    {
                        max_QL = max_QL_assembly[n];
                        max_QL_cas = n+1;
                        max_QL_lay = h+1;
                        max_QL_tvel = t+1;
                    }
                }
            }
        }
    }
    QL_max_assembly = max_QL_assembly;
    QL_max = max_QL;
    QL_max_cas = max_QL_cas;
    QL_max_lay = max_QL_lay;
    QL_max_tvel = max_QL_tvel;


    // *** KK_max and KO_max definition ***

    vector<double> max_KK(NFA, 0);
    vector<double> max_KO(NFA, 0);
    for(int n = 0; n < NFA; n++)
    {
        for(int t = 0; t < NPIN; t++)
        {
            for(int h = 0; h < NL; h++)
            {
                if(KK[n][t][h] > max_KK[n])
                    max_KK[n] = KK[n][t][h];

                if(KO[n][t][h] > max_KO[n])
                    max_KO[n] = KO[n][t][h];
            }
        }
    }
    KK_max = max_KK;
    KO_max = max_KO;

    // *** XE and SM average parameters calculation ***
    XE_aver = XE_top_aver = XE_bottom_aver = SM_aver = SM_top_aver = SM_bottom_aver = 0;
    for(int n = 0; n < NFA; n++)
    {
        for(int h = 0; h < NL; h++)
        {
            if(FA[n].PRI[h].Z - FA[n].PRI[h].height/2 >= HCORE/2)
            {
                XE_top_aver += FA[n].PRI[h].XE / NFA / NL;
                SM_top_aver += FA[n].PRI[h].SM / NFA / NL;
            }
            else if(FA[n].PRI[h].Z + FA[n].PRI[h].height/2 <= HCORE/2)
            {
                XE_bottom_aver += FA[n].PRI[h].XE / NFA / NL;
                SM_bottom_aver += FA[n].PRI[h].SM / NFA / NL;
            }
            else
            {
                XE_top_aver += FA[n].PRI[h].XE * ((FA[n].PRI[h].Z + FA[n].PRI[h].height/2 - HCORE/2)/FA[n].PRI[h].height)/ NFA / NL;
                XE_bottom_aver += FA[n].PRI[h].XE * ((HCORE/2 - (FA[n].PRI[h].Z-FA[n].PRI[h].height/2))/FA[n].PRI[h].height)/ NFA / NL;
                SM_top_aver += FA[n].PRI[h].SM * ((FA[n].PRI[h].Z + FA[n].PRI[h].height/2 - HCORE/2)/FA[n].PRI[h].height)/ NFA / NL;
                SM_bottom_aver += FA[n].PRI[h].SM * ((HCORE/2 - (FA[n].PRI[h].Z-FA[n].PRI[h].height/2))/FA[n].PRI[h].height)/ NFA / NL;
            }
            XE_aver += FA[n].PRI[h].XE / NFA / NL;
            SM_aver += FA[n].PRI[h].SM / NFA / NL;
        }
    }

    // *** Krik and max Kr, Kri calculation ***
    vector<vector<double>> TPl_tvel(NFA, vector<double>(NPIN, 0));
    for(int n = 0; n < NFA; n++)
    {
        for(int k = 0; k < NPIN; k++)
        {
            for(int h = 0; h < NL; h++)
            {
                TPl_tvel[n][k] += FA[n].PRI[h].pin_power[k] / FA[n].PRI[h].height;
            }
            Krik[n][k] = TPl_tvel[n][k] / (TP/NFA/NPIN);
        }
    }
    double max_Kr = 0;
    vector<double> max_Kri(NFA, 0);
    for(int n = 0; n < NFA; n++)
    {
        for(int k = 0; k < NPIN; k++)
        {
            if(Krik[n][k] > max_Kri[n])
            {
                max_Kri[n] = Krik[n][k];
                if(max_Kri[n] > max_Kr)
                    max_Kr = max_Kri[n];
            }
        }
    }
    Kr_max = max_Kr;
    Kri_max = max_Kri;
/*
    // *** SL_tvel calculation ***
    double average;
    for(int n = 0; n < NFA; n++)
    {
        for(int k = 0; k < NPIN; k++)
        {
            for(int h = 0; h < NL; h++)
            {
                average += FA[n].PRI[h].SL_pin[k] / NL;
            }
            SL_tvel[n][k] = average;
            average = 0;
        }
    }
*/
    // *** 12 ASSEMBLY MAX parameters sorting ***
    vector<int> x_KQ(NFA, 0);       vector<double> y_KQ(NFA, 0);
    vector<int> x_KV(NFA, 0);       vector<double> y_KV(NFA, 0);
    vector<int> x_KO(NFA, 0);       vector<double> y_KO(NFA, 0);
    vector<int> x_WT_out(NFA, 0);   vector<double> y_WT_out(NFA, 0);
    vector<int> x_TP(NFA, 0);       vector<double> y_TP(NFA, 0);
    vector<int> x_dT(NFA, 0);       vector<double> y_dT(NFA, 0);
    vector<int> x_SL(NFA, 0);       vector<double> y_SL(NFA, 0);
    vector<int> x_QL(NFA, 0);       vector<double> y_QL(NFA, 0);

    // 1. KQ sorting-----------------------------
    double tx, ty;
    int j = 0;
    for(int i = 0; i < NFA; i++)
    {
        y_KQ[i] = KQ[i];
        x_KQ[i] = i+1;
    }
    for(int i=1; i< NFA; i++)
    {
        tx = x_KQ[i];
        ty = y_KQ[i];
        j = i -1;
        while(j >= 0 && y_KQ[j] > ty)
        {
            x_KQ[j+1] = x_KQ[j];
            y_KQ[j+1] = y_KQ[j];
            j--;
        }
        x_KQ[j+1] = tx;
        y_KQ[j+1] = ty;
    }
    reverse(x_KQ.begin(), x_KQ.end());
    reverse(y_KQ.begin(), y_KQ.end());
    KQ_12 = x_KQ;
    // 2. KV sorting-----------------------------
    for(int i = 0; i < NFA; i++)
    {
        y_KV[i] = KV_max_assembly[i];
        x_KV[i] = i+1;
    }
    for(int i=1; i< NFA; i++)
    {
        tx = x_KV[i];
        ty = y_KV[i];
        j = i -1;
        while(j >= 0 && y_KV[j] > ty)
        {
            x_KV[j+1] = x_KV[j];
            y_KV[j+1] = y_KV[j];
            j--;
        }
        x_KV[j+1] = tx;
        y_KV[j+1] = ty;
    }
    reverse(x_KV.begin(), x_KV.end());
    reverse(y_KV.begin(), y_KV.end());
    KV_12 = x_KV;
;
    // 3. KO sorting-----------------------------
    for(int i = 0; i < NFA; i++)
    {
        y_KO[i] = KO_max[i];
        x_KO[i] = i+1;
    }
    for(int i=1; i< NFA; i++)
    {
        tx = x_KO[i];
        ty = y_KO[i];
        j = i -1;
        while(j >= 0 && y_KO[j] > ty)
        {
            x_KO[j+1] = x_KO[j];
            y_KO[j+1] = y_KO[j];
            j--;
        }
        x_KO[j+1] = tx;
        y_KO[j+1] = ty;
    }
    reverse(x_KO.begin(), x_KO.end());
    reverse(y_KO.begin(), y_KO.end());
    KO_12 = x_KO;
    // 4. T_out sorting-----------------------------
    for(int i = 0; i < NFA; i++)
    {
        y_WT_out[i] = FA[i].WT_out;
        x_WT_out[i] = i+1;
    }
    for(int i=1; i< NFA; i++)
    {
        tx = x_WT_out[i];
        ty = y_WT_out[i];
        j = i -1;
        while(j >= 0 && y_WT_out[j] > ty)
        {
            x_WT_out[j+1] = x_WT_out[j];
            y_WT_out[j+1] = y_WT_out[j];
            j--;
        }
        x_WT_out[j+1] = tx;
        y_WT_out[j+1] = ty;
    }
    reverse(x_WT_out.begin(), x_WT_out.end());
    reverse(y_WT_out.begin(), y_WT_out.end());
    WT_out_12 = x_WT_out;
    // 5. TP sorting-----------------------------
    for(int i = 0; i < NFA; i++)
    {
        y_TP[i] = FA[i].TP;
        x_TP[i] = i+1;
    }
    for(int i=1; i< NFA; i++)
    {
        tx = x_TP[i];
        ty = y_TP[i];
        j = i -1;
        while(j >= 0 && y_TP[j] > ty)
        {
            x_TP[j+1] = x_TP[j];
            y_TP[j+1] = y_TP[j];
            j--;
        }
        x_TP[j+1] = tx;
        y_TP[j+1] = ty;
    }
    reverse(x_TP.begin(), x_TP.end());
    reverse(y_TP.begin(), y_TP.end());
    TP_12 = x_TP;
    // 6. dT sorting-----------------------------
    for(int i = 0; i < NFA; i++)
    {
        y_dT[i] = FA[i].dT;
        x_dT[i] = i+1;
    }
    for(int i=1; i< NFA; i++)
    {
        tx = x_dT[i];
        ty = y_dT[i];
        j = i -1;
        while(j >= 0 && y_dT[j] > ty)
        {
            x_dT[j+1] = x_dT[j];
            y_dT[j+1] = y_dT[j];
            j--;
        }
        x_dT[j+1] = tx;
        y_dT[j+1] = ty;
    }
    reverse(x_dT.begin(), x_dT.end());
    reverse(y_dT.begin(), y_dT.end());
    dT_12 = x_dT;
    // 7. SL sorting-----------------------------
    for(int i = 0; i < NFA; i++)
    {
        y_SL[i] = FA[i].SL;
        x_SL[i] = i+1;
    }
    for(int i=1; i< NFA; i++)
    {
        tx = x_SL[i];
        ty = y_SL[i];
        j = i -1;
        while(j >= 0 && y_SL[j] > ty)
        {
            x_SL[j+1] = x_SL[j];
            y_SL[j+1] = y_SL[j];
            j--;
        }
        x_SL[j+1] = tx;
        y_SL[j+1] = ty;
    }
    reverse(x_SL.begin(), x_SL.end());
    reverse(y_SL.begin(), y_SL.end());
    SL_12 = x_SL;
    // 8. QL sorting-----------------------------
    for(int i = 0; i < NFA; i++)
    {
        y_QL[i] = QL_max_assembly[i];
        x_QL[i] = i+1;
    }
    for(int i=1; i< NFA; i++)
    {
        tx = x_QL[i];
        ty = y_QL[i];
        j = i -1;
        while(j >= 0 && y_QL[j] > ty)
        {
            x_QL[j+1] = x_QL[j];
            y_QL[j+1] = y_QL[j];
            j--;
        }
        x_QL[j+1] = tx;
        y_QL[j+1] = ty;
    }
    reverse(x_QL.begin(), x_QL.end());
    reverse(y_QL.begin(), y_QL.end());
    QL_12 = x_QL;


}

void CORE::find_adj_assembly()
{
    double x, y, range;
    vector<vector<int>>  neigh(NFA);
    for (unsigned int n = 0; n < NFA; n++)
    {

        for (unsigned int nn=0; nn < NFA; nn++)
        {
            x = abs(FA[n].X - FA[nn].X) * FASTEP / 2.0;
            y = abs(FA[n].Y - FA[nn].Y) * FASTEP * SQRT3 / 2.0;
            range = pow(x*x+y*y, 0.5);

            if (range < FASTEP*1.01 && n!=nn)
            {
                neigh[n].push_back(nn);
            }
        }


        FA[n].ADJ.resize(neigh[n].size());
        FA[n].num_adj = neigh[n].size();
        for(int nn = 0; nn < FA[n].num_adj; nn++)
        {
            FA[n].ADJ[nn] = & FA[neigh[n][nn]];
            for (unsigned int h =0; h < NL; ++h)
                FA[n].PRI[h].ADJ_PRISM.push_back(&(FA[n].ADJ[nn]->PRI[h]));
        }
    }

}

void CORE::formation_core()
{

    vector <int> orbFA =  {19,18,17,17,18,19,19,16,15,14,13,14,15,16,19,18,15,12,11,
            10,10,11,12,15,18,17,14,11,9,8,7,8,9,11,14,17,17,13,10,8,6,
            5,5,6,8,10,13,17,18,14,10,7,5,4,3,4,5,7,10,14,18,19,15,11,
            8,5,3,2,2,3,5,8,11,15,19,16,12,9,6,4,2,1,2,4,6,9,12,16,19,
            15,11,8,5,3,2,2,3,5,8,11,15,19,18,14,10,7,5,4,3,4,5,7,10,14,
            18,17,13,10,8,6,5,5,6,8,10,13,17,17,14,11,9,8,7,8,9,11,14,17,
            18,15,12,11,10,10,11,12,15,18,19,16,15,14,13,14,15,16,19,19,18,
            17,17,18,19};


    FA.resize(NFA);

    for (unsigned int n =0; n < NFA; n++)
    {
        FA[n].init(this);
        FA[n].orb = orbFA[n]-1;
        FA[n].NL = NL;
    }


    read_fuel_base();
    read_map();

    int  n=0, x_start;

    int rows[] = {6, 9, 10, 11, 12, 13, 14, 13, 14, 13, 12, 11, 10, 9, 6};
    for (unsigned int y = 15; y >0; y--)
    {
        if (y == 15 || y == 1)
            x_start = 24;
        else if (y == 8)
            x_start = 17;
        else if (y < 8)
            x_start = 23 - y;
        else
            x_start = 7 + y;
        for (unsigned int j =0 ; j < rows[y-1]; j++)
        {
            FA[n].n = n + 1;
            FA[n].X = x_start+j*2;
            FA[n].Y = y;
            FA[n].S = SQRT3 * FASTEP * FASTEP / 2.0;

            n++;
        }
    }

    find_adj_assembly();
    V = 0;
    for (unsigned int n =0; n < NFA; n++)
    {
        FA[n].division();
        FA[n].V = FA[n].S * FA[n].height;
        V += FA[n].V;

    }


    for (unsigned int n =0; n < NFA; n++)
    {
        FA[n].albedo_put();
        for (unsigned int h = 0; h < NL; h++)
            FA[n].PRI[h].find_adj_node();
    }

//    sparse_matrix.resize(NFA*NL*DIV*2,NFA*NL*DIV*2);
//    flux.resize(NFA*NL*DIV*2);
//    neutron_power.resize(NFA*NL*DIV*2);

    field_lay.setlength(NFA*DIV,3);
    for (unsigned int n =0; n < NFA; n++)
    {
        for (unsigned int d = 0; d < DIV; d++)
        {
            field_lay(n*DIV+d, 0) = FA[n].PRI[0].ND[d].X;
            field_lay(n*DIV+d, 1) = FA[n].PRI[0].ND[d].Y;
        }
    }
}


double CORE::internal_iteration()
{
    double delta = 0;
    #pragma omp parallel for num_threads(N_pot)
    for (unsigned int n = 0; n < NFA; ++n)
        delta += FA[n].internal_iteration();

    //cout << "after inter " << delta / NFA << endl;
    return delta / NFA;
}


void CORE::make_copy(CORE & ACORE)
{
    //интегральное копирование для наполнения структур
    *this = ACORE;

    for(unsigned int n = 0; n < NFA; n++)
    {
        FA[n].myCore = this;

        //Связка КНИ с ТВС
        if (ACORE.FA[n].kni)
        {
            FA[n].kni = &kni[ACORE.FA[n].kni->number-1];
            kni[ACORE.FA[n].kni->number-1].myAssembly = &FA[n];
        }
        else
            FA[n].kni = 0;


        //Связка соседей ТВС
        for(unsigned int r = 0; r < FA[n].ADJ.size(); r++)
            FA[n].ADJ[r] = &FA[ACORE.FA[n].ADJ[r]->n-1];

        //Работа с призмами
        for(unsigned int h = 0; h < NL; h++)
        {
            FA[n].PRI[h].myCore = this;
            FA[n].PRI[h].myAssembly = &FA[n];

            //Связка соседей призм
            for(unsigned hh = 0; hh < FA[n].PRI[h].ADJ_PRISM.size(); hh++)
                FA[n].PRI[h].ADJ_PRISM[hh] = &FA[ACORE.FA[n].PRI[h].ADJ_PRISM[hh]->n-1].PRI[h];

            //Работа с нодами
            for (unsigned int d = 0; d < DIV; d++)
            {
                FA[n].PRI[h].ND[d].myCore = this;
                FA[n].PRI[h].ND[d].myPrism = &FA[n].PRI[h];

                //Связка соседей ноды
                for(unsigned int r = 0; r < FA[n].PRI[h].ND[d].ADJ_R_NODE.size(); r++)
                {
                    int nn = ACORE.FA[n].PRI[h].ND[d].ADJ_R_NODE[r]->myPrism->n-1;
                    int nh = ACORE.FA[n].PRI[h].ND[d].ADJ_R_NODE[r]->myPrism->h;
                    int nd = ACORE.FA[n].PRI[h].ND[d].ADJ_R_NODE[r]->d-1;

                    FA[n].PRI[h].ND[d].ADJ_R_NODE[r] = &FA[nn].PRI[nh].ND[nd];
                }
                for(unsigned int a = 0; a < FA[n].PRI[h].ND[d].ADJ_A_NODE.size(); a++)
                {
                    int nn = ACORE.FA[n].PRI[h].ND[d].ADJ_A_NODE[a]->myPrism->n-1;
                    int nh = ACORE.FA[n].PRI[h].ND[d].ADJ_A_NODE[a]->myPrism->h;
                    int nd = ACORE.FA[n].PRI[h].ND[d].ADJ_A_NODE[a]->d-1;

                    FA[n].PRI[h].ND[d].ADJ_A_NODE[a] = &FA[nn].PRI[nh].ND[nd];
                }

            }

        }
    }
}

void CORE::calc_NED()
{
    int num = 0;
    double dpz_power = 0;
    double calc_power = 0;

    for (unsigned int n = 0; n < NFA; n++)
    {

        for (unsigned int h = 0 ; h < NLDPZ; ++h)
            if (FA[n].kni && FA[n].kni->reliability[h])
            {
                num++;
                dpz_power += FA[n].kni->TPl[h];
                calc_power += FA[n].TPl_7[h];

            }

    }

    NED = dpz_power / calc_power * TP;


    //Расчет значений KV по ДПЗ
    for (int k = 0; k < NKNI; k++)
    {
        for (int h = 0; h < NLDPZ; h++)
            if(kni[k].reliability[h])
                kni[k].KV_dpz[h] = kni[k].TPl[h] / NED * NFA * HCORE / 100.0;
            else
                kni[k].KV_dpz[h] = 0;
    }

}

void CORE::normalization()
{
    double tTP = 0;
    double tNP = 0;
    for (unsigned int n =0 ; n < NFA; ++n)
    {
        FA[n].normalization();
        tTP += FA[n].TP;
        tNP += FA[n].NP;
    }

    TP = tTP;
    NP = tNP;

}


void CORE::poisoning_xe_sm()
{
    ro_XE = 0;
    ro_SM = 0;
    ro_XE_eq = 0;
    ro_SM_eq = 0;

    for(unsigned int n = 0; n < NFA; n++)
    {
        FA[n].poisoning_xe_sm();
        ro_XE += FA[n].ro_XE * FA[n].V / V;
        ro_SM += FA[n].ro_SM * FA[n].V / V;
        ro_XE_eq += FA[n].ro_XE_eq * FA[n].V / V;
        ro_SM_eq += FA[n].ro_SM_eq * FA[n].V / V;
    }
}



void CORE::read_PATH()
{
    ifstream inFile;
    string stri, stmp, path;
    //string HomeDir(::pProg->pSettings->ImCoreDataPath);
    path = HOMEDIR + "idb/imcore/PATH.imcore";
    inFile.open(path.c_str());
    int cnt = 0;


    PATH.resize(29);


    if(!inFile)
    {
        write_to_log(path);
    }


    while (getline(inFile, stri))
    {
        path.clear();
        istringstream iss(stri);
        iss >> stmp;
        iss >> path;
        PATH[cnt++] = HOMEDIR + (path).c_str();
        //cout << cnt -1 << " " << PATH[cnt-1]<< endl;

    }
    inFile.close();
}

void CORE::read_inp_file()
{
    ifstream inFile;
    char tmp[40];
    inFile.open(PATH[0]);

    if(!inFile)
    {
       write_to_log("cannot open " + PATH[0]);
    }

    inFile >> tmp >> NOMINAL_POWER;
    inFile >> tmp >> NL;
    inFile >> tmp >> DIV;
    inFile >> tmp >> HCORE;
    inFile >> tmp >> FASTEP;
    inFile >> tmp >> NFA;
    inFile >> tmp >> NPIN;
    inFile >> tmp >> NFPIN;
    inFile >> tmp >> NKNI;
    inFile >> tmp >> DEX;
    inFile >> tmp >> DIN;
    inFile >> tmp >> LXE;
    inFile >> tmp >> LI;
    inFile >> tmp >> LPM;
    inFile >> tmp >> EVTOJ;
    inFile >> tmp >> BARN;
    inFile >> tmp >> LRh104;
    inFile >> tmp >> LRh104m;
    inFile >> tmp >> sRh104;
    inFile >> tmp >> sRh104m;
    inFile >> tmp >> ALFA;
    inFile >> tmp >> h1;
    inFile >> tmp >> g1;
    inFile >> tmp >> AFT[0];
    inFile >> tmp >> AFT[1];
    inFile >> tmp >> AFT[2];
    inFile >> tmp >> AFT[3];
    inFile >> tmp >> AFT[4];
    inFile >> tmp >> CtoK;
    inFile >> tmp >> ns;
    inFile >> tmp >> nd;
    inFile >> tmp >> nc;
    inFile >> tmp >> ntc;
    inFile >> tmp >> NLDPZ;
    inFile >> tmp >> NTC;
    inFile >> tmp >> NSUZ;
    inFile >> tmp >> NGSUZ;
    inFile >> tmp >> NPT;
    inFile >> tmp >> NKV;
    inFile >> tmp >> NJU;
    inFile >> tmp >> dTin_power;
    inFile >> tmp >> pin_step;
    inFile >> tmp >> P_out;
    inFile >> tmp >> P_drop;
    inFile >> tmp >> N_pot;
    inFile >> tmp >> DBG_FLG;

    inFile.close();
}

void CORE::read_albedo()
{
    ifstream inFile;
    string stri, tmp;
    double num;
    inFile.open(PATH[10]);

    if(!inFile)
    {
        write_to_log("cannot open " + PATH[10]);
    }


    getline(inFile, stri);
    double m_t= 1.0;
    double m_r= 1.00;
    stringstream iss;
    iss << stri;
    iss >> tmp;
    while(iss >> num)
        alb_top_11.push_back(num * m_t);
    iss.clear();

    getline(inFile, stri);
    iss<<stri;
    iss >> tmp;
    while(iss >> num)
        alb_top_12.push_back(num * m_t);
    iss.clear();

    getline(inFile, stri);
    iss<<stri;
    iss >> tmp;
    while(iss >> num)
        alb_top_22.push_back(num * m_t);
    iss.clear();

    getline(inFile, stri);
    iss<<stri;
    iss >> tmp;
    while(iss >> num)
        alb_bottom_11.push_back(num);
    iss.clear();

    getline(inFile, stri);
    iss<<stri;
    iss >> tmp;
    while(iss >> num)
        alb_bottom_12.push_back(num);
    iss.clear();

    getline(inFile, stri);
    iss<<stri;
    iss >> tmp;
    while(iss >> num)
        alb_bottom_22.push_back(num);
    iss.clear();

    getline(inFile, stri);
    iss<<stri;
    iss >> tmp;
    while(iss >> num)
        alb_side_11.push_back(num * m_r);
    iss.clear();

    getline(inFile, stri);
    iss<<stri;
    iss >> tmp;
    while(iss >> num)
        alb_side_12.push_back(num * m_r);
    iss.clear();

    getline(inFile, stri);
    iss<<stri;
    iss >> tmp;
    while(iss >> num)
        alb_side_22.push_back(num * m_r);
    iss.clear();

}

void CORE::read_fuel_base()
{
    vector<string> line;
    string stri, parameter;
    int cnt_1 = 0;
    int cnt_2 = 0;


    ifstream inFile;
    inFile.open(PATH[22]);

    if(!inFile)
    {
        write_to_log("cannot open " + PATH[22]);
    }

    while (getline(inFile, stri))
    {
        xs_data->fuel_base.push_back(line);
        cnt_2 = 0;

        istringstream iss(stri);
        while(iss >> parameter)
        {
            xs_data->fuel_base[cnt_1].push_back(parameter);
        }

        //if(cnt_2 < 3)
        //{
        //    write_to_log("fuel_base.imcore input ERROR");
        //}
        cnt_1++;
    }
}

void CORE::read_map()
{
    string stri, type;
    double num;
    int cnt=0;
    int assembly = 0, layer = 0;
    vector<vector<string>> map;
    vector<string> line;

    ifstream inFile;
    inFile.open(PATH[2]);

    if(!inFile)
    {
        write_to_log("cannot open " + PATH[2]);
    }
    getline(inFile, stri);
    istringstream iss(stri);
    while(iss >> num)
    {
        for (unsigned int n=0; n<NFA;n++)
        {
            FA[n].PRI[cnt].height = num;
            FA[n].PRI[cnt].part = num / HCORE;
        }
        cnt++;
    }

    if (cnt!=NL)
    {
        write_to_log("Number of layers in map and init file is different" );
    }

    getline(inFile, stri);
    cnt = 0;

    while(getline(inFile, stri))
    {
        istringstream iss2(stri);
        while(iss2 >> type)
            line.push_back(type);

        map.push_back(line);
        line.clear();
        getline(inFile, stri);
    }

    for(int row = map.size()-1; row >= 0; row--)
    {
        for(int cas = 0; cas < map[row].size(); cas++)
        {
            FA[assembly++].type = map[row][cas];
        }
    }


    for (unsigned int n =0; n < NFA; n++)
        for (unsigned int h =0; h < NL; h++)
        {
            if (h == 0)
                FA[n].PRI[h].Z = FA[n].PRI[h].height / 2.0;
            else
                FA[n].PRI[h].Z = FA[n].PRI[h].height / 2.0 +  FA[n].PRI[h-1].height / 2.0 + FA[n].PRI[h-1].Z ;
        }

}

void CORE::read_put_sl()
{
    ifstream inFile;
    string stri;
    double num;
    vector<double> v_1d;
    vector <vector<double>> v_2d;
    inFile.open(PATH[5]);

    if(!inFile)
    {
        write_to_log("cannot open " +PATH[5]);
    }

    getline(inFile, stri);
    istringstream iss(stri);
    iss >> stri;
    time.read(stri);
    iss >> stri;
    TEFF = atof(stri.c_str());

    while(getline(inFile, stri))
    {
        istringstream iss(stri);
        while(iss>>num)
            v_1d.push_back(num);
        if (v_1d.size() != NL)
        {
            write_to_log("Error in sl file. Wrong number of lay");
        }
        v_2d.push_back(v_1d);
        v_1d.clear();
    }
    if (v_2d.size() != NFA)
    {
        write_to_log("Error in sl file. Wrong number of assemblay");
    }

    for (unsigned int n =0; n < NFA; ++n)
        for (unsigned int h = 0; h < NL; ++h)
        {
            FA[n].PRI[h].SL = v_2d[n][h];
            for (unsigned int pin = 0; pin < NPIN; pin++)
            {
                if(pin+1 == 58 || pin+1 == 71 || pin+1 == 76 ||
                   pin+1 == 103|| pin+1 == 108|| pin+1 == 112||
                   pin+1 == 124|| pin+1 == 148|| pin+1 == 161||
                   pin+1 == 166|| pin+1 == 171|| pin+1 == 184||
                   pin+1 == 208|| pin+1 == 220|| pin+1 == 224||
                   pin+1 == 229|| pin+1 == 256|| pin+1 == 261||
                   pin+1 == 274)

                   FA[n].PRI[h].PinSl[pin] = 0;
                else
                    FA[n].PRI[h].PinSl[pin] = FA[n].PRI[h].SL;
            }
        }

}

void CORE::read_put_sl_pin()
{
    ifstream inFile;
    string stri;
    double num;
    vector<double> v_1d;
    vector <vector<double>> v_2d;
    inFile.open(PATH[24]);

    if(!inFile)
    {
        write_to_log("cannot open " + PATH[24]);

    }
    else
    {
        while(getline(inFile, stri))
        {
            istringstream iss(stri);
            while(iss>>num)
                v_1d.push_back(num);
            if (v_1d.size() != NPIN)
            {
                write_to_log("Error in sl pin file. Wrong number of pin");
            }
            v_2d.push_back(v_1d);
            v_1d.clear();
        }
        if (v_2d.size() != NFA*NL)
        {
            write_to_log("Error in sl pin file. Wrong number of assemblay or lay");
        }

        for (unsigned int n = 0; n < NFA; ++n)
            for (unsigned int h = 0; h < NL; ++h)
            {
                for (unsigned int p = 0; p < NPIN; p++)
                    FA[n].PRI[h].PinSl[p] = v_2d[h + n*NL][p];

            }
    }

}



void CORE::read_put_sm()
{
    ifstream inFile;
    string stri;
    double num;
    vector<double> v_1d;
    vector <vector<double>> v_2d;
    inFile.open(PATH[3]);

    if(!inFile)
    {
        write_to_log("cannot open " + PATH[3]);
    }


    while(getline(inFile, stri))
    {
        if (!stri.empty())
        {

            istringstream iss(stri);
            while(iss>>num)
                v_1d.push_back(num);
            if (v_1d.size() != NL)
            {
                write_to_log("Error in sm file. Wrong number of lay");
            }
            v_2d.push_back(v_1d);
            v_1d.clear();

        }

    }
    if (v_2d.size() != NFA)
    {
        write_to_log("Error in sm file. Wrong number of assemblay");
    }

    for (unsigned int n =0; n < NFA; ++n)
        for (unsigned int h = 0; h < NL; ++h)
            FA[n].PRI[h].SM = v_2d[n][h];

}
void CORE::read_put_xe()
{
    ifstream inFile;
    string stri;
    double num;
    vector<double> v_1d;
    vector <vector<double>> v_2d;
    inFile.open(PATH[4]);

    if(!inFile)
    {
        write_to_log("cannot open " + PATH[4]);
    }


    while(getline(inFile, stri))
    {
        if (!stri.empty())
        {

            istringstream iss(stri);
            while(iss>>num)
                v_1d.push_back(num);
            if (v_1d.size() != NL)
            {
                write_to_log("Error in xe file. Wrong number of lay");
            }
            v_2d.push_back(v_1d);
            v_1d.clear();

        }
    }
    if (v_2d.size() != NFA)
    {
        write_to_log("Error in xe file. Wrong number of assemblay");
    }

    for (unsigned int n =0; n < NFA; ++n)
        for (unsigned int h = 0; h < NL; ++h)
            FA[n].PRI[h].XE = v_2d[n][h];

}

void CORE::read_put_pm()
{
    ifstream inFile;
    string stri;
    double num;
    vector<double> v_1d;
    vector <vector<double>> v_2d;
    inFile.open(PATH[20]);

    if(!inFile)
    {
        write_to_log("cannot open " + PATH[20]);
    }


    while(getline(inFile, stri))
    {
        if (!stri.empty())
        {

            istringstream iss(stri);
            while(iss>>num)
                v_1d.push_back(num);
            if (v_1d.size() != NL)
            {
                write_to_log("Error in pm file. Wrong number of lay");
            }
            v_2d.push_back(v_1d);
            v_1d.clear();

        }

    }
    if (v_2d.size() != NFA)
    {
        write_to_log("Error in pm file. Wrong number of assemblay");
    }

    for (unsigned int n =0; n < NFA; ++n)
        for (unsigned int h = 0; h < NL; ++h)
            FA[n].PRI[h].PM = v_2d[n][h];

}

void CORE::read_put_i()
{
    ifstream inFile;
    string stri;
    double num;
    vector<double> v_1d;
    vector <vector<double>> v_2d;
    inFile.open(PATH[19]);

    if(!inFile)
    {
        write_to_log("cannot open " + PATH[19]);
    }


    while(getline(inFile, stri))
    {
        if (!stri.empty())
        {

            istringstream iss(stri);
            while(iss>>num)
                v_1d.push_back(num);
            if (v_1d.size() != NL)
            {
                write_to_log("Error in sm file. Wrong number of lay");
            }
            v_2d.push_back(v_1d);
            v_1d.clear();

        }

    }
    if (v_2d.size() != NFA)
    {
        write_to_log("Error in sm file. Wrong number of assemblay");
    }

    for (unsigned int n =0; n < NFA; ++n)
        for (unsigned int h = 0; h < NL; ++h)
            FA[n].PRI[h].I = v_2d[n][h];

}

void CORE::read_flow()
{
    double num;
    vector<double> tmp;

    ifstream inFile;
    inFile.open(PATH[7]);

    if(!inFile)
    {

        write_to_log("cannot open " + PATH[7]);

    }

    while(inFile >> num)
        tmp.push_back(num);

    if(tmp.size() != NFA)
    {
        write_to_log("Error in flow file, num of flow value = " + to_string(tmp.size()) + " num of assembles "+ to_string(NFA) );
    }

    double summa = 0;
    for (unsigned int n = 0; n < NFA; ++n)
        summa+=tmp[n];
    for (unsigned int n = 0; n < NFA; ++n)
        FA[n].flow = tmp[n] / summa;

}

void CORE::read_put_rod_parts()
{
    for (unsigned int n = 0; n < NFA; n++)
    {
        for (unsigned int h = 0; h < NL; h++)
        {
            FA[n].PRI[h].Rod_part.push_back(1);
            for (unsigned int i = 0; i < suz.RODS[0].type_length.size(); i++)
                FA[n].PRI[h].Rod_part.push_back(0);
        }
    }
}


void CORE::read_thermocouple()
{
    ifstream inFile;
    string stri;
    inFile.open(PATH[15]);
    double num, num2;
    if(!inFile)
    {
        write_to_log("cannot open " + PATH[15]);
    }
    while(getline(inFile, stri))
    {
        istringstream iss(stri);
        iss >> num;
        iss >> num2;
        FA[num2-1].NTC = num;
    }
}

void CORE::read_kni()
{
    ifstream inFile;
    ifstream inFileZE;
    string stri;
    string striZE;
    double number, num, numZE;
    inFile.open(PATH[9]);
    inFileZE.open(PATH[6]);

    if(!inFile)
    {
        write_to_log("cannot open " + PATH[9] );
    }

    if(!inFileZE)
    {
        write_to_log("cannot open " + PATH[6]);
    }

    getline(inFile, stri);
    for (unsigned int n = 0; n < NKNI; n++)
    {
        getline(inFile, stri);
        getline(inFileZE, striZE);
        istringstream iss(stri);
        istringstream issZE(striZE);
        iss >> number;


        FA[number-1].kni = & kni[n];
        FA[number-1].kni->number = n+1;

        FA[number-1].kni->CIMB = xs_data->CIMB_bank[FA[number-1].PRI[1].type];
        FA[number-1].kni->myAssembly = & FA[number-1];
        FA[number-1].kni->orb = FA[number-1].orb;
        FA[number-1].kni->resize();

        for(int h = 0; h < FA[0].SL_7.size(); h++)
        {
          vector<double> tmp(NL,0);
          iss >> num; issZE >> numZE;
          FA[number-1].kni->positions.push_back(num);
          FA[number-1].kni->ZE.push_back(numZE);
          FA[number-1].kni->PRI_parts.push_back(tmp);
        }
        iss >> FA[number-1].kni->displacement;
        iss >> FA[number-1].kni->Emitter_length;
        iss >> FA[number-1].kni->NU;
        iss >> FA[number-1].kni->BETA;
        iss >> FA[number-1].kni->Ka;
        iss >> FA[number-1].kni->D;
        FA[number-1].kni->LRh104 = LRh104;
        FA[number-1].kni->LRh104m = LRh104m;
        FA[number-1].kni->sRh104 = sRh104;
        FA[number-1].kni->sRh104m = sRh104m;
        FA[number-1].kni->ALFA = ALFA;
        FA[number-1].kni->g1 = g1;
        FA[number-1].kni->h1 = h1;

        double PRI_top;
        double PRI_bottom;
        double DPZ_top;
        double DPZ_bottom;
        for (unsigned int k =0; k < NLDPZ; k++)
        {
            DPZ_top =  FA[number-1].kni->positions[k] +FA[number-1].kni->displacement + FA[number-1].kni->Emitter_length*100.0/2.0;
            DPZ_bottom =  FA[number-1].kni->positions[k]+FA[number-1].kni->displacement - FA[number-1].kni->Emitter_length*100.0/2.0;
            for (unsigned int h = 0; h < NL; h++)
            {
                PRI_top = FA[number-1].PRI[h].Z+FA[number-1].PRI[h].height/2.0;
                PRI_bottom = FA[number-1].PRI[h].Z-FA[number-1].PRI[h].height/2.0;

                if(PRI_bottom < DPZ_bottom && PRI_top < DPZ_top && PRI_top > DPZ_bottom)
                    FA[number-1].kni->PRI_parts[k][h] = (PRI_top - DPZ_bottom) / FA[number-1].kni->Emitter_length/100.0;
                else if (PRI_bottom > DPZ_bottom && PRI_top < DPZ_top)
                    FA[number-1].kni->PRI_parts[k][h] = FA[number-1].PRI[h].height / FA[number-1].kni->Emitter_length/100.0;
                else if (PRI_bottom > DPZ_bottom && PRI_top > DPZ_top && PRI_bottom < DPZ_top)
                    FA[number-1].kni->PRI_parts[k][h] = (DPZ_top - PRI_bottom) / FA[number-1].kni->Emitter_length/100.0;
            }
        }
    }

    for (unsigned int h = 0; h < NLDPZ; h++)
        for (unsigned int n = 0; n < NFA; n++)
            FA[n].Z7[h] = FA[0].kni->positions[h];

    double Z7_top;
    double Z7_bottom;
    double PRI_top;
    double PRI_bottom;
    double heigt_of_7lays_field = FA[82].kni->Emitter_length*100.0;
    for (unsigned int n = 0; n < NFA; n++)
        for (unsigned int k =0; k < 7; k++)
            {
                FA[n].PRI_parts[k].resize(NL,0);
                Z7_top =  FA[n].Z7[k] + heigt_of_7lays_field/2.0;
                Z7_bottom =  FA[n].Z7[k] - heigt_of_7lays_field/2.0;
                for (unsigned int h = 0; h < NL; h++)
                {
                    PRI_top = FA[n].PRI[h].Z+FA[n].PRI[h].height/2.0;
                    PRI_bottom = FA[n].PRI[h].Z-FA[n].PRI[h].height/2.0;

                    if(PRI_bottom < Z7_bottom && PRI_top < Z7_top && PRI_top > Z7_bottom)
                        FA[n].PRI_parts[k][h] = (PRI_top - Z7_bottom) / heigt_of_7lays_field;
                    else if (PRI_bottom > Z7_bottom && PRI_top < Z7_top)
                        FA[n].PRI_parts[k][h] = FA[n].PRI[h].height / heigt_of_7lays_field;
                    else if (PRI_bottom > Z7_bottom && PRI_top > Z7_top && PRI_bottom < Z7_top)
                        FA[n].PRI_parts[k][h] = (Z7_top - PRI_bottom) / heigt_of_7lays_field;
                }
            }

}


void CORE::recovery()
{

    //Перевод токов в ЭВ
    for(int k = 0 ; k < NKNI; k++)
            kni[k].current_conv();


    //Расчет значений тепловой мощности по ДПЗ и KV по ДПЗ
    calc_NED();



    // расчет средних значений KV по ДПЗ на орбите симметрии
    vector<vector <int> > num_on_orb(19, vector<int>(7,0));
    for (unsigned int o = 0; o < 19; ++o)
        for (unsigned int h = 0; h < NLDPZ; ++h)
            aver_of_orb[o][h] = 0;
    for(int k = 0 ; k < NKNI; k++)
        for (unsigned int h = 0; h < NLDPZ; ++h)
            if(kni[k].reliability[h])
            {
                num_on_orb[kni[k].orb][h]++;
                aver_of_orb[kni[k].orb][h] += kni[k].KV_dpz[h] ;
            }
    for (unsigned int o = 0; o < 19; ++o)
        for (unsigned int h = 0; h < NLDPZ; ++h)
            if(num_on_orb[o][h])
            {
                aver_of_orb[o][h] /= num_on_orb[o][h];
            }


    // *** KV_16 calculation ***
    for(int n = 0; n < NFA; n++)
    {
        for(int h = 0; h < NL; h++)
        {
            TPl[n][h] = FA[n].PRI[h].TP / FA[n].PRI[h].height;
            KV[n][h]  = TPl[n][h] / ((TP/HCORE)/NFA);
        }

        for (unsigned int k = 0; k < NLDPZ; k++)
        {
            KV_7[n][k] = 0;
            for(int h = 0; h < NL; h++)
                KV_7[n][k] += KV[n][h] * FA[n].PRI_parts[k][h];
        }

    }

    //расчет поправок в кассетах
    for (unsigned  int n = 0; n < NFA; n++)
        FA[n].recovery();




    for (unsigned int n = 0; n < NFA; ++n)
    {
        //получение сплайна
        cubic_spline splineForAssembly;
        splineForAssembly.build_spline(FA[n].Z7, FA[n].KDPZ7);

        for (unsigned int h = 0; h < NL ; ++h)
        {
            //получение значений в N слоях
            FA[n].KDPZN[h] = splineForAssembly.f(FA[n].ZN[h]);

            //Применение полученных поправок
            for (unsigned int d = 0; d < DIV; ++d)
            {
                FA[n].PRI[h].ND[d].TP *= FA[n].KDPZN[h] * FA[n].KTECON;
                FA[n].PRI[h].ND[d].NP *= FA[n].KDPZN[h] * FA[n].KTECON;
            }
        }

    }




    //Пересчет и нормировка энерговыделения после корректировки по показаниям ДПЗ
    TP = 0;
    for (unsigned int n = 0; n < NFA; n++)
        for (unsigned int h = 0; h < NL; h++)
            for (unsigned int d =0; d < DIV; d++)
                TP+=FA[n].PRI[h].ND[d].TP;

    normalization();

}

void CORE::reload(double PPR_time)
{
    std::cout << "NEED write new reload function" << std::endl;
    //assembly_filing();
    //new_assembly();
}

void CORE::assembly_filling(int internal, int external)
{
    FA[internal].type = FA[external].type;
    // Macro-fields filling
    for(int h = 0; h < NL; h++)
    {
        FA[internal].PRI[h].type  = FA[external].PRI[h].type;
        FA[internal].PRI[h].SL    = FA[external].PRI[h].SL;
        FA[internal].PRI[h].XE    = FA[external].PRI[h].XE;
        FA[internal].PRI[h].I     = FA[external].PRI[h].I ;
        FA[internal].PRI[h].SM    = FA[external].PRI[h].SM;
        FA[internal].PRI[h].PM    = FA[external].PRI[h].PM;
    }
    // tvel field filling
    for(int h = 0; h < NL; h++)
        FA[internal].PRI[h].PinSl = FA[external].PRI[h].PinSl;
}

/*!
 * \brief CORE::forecast Функция обновления ТВС на свежую
 * \param[in] internal Номер ТВС которую необходимо обновить на свежую
 * \param[in] type Тип топлива свежей ТВС
*/
void CORE::new_assembly(int internal, string type)
{
    FA[internal].type = type;
    // Macro-fields filling
    for(int h = 0; h < NL; h++)
    {
        FA[internal].PRI[h].SL   = 0;
        FA[internal].PRI[h].XE   = 0;
        FA[internal].PRI[h].I    = 0;
        FA[internal].PRI[h].SM   = 0;
        FA[internal].PRI[h].PM   = 0;
    }

    for(int h = 0; h < NL; h++)
        for(int n = 0; n < NPIN; n++)
            FA[internal].PRI[h].PinSl[n] = 0;

    FA[internal].division();
}

/*!
 * \brief CORE::forecast Функция чтения алгоритма перегрузки топлива
*/
vector<vector<int>> CORE::read_fuel_reload_algorithm( vector<string> fresh_types)
{
    std::cout << "NEED write new read_fuel_reload_algorithm" << std::endl;
}

void CORE::update()
{
    #pragma omp parallel for num_threads(N_pot)
    for (unsigned int n = 0; n < NFA; ++n)
    {
        if (fAbsorb == ON)
            FA[n].absorbers();

        if(fTH == ON)
        {
            for (unsigned int h =0 ; h <NL; ++h)
                xs_data->therm_update(FA[n].PRI[h]);

            if (fWT == ON)
                FA[n].wt_calc();

            if (fFT == ON)
                FA[n].ft_calc();
        }

        for (unsigned int h =0 ; h <NL; ++h)
            xs_data->cs_update(FA[n].PRI[h]);
    }
}

void CORE::zero_power_calculate()
{
    cout << "zero power" << endl;

    //обнуление полей энерговыделения
    TP = 0;

    for (unsigned int n = 0; n < NFA; n++)
    {
        FA[n].TP = 0;
        for (unsigned int h = 0; h < NL; ++h)
        {
            FA[n].PRI[h].TP = 0;
            FA[n].PRI[h].NP = 0;
            FA[n].PRI[h].F1 = 0;
            FA[n].PRI[h].F2 = 0;
            for (unsigned int d = 0; d < DIV; ++d)
            {
                FA[n].PRI[h].ND[d].TP = 0;
                FA[n].PRI[h].ND[d].NP = 0;
                FA[n].PRI[h].ND[d].F1 = 0;
                FA[n].PRI[h].ND[d].F2 = 0;
            }
            for (unsigned int p =0; p < NPIN; ++p)
                FA[n].PRI[h].pin_power[p] = 0;
        }
        //обнуление полей в семи слоях
        FA[n].interpolation_7_lays();

        //расчет отравителей
        FA[n].absorbers();

        for (unsigned int h = 0; h < NL; ++h)
        {
            FA[n].PRI[h].SL_f = FA[n].PRI[h].SL;



            FA[n].PRI[h].XE += FA[n].PRI[h].dXE;
            FA[n].PRI[h].I  += FA[n].PRI[h].dI;
            FA[n].PRI[h].SM += FA[n].PRI[h].dSM;
            FA[n].PRI[h].PM += FA[n].PRI[h].dPM;

            if(FA[n].PRI[h].XE < 0)
                FA[n].PRI[h].XE = 0;
            if(FA[n].PRI[h].I < 0)
                FA[n].PRI[h].I = 0;
            if(FA[n].PRI[h].SM < 0)
                FA[n].PRI[h].SM = 0;
            if(FA[n].PRI[h].PM < 0)
                FA[n].PRI[h].PM = 0;

            FA[n].PRI[h].XE_f = FA[n].PRI[h].XE;
            FA[n].PRI[h].SM_f = FA[n].PRI[h].SM;
            FA[n].PRI[h].PM_f = FA[n].PRI[h].PM;
            FA[n].PRI[h].I_f  = FA[n].PRI[h].I;
        }


    }
    //расчет функционалов поля
    vector<int> zero_12(12,0);
    AO = 0;
    KQ_max = 0;
    KQ_max_cas = 0;
    KV_max = 0;
    KV_max_cas = 0;
    KV_max_lay = 0;
    KV_7_max = 0;
    KV_7_max_cas = 0;
    KV_7_max_lay = 0;

    QL_max = 0;
    QL_max_cas = 0;
    QL_max_lay = 0;
    QL_max_tvel = 0;
    Kr_max = 0;

    QL_12 = zero_12;
    SL_12 = zero_12;
    dT_12 = zero_12;
    TP_12 = zero_12;
    WT_out_12 = zero_12;
    KO_12 = zero_12;
    KV_12 = zero_12;
    KQ_12 = zero_12;



    for (unsigned int n = 0; n < NFA; n++)
    {
        assembly_AO[n] = 0;
        if(FA[n].kni != 0)
            DPZ_AO[FA[n].kni->number-1] = 0;
        KQ[n] = 0;
        KV_max_assembly[n] = 0;
        QL_max_assembly[n] = 0;
        KK_max[n] = 0;
        KO_max[n] = 0;
        Kri_max[n] = 0;

        for(unsigned int h = 0; h <NL; h++)
        {
            for(unsigned int t = 0; t < NPIN; t++)
            {
                QL[n][t][h] = 0;
                KK[n][t][h] = 0;
                KO[n][t][h] = 0;
                Krik[n][t] = 0;
            }
            KV[n][h] = 0;
            TPl[n][h] = 0;
        }

        for(unsigned int h = 0; h < 7; h++)
        {
            KV_7[n][h] = 0;

        }

    }

    // *** XE and SM average parameters calculation ***
    XE_aver = XE_top_aver = XE_bottom_aver = SM_aver = SM_top_aver = SM_bottom_aver = 0;
    for(int n = 0; n < NFA; n++)
    {
        for(int h = 0; h < NL; h++)
        {
            if(FA[n].PRI[h].Z - FA[n].PRI[h].height/2 >= HCORE/2)
            {
                XE_top_aver += FA[n].PRI[h].XE / NFA / NL;
                SM_top_aver += FA[n].PRI[h].SM / NFA / NL;
            }
            else if(FA[n].PRI[h].Z + FA[n].PRI[h].height/2 <= HCORE/2)
            {
                XE_bottom_aver += FA[n].PRI[h].XE / NFA / NL;
                SM_bottom_aver += FA[n].PRI[h].SM / NFA / NL;
            }
            else
            {
                XE_top_aver += FA[n].PRI[h].XE * ((FA[n].PRI[h].Z + FA[n].PRI[h].height/2 - HCORE/2)/FA[n].PRI[h].height)/ NFA / NL;
                XE_bottom_aver += FA[n].PRI[h].XE * ((HCORE/2 - (FA[n].PRI[h].Z-FA[n].PRI[h].height/2))/FA[n].PRI[h].height)/ NFA / NL;
                SM_top_aver += FA[n].PRI[h].SM * ((FA[n].PRI[h].Z + FA[n].PRI[h].height/2 - HCORE/2)/FA[n].PRI[h].height)/ NFA / NL;
                SM_bottom_aver += FA[n].PRI[h].SM * ((HCORE/2 - (FA[n].PRI[h].Z-FA[n].PRI[h].height/2))/FA[n].PRI[h].height)/ NFA / NL;
            }
            XE_aver += FA[n].PRI[h].XE / NFA / NL;
            SM_aver += FA[n].PRI[h].SM / NFA / NL;
        }
    }

    TP = 3000;

    for (unsigned int n = 0; n < NFA; n++)
    {
        FA[n].TP = 18;
        for (unsigned int h = 0; h < NL; ++h)
        {
            FA[n].PRI[h].TP = 0.5;
            FA[n].PRI[h].NP = 1e14;
            FA[n].PRI[h].F1 = 1e14;
            FA[n].PRI[h].F2 = 1e13;
            for (unsigned int d = 0; d < DIV; ++d)
            {
                FA[n].PRI[h].ND[d].TP = 0.01;
                FA[n].PRI[h].ND[d].NP = 1e14;
                FA[n].PRI[h].ND[d].F1 = 1e14;
                FA[n].PRI[h].ND[d].F2 = 1e13;
            }
            for (unsigned int p =0; p < NPIN; ++p)

                FA[n].PRI[h].pin_power[p] = 0.001;
        }
    }
}

void CORE::write_info() {
    if (DBG_FLG) {
        char * sBufer = new char[10000000];
        int current_size = 0;


        current_size += sprintf(&sBufer[current_size], "dTIME = %10.4e\n",     dTIME         );
        current_size += sprintf(&sBufer[current_size], "POWER = %10.2f\n",     HEAT_POWER    );
        current_size += sprintf(&sBufer[current_size], "FLOW  = %10.2f\n",     G             );
        current_size += sprintf(&sBufer[current_size], "PRESS = %10.2f\n",     P_out         );
        current_size += sprintf(&sBufer[current_size], "CB    = %10.2f\n",     CB            );
        current_size += sprintf(&sBufer[current_size], "THN1  = %10.2f\n",     txn[0]        );
        current_size += sprintf(&sBufer[current_size], "THN2  = %10.2f\n",     txn[1]        );
        current_size += sprintf(&sBufer[current_size], "THN3  = %10.2f\n",     txn[2]        );
        current_size += sprintf(&sBufer[current_size], "THN4  = %10.2f\n\n",   txn[3]        );


        current_size += sprintf(&sBufer[current_size], "%10s%10s%10s%20s%20s%20s%20s%20s%10s%10s%10s%10s%10s%20s%10s\n", "NFA", "NLAY", "TYPE", "SL", "TP", "NP", "XE", "SM", "WD", "WT", "FT", "CB", "WTin", "CP","flow");

        for ( int n = 0; n < NFA; n++ ) {
            for ( int h = 0; h < NL; h++) {
                current_size += sprintf(&sBufer[current_size],
               "%10d%10d%10d%20.3f%20.3e%20.3e%20.3e%20.3e%10.4f%10.2f%10.2f%10.3f%10.3f%20.4e%10.3e\n",
               n, h, FA[n].PRI[h].type, FA[n].PRI[h].SL, FA[n].PRI[h].TP, FA[n].PRI[h].NP, FA[n].PRI[h].XE, FA[n].PRI[h].SM, FA[n].PRI[h].WD, FA[n].PRI[h].WT, FA[n].PRI[h].FT, FA[n].PRI[h].WD*CB, FA[n].WT_in, FA[n].PRI[h].CP, FA[n].flow );
            }
        }

        FILE * outFile = fopen((PATH[DebugDir] + "/INFO.txt").c_str(), "w");
        if ( !outFile ) {
            cout << "cannot open " << (PATH[DebugDir] + "/INFO.txt").c_str() << endl;
        } else {
            fprintf(outFile, "%s", sBufer);
            fclose(outFile);
        }


        delete sBufer;
    }


}
