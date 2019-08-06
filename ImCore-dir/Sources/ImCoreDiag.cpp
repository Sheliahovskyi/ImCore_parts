#include <ImCoreMain.h>

short RefreshDiagSync(const char* pSuffix)
{
	long 	ItemCode = ERR_UNIT_0, RetCode = ERR_UNIT_0;
	short	RetStatus	= _STATUS_GREEN;
	// определим состояние потоков синхронизации
	CStr	ItemText;
	short	ItemStatus	= _STATUS_GREEN;
	long	ItemID		= SYNC_ID;
	long	ItemPos		= ::pProg->pDiagTbl->pItemIDCol->Find(ItemID);
	if(!ItemPos) ItemPos = ::pProg->pDiagTbl->AddDiagItem(ItemID, PROG_ID, "Синхронизация", 0, IDI_SYNC_GREEN, IDI_SYNC_YELLOW, IDI_SYNC_RED, IDI_SYNC_GRAY);
	ItemText = ::pProg->pSettings->CompIdent;	ItemText += pSuffix;
	::pProg->pDiagTbl->pShortNameCol->SetStr(ItemPos, ItemText);
	long MainItemPos 	= ItemPos;		// сохраним для дальнейшей установки состояния

	long NameMainLen	= ::pProg->pSettings->MainServer.GetLength();
	long NameReservLen	= ::pProg->pSettings->ReservServer.GetLength();
	// определим состояние первого дочернего элемента
	ItemText	= "Не указаны сервера данных";
	ItemStatus	= _STATUS_GREEN;
	ItemID		= MAKE_CHILD_ID(SYNC_ID, 1);
	ItemPos		= ::pProg->pDiagTbl->pItemIDCol->Find(ItemID);
	if(ItemPos && (NameMainLen||NameReservLen)){ ::pProg->pDiagTbl->DelDiagItem(ItemID, SYNC_ID); ItemPos = 0; }
	else if(!ItemPos && !NameMainLen && !NameReservLen) ItemPos = ::pProg->pDiagTbl->AddDiagItem(ItemID, SYNC_ID, "");
	if(ItemPos)
	{
		ItemStatus = _STATUS_YELLOW;
		ItemCode 	= ERR_UNIT_1;
	}
	::pProg->pDiagTbl->UpdateDiagItem(ItemPos, ItemText, ItemStatus, 0);
//	ItemText = ::pProg->pSettings->CompIdent;	ItemText += CStr("SY00");
//	::pProg->pDiagTbl->pShortNameCol->SetStr(ItemPos, ItemText);
	if(RetStatus < ItemStatus)
	{
		RetCode = ItemCode;
		RetStatus = ItemStatus;
	}

	if(::pProg->pSettings->bEnableScanData)
	{
		// определим состояние дочернего элемента
		ItemText	= "ИБД приема данных от сервера";
		ItemStatus	= _STATUS_GREEN;
		ItemCode 	= ERR_UNIT_0;
		ItemID		= MAKE_CHILD_ID(SYNC_ID, 2);
		ItemPos		= ::pProg->pDiagTbl->pItemIDCol->Find(ItemID);
		if(ItemPos && !NameMainLen && !NameReservLen){ ::pProg->pDiagTbl->DelDiagItem(ItemID, SYNC_ID); ItemPos = 0; }
		else if(!ItemPos && (NameMainLen||NameReservLen)) ItemPos = ::pProg->pDiagTbl->AddDiagItem(ItemID, SYNC_ID, "");
		::pProg->pDiagTbl->UpdateDiagItem(ItemPos, ItemText, ItemStatus, 0);
		ItemText = ::pProg->pSettings->CompIdent;	ItemText += CStr("SYINP");
		::pProg->pDiagTbl->pShortNameCol->SetStr(ItemPos, ItemText);
		int ItemIDScanIdb		= ItemID;
		int ItemPosScanIdb		= ItemPos;
		int ItemStatusScanIdb	= ItemStatus;

		// определим состояние дочернего элемента
		ItemText	= "";
		ItemStatus	= _STATUS_GREEN;
		ItemCode 	= ERR_UNIT_0;
		ItemID		= MAKE_CHILD_ID(ItemIDScanIdb, 1);
		ItemPos		= ::pProg->pDiagTbl->pItemIDCol->Find(ItemID);
		if(ItemPos && !NameMainLen && !NameReservLen){ ::pProg->pDiagTbl->DelDiagItem(ItemID, ItemIDScanIdb); ItemPos = 0; }
		else if(!ItemPos && (NameMainLen||NameReservLen)) ItemPos = ::pProg->pDiagTbl->AddDiagItem(ItemID, ItemIDScanIdb, "");
		if(ItemPos)
		{
			long SemValue = ::pProg->GetFlag(FLG_SYNC_SCAN_IDB);
			switch(SemValue)
			{
				case SV_PROCESS:
					ItemText = "Производится синхронизация ИБД";
					ItemCode 	= ERR_UNIT_4;
					break;
				case SV_READY:
					ItemText = CStr("Синхронизация ИБД с сервером ")+::pProg->pScanIdb->SyncServerName+" выполнена успешно";
					ItemCode 	= ERR_UNIT_0;
					break;
				case SV_ERROR:
					ItemText = CStr("Синхронизация ИБД не выполнена (")+::ErrorSyncScanIdb+")";
					ItemCode 	= ERR_UNIT_2;
					ItemStatus	= _STATUS_YELLOW;
					break;
				default:
					ItemText = "Синхронизация ИБД не выполнялась";
					ItemCode 	= ERR_UNIT_3;
					ItemStatus 	= _STATUS_YELLOW;
					break;
			}
			::pProg->pDiagTbl->UpdateDiagItem(ItemPos, ItemText, ItemStatus, 0);
	//		ItemText = ::pProgIvs->pSettings->CompIdent;	ItemText += CStr("SY00");
	//		::pProgIvs->pDiagTbl->pShortNameCol->SetStr(ItemPos, ItemText);
		}
		if(ItemStatusScanIdb < ItemStatus) ItemStatusScanIdb = ItemStatus;

		if(RetStatus < ItemStatusScanIdb)
		{
			RetCode = ItemCode;
			RetStatus = ItemStatus;
		}
		::pProg->pDiagTbl->UpdateDiagItem(ItemPosScanIdb, NULL, ItemStatusScanIdb, 0);
	}

	if(::pProg->pSettings->bEnableRegisterData)
	{
		// определим состояние дочернего элемента
		ItemText	= "ИБД регистрации данных на сервере";
		ItemStatus	= _STATUS_GREEN;
		ItemCode 	= ERR_UNIT_0;
		ItemID		= MAKE_CHILD_ID(SYNC_ID, 3);
		ItemPos		= ::pProg->pDiagTbl->pItemIDCol->Find(ItemID);
		if(ItemPos && !NameMainLen && !NameReservLen){ ::pProg->pDiagTbl->DelDiagItem(ItemID, SYNC_ID); ItemPos = 0; }
		else if(!ItemPos && (NameMainLen||NameReservLen)) ItemPos = ::pProg->pDiagTbl->AddDiagItem(ItemID, SYNC_ID, "");
		::pProg->pDiagTbl->UpdateDiagItem(ItemPos, ItemText, ItemStatus, 0);
		ItemText = ::pProg->pSettings->CompIdent;	ItemText += CStr("SYOUT");
		::pProg->pDiagTbl->pShortNameCol->SetStr(ItemPos, ItemText);
		int ItemIDRegIdb 	= ItemID;
		int ItemPosRegIdb	= ItemPos;
		int ItemStatusRegIdb	= ItemStatus;

		// определим состояние пятого дочернего элемента
		ItemText	= "";
		ItemStatus	= _STATUS_GREEN;
		ItemCode 	= ERR_UNIT_0;
		ItemID		= MAKE_CHILD_ID(ItemIDRegIdb, 1);
		ItemPos		= ::pProg->pDiagTbl->pItemIDCol->Find(ItemID);
		if(ItemPos && !NameMainLen && !NameReservLen){ ::pProg->pDiagTbl->DelDiagItem(ItemID, ItemIDRegIdb); ItemPos = 0; }
		else if(!ItemPos && (NameMainLen||NameReservLen)) ItemPos = ::pProg->pDiagTbl->AddDiagItem(ItemID, ItemIDRegIdb, "");
		if(ItemPos)
		{
			long SemValue = ::pProg->GetFlag(FLG_SYNC_REG_IDB);
			switch(SemValue)
			{
				case SV_PROCESS:
					ItemText = "Производится синхронизация ИБД";
					ItemCode 	= ERR_UNIT_4;
					break;
				case SV_READY:
					ItemText = CStr("Синхронизация ИБД с сервером ")+::pProg->pRegIdb->SyncServerName+" выполнена успешно";
					ItemCode 	= ERR_UNIT_0;
					break;
				case SV_ERROR:
					ItemText = CStr("Синхронизация ИБД не выполнена (")+::ErrorSyncRegIdb+")";
					ItemCode 	= ERR_UNIT_2;
					ItemStatus = _STATUS_YELLOW;
					break;
				default:
					ItemText = "Синхронизация ИБД не выполнялась";
					ItemCode 	= ERR_UNIT_3;
					ItemStatus = _STATUS_YELLOW;
					break;
			}
			::pProg->pDiagTbl->UpdateDiagItem(ItemPos, ItemText, ItemStatus, 0);
	//		ItemText = ::pProg->pSettings->CompIdent;	ItemText += CStr("SY00");
	//		::pProg->pDiagTbl->pShortNameCol->SetStr(ItemPos, ItemText);
		}
		if(ItemStatusRegIdb < ItemStatus) ItemStatusRegIdb = ItemStatus;

		if(RetStatus < ItemStatusRegIdb)
		{
			RetCode = ItemCode;
			RetStatus = ItemStatus;
		}
		::pProg->pDiagTbl->UpdateDiagItem(ItemPosRegIdb, NULL, ItemStatusRegIdb, 0);
	}

	ItemCode = GetErrCode( (long)SYNC_ID, RetCode, RetStatus);
	::pProg->pDiagTbl->pItemStatusCol->Set(MainItemPos, RetStatus);
	::pProg->pDiagTbl->pErrCodeCol->Set(MainItemPos, ItemCode);

	return RetStatus;
}
short RefreshDiagIdb(const char* pSuffix)
{
	short	RetStatus	= _STATUS_GREEN;
	// определим состояние информационной базы данных
	CStr	ItemText;
	short	ItemStatus	= _STATUS_GREEN;
	long 	ItemCode,RetCode=ERR_UNIT_0;
	long	ItemID		= INFO_DATA_BASE_ID;
	long	ItemPos		= ::pProg->pDiagTbl->pItemIDCol->Find(ItemID);

	if(!ItemPos)
	{
		ItemText	= "Информационная база данных";
		ItemPos = ::pProg->pDiagTbl->AddDiagItem(ItemID, PROG_ID, ItemText, 0, IDI_THREAD_GREEN, IDI_THREAD_YELLOW, IDI_THREAD_RED, IDI_THREAD_GRAY);
	}

	ItemText = ::pProg->pSettings->CompIdent;	ItemText += pSuffix;
	::pProg->pDiagTbl->pShortNameCol->SetStr(ItemPos, ItemText);

	long MainItemPos = ItemPos;		// сохраним для дальнейшей установки состояния

	if(::pProg->pSettings->bEnableScanData)
	{
		// определим состояние дочернего элемента
		ItemText	= "";
		ItemStatus	= _STATUS_GREEN;
		ItemCode 	= ERR_UNIT_1;
		ItemID		= MAKE_CHILD_ID(INFO_DATA_BASE_ID, 1);
		ItemPos		= ::pProg->pDiagTbl->pItemIDCol->Find(ItemID);
		if(!ItemPos) ItemPos = ::pProg->pDiagTbl->AddDiagItem(ItemID, INFO_DATA_BASE_ID, "");
		long FlgValue = ::pProg->GetFlag(FLG_INIT_SCAN_IDB);
		ItemText = "Прием данных от сервера. ";
		if(FlgValue == SV_READY)
		{
			ItemText   += CStr("Загружено ") + ::pProg->pScanIdb->pParamLst->GetCnt() + " переменных, отвергнуто " + ::pProg->pScanIdb->RejectParamCnt;
			ItemStatus	= _STATUS_GREEN;
			ItemCode 	= ERR_UNIT_0;
		}
		else
		{
			ItemText   += ::ErrorInitScanIdb;
			ItemStatus	= _STATUS_RED;
			ItemCode 	= ERR_UNIT_1;
		}
		::pProg->pDiagTbl->UpdateDiagItem(ItemPos, ItemText, ItemStatus, 0);
		ItemText = ::pProg->pSettings->CompIdent;	ItemText += CStr("IBINP");
		::pProg->pDiagTbl->pShortNameCol->SetStr(ItemPos, ItemText);
		if(RetStatus < ItemStatus)
		{
			RetCode = ItemCode;
			RetStatus = ItemStatus;
		}
	}

	if(::pProg->pSettings->bEnableRegisterData)
	{
		// определим состояние дочернего элемента
		ItemText	= "";
		ItemStatus	= _STATUS_GREEN;
		ItemCode 	= ERR_UNIT_1;
		ItemID		= MAKE_CHILD_ID(INFO_DATA_BASE_ID, 2);
		ItemPos		= ::pProg->pDiagTbl->pItemIDCol->Find(ItemID);
		if(!ItemPos) ItemPos = ::pProg->pDiagTbl->AddDiagItem(ItemID, INFO_DATA_BASE_ID, "");
		long FlgValue = ::pProg->GetFlag(FLG_INIT_REG_IDB);
		ItemText = "Регистрация данных в сервере. ";
		if(FlgValue == SV_READY)
		{
			ItemText   += CStr("Загружено ") + ::pProg->pRegIdb->pParamLst->GetCnt() + " переменных, отвергнуто " + ::pProg->pRegIdb->RejectParamCnt;
			ItemStatus	= _STATUS_GREEN;
			ItemCode 	= ERR_UNIT_0;
		}
		else
		{
			ItemText   += ::ErrorInitRegIdb;
			ItemStatus	= _STATUS_RED;
			ItemCode 	= ERR_UNIT_2;
		}
		::pProg->pDiagTbl->UpdateDiagItem(ItemPos, ItemText, ItemStatus, 0);
		ItemText = ::pProg->pSettings->CompIdent;	ItemText += CStr("IBOUT");
		::pProg->pDiagTbl->pShortNameCol->SetStr(ItemPos, ItemText);
		if(RetStatus < ItemStatus)
		{
			RetCode = ItemCode;
			RetStatus = ItemStatus;
		}
	}

	ItemCode = GetErrCode( (long)INFO_DATA_BASE_ID, RetCode, RetStatus);
	::pProg->pDiagTbl->pItemStatusCol->Set(MainItemPos, RetStatus);
	::pProg->pDiagTbl->pErrCodeCol->Set(MainItemPos, ItemCode);

	return RetStatus;
}
short RefreshDiagRegister(const char *Suffix)
{
	short			RetStatus = _STATUS_GREEN, ItemStatus = _STATUS_GREEN;
	long 			ItemCode  = ERR_UNIT_0, RetCode	= ERR_UNIT_0;
	CDiagTbl*		pDiagTbl 		= ::pProg->pDiagTbl;
	long 			MainItemPos 	= 0;
	long			ItemID			= 0;
	long			ItemPos			= 0;
	CStr			ItemText;
		
	ItemPos = pDiagTbl->pItemIDCol->Find(REGISTER_ID);
	ItemText = "Регистрация на сервере ОБДРВ";
	if(!::pProg->pSettings->bEnableRegisterData)
	{
		RetStatus	= _STATUS_YELLOW;
		RetCode 	= ERR_UNIT_1;
		ItemText += CStr(" (") + "отключена" + ")";
	}

	if(!ItemPos) ItemPos = 	pDiagTbl->AddDiagItem(REGISTER_ID, PROG_ID, (char*)ItemText, ItemStatus, IDI_THREAD_GREEN,IDI_THREAD_YELLOW, IDI_THREAD_RED, IDI_THREAD_GRAY,CStr(::pProg->pSettings->CompIdent) + Suffix);
	else 					pDiagTbl->UpdateDiagItem(ItemPos, (char*)ItemText, ItemStatus, 0);

	MainItemPos = ItemPos;

	int SrvCnt = ::pProg->pRegisterServersLst->GetCnt();

//printf("SrvCnt=%d, ParamCnt=%d\n", SrvCnt, ::pProg->pRegIdb->pParamLst->GetCnt());
	if(!SrvCnt || !::pProg->pSettings->bEnableRegisterData || !::pProg->pRegIdb->pParamLst->GetCnt())
	{
		ItemID		= MAKE_CHILD_ID(REGISTER_ID, 1);
		ItemPos		= pDiagTbl->pItemIDCol->Find(ItemID);
		ItemText	= "";
		ItemStatus	= _STATUS_YELLOW;
		ItemCode	= ERR_UNIT_2;
		if(!::pProg->pSettings->bEnableRegisterData && ItemPos) pDiagTbl->DelDiagItem(ItemID, REGISTER_ID);
		else
		{
			ItemText = !SrvCnt ? "Не указаны сервера" : "Нет переменных для регистрации";
			pDiagTbl->UpdateDiagItem(ItemPos, (char*)ItemText, ItemStatus, 0);
		}
	}
	else
	{
		for(int SrvPos=1 ; SrvPos<=SrvCnt ; SrvPos++)
		{
			CRegisterServer* pServer = (CRegisterServer*)::pProg->pRegisterServersLst->Get(SrvPos);
			int SrvItemID	= MAKE_CHILD_ID(REGISTER_ID, SrvPos);
			int SrvDiagPos	= pDiagTbl->pItemIDCol->Find(SrvItemID);
			int SrvStatus	= _STATUS_GREEN;
			if(!SrvDiagPos) SrvDiagPos = pDiagTbl->AddDiagItem(SrvItemID, REGISTER_ID, "");
			ItemText = CStr("Сервер ") + pServer->ServerName;
			if(pServer->RegThreadPid) ItemText += CStr(" (") + pServer->RegThreadPid + ")";
			pDiagTbl->UpdateDiagItem(SrvDiagPos, ItemText, SrvStatus, 0);

			int SrvChildID		= MAKE_CHILD_ID(SrvItemID, 1);
			int SrvChildPos		= pDiagTbl->pItemIDCol->Find(SrvChildID);
			int SrvChildStatus	= _STATUS_GREEN;
			if(!SrvChildPos) SrvChildPos = pDiagTbl->AddDiagItem(SrvChildID, SrvItemID, "", SrvChildStatus);

			if(pServer->RegThreadPid && !pServer->Error.GetLength())
			{
				ItemText		= CStr("Последний пакет : ") + TimeToStr(&pServer->LastRegTime) + ". Очередь :" + pServer->pRegisterPackLst->ItemCnt;
				SrvChildStatus	= _STATUS_GREEN;
			}
			else if(pServer->Error.GetLength())
			{
				ItemText		= pServer->Error;
				SrvChildStatus	= _STATUS_RED;
				ItemCode 		= ERR_UNIT_3;
			}
			pDiagTbl->UpdateDiagItem(SrvChildPos, (char*)ItemText, SrvChildStatus, 0);

			if(SrvStatus < SrvChildStatus)
			{
				RetCode		= ItemCode;
				SrvStatus	= SrvChildStatus;
			}
			::pProg->pDiagTbl->UpdateDiagItem(SrvDiagPos, NULL, SrvStatus, 0);

			if(RetStatus < SrvStatus)
			{
				RetStatus	= SrvStatus;
				RetCode		= ItemCode;
			}
		}
	}
	pDiagTbl->UpdateDiagItem(MainItemPos, NULL, RetStatus,0,GetErrCode((long)REGISTER_ID, RetCode, RetStatus));

	return RetStatus;
}
short RefreshDiagScan(const char *Suffix)
{
	short	RetStatus	= _STATUS_GREEN;
	long 	ItemCode= ERR_UNIT_0,RetCode = ERR_UNIT_0;
	// определим состояние потока сканирования
	CStr	ItemText;
	short	ItemStatus	= _STATUS_GREEN;
	long	ItemID		= SCAN_ID;
	long	ItemPos		= ::pProg->pDiagTbl->Find("ItemID", ItemID);
	if(!ItemPos) ItemPos = ::pProg->pDiagTbl->AddDiagItem(ItemID, PROG_ID, "", 0, IDI_THREAD_GREEN, IDI_THREAD_YELLOW, IDI_THREAD_RED, IDI_THREAD_GRAY,CStr(::pProg->pSettings->CompIdent) + Suffix);
	ItemText	= "Прием данных";
	if(::pidScanData) ItemText += CStr(" (") + (int)::pidScanData + ")";
	if(!::pProg->pSettings->bEnableScanData)
	{
//		RetStatus	= _STATUS_YELLOW;
		RetCode 	= ERR_UNIT_1;
		ItemText += CStr(" (") + "отключен" + ")";
	}
	::pProg->pDiagTbl->SetStr("Text", ItemPos, ItemText);

	long MainItemPos = ItemPos;		// сохраним для дальнейшей установки состояния

	// определим состояние первого дочернего элемента
	ItemText	= "";
	ItemStatus	= _STATUS_GREEN;
	ItemCode 	= ERR_UNIT_0;
	ItemID		= MAKE_CHILD_ID(SCAN_ID, 1);
	ItemPos		= ::pProg->pDiagTbl->Find("ItemID", ItemID);
	if(!ItemPos) ItemPos = ::pProg->pDiagTbl->AddDiagItem(ItemID, SCAN_ID, "");
	long SemValue = ::pProg->GetFlag(FLG_START_SCAN_THREAD);
	if(!::pProg->pSettings->bEnableScanData)
	{
		ItemText	= "Прием данных отключен";
		ItemStatus	= _STATUS_YELLOW;
		ItemCode 	= ERR_UNIT_1;
	}
	else if(SemValue == SV_PROCESS && ::pProg->ScanRequestID && !::ErrorScanData.GetLength())
	{
		ItemText	= CStr("Принимаются данные от сервера ") + pProg->CurScanServerName;
		ItemStatus	= _STATUS_GREEN;
		ItemCode 	= ERR_UNIT_0;
	}
	else if(::ErrorScanData.GetLength())
	{
		ItemText	= ::ErrorScanData;
		ItemStatus	= _STATUS_RED;
		ItemCode 	= ERR_UNIT_3;
	}
	else
	{
		ItemText	= "В ИБД обсутствуют параметры";
		ItemStatus	= _STATUS_YELLOW;
		ItemCode 	= ERR_UNIT_2;
	}
	::pProg->pDiagTbl->UpdateDiagItem(ItemPos, ItemText, ItemStatus, 0);

	if(RetStatus < ItemStatus)
	{
		RetCode = ItemCode;
		RetStatus = ItemStatus;
	}

	// определим состояние второго дочернего элемента
	ItemText	= "";
	ItemStatus	= _STATUS_GREEN;
	ItemID		= MAKE_CHILD_ID(SCAN_ID, 2);
	ItemPos		= ::pProg->pDiagTbl->Find("ItemID", ItemID);
	if(ItemPos && (SemValue!=SV_PROCESS || ::pProg->CurScanLayerTime.IsNull() || ::ErrorScanData.GetLength())){ ::pProg->pDiagTbl->DelDiagItem(ItemID, SCAN_ID); ItemPos = 0; }
	else if(!ItemPos && !::pProg->CurScanLayerTime.IsNull() && !::ErrorScanData.GetLength()) ItemPos = ::pProg->pDiagTbl->AddDiagItem(ItemID, SCAN_ID, "");
	if(ItemPos)
	{
		ItemText = CStr("Время последнего пакета ") + TimeToStr(&::pProg->CurScanLayerTime, MTD_TIMESTAMP);
		::pProg->pDiagTbl->SetStr("Text", ItemPos, ItemText);
		::pProg->pDiagTbl->UpdateDiagItem(ItemPos, ItemText, ItemStatus, 0);
	}

	return ::pProg->pDiagTbl->UpdateDiagItem(MainItemPos, NULL, RetStatus,0,GetErrCode((long)SCAN_ID, RetCode, RetStatus));
}
void* DiagnosticThread(void* pStartThreadParams)
{// диагностический поток
#ifdef __sparc
	SetAbortProc(AbortProg);
#endif

	CStr	ErrorText;
	long	LastDiagnosticTime	= 0;
	long	LastTime			= 0;
	short	MainItemStatus		= _STATUS_GREEN;

	::pProg->SetFlag(FLG_STOP_DIAGNOSTIC_THREAD,	SV_READY);
	::pProg->SetFlag(FLG_RUN_DIAGNOSTIC,			SV_READY);

	::pidDiagnostic = ThreadPID();
	::pProg->AddEvent(IDI_EVENT_INFO, MakeError("Diagnostic", NULL, "Старт потока"));

	CBaseProgWnd* pMainWindow = ::pProgApp ? (CBaseProgWnd*)::pProgApp->getMainWindow() : NULL;

	while(true)
	{
		if(	::pProg->GetFlag(FLG_STOP_DIAGNOSTIC_THREAD) != SV_NEEDED &&
			::pProg->GetFlag(FLG_RUN_DIAGNOSTIC)  == SV_READY  &&
			CrTime()-LastDiagnosticTime>=(long)::pProg->pSettings->DiagInterval)
		{// если надо провести диагностику
			::pProg->SetFlag(FLG_RUN_DIAGNOSTIC, SV_PROCESS);

			if(::pProg->pDiagTbl->Lock())
			{
				long MainItemPos = ::pProg->pDiagTbl->pItemIDCol->Find(PROG_ID);
				if(!MainItemPos)
				{// добавляется первый элемент
					MainItemPos = ::pProg->pDiagTbl->AddDiagItem(PROG_ID, MAIN_ID, CStr(::pProg->pSettings->ModuleName)+". Версия "+MakeVersionStr(), _STATUS_GREEN, IDI_SRV_GREEN, IDI_SRV_YELLOW, IDI_SRV_RED, IDI_SRV_GRAY,CStr(::pProg->pSettings->CompIdent));
				}
				//::pProg->pDiagTbl->SetStr("ShortName", MainItemPos, ::pProg->pSettings->CompIdent);

				short SyncStatus	= RefreshDiagSync("SY");
				short IdbStatus		= RefreshDiagIdb("IB");
				short RegStatus		= ::pProg->GetFlag(FLG_INIT_REG_IDB)==SV_READY ? RefreshDiagRegister("SM") : _STATUS_GREEN;
				short ScanStatus 	= ::pProg->GetFlag(FLG_INIT_SCAN_IDB)==SV_READY ? RefreshDiagScan("RP") : _STATUS_GREEN;

				// установим результирующий статус сервера данных
				long	MainErrCode = ERR_UNIT_0;
				MainItemStatus = _STATUS_GREEN;
				if(MainItemStatus < SyncStatus)			{ MainItemStatus = SyncStatus; MainErrCode = ERR_UNIT_2;}
				if(MainItemStatus < IdbStatus)			{ MainItemStatus = IdbStatus; MainErrCode = ERR_UNIT_1;}
				if(MainItemStatus < RegStatus)			{ MainItemStatus = RegStatus; MainErrCode = ERR_UNIT_4;}
				if(MainItemStatus < ScanStatus)			{ MainItemStatus = ScanStatus; MainErrCode = ERR_UNIT_3;}

				::pProg->pDiagTbl->UpdateDiagItem(MainItemPos, NULL, MainItemStatus,0,GetErrCode((long)PROG_ID, MainErrCode, MainItemStatus));
				::pProg->pDiagTbl->Unlock();
			}
			LastDiagnosticTime = CrTime();
			::pProg->SetFlag(FLG_RUN_DIAGNOSTIC, SV_READY);
		}
		else if(::pProg->GetFlag(FLG_STOP_DIAGNOSTIC_THREAD) == SV_NEEDED)
		{// надо выгружаться
			break;
		}
		else
		{// заснем немного
			ThreadYield();
		}

		pMainWindow = ::pProgApp ? (CBaseProgWnd*)::pProgApp->getMainWindow() : NULL;
		if( pMainWindow && pMainWindow->bLiveWnd==true && pMainWindow->pDiagTabItem && CrTime()-LastTime>=1)
		{// раз в секунду обновим иконку в лепестке
			FXIcon* pOldIcon = pMainWindow->pDiagTabItem->getIcon();
			FXIcon* pNewIcon = NULL;
			switch(MainItemStatus)
			{
				case _STATUS_RED:
					{
						pNewIcon = pOldIcon==::GetIcon(IDI_DIAG_RED_1) ? ::GetIcon(IDI_DIAG_RED_2) : ::GetIcon(IDI_DIAG_RED_1);
					}
					break;
				case _STATUS_YELLOW:
					{
						pNewIcon = pOldIcon==::GetIcon(IDI_DIAG_YELLOW_1) ? ::GetIcon(IDI_DIAG_YELLOW_2) : ::GetIcon(IDI_DIAG_YELLOW_1);
					}
					break;
				default:
					{
						pNewIcon = pOldIcon==::GetIcon(IDI_DIAG_GREEN_1) ? ::GetIcon(IDI_DIAG_GREEN_2) : ::GetIcon(IDI_DIAG_GREEN_1);
					}
					break;
			}
			pMainWindow->AddCommand(CProgWnd::ID_DIAG_UPDATE, pNewIcon);
			pMainWindow->AddCommand(CProgWnd::ID_UPDATE_VALUE_COL, NULL);
			pMainWindow->AddCommand(CMainWindow::ID_SYNC_DIAGNOSTIC, NULL);
			LastTime = CrTime();
		}
	}
	::pProg->AddEvent(IDI_EVENT_INFO, MakeError("Diagnostic", NULL, "Останов потока"));
	::pidDiagnostic = 0;

	::pProg->SetFlag(FLG_STOP_DIAGNOSTIC_THREAD,	SV_READY);

	ThreadExit(0);	

	return NULL;
}

