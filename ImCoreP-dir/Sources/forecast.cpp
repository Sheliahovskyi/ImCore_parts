#include "forecast.h"

FORECAST::FORECAST(char * HOMEDIR) {
    pCORE            = new CORE;
    pXS              = new XS;
    pCORE->xs_data  = pXS;
    pCORE->HOMEDIR = HOMEDIR;
    pCORE->start();

    pARCH            = new ARCHIVE(pCORE);
}

FORECAST::~FORECAST() {
    delete pCORE;
    delete pXS;
    delete pARCH;
}

/*!
 \brief CORE::forecast ������� ������ ����������� ������ � ����������� ����������
*/
void FORECAST::Forecast() {/*
    while (true)
    {
        if(pProg->GetFlag(FLG_FC_STATUS) == SV_READY)
        {
            pProg->SetFlag(FLG_FC_STATUS, SV_PROCESS);
            for(int n = 0; n < Etaps_number; n++)
            {
                if(forecast_list[n].status)
                {
                    zero_etap_param_setting(&forecast_list[n]);
                    break;
                }
            }


            for(int n = 0; n < Etaps_number; n++)
            {
                delta_per_step_calc(&forecast_list[n+1]);


            }

        }
        else if (pProg->GetFlag(FLG_FC_STATUS) == SV_DISABLE)
        {
            pProg->SetFlag(FLG_FC_STATUS, SV_NEEDED);
        }

        ThreadYield();
    }
*/}

void FORECAST::ReadForecastInfo( CPredictRequestHeader* pHeader, LST* pEtapLst ) {
    // ������� ��������� �����
    struct tm StartTimeTM;
    pHeader->StartRequestTime.Get( &StartTimeTM );
    TIME_IMCORE StartTime = TIME_IMCORE( StartTimeTM );

    StartDate = StartTime.givedate();
    StartTime1970 = StartTime.give_sec_from_1970();

    EtapsNumber = pHeader->EtapCounter;
    FC_Type = pHeader->RequestType;
    FC_Name = pHeader->RequestName;

    FC_List.clear();

    for(int EtapPos=1 ; EtapPos<=pEtapLst->GetCnt() ; EtapPos++)
    {
        EtapData* pEtapData = ( EtapData* )pEtapLst->Get( EtapPos );
        struct tm EtapDurationTM;
        pEtapData->EtapDuration.Get(&EtapDurationTM);

        FC_List.push_back(*pEtapData);
    }
}

void FORECAST::ReadStartCoreState( CoreState* core ) {
    pCORE->TEFF = core->Teff;
    pCORE->CB   = core->CB;
    pCORE->P_out = core->Pout;
    pCORE->P_drop = core->Pdrop;
    pCORE->set_pressure();
    pCORE->WT_in = core->Tin;
    for ( unsigned int n = 0; n < pCORE->NFA; n++ ) {
        pCORE->FA[n].WT_in = core->Tin;
    }

    for( unsigned int h = 0; h < pCORE->NL; h++ ) {
        for ( unsigned int n=0; n<pCORE->NFA;n++ ) {
            pCORE->FA[n].PRI[h].type        = core->CoreMap[n][h];
            pCORE->FA[n].PRI[h].height      = core->PrismHight[n][h];
            pCORE->FA[n].PRI[h].SL          = core->SL[n][h];
            pCORE->FA[n].PRI[h].SM          = core->SM[n][h];
            pCORE->FA[n].PRI[h].PM          = core->PM[n][h];
            pCORE->FA[n].PRI[h].XE          = core->XE[n][h];
            pCORE->FA[n].PRI[h].I           = core->I[n][h];
            for ( unsigned int i = 0; i < pCORE->NPIN; i++ ) {
                pCORE->FA[n].PRI[h].PinSl[i] = core->PinSL[n][h][i];
            }
        }
    }

    for( unsigned int h = 0; h < pCORE->NKNI; h++ ) {
        for ( unsigned int n=0; n<pCORE->NLDPZ;n++ ) {
            pCORE->kni[n].ZE[h] = core->ZE[n][h];
        }
    }

    for ( unsigned int n = 0; n < pCORE->NSUZ; n++ ) {
        pCORE->suz.RODS[n].position = core->HSuz[n];
    }
}

void FORECAST::ZeroEtapParamSetting( EtapData* InitState ) {
    int NRCP = 0;

    fCrit           = InitState->iCritParam;
    fAOStrategy     = InitState->iAOStrategy;
    fSUZStrategy    = InitState->iSZStrategy;
    fInterception   = InitState->iSZInterception;

    CurrentEtapNumber = InitState->EtapNumber;

    struct tm StartTimeTM;
    InitState->EtapDuration.Get( &StartTimeTM );
    TIME_IMCORE EtapDuration = TIME_IMCORE( StartTimeTM );

    EtapStatus = InitState->status;

    pCORE->HEAT_POWER = InitState->iPOWER;
    pCORE->CB = InitState->iCB;
    pCORE->WT_in = InitState->iTIN;
    for ( unsigned int n = 0; n < pCORE->NFA; n++ ) {
        pCORE->FA[n].WT_in = InitState->iTIN;
    }
    pCORE->G = InitState->iGV1K * pCORE->xs_data->wd_calc( InitState->iTIN, (InitState->iP1K - pCORE->P_drop) );
    pCORE->P_out = InitState->iP1K;
    pCORE->set_pressure();
    NRCP += InitState->iSGCN1;
    NRCP += InitState->iSGCN2;
    NRCP += InitState->iSGCN3;
    NRCP += InitState->iSGCN4;
    pCORE->npt = NRCP;


    for ( unsigned int n = 0; n < pCORE->NGSUZ; n++ ) {
        pCORE->suz.move_group(10, InitState->iH10);
        pCORE->suz.move_group(9,  InitState->iH9);
        pCORE->suz.move_group(8,  InitState->iH8);
        pCORE->suz.move_group(7,  InitState->iH7);
        pCORE->suz.move_group(6,  InitState->iH6);
        pCORE->suz.move_group(5,  InitState->iH5);
        pCORE->suz.move_group(4,  InitState->iH4);
        pCORE->suz.move_group(3,  InitState->iH3);
        pCORE->suz.move_group(2,  InitState->iH2);
        pCORE->suz.move_group(1,  InitState->iH1);
    }





}

void FORECAST::SetStateParam( EtapData* CoreState ) {

}

void FORECAST::StateParamCalc( EtapData* LastState ) {

}

void FORECAST::WriteStateResult( ResultData* answer ) {

}
