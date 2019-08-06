#include <ImCorePMain.h>

short RefreshClients(const char *Suffix)
{
	short	RetStatus	= _STATUS_GREEN;
	int 	ClientsCode = ERR_UNIT_0, RetCode = ERR_UNIT_0, ItemCode = ERR_UNIT_0;
	// определим состояние потока сканирования
	CStr	ItemText;
	short	ItemStatus	= _STATUS_GREEN;
	int		ItemID		= CLIENTS_ID;
	int		ItemPos		= ::pProg->pDiagTbl->Find("ItemID", ItemID);
	if(!ItemPos) ItemPos = ::pProg->pDiagTbl->AddDiagItem(ItemID, PROG_ID, "", 0, IDI_THREAD_GREEN, IDI_THREAD_YELLOW, IDI_THREAD_RED, IDI_THREAD_GRAY,CStr(::pProg->pSettings->CompIdent) + Suffix);
	ItemText	= "Клиенты";
	::pProg->pDiagTbl->SetStr("Text", ItemPos, ItemText);

	long MainItemPos = ItemPos;		// сохраним для дальнейшей установки состояния

	short ClientsStatus = _STATUS_GREEN;
	if(::pProg->pClientLst->Lock())
	{
		LONG_LST	DiagnPosLst(10);
		int 		DiagnPosCnt = ::pProg->pDiagTbl->FillChildsPosLst(CLIENTS_ID, DiagnPosLst);
		for(int DiagnPos=DiagnPosCnt ; DiagnPos>=1 ; DiagnPos--)
		{
			int	DiagChildPos = DiagnPosLst.Get(DiagnPos);
			CPredictClient* pClient = (CPredictClient*)::pProg->pDiagTbl->Get("UserData", DiagChildPos);
			if(!pClient || !::pProg->pClientLst->Find((long)pClient))
			{
				::pProg->pDiagTbl->DelDiagItem(::pProg->pDiagTbl->Get("ItemID", DiagChildPos), CLIENTS_ID, DiagChildPos);
			}
		}

		int ClientCnt = ::pProg->pClientLst->GetCnt();
		for(int ClientPos=1 ; ClientPos<=ClientCnt ; ClientPos++)
		{
			CPredictClient* pClient = (CPredictClient*)::pProg->pClientLst->Get(ClientPos);
			ItemText	= CStr(pClient->ClientHost)+":"+pClient->ClientPort+". Поток обработки "+pClient->ThreadPid;
			ItemStatus	= _STATUS_GREEN;
			ItemCode	= ERR_UNIT_0;
			ItemID		= MAKE_CHILD_ID(CLIENTS_ID, ClientPos);
			ItemPos		= ::pProg->pDiagTbl->Find("UserData", (long)pClient);

			if(!ItemPos)
			{
				ItemPos = ::pProg->pDiagTbl->AddDiagItem(ItemID, CLIENTS_ID, "");
				::pProg->pDiagTbl->Set("UserData", ItemPos, (long)pClient);
			}

			if(!pClient->ThreadError.IsEmpty())
			{
				ItemText	+= CStr(". Ошибка : ") + pClient->ThreadError;
				ItemStatus	= _STATUS_RED;
				ItemCode 	= ERR_UNIT_3;
			}
			::pProg->pDiagTbl->UpdateDiagItem(ItemPos, ItemText, ItemStatus, 0);

			if(ClientsStatus < ItemStatus)
			{
				ClientsCode = ItemCode;
				ClientsStatus = ItemStatus;
			}
		}
		::pProg->pClientLst->Unlock();
	}
	if(RetStatus < ClientsStatus)
	{
		RetCode = ClientsCode;
		RetStatus = ClientsStatus;
	}

	return ::pProg->pDiagTbl->UpdateDiagItem(MainItemPos, NULL, RetStatus, 0, GetErrCode((long)MAIN_ID, RetCode, RetStatus));
}

void* DiagnosticThread(void* pStartThreadParams)
{// диагностический поток
#ifdef __sparc
	SetAbortProc(AbortGate);
#endif

	CStr	ErrorText;
	long	LastDiagnosticTime	= 0;
	long	LastTime			= 0;
	short	MainItemStatus		= _STATUS_GREEN;

	::pProg->SetFlag(FLG_STOP_DIAGNOSTIC_THREAD,	SV_READY);
	::pProg->SetFlag(FLG_RUN_DIAGNOSTIC,			SV_READY);

	::pidDiagnostic = ThreadPID();
	::pProg->AddEvent(IDI_EVENT_INFO, MakeError("Diagnostic", NULL, "Старт потока"));

	CProgWnd* pMainWindow = ::pProgApp ? (CProgWnd*)::pProgApp->getMainWindow() : NULL;

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

				short ClientsStatus = RefreshClients("");

				// установим результирующий статус сервера данных
				long	MainErrCode = ERR_UNIT_0;
				MainItemStatus = _STATUS_GREEN;
				if(MainItemStatus < ClientsStatus){ MainItemStatus = ClientsStatus; MainErrCode = ERR_UNIT_3; }

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

		pMainWindow = ::pProgApp ? (CProgWnd*)::pProgApp->getMainWindow() : NULL;
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
			//pMainWindow->AddCommand(CProgWnd::ID_UPDATE_VALUE_COL, NULL);
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
	SetAbortProc(AbortGateGcn);
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
