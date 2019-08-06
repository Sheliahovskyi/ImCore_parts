#include <ImCoreMain.h>

void FlushDataBuff(InputBuff* pBuff)
{
	FILE* pFile = fopen("/home/prog/Svrk/test/test", "at");
	if(pFile)
	{
		struct tm tmTime; pBuff->Header.Time.Get(&tmTime);

		char Header[20]; sprintf(Header, "%04i.%02i.%02i_%02i:%02i:%02i",
				tmTime.tm_year+1900, tmTime.tm_mon+1, tmTime.tm_mday,
				tmTime.tm_hour, tmTime.tm_min, tmTime.tm_sec);

		fprintf(pFile, "%s\n", Header);
		fprintf(pFile, "Suz[0]=%g , %d\n", pBuff->Data.Suz[0].fValue, pBuff->Data.Suz[0].cStatus);
		fprintf(pFile, "Tp[0]=%g , %d\n", pBuff->Data.Tp[0].fValue, pBuff->Data.Tp[0].cStatus);
		fprintf(pFile, "Ied[0]=%g , %d\n", pBuff->Data.Ied[0][0].fValue, pBuff->Data.Ied[0][0].cStatus);

		fclose(pFile);
	}
}

/////////////////////////////////////////////////////////////////////
// поток разбора+расчёта принятых данных
short CProg::PrepareDataThread(const char* pThreadName, CStr* pError)
{
	CStr	Error, PrepareFile;
	short	Result = 1;
	bool	bStartCalcTEFF = true;

//	double	dLastPrepareDataTime = 0.0;

	ErrorPrepareData.Empty();
	
	while(Result > 0)
	{// основной цикл потока

		if( GetFlag(FLG_STOP_PREPARE_DATA_THREAD)!=SV_NEEDED &&
			( GetFlag(FLG_CHANGE_SCAN_DATA)==SV_NEEDED		||
			  GetFlag(FLG_CHANGE_REBUILD_DATA)==SV_NEEDED	||
			  GetFlag(FLG_CHANGE_DIF_DATA)==SV_NEEDED) )
		{
			SetFlag(FLG_RUN_PREPARE_DATA_THREAD, SV_PROCESS);

/*
CTime Time = GetCurrentTime();
puts(CStr() + &Time + " CProg::PrepareDataThread");
puts(CStr("    ") + " SCAN_DATA=" + (int)GetFlag(FLG_CHANGE_SCAN_DATA));
puts(CStr("    ") + " BCN_TEXT_FILE=" + (int)GetFlag(FLG_CHANGE_BCN_TEXT_FILE));
puts(CStr("    ") + " BCN_MONITOR_FILE=" + (int)GetFlag(FLG_CHANGE_BCN_MONITOR_FILE));
puts(CStr("    ") + " BCN_UPDATER_FILE=" + (int)GetFlag(FLG_CHANGE_BCN_UPDATER_FILE));
*/

//puts("start CALC ");
			if( GetFlag(FLG_CHANGE_SCAN_DATA)==SV_NEEDED )
			{
//puts("FLG_CHANGE_SCAN_DATA==SV_NEEDED");
				if(bStartCalcTEFF)
				{
				    if(pCalc->bStartAutoCalc) pCalc->CalcStartTEFF();
				    bStartCalcTEFF = false;
				}
				pCalc->Calculation(FLG_CHANGE_SCAN_DATA);
				PrepareScanData();
			}
			
			if(bStartCalcTEFF)
			{
				CrSleep(1000000);
				continue;
			}

			if(GetFlag(FLG_CHANGE_REBUILD_DATA) == SV_NEEDED)
			{//
//puts("\nFLG_CHANGE_REBUILD_DATA==SV_NEEDED");
				pCalc->Calculation(FLG_CHANGE_REBUILD_DATA);
				::pProg->SetFlag(FLG_CHANGE_REBUILD_DATA, SV_READY);
			}

			if(GetFlag(FLG_CHANGE_DIF_DATA) == SV_NEEDED)
			{//
//puts("\nFLG_CHANGE_DIF_DATA==SV_NEEDED");
				pCalc->Calculation(FLG_CHANGE_DIF_DATA);
				::pProg->SetFlag(FLG_CHANGE_DIF_DATA,  SV_READY);
			}

			// формируем пакет на регистрацию в сервер
			int			PackageSizeChan	= sizeof(CTime) + sizeof(int)*3;
			int			ParamCnt		= pRegIdb->pParamLst->GetCnt();
			LONG_LST	ChangesParamLstChan(ParamCnt);
			for(int ParamPos=1 ; ParamPos<=ParamCnt ; ParamPos++)
			{// отбор изменившихся переменных для регистрации
				CRegParamInfo* pParamInfo = (CRegParamInfo*)pRegIdb->pParamLst->Get(ParamPos);
				if(pParamInfo->ChangeFlag)
				{
					pParamInfo->ChangeFlag = 0;
					ChangesParamLstChan.Add((long)pParamInfo);
					PackageSizeChan += sizeof(int) + pParamInfo->ValueByteSize + sizeof(char);
				}
			}

			CTime CurTime = GetCurrentTime();


//printf("---------------- start register, time=%s\n", (char*)TimeToStr(&CurTime));


			int	ChangesParamCntChan = ChangesParamLstChan.GetCnt();
			if(ChangesParamCntChan)
			{// отбор на регистрация
				unsigned char* pRegisterPackage = MakeRegisterPackage(PackageSizeChan, 0);
				for(int ChangesParamPos=1 ; ChangesParamPos<=ChangesParamCntChan ; ChangesParamPos++)
				{
					CRegParamInfo* pParamInfo = (CRegParamInfo*)ChangesParamLstChan.Get(ChangesParamPos);
					AppendRegisterPackage(pRegisterPackage, pParamInfo, pParamInfo->Value);
				}
				SendRegisterPackage(CurTime, pRegisterPackage, PackageSizeChan);
			}

//printf("---------------- end register, time=%s\n", (char*)TimeToStr(&CurTime));

			SetFlag(FLG_RUN_PREPARE_DATA_THREAD, SV_READY);
		}
		else if(GetFlag(FLG_STOP_PREPARE_DATA_THREAD) == SV_NEEDED)
		{// надо выгружаться
			break;
		}
		else
		{// заснем немного если не надо регистрировать данные
			ThreadYield();
		}
	}// конец основного цикла потока

	if(Result<=0 && Error.GetLength())
	{
		if(pError) *pError = Error;
		else puts((char*)Error);
	}

	return Result;
}
void CProg::PrepareScanData()
{
	InputBuff		Buff;	MemSet(&Buff, 0, sizeof(Buff));
	InputHeader*	pHeader	= &Buff.Header;
	InputData*		pData	= &Buff.Data;

	// заполним заголовок
	pHeader->Time = CurScanLayerTime;

	// заполним данные
	if(pScanIdb && pScanIdb->Lock())
	{
		int ParamCnt = ::pProg->pScanIdb->pParamInfoLst->GetCnt();
		for(int ParamPos=1 ; ParamPos<=ParamCnt ; ParamPos++)
		{
			CScanParamInfo* pParamInfo = (CScanParamInfo*)pScanIdb->pParamInfoLst->Get(ParamPos);
			if(pParamInfo->DataOffset >= 0)
			{
				Val* pVal = (Val*)(((unsigned char*)pData) + pParamInfo->DataOffset);
				pVal->fValue	= pParamInfo->GetValue();
				pVal->cStatus	= pParamInfo->GetStatus();
			}
		}
		pScanIdb->Unlock();
	}
	if(pRegIdb && pRegIdb->Lock())
	{
		int ParamCnt = pRegIdb->pParamInfoLst->GetCnt();
		for(int ParamPos=1 ; ParamPos<=ParamCnt ; ParamPos++)
		{
			CRegParamInfo* pParamInfo = (CRegParamInfo*)pRegIdb->pParamInfoLst->Get(ParamPos);
			if(pParamInfo->DataOffset >= 0)
			{
				Val* pVal = (Val*)(((unsigned char*)pData) + pParamInfo->DataOffset);
				pVal->fValue	= pParamInfo->GetValue();
				pVal->cStatus	= pParamInfo->GetStatus();
			}
		}
		pRegIdb->Unlock();
	}

    ::pProg->pAkz->getScanData(&Buff);
    if(::pProg->GetFlag(FLG_INIT_CORE_THREAD_DATA)==SV_READY) {
        ::pProg->pAkz->recovery();
    }

}