void* DoutThread(void* pStartThreadParams)
{
#ifdef __sparc
	SetAbortProc(AbortProgGcn);
#endif

	::pProg->SetFlag(FLG_STOP_DOUT_THREAD, SV_READY);
	::pProg->SetFlag(FLG_RUN_DOUT_THREAD, SV_READY);

	::pidDout = ThreadPID();
	::pProg->AddEvent(IDI_EVENT_INFO, MakeError("Dout", NULL, "Старт потока"));

	while(true) 
	{
		if(	::pProg->GetFlag(FLG_STOP_DOUT_THREAD) != SV_NEEDED && ::pProg->GetFlag(FLG_RUN_DOUT_THREAD) == SV_READY )
		{// если надо передать диагностику
			::pProg->SetFlag(FLG_RUN_DOUT_THREAD, SV_PROCESS);

			bool beg_write=false; 
			if(::pProg->pDiagnGate->ConfDiagnClient())
			{      
				int	cur_takt=0, num_takt = 50;
				while (((beg_write=::pProg->pDiagnGate->IsBegDiagnWrite(::pProg->pSettings->CompIdent))==false) && ( ++cur_takt < num_takt )) CrSleep(200);

				if(beg_write && ::pProg->pDiagTbl->Lock() )
				{
					if(::pProg->pDiagnGate->DiagnWrite(::pProg->pDiagTbl,::pProg->pProtocol))
						::pProg->pDiagnGate->EndDiagnWrite(::pProg->pSettings->CompIdent);
					::pProg->pDiagTbl->Unlock();
				}
			}

			::pProg->SetFlag(FLG_RUN_DOUT_THREAD, SV_READY);
			ThreadYield();
		}
		else if(::pProg->GetFlag(FLG_STOP_DOUT_THREAD) == SV_NEEDED)
		{// надо выгружаться
			break;
		}
		else ThreadYield();
	}	

	::pProg->AddEvent(IDI_EVENT_INFO, MakeError("Dout", NULL, "Останов потока"));
	::pidDout = 0;

	::pProg->SetFlag(FLG_STOP_DOUT_THREAD, SV_READY);

	ThreadExit(0);

	return NULL;
}
