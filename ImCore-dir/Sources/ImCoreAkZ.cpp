#include "ImCoreAkZ.h"

ImCoreAkZ::ImCoreAkZ(char * HOMEDIR) {
    pCore1 = new CORE;
    pCore2 = new CORE;
    pXS = new XS;
    pBufer = new bufer(pCore1);
    
    pCore1->xs_data = pXS;
    
    pCore1->HOMEDIR = HOMEDIR;
    pCore1->HOMEDIR += "/";
}


ImCoreAkZ::~ImCoreAkZ() {
    if ( pCore1 ) {
        delete pCore1;
    }
    if ( pCore2 ) {
        delete pCore2;
    }
    if ( pXS ) {
        delete pXS;
    }
    if ( pBufer ) {
        delete pBufer;
    }
}

void ImCoreAkZ::init() {
    pCore1->start();
    pCore1->t_w = new test_writer(pCore1->PATH[CORE::TestWriterFile]);
    pBufer = new bufer(pCore1);
    pBufer->set_ZE(pCore1);
    pCore1->write_info();
}

void ImCoreAkZ::initCalc()
{
    pCore1->CoreName = "INIT";
    
    
    std::cout << "TREAD INIT CALC" << std::endl;
    string shufle = "                     ";
    pCore1->fCrit = CORE::Cb;             // Parametr for finding critical state, boron acid is using
    pCore1->fXe = CORE::current;          // Concentration of Xe-135 calculate with evolution
    pCore1->fSm = CORE::current;          // Concentration of Sm-149 calculate with evolution
    pCore1->fRECOVERY = CORE::ON;         // Recovory with DPZ is using
    pCore1->fTH = CORE::ON;               // Termal-hydravlic parametrs calculate
    pCore1->fWT = CORE::ON;               // Water calculate
    pCore1->fFT = CORE::ON;               // Fuel calculate
    pCore1->fAbsorb = CORE::ON;

    pCore1->t_w->write(shufle+pCore1->time.givetxt() + "   начало первого расчета, жду мощности > 0" );

    pBufer->get_plant_read(pCore1);
    
    while (pCore1->npt < 2 || pCore1->HEAT_POWER <0.1 || pCore1->P_out < 0.1) {

        if ( pCore1->dTIME > 1.0 / 24.0 / 3600.0 / 2.0 ) {
            pCore1->zero_power_calculate();
            putRecoveryResult();
            putDiffCalcResult();
            pCore1->update_files();
            pCore1->t_w->write(shufle+pCore1->time.givetxt() + "   расчет при нулевой мощности" );
            ::pProg->SetFlag(FLG_CHANGE_DIF_DATA, SV_NEEDED);
            ::pProg->SetFlag(FLG_CHANGE_REBUILD_DATA, SV_NEEDED);
        }
        usleep(10000);
        pBufer->get_plant_read(pCore1);

    }

   pCore1->diffusion_equetion();
   pCore1->t_w->write(shufle+pCore1->time.givetxt() + "   Провел расчет с мощностью" );

   pCore1->debug_write(pCore1->time.givetxt());

   pBufer->set_DU_results(pCore1);
   pBufer->set_ZE(pCore1);
   pCore1->t_w->write(shufle+pCore1->time.givetxt() + "   Подготовился для парралельной работы" );

   putRecoveryResult();
   putDiffCalcResult();
   pCore1->update_files();
   ::pProg->SetFlag(FLG_CHANGE_DIF_DATA, SV_NEEDED);
   ::pProg->SetFlag(FLG_CHANGE_REBUILD_DATA, SV_NEEDED);
    
   pCore2->make_copy(*pCore1);
   pCore2->PATH[CORE::DebugDir] = pCore1->HOMEDIR + "dbg2";
   pCore2->write_info();

   pCore1->CoreName = "REC";
   pCore2->CoreName = "DIF";
}

void ImCoreAkZ::threadDU() {

    TIME_IMCORE time_debug = pCore2->time;
    string shufle = "                                                                                                          ";

    while(true)
    {//

        if( ::pProg->GetFlag(FLG_STOP_DIF_CALC)!=SV_NEEDED &&
            ::pProg->GetFlag(FLG_RUN_DIF_CALC) !=SV_PROCESS) {
			::pProg->SetFlag(FLG_RUN_DIF_CALC, SV_PROCESS);

            pBufer->get_plant_read(pCore2);
            pBufer->writeInputToFile(pCore2);

            pCore2->t_w->write(shufle+pCore2->time.givetxt() + "   proc2 забрал входные данные" );
            if (pCore2->npt < 2 || pCore2->HEAT_POWER < 0.1 || pCore2->P_out < 0.1)
            {
                pCore2->zero_power_calculate();
                pCore2->t_w->write(shufle+pCore2->time.givetxt() + "   proc2 расчет нулевой мощности" );
                pCore2->update_files();
            }
            else if ( pCore2->G < 20.0 * 1E9 / 3600.0 || 
                      pCore2->WT_in < 500 || 
                      pCore2->WT_in > 600 ||
                      pCore2->HEAT_POWER > 3600 ) {
                pCore2->write_to_log("somthing wrong in input data ");
                usleep(50000);
            }
            else
            {
                double t1 = clock();

                if (pCore2->HEAT_POWER/pCore2->NOMINAL_POWER <0.1) {
                    pCore2->fRECOVERY = CORE::OFF;
                }
                pCore2->diffusion_equetion();
                pCore2->t_w->write(shufle+pCore2->time.givetxt() + "   proc2 Провел расчет ДУ, заняло" + to_string((clock()-t1)/CLOCKS_PER_SEC) );
                pCore2->fRECOVERY = CORE::ON;
                pBufer->set_DU_results(pCore2);
                pCore2->update_files();
                pCore2->t_w->write( shufle+ pCore2->time.givetxt() + "   proc2 Провел запись файлов, заняло " +to_string((clock()-t1)/CLOCKS_PER_SEC) );
            }

            if(abs(time_debug.diff(&pCore2->time) / 3600.0 / 24.0 /1000.0) > 0.25)
            {   // раз в 6 часов запись того, что происходит в файл
                time_debug = pCore2->time;
                pCore2->debug_write(pCore2->time.givetxt());
            }


            putDiffCalcResult();


            ::pProg->SetFlag(FLG_CHANGE_DIF_DATA, SV_NEEDED);
            ::pProg->SetFlag(FLG_RUN_DIF_CALC, SV_READY);

			ThreadYield();
		}
		else if(::pProg->GetFlag(FLG_STOP_DIF_CALC) == SV_NEEDED)
        {
			break;
		}
		else
        {
			ThreadYield();
		}
    }
}

void ImCoreAkZ::recovery()
{
    double t1 = clock();
    pBufer->get_DU_results(pCore1);
    pCore1->write_info();
    if (pCore1->fRECOVERY == CORE::ON && pCore1->HEAT_POWER/pCore1->NOMINAL_POWER > 0.1)
    {
        pCore1->t_w->write(pCore1->time.givetxt() + "   proc1 Начинает восстановление" );
        pCore1->recovery();
        for (unsigned int k = 0; k < pCore1->NKNI; k++ ) {
            pCore1->kni[k].dpz_charge();
        }
        pBufer->set_ZE(pCore1);
        putRecoveryResult();
        pCore1->t_w->write(pCore1->time.givetxt() + "   proc1 Провел восстановление, заняло " +to_string((clock()-t1)/CLOCKS_PER_SEC) );
        ::pProg->SetFlag(FLG_CHANGE_REBUILD_DATA, SV_NEEDED);
    }
    
}

void ImCoreAkZ::getScanData(InputBuff *pDataBuff) {
    pBufer->getPlantData(pDataBuff);
}

void ImCoreAkZ::putRecoveryResult() {
    
    unsigned int NFA = pCore1->NFA;
    unsigned int NL = pCore1->NL;
    
    //RECORD 1

    ::pProg->pCalc->MonRecord1[0] = 3;
    ::pProg->pCalc->MonRecord1[1] = pCore1->time.give_sec_from_1970();
    ::pProg->pCalc->MonRecord1[2] = NFA;
    ::pProg->pCalc->MonRecord1[3] = pCore1->NPIN;
    ::pProg->pCalc->MonRecord1[4] = NL;
    ::pProg->pCalc->MonRecord1[5] = pCore1->NKNI;
    ::pProg->pCalc->MonRecord1[6] = pCore1->NLDPZ;
    ::pProg->pCalc->MonRecord1[7] = 0;

    //RECORD 2
    for (unsigned int n = 0; n < NFA; n++)
        for (unsigned int h = 0; h < NL; h++)
            ::pProg->pCalc->MonRecord2[n][h] = pCore1->KV[n][h];

    //RECORD 3
    for (unsigned int n = 0; n < NFA; n++)
        for (unsigned int h = 0; h < NL; h++)
            ::pProg->pCalc->MonRecord3[n][h] = 0.0;


    //RECORD 4
    ::pProg->pCalc->MonRecord4[0] = pCore1->CB;
    ::pProg->pCalc->MonRecord4[1] = pCore1->TP;
    ::pProg->pCalc->MonRecord4[2] = pCore1->TP * 1e6 / pCore1->HCORE/pCore1->NFA/pCore1->NFPIN;
    ::pProg->pCalc->MonRecord4[3] = 0.0;
    ::pProg->pCalc->MonRecord4[4] = pCore1->ro_XE;
    ::pProg->pCalc->MonRecord4[5] = pCore1->ro_SM;

    
}

void ImCoreAkZ::putDiffCalcResult() {
    
    unsigned int NFA = pCore2->NFA;
    unsigned int NL = pCore2->NL;
    unsigned int NPIN = pCore2->NPIN;
    unsigned int NKNI = pCore2->NKNI;
    unsigned int NLDPZ = pCore2->NLDPZ;
    
    
    //RECORD 1

    ::pProg->pCalc->UpdRecord1[0] = 3;
    ::pProg->pCalc->UpdRecord1[1] = pCore2->time.give_sec_from_1970();
    ::pProg->pCalc->UpdRecord1[2] = NFA;
    ::pProg->pCalc->UpdRecord1[3] = NPIN;
    ::pProg->pCalc->UpdRecord1[4] = NL;
    ::pProg->pCalc->UpdRecord1[5] = NKNI;
    ::pProg->pCalc->UpdRecord1[6] = NLDPZ;
    ::pProg->pCalc->UpdRecord1[7] = 0;

    //RECORD 2
    for (unsigned int n = 0; n < NFA; n++)
        for (unsigned int h = 0; h < NL; h++)
            ::pProg->pCalc->UpdRecord2[n][h] = pCore2->KV[n][h];

    //RECORD 3
    for (unsigned int n = 0; n < NFA; n++)
        for (unsigned int h = 0; h < NL; h++)
            ::pProg->pCalc->UpdRecord3[n][h] = 0.0;

    //RECORD 4
    for (unsigned int n = 0; n < NFA; n++)
        for (unsigned int pin = 0; pin < NPIN; pin++)
            for (unsigned int h = 0; h < NL; h++)
                ::pProg->pCalc->UpdRecord4[n][pin][h] =  pCore2->KK[n][pin][h];

    //RECORD 5
    for (unsigned int n = 0; n < NFA; n++)
        for (unsigned int h = 0; h < NL; h++)
           ::pProg->pCalc->UpdRecord5[n][h] = pCore2->FA[n].PRI[h].SL;

    //RECORD 6
    float pin_2d_Sl;
    for (unsigned int n = 0; n < NFA; n++)
        for (unsigned int pin = 0; pin < NPIN; pin++) {
            pin_2d_Sl = 0;
            for (unsigned int h = 0; h < NL; h++)
                pin_2d_Sl += pCore2->FA[n].PRI[h].PinSl[pin]*pCore2->FA[n].PRI[h].part;

            ::pProg->pCalc->UpdRecord6[n][pin] = pin_2d_Sl;
        }

    //RECORD 7
    for (unsigned int n = 0; n < NFA; n++)
        for (unsigned int h = 0; h < NL; h++)
            ::pProg->pCalc->UpdRecord7[n][h] = pCore2->FA[n].PRI[h].I * 1E24;

    //RECORD 8
    for (unsigned int n = 0; n < NFA; n++)
        for (unsigned int h = 0; h < NL; h++)
           ::pProg->pCalc->UpdRecord8[n][h] = pCore2->FA[n].PRI[h].XE * 1E24;

    //RECORD 9
    for (unsigned int n = 0; n < NFA; n++)
        for (unsigned int h = 0; h < NL; h++)
            ::pProg->pCalc->UpdRecord9[n][h] = pCore2->FA[n].PRI[h].PM * 1E24;

    //RECORD 10
    for (unsigned int n = 0; n < NFA; n++)
        for (unsigned int h = 0; h < NL; h++)
            ::pProg->pCalc->UpdRecord10[n][h] = pCore2->FA[n].PRI[h].SM * 1E24;

    //RECORD 11
    for (unsigned int k = 0; k < NKNI; k++)
        for (unsigned int h = 0; h < NLDPZ; h++)
            ::pProg->pCalc->UpdRecord11[k][h] = pCore2->kni[k].SED[h];

    //RECORD 12
    ::pProg->pCalc->UpdRecord12[0] = pCore2->CB;
    ::pProg->pCalc->UpdRecord12[1] = pCore2->TP;
    ::pProg->pCalc->UpdRecord12[2] = pCore2->TP * 1e6 / pCore2->HCORE/NFA/pCore2->NFPIN;
    ::pProg->pCalc->UpdRecord12[3] = 0.0;
    ::pProg->pCalc->UpdRecord12[4] = pCore2->ro_XE;
    ::pProg->pCalc->UpdRecord12[5] = pCore2->ro_SM;
    ::pProg->pCalc->UpdRecord12[6] = 0.0;
    ::pProg->pCalc->UpdRecord12[7] = 0.0;
    ::pProg->pCalc->UpdRecord12[8] = 0.0;
    ::pProg->pCalc->UpdRecord12[9] = 0.0;
    ::pProg->pCalc->UpdRecord12[10] = 0.0;
}


/*!
 * \brief bufer::bufer Конструктор, расширяет все поля до необходимого размера
 * \param inCORE Указатель на объект зоны, необходим для задания корректных размеров массивов
 *
 * \author Шлапак Игорь
 * \date 25.10.2018
 */

bufer::bufer(CORE* inCORE)
{
    NTC = inCORE->NTC;
    NKNI = inCORE->NKNI;
    NSUZ = inCORE->NSUZ;
    NGSUZ = inCORE->NGSUZ;
    NPT = inCORE->NPT;
    NLDPZ = inCORE->NLDPZ;
    NL = inCORE->NL;
    DIV = inCORE->DIV;
    NFA = inCORE->NFA;



    TC.resize(NTC);
    I.resize(NKNI, std::vector<double> (NLDPZ));
    suz.resize(NSUZ);
    gsuz.resize(NGSUZ);
    spt.resize(NPT);
    txn.resize(NPT);

    ZE.resize(NKNI,std::vector<double>(NLDPZ));
    KC_dipl.resize(NKNI,std::vector<double>(NLDPZ));
    TPl_dipl.resize(NKNI,std::vector<double>(NLDPZ));
    WD_dipl.resize(NKNI,std::vector<double>(NLDPZ));
    SL_dipl.resize(NKNI,std::vector<double>(NLDPZ));

    TPl_DPZ.resize(NKNI,std::vector<double>(NLDPZ));
    TPl7_AS.resize(NFA,std::vector<double>(NLDPZ));

    TP_ND.resize(NFA,std::vector<std::vector<double>>(NL,std::vector<double>(DIV)));
    NP_ND.resize(NFA,std::vector<std::vector<double>>(NL,std::vector<double>(DIV)));

    TP_PR.resize(NFA,std::vector<double>(NL));
    NP_PR.resize(NFA,std::vector<double>(NL));

    TP_AS.resize(NFA);
    NP_AS.resize(NFA);


    fPR = no_data;
    fDU = no_data;
}

/*!
 * \brief bufer::set_plant_read Запись plant.read в буфер
 * \param inCORE Указатель на объект зоны
 * \author Шлапак Игорь
 * \date 25.10.2018
 */
void bufer::set_ZE(CORE* inCORE)
{
    unsigned int status_fPR;

    //Если флаг опущен то доступ к данным не разрешен
    //ждем 10 мс и пробуем снова
    while(fPR == close)
    {
        usleep(10000);
    }

    status_fPR = fPR;

    //в случае поднятого флага, опускаем его, давая понять что заняли буфер
    fPR = close;

    for(unsigned int k = 0; k < NKNI; k++)
        for(unsigned int h = 0; h < NLDPZ; h++)
        {
            ZE[k][h] = inCORE->kni[k].ZE[h];
        }

    fPR = status_fPR;
}

/*!
 * \brief bufer::get_plant_read Чтение plant.read из буфера
 * \param inCORE Указатель на объект зоны
 * \author Шлапак Игорь
 * \date 26.10.2018
 */
void bufer::get_plant_read(CORE* inCORE)
{
    //Если флаг опущен то доступ к данным не разрешен
    //ждем 10 мс и пробуем снова
    while(fPR != new_info)
    {
        usleep(10000);
    }

    //в случае поднятого флага, опускаем его, давая понять что заняли буфер
    fPR = close;

    TIME_IMCORE old;
    old = inCORE->time;

    // время считанного файла
    inCORE->time = time_read;

    inCORE->dTIME = old.diff(&time_read) / 3600.0 / 24.0 /1000.0 ;
    if (inCORE->dTIME < 0)
        inCORE->dTIME = 0;

    //считываем признак работы петли и температуру тн на входе в зону по петлям
    for(unsigned int i = 0; i < inCORE->NPT; i++)
    {
        inCORE->spt[i] = spt[i];
        inCORE->txn[i] = txn[i];
    }

    //количество работающих петель
    inCORE->npt = npt;

    //тепловая мощность
    inCORE->HEAT_POWER = power;

    //расход
    inCORE->G = flow;

    //давление
    inCORE->P_out = P_out;

    //чтение токов
    for(unsigned int k = 0; k < NKNI; k++) {
        for(unsigned int h = 0; h < NLDPZ; h++)
        {
            inCORE->kni[k].I[h] = I[k][h];
            inCORE->kni[k].ZE[h] = ZE[k][h];
            if(inCORE->dTIME > 5e-4) {
                inCORE->kni[k].I_pr[h]   = I[k][h];
                inCORE->kni[k].Tok_pr[h] = I[k][h];
                inCORE->kni[k].Tok[h]    = I[k][h];
            }
        }
    }

    //чтение позиции ОР СУЗ
    for (unsigned int s = 0; s < inCORE->NSUZ; s++)
        inCORE->suz.RODS[s].position = suz[s];

    //чтение позиции групп ОР СУЗ
    for (unsigned int s = 0; s < inCORE->NGSUZ; s++)
        inCORE->suz.GROUPS[s] = gsuz[s];

    //чтение показаний термопар
    for(unsigned int n = 0; n < inCORE->NFA; n++)
    {
        if (inCORE->FA[n].NTC)
            inCORE->FA[n].thermocouple = TC[inCORE->FA[n].NTC-1];
    }

    inCORE->WT_in = (spt[0] * txn[0]
            +spt[1] * txn[1]
            +spt[2] * txn[2]
            +spt[3] * txn[3])
            /npt+273.15;

    for (unsigned int n = 0 ; n  < NFA; n++)
        inCORE->FA[n].WT_in = inCORE->WT_in;

    inCORE->set_pressure();
    inCORE->test_dpz();

    fPR = old_info;
}


/*!
 * \brief bufer::set_DU_results Заполнение буфера результатом решения ДУ
 * \param inCORE Указатель на зону с которой беруться данные
 * \author Шлапак Игорь
 * \date 26.10.2018
 */

void bufer::set_DU_results(CORE * inCORE)
{
    //Если флаг опущен то доступ к данным не разрешен
    //ждем 10 мс и пробуем снова
    while(fDU == close)
    {
        usleep(10000);
    }

    //в случае поднятого флага, опускаем его, давая понять что заняли буфер
    fDU = close;

    TP_CO = inCORE->TP;
    NP_CO = inCORE->NP;
    ro_XE = inCORE->ro_XE;
    ro_SM = inCORE->ro_SM;
    CB = inCORE->CB;

    for (int k = 0; k < inCORE->NKNI; k++) {
        for(unsigned int h = 0; h < inCORE->NLDPZ; h++) {
            KC_dipl[k][h]  = inCORE->kni[k].KC_displ[h];
            TPl_dipl[k][h] = inCORE->kni[k].TPl_displ[h];
            WD_dipl[k][h]  = inCORE->kni[k].WD_displ[h];
            SL_dipl[k][h]  = inCORE->kni[k].SL_displ[h];
            TPl_DPZ[k][h]  = inCORE->kni[k].TPl[h];
        }
    }

    for(unsigned int n = 0 ; n < inCORE->NFA; n++)
    {
        for(unsigned int h = 0; h < inCORE->NLDPZ; h++)
            TPl7_AS[n][h] = inCORE->FA[n].TPl_7[h];

        for(unsigned int h = 0; h < inCORE->NL; h++)
        {
            TP_PR[n][h] = inCORE->FA[n].PRI[h].TP;
            NP_PR[n][h] = inCORE->FA[n].PRI[h].NP;

            for(unsigned int d = 0; d < inCORE->DIV; d++)
            {
                TP_ND[n][h][d] = inCORE->FA[n].PRI[h].ND[d].TP;
                NP_ND[n][h][d] = inCORE->FA[n].PRI[h].ND[d].NP;
            }
        }

        TP_AS[n] = inCORE->FA[n].TP;
        NP_AS[n] = inCORE->FA[n].NP;
    }



    fDU = new_info;
}


/*!
 * \brief bufer::get_DU_results Заполнение зоны из буфера
 * \param inCORE Указатель на зону которая будет заполнена данными
 * \author Шлапак Игорь
 * \date 26.10.2018
 */

void bufer::get_DU_results(CORE * inCORE)
{
    unsigned int status_fPR;
    //Если флаг опущен то доступ к данным не разрешен
    //ждем 10 мс и пробуем снова
    while( fDU == close || fDU == no_data || fPR == close )
    {
        usleep(10000);
    }
    status_fPR = fPR;

    //в случае поднятого флага, опускаем его, давая понять что заняли буфер
    fDU = close;
    fPR = close;

    TIME_IMCORE old;
    old = inCORE->time;

    // время считанного файла
    inCORE->time = time_read;


    inCORE->dTIME = old.diff(&time_read) / 3600.0 / 24.0 /1000.0 ;
    if (inCORE->dTIME < 0)
        inCORE->dTIME = 0;

    for(unsigned int k = 0; k < NKNI; k++) {
        for(unsigned int h = 0; h < NLDPZ; h++)
        {
            inCORE->kni[k].I[h] = I[k][h];
            inCORE->kni[k].ZE[h] = ZE[k][h];
            if(inCORE->dTIME > 5e-4) {
                inCORE->kni[k].I_pr[h]   = I[k][h];
                inCORE->kni[k].Tok_pr[h] = I[k][h];
                inCORE->kni[k].Tok[h]    = I[k][h];
            }
        }
    }

    //чтение показаний термопар
    for(unsigned int n = 0; n < inCORE->NFA; n++)
    {
        if (inCORE->FA[n].NTC)
            inCORE->FA[n].thermocouple = TC[inCORE->FA[n].NTC-1];
    }

    inCORE->WT_in = (spt[0] * txn[0]
            +spt[1] * txn[1]
            +spt[2] * txn[2]
            +spt[3] * txn[3])
            /npt+273.15;

    for (unsigned int n = 0 ; n  < NFA; n++)
        inCORE->FA[n].WT_in = inCORE->WT_in;

    inCORE->set_pressure();
    inCORE->test_dpz();

    inCORE->TP = TP_CO;
    inCORE->NP = NP_CO;
    inCORE->ro_XE = ro_XE;
    inCORE->ro_SM = ro_SM;
    inCORE->CB = CB;

    for (int k = 0; k < inCORE->NKNI; k++) {
        for(unsigned int h = 0; h < inCORE->NLDPZ; h++) {
            inCORE->kni[k].KC_displ[h]  = KC_dipl[k][h];
            inCORE->kni[k].TPl_displ[h] = TPl_dipl[k][h];
            inCORE->kni[k].WD_displ[h]  = WD_dipl[k][h];
            inCORE->kni[k].SL_displ[h]  = SL_dipl[k][h];
            inCORE->kni[k].TPl[h]       = TPl_DPZ[k][h];
        }
    }
    for(unsigned int n = 0 ; n < NFA; n++)
    {
        for(unsigned int h = 0; h < NLDPZ; h++)
            inCORE->FA[n].TPl_7[h] = TPl7_AS[n][h];

        for(unsigned int h = 0; h < NL; h++)
        {
            inCORE->FA[n].PRI[h].TP = TP_PR[n][h];
            inCORE->FA[n].PRI[h].NP = NP_PR[n][h];

            for(unsigned int d = 0; d < inCORE->DIV; d++)
            {
                inCORE->FA[n].PRI[h].ND[d].TP = TP_ND[n][h][d];
                inCORE->FA[n].PRI[h].ND[d].NP = NP_ND[n][h][d];
            }
        }

        inCORE->FA[n].TP = TP_AS[n];
        inCORE->FA[n].NP = NP_AS[n];
    }



    fDU = old_info;
    fPR = status_fPR;
}


void bufer::writeInputToFile(CORE * inCore) {
    if (inCore->DBG_FLG) {
        char * sBufer = new char[10000000];
        int current_size = 0;
        FILE * outFile = fopen((inCore->PATH[CORE::DebugDir] + "/input.txt").c_str(), "r");

        if ( outFile == 0 ) {
            outFile = fopen((inCore->PATH[CORE::DebugDir] + "/input.txt").c_str(), "w");
            fprintf(outFile,"%16s%10s%10s%10s|%10s|%10s|%10s|%10s|%10s|\n", "TIME", "POWER", "FLOW", "PRESS", "THN", "SPT", "SUZ", "IED", "TC");
        }
        fclose(outFile);

        current_size += sprintf(&sBufer[current_size], "%16s", time_read.givedate().c_str());
        current_size += sprintf(&sBufer[current_size], "%10.2f", power);
        current_size += sprintf(&sBufer[current_size], "%16.4e", flow * 3600 / 1E9);
        current_size += sprintf(&sBufer[current_size], "%10.2f", P_out);

        current_size += sprintf(&sBufer[current_size], "|");
        for ( int i = 0; i < NPT; i++ ) {
            current_size += sprintf(&sBufer[current_size], "%10.2f", txn[i]);
        }

        current_size += sprintf(&sBufer[current_size], "|");
        for ( int i = 0; i < NPT; i++ ) {
            current_size += sprintf(&sBufer[current_size], "%4d", spt[i]);
        }

        current_size += sprintf(&sBufer[current_size], "|");
        for ( int i = 0; i < NSUZ; i++ ) {
            current_size += sprintf(&sBufer[current_size], "%10.2f", suz[i]);
        }

        current_size += sprintf(&sBufer[current_size], "|");
        for ( int k = 0; k < NKNI; k++ ) {
            for (int h = 0; h < NLDPZ; h++ ) {
                current_size += sprintf(&sBufer[current_size], "%10.4f", I[k][h]);
            }
        }

        current_size += sprintf(&sBufer[current_size], "|");
        for ( int i = 0; i < NTC; i++ ) {
            current_size += sprintf(&sBufer[current_size], "%10.2f", TC[i]);
        }
        current_size += sprintf(&sBufer[current_size], "|\n");

        outFile = fopen((inCore->PATH[CORE::DebugDir] + "/input.txt").c_str(), "a");
        fprintf(outFile, "%s", sBufer);
        fclose(outFile);
        delete sBufer;
    }


}


void bufer::getPlantData(InputBuff* pDataBuff) {


    while(fPR == close)
    {
        usleep(10000);
    }

    fPR = close;

    struct tm tmTime;
    pDataBuff->Header.Time.Get(&tmTime);
    time_read = TIME_IMCORE(tmTime);

    for (unsigned int s = 0; s < NSUZ; s++) {
        //printf("SUZ Value = %g, Status = %d\n",pDataBuff->Data.Suz[s].fValue,pDataBuff->Data.Suz[s].cStatus);
        if ( pDataBuff->Data.Suz[s].cStatus == 0 ) {
             suz[s] = pDataBuff->Data.Suz[s].fValue * 3.56;
        } else {
            suz[s] = 356.0;
        }
    }

    for (unsigned int sg = 0; sg < NGSUZ; sg++) {

        if ( pDataBuff->Data.SuzG[sg].cStatus == 0 ) {
             gsuz[sg] = pDataBuff->Data.SuzG[sg].fValue * 3.56;
        } else {
            gsuz[sg] = 356.0;
        }
    }


    for(unsigned int n = 0 ; n < NTC; n++)
    {
        if ( pDataBuff->Data.Tp[n].cStatus == 0 ) {
             TC[n] = pDataBuff->Data.Tp[n].fValue;
        } else {
            TC[n] = 0;
        }
    }


    for (unsigned int k = 0; k < NKNI; k++)
    {
        //printf("Ied 2L Value = %g, Status = %d\n",pDataBuff->Data.Ied[k][2].fValue,pDataBuff->Data.Ied[k][2].cStatus);
        for (int h = 0; h < NLDPZ; h++ ) {
            if ( pDataBuff->Data.Ied[k][h].cStatus == 0 ) {
                 I[k][h] = pDataBuff->Data.Ied[k][h].fValue;
            } else {
                I[k][h] = 0;
            }
        }
    }

    npt = 0;
    for ( int i = 0; i < NPT; i++ ) {
        if ( pDataBuff->Data.Stpt[i].cStatus == 0 && pDataBuff->Data.Stpt[i].fValue == 0) {
             spt[i] = 1;
             npt += 1;
        } else {
            spt[i] = 0;
        }

        //printf("TXN  Value = %g, Status = %d\n",pDataBuff->Data.Thn[i].fValue,pDataBuff->Data.Thn[i].cStatus);
        if ( pDataBuff->Data.Thn[i].cStatus == 0 ) {
             txn[i] = pDataBuff->Data.Thn[i].fValue;
        } else {
            txn[i] = 0;
        }
    }

    //printf("FLOW   Value = %g, Status = %d\n",pDataBuff->Data.Yaf_s.fValue,pDataBuff->Data.Yaf_s.cStatus);
    //printf("POWER  Value = %g, Status = %d\n",pDataBuff->Data.Nakz.fValue,pDataBuff->Data.Nakz.cStatus);
    //printf("P_out  Value = %g, Status = %d\n",pDataBuff->Data.Pakz.fValue,pDataBuff->Data.Pakz.cStatus);
    flow = pDataBuff->Data.Yaf_s.cStatus ? 0 : pDataBuff->Data.Yaf_s.fValue * 1E09 / 3600.0;
    power = pDataBuff->Data.Nakz.cStatus ? 0 : pDataBuff->Data.Nakz.fValue;
    P_out = pDataBuff->Data.Pakz.cStatus ? 0 : pDataBuff->Data.Pakz.fValue / 9.81;

    //writeInputToFile(::pProg->pAkz->pCore1);

    fPR = new_info;

}
