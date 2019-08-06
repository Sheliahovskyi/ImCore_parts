#include <ImCoreMain.h>
#include <X11/X.h>
#include <X11/Xlib.h>

char* MakeVersionStr(void)
{
	static char Version[120]; sprintf(Version, "%s.1 ������ %s, ���� %s", CrMakeVersion(), MAKE_DATE, MAKE_KERNEL); return Version;
}

CProg*		pProg			= NULL;
CProgApp*	pProgApp		= NULL;
bool		bAbortProgram	= false;

// ���������� �������������� �������  
int		pidSyncRegIdb	= 0;
int		pidInitRegIdb	= 0;
int		pidSyncScanIdb	= 0;
int		pidInitScanIdb	= 0;
int		pidScanData		= 0;
int		pidDiagnostic	= 0;
int		pidDout			= 0;
int		pidSyncExchange	= 0;
int		pidPrepareData	= 0;
int		pidInitCalc		= 0;
int		pidInitCore		= 0;
int		pidCoreDifCalc	= 0;

CStr	ErrorSyncRegIdb;
CStr	ErrorInitRegIdb;
CStr	ErrorSyncScanIdb;
CStr	ErrorInitScanIdb;
CStr	ErrorScanData;
CStr	ErrorDiagnostic;
CStr	ErrorDout;
CStr	ErrorPrepareData;
CStr	ErrorInitCalc;
CStr	ErrorInitCore;
CStr	ErrorCoreDifCalc;

void	StopProg(CProg* pLocalProg, bool bStopNetThread, bool bStopDiag);

CStr	MakeError(const char* pNameThread, const char*  pErrorNum, const char*  pMessageText)
{// ����� ��������� 
	return MakeModuleError( pProg?pProg->GetTaskName():"ImCore", (pProg?(char*)pProg->pSettings->NodeName:NULL), pNameThread, pErrorNum, pMessageText);
}	

void AbortProg(int SignalNum)
{// ��������� ������� Prog
	int	CurPid = ThreadPID();
//	printf("ImiCore pid=%d, signal=%s\n", CurPid, GetSignalName(SignalNum));
#ifdef __sparc
	printf("ImCore pid=%d, signal=%s\n", CurPid, GetSignalName(SignalNum));
	SetAbortProc(AbortProg);
	if(SignalNum==SIGALRM || SignalNum==SIGCHLD || SignalNum==SIGWINCH) return;
#endif
	if(SignalNum==SIGHUP) return;

	if( ::pProg && ::pProg->pid==CurPid )
	{// ���� ���� MAIN
		::bAbortProgram		= true;
		CProg*	pLocalProg	= ::pProg;
		pLocalProg->AddEvent(IDI_EVENT_ERROR, MakeError("Main", "60300", CStr("��������� ������� ������. ������ ") + GetSignalName(SignalNum)));

		pLocalProg->SetFlag(FLG_STOP_PROG,SV_NEEDED);
		::StopProg(pLocalProg, SS_ALL_THREADS);
		delete pLocalProg;
		::pProg = NULL;
	}
	else if( ::pProg && ::pProgApp && ::pProgApp->pid==CurPid )
	{// ������ �������
		if(SignalNum != SIGINT)
		{// ������ ������ �� Ctrl-�
			CProgApp* 	pLocalApp	= ::pProgApp;
			::pProgApp				= NULL;
			if(SignalNum == SIGTERM)
			{// ���� ����� ��������������� ������������� �� StopProg
				::pProg->AddEvent(IDI_EVENT_INFO, MakeError("Console", NULL, "������� ������"));
				::pProg->SetFlag(FLG_START_WND, SV_READY);
			}
			else
			{
				::pProg->SetFlag(FLG_START_WND, SV_ERROR);
				::pProg->AddEvent(IDI_EVENT_ERROR, MakeError("Console", "60301", CStr("��������� ������� ������. ������ ") + GetSignalName(SignalNum)));
			}
			pLocalApp->Abort(); delete pLocalApp;
			::pProg->SetFlag(FLG_STOP_PROG, SV_NEEDED);
		}
		else return;
	}
	else if( ::pProg && ::pidScanData==CurPid )
	{// ����� ������
		if( !bAbortProgram && SignalNum!=SIGINT)
		{
			::pidScanData = 0;
			if(  SignalNum == SIGTERM )
			{// ���� ����� ��������������� ������������� �� StopProg
				::pProg->AddEvent(IDI_EVENT_INFO, MakeError("ScanData", NULL, "������� ������"));
			}
			else
			{
				::pProg->AddEvent(IDI_EVENT_ERROR, MakeError("ScanData", "60302", CStr("��������� ������� ������. ������ ") + GetSignalName(SignalNum)));
				::pProg->SetFlag(FLG_START_SCAN_THREAD, SV_ERROR);
				// ��������� ������
				::pProg->SetFlag(FLG_START_SCAN_THREAD, SV_NEEDED);
			}
		}
		else return;
	}
	else if( ::pProg && ::pidDiagnostic==CurPid )
	{// ��������������� �����
		if( !bAbortProgram && SignalNum!=SIGINT)
		{	
			::pidDiagnostic = 0;
			if(  SignalNum == SIGTERM )
			{// ���� ����� ��������������� ������������� �� StopProg
				::pProg->AddEvent(IDI_EVENT_INFO, MakeError("Diagnostic", NULL, "������� ������"));
			}
			else
			{	
				::pProg->AddEvent(IDI_EVENT_ERROR, MakeError("Diagnostic", "60303", CStr("��������� ������� ������. ������ ") + GetSignalName(SignalNum)));
				::pProg->SetFlag(FLG_START_DIAGNOSTIC_THREAD, SV_ERROR);
				// ��������� ������
				::pProg->SetFlag(FLG_START_DIAGNOSTIC_THREAD, SV_NEEDED);
			}	
		}
		else return;
	}
	else if( ::pProg && ::pidDout==CurPid )
	{// ����� ������ �����������
		if( !bAbortProgram && SignalNum!=SIGINT)
		{	
			::pidDout = 0;
			if(  SignalNum == SIGTERM )
			{// ���� ����� ��������������� ������������� �� StopProg
				::pProg->AddEvent(IDI_EVENT_INFO, MakeError("Dout", NULL, "������� ������"));
			}
			else
			{	
				::pProg->AddEvent(IDI_EVENT_ERROR, MakeError("Dout", "60303", CStr("��������� ������� ������. ������ ") + GetSignalName(SignalNum)));
				::pProg->SetFlag(FLG_START_DOUT_THREAD, SV_ERROR);
				// ��������� ������
				::pProg->SetFlag(FLG_START_DOUT_THREAD, SV_NEEDED);
			}	
		}
		else return;
	}
	else if( ::pProg && ::pidPrepareData==CurPid )
	{// ����� ������� �������� ������ �� ������� BEACON
		if( !bAbortProgram && SignalNum!=SIGINT)
		{
			::pidPrepareData = 0;
			if( SignalNum == SIGTERM )
			{// ���� ����� ��������������� ������������� �� StopProg
				::pProg->AddEvent(IDI_EVENT_INFO, MakeError("PrepareData", NULL, "������� ������"));
			}
			else
			{
				::pProg->AddEvent(IDI_EVENT_ERROR, MakeError("PrepareData", "?????", CStr("��������� ������� ������. ������ ") + GetSignalName(SignalNum)));
				// ��������� ������
				::pProg->SetFlag(FLG_START_PREPARE_DATA_THREAD, SV_NEEDED);
			}
		}
		else return;
	}
	else if( ::pProg && ::pidInitCore==CurPid )
	{// �����
		if( !bAbortProgram && SignalNum!=SIGINT)
		{
			::pidInitCore = 0;
			if( SignalNum == SIGTERM )
			{// ���� ����� ��������������� ������������� �� StopProg
				::pProg->AddEvent(IDI_EVENT_INFO, MakeError("InitCoreThreadData", NULL, "������� ������"));
			}
			else
			{
				::pProg->AddEvent(IDI_EVENT_ERROR, MakeError("InitCoreThreadData", "?????", CStr("��������� ������� ������. ������ ") + GetSignalName(SignalNum)));
				// ��������� ������
				::pProg->SetFlag(FLG_INIT_CORE_THREAD_DATA, SV_NEEDED);
			}
		}
		else return;
	}
	else if( ::pProg && ::pidCoreDifCalc==CurPid )
	{// ����� ������������� ���ޣ��
		if( !bAbortProgram && SignalNum!=SIGINT)
		{
			::pidCoreDifCalc = 0;
			if( SignalNum == SIGTERM )
			{// ���� ����� ��������������� ������������� �� StopProg
				::pProg->AddEvent(IDI_EVENT_INFO, MakeError("DifCalc", NULL, "������� ������"));
			}
			else
			{
				::pProg->AddEvent(IDI_EVENT_ERROR, MakeError("DifCalc", "?????", CStr("��������� ������� ������. ������ ") + GetSignalName(SignalNum)));
				// ��������� ������
				::pProg->SetFlag(FLG_START_DIF_CALC, SV_NEEDED);
			}
		}
		else return;
	}
	else if( ::pProg && ::pidSyncRegIdb==CurPid )
	{// ������������� ��� ����������� ������ � �������
		::pidSyncRegIdb = 0;
		::ErrorSyncRegIdb = CStr("��������� ������� ������. ������ ") + GetSignalName(SignalNum);
		::pProg->AddEvent(IDI_EVENT_ERROR, MakeError("SyncRegIdb", "60312", ::ErrorSyncRegIdb));
		::pProg->SetFlag(FLG_SYNC_REG_IDB, SV_ERROR);
	}
	else if( ::pProg && ::pidInitRegIdb==CurPid )
	{// ������������� ���  ����������� ������ � �������
		::pidInitRegIdb = 0;
		::ErrorInitRegIdb = CStr("��������� ������� ������. ������ ")+GetSignalName(SignalNum);
		::pProg->AddEvent(IDI_EVENT_ERROR, MakeError("InitRegIdb", "60313", ::ErrorInitRegIdb));
		::pProg->SetFlag(FLG_INIT_REG_IDB, SV_ERROR);
	}
	else if( ::pProg && ::pidInitCalc==CurPid )
	{// ������������� ���ޣ���� �����
		::pidInitCalc = 0;
		::ErrorInitCalc = CStr("��������� ������� ������. ������ ")+GetSignalName(SignalNum);
		::pProg->AddEvent(IDI_EVENT_ERROR, MakeError("InitCalc", "60313", ::ErrorInitCalc));
		::pProg->SetFlag(FLG_INIT_CALC, SV_ERROR);
	}
	else if( ::pProg && ::pidSyncScanIdb==CurPid )
	{// ������������� ��� ��ɣ�� ������ �� �������
		::pidSyncScanIdb = 0;
		::ErrorSyncScanIdb = CStr("��������� ������� ������. ������ ") + GetSignalName(SignalNum);
		::pProg->AddEvent(IDI_EVENT_ERROR, MakeError("SyncScanIdb", "?????", ::ErrorSyncScanIdb));
		::pProg->SetFlag(FLG_SYNC_SCAN_IDB, SV_ERROR);
	}
	else if( ::pProg && ::pidInitScanIdb==CurPid )
	{// ������������� ��� ��ɣ�� ������ �� �������
		::pidInitScanIdb = 0;
		::ErrorInitScanIdb = CStr("��������� ������� ������. ������ ")+GetSignalName(SignalNum);
		::pProg->AddEvent(IDI_EVENT_ERROR, MakeError("InitScanIdb", "?????", ::ErrorInitScanIdb));
		::pProg->SetFlag(FLG_INIT_SCAN_IDB, SV_ERROR);
	}
	else if( ::pProg && pidSyncExchange==CurPid )
	{
		if( !bAbortProgram && SignalNum!=SIGINT)
		{	
			::pidSyncExchange = 0;
			::SetPrevCurrentTime(0);
			if(SignalNum == SIGTERM)
			{// ���� ����� ��������������� ������������� �� StopSrvCore
				::pProg->AddEvent(IDI_EVENT_INFO, MakeError("SyncExchange", NULL, "������� ������"));
			}
			else
			{
				::pProg->AddEvent(IDI_EVENT_ERROR, MakeError("SyncExchange", "60314", CStr("��������� ������� ������. ������ ")+GetSignalName(SignalNum)));
				// ��������� ������
				::pProg->SetFlag(FLG_START_SYNC_EXCHANGE, SV_NEEDED);
			}
		}
	}	
	else if(::pProg)
	{
		// ����������� ������
		CRegisterServer* pRegServer = NULL;
		int RegSrvCnt = ::pProg->pRegisterServersLst->GetCnt();
		for(int RegSrvPos=1 ; RegSrvPos<=RegSrvCnt ; RegSrvPos++)
		{
			CRegisterServer* pServer = (CRegisterServer*)::pProg->pRegisterServersLst->Get(RegSrvPos);
			if(pServer->RegThreadPid == CurPid){ pRegServer = pServer; break; }
		}
		if(pRegServer)
		{
			if((!bAbortProgram && SignalNum!=SIGINT) || SignalNum==SIGALRM)
			{
				pRegServer->RegThreadPid = 0;
				if(SignalNum==SIGTERM || SignalNum==SIGALRM)
				{// ���� ����� ��������������� �������������
					::pProg->AddEvent(IDI_EVENT_INFO, MakeError(pRegServer->RegThreadName, NULL, "������� ������"));
				}
				else
				{
					::pProg->AddEvent(IDI_EVENT_ERROR, MakeError(pRegServer->RegThreadName, "?????", CStr("��������� ������� ������. ������ ") + GetSignalName(SignalNum)));
					::pProg->SetFlag(pRegServer->RegThreadFlgStart, SV_ERROR);
					// ��������� ������
					::pProg->SetFlag(pRegServer->RegThreadFlgStart, SV_NEEDED);
				}
			}
			else return;
		}
	}

	ThreadExit(0);
}

void   StopProg(CProg* pLocalProg, long StopStyle)
{// ��������� ������������� ����������
	if(::pProgApp && StopStyle&SS_CONSOLE)
	{// ������� �������
		
		CrSleep(1000000); // ����� ������ ��������� �������� ����
		CMainWindow* pMainWindow = GetMainWindow();
		if(pMainWindow)
		{
			pMainWindow->AddCommand(CMainWindow::ID_CLOSE_WINDOW, NULL, true);
			double  dStartTime 	= (double)::GetCurrentTime();
			while(::pProgApp && pProgApp->pid && ((double)::GetCurrentTime()-dStartTime)<2000) ThreadYield();
			if(::pProgApp && pProgApp->pid) ThreadKill(::pProgApp->pid, SIGTERM);
			::pProgApp = NULL;
			pLocalProg->SetFlag(FLG_START_WND, SV_READY);
		}
	}

	if(StopStyle&SS_SCAN_DATA && ::pidScanData)
	{// ���� ������� ������� ����� ������, �� ��������� ���
		pLocalProg->SetFlag(FLG_STOP_SCAN_THREAD, SV_NEEDED);
		double  dStartTime 	= (double)::GetCurrentTime();
		while(::pidScanData && (double)::GetCurrentTime()-dStartTime<=2000 ) ThreadYield();
		if(::pidScanData) ThreadKill(::pidScanData, SIGTERM);
		pLocalProg->SetFlag(FLG_STOP_SCAN_THREAD, SV_READY);
	}

	if(StopStyle & SS_REG_DATA)
	{// ���� ������� ������� ����������� ������, �� ��������� ���
		int RegSrvCnt = ::pProg->pRegisterServersLst->GetCnt();
		for(int RegSrvPos=1 ; RegSrvPos<=RegSrvCnt ; RegSrvPos++)
		{
			CRegisterServer* pServer = (CRegisterServer*)::pProg->pRegisterServersLst->Get(RegSrvPos);
			if(pServer->RegThreadPid)
			{
				//ThreadKill(pServer->RegThreadPid, SIGALRM);
				pLocalProg->SetFlag(pServer->RegThreadFlgStop, SV_NEEDED);
				double dStartTime = (double)GetCurrentTime();
				while(pServer->RegThreadPid && (double)GetCurrentTime()-dStartTime<=2000 ) ThreadYield();
				if(pServer->RegThreadPid) ThreadKill(pServer->RegThreadPid, SIGTERM);
				pLocalProg->SetFlag(pServer->RegThreadFlgStop, SV_READY);
			}
		}
	}

	if(StopStyle&SS_PREPARE_DATA && ::pidPrepareData)
	{// ��������� ������� ������� �������� ������ �� ������� BEACON
		pLocalProg->SetFlag(FLG_STOP_PREPARE_DATA_THREAD, SV_NEEDED);
		double dStartTime = (double)GetCurrentTime();
		while(::pidPrepareData && (double)GetCurrentTime()-dStartTime<=2000 ) ThreadYield();
		if(::pidPrepareData) ThreadKill(::pidPrepareData, SIGTERM);
		pLocalProg->SetFlag(FLG_STOP_PREPARE_DATA_THREAD, SV_READY);
	}

	if(StopStyle&SS_DOUT && ::pidDout)
	{// ��������� ����� ������ �����������
		pLocalProg->SetFlag(FLG_STOP_DOUT_THREAD, SV_NEEDED);
		double  dStartTime 	= (double)::GetCurrentTime();
		while(::pidDout && (double)::GetCurrentTime()-dStartTime<=2000) ThreadYield();
		if(::pidDout) ThreadKill(::pidDout, SIGTERM);
		pLocalProg->SetFlag(FLG_STOP_DOUT_THREAD, SV_READY);
	}

	if(StopStyle&SS_CORE_THREAD_DATA && ::pidInitCore)
	{// ��������� �������
		pLocalProg->SetFlag(FLG_INIT_CORE_THREAD_DATA, SV_NEEDED);
		double dStartTime = (double)GetCurrentTime();
		while(::pidInitCore && (double)GetCurrentTime()-dStartTime<=2000 ) ThreadYield();
		if(::pidInitCore) ThreadKill(::pidInitCore, SIGTERM);
		pLocalProg->SetFlag(FLG_INIT_CORE_THREAD_DATA, SV_READY);
	}

	if(StopStyle&SS_DIF_CALC && ::pidCoreDifCalc)
	{// ��������� ������� ������������� ���ޣ��
		pLocalProg->SetFlag(FLG_STOP_DIF_CALC, SV_NEEDED);
		double dStartTime = (double)GetCurrentTime();
		while(::pidCoreDifCalc && (double)GetCurrentTime()-dStartTime<=2000 ) ThreadYield();
		if(::pidCoreDifCalc) ThreadKill(::pidCoreDifCalc, SIGTERM);
		pLocalProg->SetFlag(FLG_STOP_DIF_CALC, SV_READY);
	}

	if(StopStyle&SS_DIAGNOSTIC && ::pidDiagnostic)
	{// ��������� ��������������� �����
		pLocalProg->SetFlag(FLG_STOP_DIAGNOSTIC_THREAD, SV_NEEDED);
		double  dStartTime 	= (double)::GetCurrentTime();
		while(::pidDiagnostic && (double)::GetCurrentTime()-dStartTime<=2000) ThreadYield();
		if(::pidDiagnostic) ThreadKill(::pidDiagnostic, SIGTERM);
		pLocalProg->SetFlag(FLG_STOP_DIAGNOSTIC_THREAD, SV_READY);
	}
	
	if(StopStyle&SS_SYNC_EXCHANGE && ::pidSyncExchange)
	{// ��������� ����� ������ � SyncAgent (���������)
		pLocalProg->SetFlag(FLG_STOP_SYNC_EXCHANGE, SV_NEEDED);
		double dStartTime = (double)GetCurrentTime();
		while(::pidSyncExchange && (double)GetCurrentTime()-dStartTime<=2000 ) ThreadYield();
		if(::pidSyncExchange) ThreadKill(::pidSyncExchange, SIGTERM);
		pLocalProg->SetFlag(FLG_STOP_SYNC_EXCHANGE, SV_READY);
	}

	if(StopStyle&SS_SYNC_REG_IDB && ::pidSyncRegIdb)	ThreadKill(::pidSyncRegIdb, SIGTERM);
	if(StopStyle&SS_INIT_REG_IDB && ::pidInitRegIdb)	ThreadKill(::pidInitRegIdb, SIGTERM);

	if(StopStyle&SS_INIT_CALC && ::pidInitCalc)	ThreadKill(::pidInitCalc, SIGTERM);

	if(StopStyle&SS_SYNC_SCAN_IDB && ::pidSyncScanIdb)	ThreadKill(::pidSyncScanIdb, SIGTERM);
	if(StopStyle&SS_INIT_SCAN_IDB && ::pidInitScanIdb)	ThreadKill(::pidInitScanIdb, SIGTERM);

	int	FullStyle = SS_ALL_THREADS;
	if(StopStyle==FullStyle ) pLocalProg->AddEvent(IDI_EVENT_INFO, MakeError(NULL, NULL, "--- ������� ��������� ---"));
}

void StartThread(long SemID, const char* pThreadName, void* (*pFunc)(void*), void* pFuncParam)
{// �������� ����� 
	CStr Error;
	::pProg->SetFlag(SemID, SV_PROCESS);
	if(ThreadCreate(pThreadName, pFunc, pFuncParam, &Error))
	{// ���� ��������� ������ ������, ������ � ��������
		::pProg->AddEvent(IDI_EVENT_ERROR, MakeError("Main", "60307", Error));
		::pProg->SetFlag(SemID, SV_ERROR);
	}
}
int main(int argc, char *argv[])
{
	if(argc>1 && ParseCommandLine(argc, argv, MakeVersionStr)) return 0;

	if(!XInitThreads()) puts(MakeError("Main", "60398", "XInitThreads error"));
	ThreadSetConcurrency(30);

	// ������������� ���������� ��������
	SetAbortProc(AbortProg);
	short Result = ::IsLoadTask(ThreadPID(false),"SRVHOME", "ImCore", argc>=2?argv[1]:NULL);

	if(Result == 1)
	{// ���� ������ �������� - ���������� ��������
		CProg*	pProg = new CProg(CStr(getenv("SRVHOME")) + "/ImCore", argc>=2?argv[1]:NULL);
		CProgSettings*	pSettings					= pProg->pSettings;
		long			PrevStartDiagnosticTime		= 0;
		long			PrevStartDoutTime			= 0;
		long			PrevStartScanDataTime		= 0;
		long			PrevStartRegDataTime		= 0;
		long			PrevStartSyncExchangeTime	= 0;
		long			PrevStartPrepareDataTime	= 0;

		bool			bEnableDirect = false;
		CStr			strUID(GrantsGetUID());
		CStartThread	ThreadParams;

		if( !::GrantsIsValid() || GrantsIsEnable(pSettings->NodeName,::pProg->GetTaskName(), strUID, "DirectModule") )
			bEnableDirect = true;
		else pProg->AddEvent(IDI_EVENT_ASTERISK,MakeError("Main", NULL, "�� ���������� ���� ��� ������� ���������"),(char*)strUID);
		
		if(bEnableDirect && pProg->MsgQueueID>=0 && pProg->SemArrayID>=0)
		{// ���� ������� ��������� �������

			// ����������������� ��������
			::pProg->SetFlag(FLG_SYNC_REG_IDB,						SV_NEEDED);
			::pProg->SetFlag(FLG_SYNC_SCAN_IDB,						SV_NEEDED);
			::pProg->SetFlag(FLG_START_DIAGNOSTIC_THREAD,			SV_NEEDED);
			::pProg->SetFlag(FLG_START_DOUT_THREAD,					SV_NEEDED);
			::pProg->SetFlag(FLG_START_SCAN_THREAD,					SV_NEEDED);
			::pProg->SetFlag(FLG_START_SYNC_EXCHANGE,				SV_NEEDED);
			::pProg->SetFlag(FLG_START_PREPARE_DATA_THREAD,			SV_NEEDED);
			::pProg->SetFlag(FLG_INIT_CALC,							SV_NEEDED);
			::pProg->SetFlag(FLG_UPDATE_CONSTANTS,					SV_READY);

			::pProg->SetFlag(FLG_CHANGE_SCAN_DATA,					SV_DISABLE);
			::pProg->SetFlag(FLG_CHANGE_REBUILD_DATA,				SV_DISABLE);
			::pProg->SetFlag(FLG_CHANGE_DIF_DATA,					SV_DISABLE);

			while(true)
			{
 				// ���������� ������� ��������� ��������� � ��������� ���������
				ThreadParams.MsgType = pProg->ScanMsgQueue(ThreadParams.MsgText, ThreadParams.UID, &ThreadParams.AnswerQueueID, &ThreadParams.MsgID);

				if( ThreadParams.MsgType > 0 )
				{// �������� ������� ���������

					if(ThreadParams.MsgType == MSG_CLOSE)
					{// ��������� ���������� ���������
						::pProg->AddEvent(IDI_EVENT_INFO,MakeError("Main", NULL, "��������� ������� �� ������� ��������� "),ThreadParams.UID);
						if( GrantsIsEnable(::pProg->pSettings->NodeName, ::pProg->GetTaskName(), ThreadParams.UID, "DirectModule") )
						{
							::MsgSendAnswer(NULL, ThreadParams.AnswerQueueID, MSGA_WAIT, ThreadParams.MsgType, ThreadParams.MsgID);
							::pProg->SetFlag(FLG_STOP_PROG, SV_NEEDED);
						}
						else
						{
							CStr	EventText("������������ ���� ��� �������� ���������");
							pProg->AddEvent(IDI_EVENT_ASTERISK,MakeError("Main", NULL, EventText),ThreadParams.UID);
							::MsgSendAnswer(EventText, ThreadParams.AnswerQueueID, MSGA_READY, ThreadParams.MsgType, ThreadParams.MsgID);
						}
					} 
					else if( ThreadParams.MsgType == MSG_DISPLAY )
					{// ��������� ����������� �������
						if( pProg->pSettings->bEnablePuts )
							pProg->AddEvent(IDI_EVENT_INFO,MakeError("Main", NULL, CStr("��������� ������� �� ������ �������")),ThreadParams.UID);
						
						if( GrantsIsEnable(::pProg->pSettings->NodeName, ::pProg->GetTaskName(), ThreadParams.UID, "DirectConsole") )
						{
							// �������� ����� �������� ������������
							GrantsChangeUser(ThreadParams.UID,pProg);
							CMainWindow* pMainWindow = GetMainWindow();
							
							if(pMainWindow && pProg->GetFlag(FLG_START_WND) == SV_PROCESS)
							{// ������� �������� ����
								CApp*	pApp = (CApp*)pMainWindow->getApp();
								CStr	DisplayName( pApp?(char*)pApp->DisplayName:"");
								::MsgSendAnswer( CStr("������� ������� ����� �� ")+DisplayName, ThreadParams.AnswerQueueID, MSGA_READY, ThreadParams.MsgType, ThreadParams.MsgID);
								
								if(pMainWindow->isIconified()) pMainWindow->deiconify();
								pMainWindow->ActivateWindow();
							}
							else
							{
								::pProg->SetFlag(FLG_START_WND, SV_NEEDED);
								
								CStr	MsgText("������������ �������� �������"); if( strlen(ThreadParams.MsgText) ) MsgText += CStr(" �� ") + ThreadParams.MsgText;
								::MsgSendAnswer(MsgText, ThreadParams.AnswerQueueID, MSGA_READY, ThreadParams.MsgType, ThreadParams.MsgID);
							}
						}
						else
						{// �������� � �������
							CStr	EventText("������������ ���� ��� �������� �������");
							pProg->AddEvent(IDI_EVENT_ASTERISK,MakeError("Main", NULL, EventText),ThreadParams.UID);
							::MsgSendAnswer(EventText, ThreadParams.AnswerQueueID, MSGA_READY, ThreadParams.MsgType, ThreadParams.MsgID);
						} 							
					}
					else if( ThreadParams.MsgType == MSG_CLOSE_DISPLAY )
					{// ������� ����������� �������
						if( pProg->pSettings->bEnablePuts )
							pProg->AddEvent(IDI_EVENT_INFO,MakeError("Main", NULL, CStr("��������� ������� �� �������� �������")),ThreadParams.UID);
						
						if( GrantsIsEnable(::pProg->pSettings->NodeName, ::pProg->GetTaskName(), ThreadParams.UID, "DirectConsole") )
						{
							if(::pProgApp)
							{
								StopProg(::pProg, SS_CONSOLE);
								::MsgSendAnswer("��������� �������� �������", ThreadParams.AnswerQueueID, MSGA_READY, ThreadParams.MsgType, ThreadParams.MsgID);
							}
							else ::MsgSendAnswer("������� ������� �����", ThreadParams.AnswerQueueID, MSGA_READY, ThreadParams.MsgType, ThreadParams.MsgID);
						}
						else
						{// �������� � �������
							CStr	EventText( "������������ ���� ��� �������� �������");							
							pProg->AddEvent(IDI_EVENT_ASTERISK,MakeError("Main", NULL, EventText),ThreadParams.UID);
							::MsgSendAnswer(EventText, ThreadParams.AnswerQueueID, MSGA_READY, ThreadParams.MsgType, ThreadParams.MsgID);
						}
					}
					else if( ThreadParams.MsgType == MSG_SET_SETTING )
					{// ���������� ����� �������� ��� �������� *.ini �����
						if( pProg->pSettings->bEnablePuts )
							pProg->AddEvent(IDI_EVENT_INFO,MakeError("Main", NULL, CStr("��������� ������� �� ��������� �������� ") + ThreadParams.MsgText),ThreadParams.UID);
						if( GrantsIsEnable(::pProg->pSettings->NodeName, ::pProg->GetTaskName(), ThreadParams.UID, "EditSettings") )
						{
							::pProg->pSettings->PrepareMsg(ThreadParams.MsgText);
							::MsgSendAnswer(CStr("��������� ��������� ��������� ") + ThreadParams.MsgText, ThreadParams.AnswerQueueID, MSGA_READY, ThreadParams.MsgType, ThreadParams.MsgID);
						}
						else
						{
							CStr	EventText("������������ ���� ��� ��������� ��������");							
							pProg->AddEvent(IDI_EVENT_ASTERISK,MakeError("Main", NULL, EventText),ThreadParams.UID);
							::MsgSendAnswer(EventText, ThreadParams.AnswerQueueID, MSGA_READY, ThreadParams.MsgType, ThreadParams.MsgID);
						} 
					}
					else if( ThreadParams.MsgType == MSG_INPUT_TEFF )
					{// ��������� ����������� �������
						if( pProg->pSettings->bEnablePuts )
							pProg->AddEvent(IDI_EVENT_INFO,MakeError("Main", NULL, CStr("��������� ������� �� ���� Teff")),ThreadParams.UID);

						if(::pProgApp) StopProg(::pProg, SS_CONSOLE);

						::pProg->TeffInputDisplay = ThreadParams.MsgText;
						::pProg->SetFlag(FLG_INPUT_TEFF_USER_CALL,	SV_NEEDED);
						::pProg->SetFlag(FLG_INPUT_TEFF,			SV_NEEDED);
						SendMQMessage(::pProg->ExeName, ::pProg->GetTaskName(), MSG_DISPLAY, ThreadParams.MsgText);

						CStr MsgText("����������� ���� Teff"); if( strlen(ThreadParams.MsgText) ) MsgText += CStr(" �� ") + ThreadParams.MsgText;
						::MsgSendAnswer(MsgText, ThreadParams.AnswerQueueID, MSGA_READY, ThreadParams.MsgType, ThreadParams.MsgID);
					}
					else if( ThreadParams.MsgType == MSG_UPDATE_CONSTANTS )
					{// ��������� � ����� constants.idb
						if( pProg->pSettings->bEnablePuts )
							pProg->AddEvent(IDI_EVENT_INFO,MakeError("Main", NULL, CStr("��������� ������� �� ���������� constants.idb")),ThreadParams.UID);

						::pProg->SetFlag(FLG_UPDATE_CONSTANTS,	SV_NEEDED);

						CStr MsgText("������������ ���������� constants.idb");
						::MsgSendAnswer(MsgText, ThreadParams.AnswerQueueID, MSGA_READY, ThreadParams.MsgType, ThreadParams.MsgID);
					}
					else ::MsgSendAnswer(CStr("������� ") + ThreadParams.MsgType + " �� ��������������", ThreadParams.AnswerQueueID, MSGA_READY, ThreadParams.MsgType, ThreadParams.MsgID);
				}// ����� ��������� ������� ���������

				// ***************************************************************
				{// ��������� �������
					long  CurTime = CrTime();
					if( ::pProg->GetFlag(FLG_STOP_PROG)==SV_NEEDED )
					{// ��������� �������� ���������� ���������

						// ��������� ������������
						StopProg(::pProg, SS_ALL_THREADS);

						// ����� � ������� ������� 
						if( ThreadParams.AnswerQueueID ) ::MsgSendAnswer("������� ��������", ThreadParams.AnswerQueueID, MSGA_READY, ThreadParams.MsgType, ThreadParams.MsgID);

						// ����� �� ��������� �����
						break; 
					}

					if( ::pProg->GetFlag(FLG_START_DIAGNOSTIC_THREAD) == SV_NEEDED &&
						(::pProg->pSettings->bEnableDiagnostic) &&
						(CurTime > PrevStartDiagnosticTime + 3) )
					{// ������ ��������������� ������
						PrevStartDiagnosticTime = CurTime;
						StartThread(FLG_START_DIAGNOSTIC_THREAD, "Diagnostic", DiagnosticThread);
					}

					if( ::pProg->GetFlag(FLG_START_DOUT_THREAD) == SV_NEEDED &&
						(CurTime > PrevStartDoutTime + 3) )
					{// ������ ������ ������ �����������
						PrevStartDoutTime = CurTime;
						StartThread(FLG_START_DOUT_THREAD, "Dout", DoutThread);
					}

					if( ::pProg->GetFlag(FLG_START_WND)==SV_NEEDED &&
						(!::pProg->pSettings->bEnableScanData || ::pProg->GetFlag(FLG_INIT_SCAN_IDB)==SV_READY)  &&
						(!::pProg->pSettings->bEnableRegisterData || ::pProg->GetFlag(FLG_INIT_REG_IDB)==SV_READY))
					{// ������ ������� ����������
						CStartThread*	pStartThreadParams = new CStartThread;
						*pStartThreadParams = ThreadParams;
						sprintf(pStartThreadParams->MsgText, "%s %s", argv[0],(strlen(ThreadParams.MsgText)?ThreadParams.MsgText:" "));
						StartThread(FLG_START_WND, "Console", ShowApp, pStartThreadParams);
					} 

					if( ::pProg->GetFlag(FLG_INIT_CORE_THREAD_DATA)==SV_NEEDED &&
						::pProg->GetFlag(FLG_INIT_CALC)==SV_READY)
					{// ������
						StartThread(FLG_INIT_CORE_THREAD_DATA, "InitCoreThreadData", InitCoreThreadData, NULL);
					}

					if( ::pProg->GetFlag(FLG_START_DIF_CALC)==SV_NEEDED &&
						::pProg->GetFlag(FLG_INIT_CORE_THREAD_DATA)==SV_READY &&
						::pProg->GetFlag(FLG_INIT_CALC)==SV_READY)
					{// ������
						StartThread(FLG_START_DIF_CALC, "CoreDifCalc", CoreDifCalc, NULL);
					}

					if(::pProg->pSettings->bEnableRegisterData)
					{
						if( ::pProg->GetFlag(FLG_SYNC_REG_IDB)==SV_NEEDED )
						{// ������������� ��� ����������� ������ � �������
							StartThread(FLG_SYNC_REG_IDB, "SyncRegIdb", SyncRegIdb);
						}
						if( ::pProg->GetFlag(FLG_INIT_REG_IDB)==SV_NEEDED )
						{// ������������� ��� ����������� ������ � �������
							StartThread(FLG_INIT_REG_IDB, "InitRegIdb", InitRegIdb);
						}

						if( ::pProg->GetFlag(FLG_INIT_REG_IDB)==SV_READY &&
							(::pProg->pSettings->bEnableRegisterData) &&
							(CurTime > PrevStartRegDataTime + 3) )
						{
							PrevStartRegDataTime = CurTime;
							int RegSrvCnt = ::pProg->pRegisterServersLst->GetCnt();
							for(int RegSrvPos=1 ; RegSrvPos<=RegSrvCnt ; RegSrvPos++)
							{
								CRegisterServer* pServer = (CRegisterServer*)::pProg->pRegisterServersLst->Get(RegSrvPos);
								if(!pServer->RegThreadPid && ::pProg->GetFlag(pServer->RegThreadFlgStart)==SV_NEEDED)
									StartThread(pServer->RegThreadFlgStart, pServer->RegThreadName, RegData, pServer);
							}
						}
					}

					if(::pProg->pSettings->bEnableScanData)
					{
						if( ::pProg->GetFlag(FLG_SYNC_SCAN_IDB)==SV_NEEDED )
						{// ������������� ��� ��ɣ�� ������ �� �������
							StartThread(FLG_SYNC_SCAN_IDB, "SyncScanIdb", SyncScanIdb);
						}
						if( ::pProg->GetFlag(FLG_INIT_SCAN_IDB)==SV_NEEDED )
						{// ������������� ��� ��ɣ�� ������ �� �������
							StartThread(FLG_INIT_SCAN_IDB, "InitScanIdb", InitScanIdb);
						}
						if( ::pProg->GetFlag(FLG_START_SCAN_THREAD)==SV_NEEDED &&
							::pProg->GetFlag(FLG_INIT_SCAN_IDB)==SV_READY &&
							(::pProg->pSettings->MainServer.GetLength()||::pProg->pSettings->ReservServer.GetLength()) &&
							(CurTime > PrevStartScanDataTime + 3) )
						{
							PrevStartScanDataTime = CurTime;
							StartThread(FLG_START_SCAN_THREAD, "ScanData", ScanData);
						}

						if( ::pProg->GetFlag(FLG_START_PREPARE_DATA_THREAD)==SV_NEEDED &&
							::pProg->GetFlag(FLG_INIT_REG_IDB)==SV_READY &&
							::pProg->GetFlag(FLG_INIT_CALC)==SV_READY &&
							(CurTime > PrevStartPrepareDataTime+3) )
						{
							PrevStartPrepareDataTime = CurTime;
							StartThread(FLG_START_PREPARE_DATA_THREAD, "PrepareData", PrepareData);
						}
					}

					if( ::pProg->GetFlag(FLG_INIT_CALC)==SV_NEEDED &&
						::pProg->GetFlag(FLG_INIT_REG_IDB)==SV_READY &&
						::pProg->GetFlag(FLG_INIT_SCAN_IDB)==SV_READY)
					{// ������������� ���ޣ���� �����
						StartThread(FLG_INIT_CALC, "InitCalc", InitCalc);
					}

					if( ::pProg->GetFlag(FLG_START_SYNC_EXCHANGE) == SV_NEEDED &&
						(CurTime > PrevStartSyncExchangeTime + 3) )
					{// ��������� ����� ������ � SyncAgent
						PrevStartSyncExchangeTime = CurTime;
						StartThread(FLG_START_SYNC_EXCHANGE, "SyncExchange", SyncExchangeThread, NULL);
					}
				}// ����� ����������


				{// ����� ������� ��������� ����������, ���� ��� ������ ������
					ThreadYield();
				}
			}
		}
		else
		{
			printf("MsgQueueID=%d, SemArrayID=%d\n", pProg->MsgQueueID, pProg->SemArrayID); // ��������� ���� ������ � �������� �����
		}

		delete pProg;
	}
	else if( Result == 2 )
	{// ���� ��������� ������ ���������
		puts(MakeError("Main", "60308", "��������� ��� ��������"));
	}
	else if( Result == 3 )
	{// ������� ����������� ���������� SRVHOME
		puts(MakeError("Main", "60309", "������� ����������� ���������� ��������� SRVHOME"));
	}
	else if( Result == -1 )
	{// ������ ��� ������ �� ������
		puts(MakeError("Main", "60310", CStr("������ ��� ��������� ������� pgrep - ") + strerror(errno)));
	}

	return 0;
}




///////////////////////////////////////////////////////////////////////////
// ������ ProgApp
void* ShowApp(void* pStartThreadParams)
{
#ifdef __sparc
	SetAbortProc(AbortProg);
#endif
	bool bInputTeff = ::pProg->GetFlag(FLG_INPUT_TEFF)==SV_NEEDED  ? true : false;

	CStartThread*  pStartParams 	= (CStartThread*)pStartThreadParams;
	// ��������� ������ ���������� ��������� ������
	char	Path[256];	Path[0]	= 0;
	char	Host[256];	Host[0]	= 0; sscanf(pStartParams->MsgText, "%s %s", Path, Host);
	int		Argc				= 3;
	const char*	ppArgv[4];

	if(!StrLen(Host))
	{// ��� ����� �������� �� ������� ���������
		StrCpy(Host, getenv("DISPLAY"));
	}		

	if(!StrStr(Host, ":")) StrCat(Host, ":0.0");

	ppArgv[0] = Path;
	ppArgv[1] = "-display";
	ppArgv[2] = Host;
	ppArgv[3] = "\0" ;

	delete pStartParams; // �.�. ������� �� new  � Main

	if(CheckXServer(Host))
	{
		// ��������� ���������� ���� ���������� App �������
		::fxthreadapp	= 1;
		// ������� ����������
		CProgApp* pLocApp = new CProgApp(Host);
		pLocApp->init(Argc, (char**)ppArgv);

		if(bInputTeff)	new CInputTeffWnd(pLocApp);
		else			new CProgWnd(pLocApp);

		pLocApp->create();
		::pProgApp = pLocApp;
		::pProg->AddEvent(IDI_EVENT_INFO,MakeError("Console", NULL, "����� ������"));
		::pProgApp->run();
		::pProg->AddEvent(IDI_EVENT_INFO, MakeError("Console", NULL, "������� ������"));
		delete ::pProgApp;
		::pProgApp = NULL;
	}
	else ::pProg->AddEvent(IDI_EVENT_ERROR, MakeError("Console", NULL, CStr("�������� ���������� ������. ������� \"")+Host+"\" ����������." ));
	::pProg->SetFlag(FLG_START_WND, SV_READY);
	
	ThreadExit(0);
	
	return NULL;
}

void* InitRegIdb(void* pStartThreadParams)
{// ������������� ��� ����������� ������ � �������
#ifdef __sparc
	SetAbortProc(AbortProg);
#endif

	CStr	ErrorText;

	const char* pThreadName = "InitRegIdb";

	::pidInitRegIdb = ThreadPID();
	::pProg->AddEvent(IDI_EVENT_INFO, MakeError(pThreadName, NULL, "����� ������"));

	if( ::pProg->pRegIdb->InitIdb(&ErrorText) )
	{
		::pProg->AddEvent(IDI_EVENT_INFO, MakeError(pThreadName, NULL, CStr("������� ������. ������������� ��� ���������. ��������� ") + ::pProg->pRegIdb->pParamLst->GetCnt() + ", ���������� " + ::pProg->pRegIdb->RejectParamCnt + " ����������"));
		::pProg->SetFlag(FLG_INIT_REG_IDB, SV_READY);
	}
	else
	{
		::ErrorInitRegIdb = ErrorText;
		::pProg->AddEvent(IDI_EVENT_ERROR, MakeError(pThreadName, NULL, CStr("�������� ���������� ������ (") + ErrorText + ")"));
		::pProg->SetFlag(FLG_INIT_REG_IDB, SV_ERROR);
	}

	::pidInitRegIdb = 0;
	
	ThreadExit(0);
	
	return NULL;
}

void* SyncRegIdb(void* pStartThreadParams)
{// ������������� ��� ����������� ������ � �������
#ifdef __sparc
	SetAbortProc(AbortProg);
#endif
	short 		SyncResult 	= 0;
	CStr		ErrorText;

	const char* pThreadName = "SyncRegDataIdb";

	::pidSyncRegIdb	= ThreadPID();

	::pProg->AddEvent(IDI_EVENT_INFO, MakeError(pThreadName, NULL, "����� ������"));

	long MainServerLen		= ::pProg->pSettings->MainServer.GetLength();
	long ReservServerLen	= ::pProg->pSettings->ReservServer.GetLength();

	::pProg->pRegIdb->SyncServerName = "";

	if(MainServerLen || ReservServerLen)
	{
		if(MainServerLen)
		{// �������� ������ ������
			SyncResult = ::pProg->pRegIdb->SyncIdb(::pProg->pSettings->MainServer, &ErrorText);
			if(SyncResult) ::pProg->pRegIdb->SyncServerName = ::pProg->pSettings->MainServer;
			else if(ReservServerLen) ::pProg->AddEvent(IDI_EVENT_ASTERISK, MakeError(pThreadName, NULL, ErrorText));
		}
		if(!SyncResult && ReservServerLen)
		{// ��������� ������ ������
			SyncResult 	= ::pProg->pRegIdb->SyncIdb(::pProg->pSettings->ReservServer, &ErrorText);
			::pProg->pRegIdb->SyncServerName = ::pProg->pSettings->ReservServer;
		}

		if(SyncResult)
		{
			::pProg->AddEvent(IDI_EVENT_INFO, MakeError(pThreadName, NULL, CStr("������� ������") + ". ������������� ��� ��������� � ������� " + ::pProg->pRegIdb->SyncServerName));
			::pProg->SetFlag(FLG_SYNC_REG_IDB, SV_READY);
		}
		else
		{
			::ErrorSyncRegIdb = ErrorText;
			::pProg->AddEvent(IDI_EVENT_ASTERISK, MakeError(pThreadName, NULL, (char*)(CStr("�������� ���������� ������ (") + ErrorText + ")") ));
			::pProg->SetFlag(FLG_SYNC_REG_IDB, SV_ERROR);
		}

		// ��������� ������������� ���
		::pProg->SetFlag(FLG_INIT_REG_IDB, SV_NEEDED);
	}
	else
	{
		::ErrorSyncRegIdb = "�� ������� ����� ��������� � ���������� �������� ������.";
		::pProg->AddEvent(IDI_EVENT_ERROR, MakeError(pThreadName, NULL, CStr("�������� ���������� ������ (") + MakeError(pThreadName, "????", ::ErrorSyncRegIdb)+")"));
		::pProg->SetFlag(FLG_SYNC_REG_IDB, SV_ERROR);
	}

	::pidSyncRegIdb = 0;

	ThreadExit(0);

	return NULL;
}

void* RegData(void* pStartThreadParams)
{// ����������� � ���������� ������
#ifdef __sparc
	SetAbortProc(AbortProg);
#endif

	CRegisterServer* pServer = (CRegisterServer*)pStartThreadParams;

	::pProg->SetFlag(pServer->RegThreadFlgStop, SV_READY);
	pServer->RegThreadPid = ThreadPID();
	::pProg->AddEvent(IDI_EVENT_INFO, MakeError(pServer->RegThreadName, NULL, "����� ������"));
	if(!::pProg->RegThread(pServer))
	{
		::pProg->AddEvent(IDI_EVENT_ERROR, MakeError(pServer->RegThreadName, NULL, (char*)(CStr("�������� ���������� ������ (") + pServer->Error + ")")));
		pServer->RegThreadPid = 0;

		// ��������� ������ �����������
		::pProg->SetFlag(pServer->RegThreadFlgStart, SV_NEEDED);
	}
	else ::pProg->AddEvent(IDI_EVENT_INFO, MakeError(pServer->RegThreadName, NULL, "������� ������"));
	pServer->RegThreadPid = 0;

	::pProg->SetFlag(pServer->RegThreadFlgStop, SV_READY);

	ThreadExit(0);

	return NULL;
}

void* InitScanIdb(void* pStartThreadParams)
{// ������������� ��� ��ɣ�� ������ �� �������
#ifdef __sparc
	SetAbortProc(AbortProg);
#endif

	CStr	ErrorText;

	const char* pThreadName = "InitScanIdb";

	::pidInitScanIdb = ThreadPID();
	::pProg->AddEvent(IDI_EVENT_INFO, MakeError(pThreadName, NULL, "����� ������"));

	if( ::pProg->pScanIdb->InitIdb(&ErrorText) )
	{
		::pProg->AddEvent(IDI_EVENT_INFO, MakeError(pThreadName, NULL, CStr("������� ������. ������������� ��� ���������. ��������� ") + ::pProg->pScanIdb->pParamLst->GetCnt() + ", ���������� " + ::pProg->pScanIdb->RejectParamCnt + " ����������"));
		::pProg->SetFlag(FLG_INIT_SCAN_IDB, SV_READY);
	}
	else
	{
		::ErrorInitScanIdb = ErrorText;
		::pProg->AddEvent(IDI_EVENT_ERROR, MakeError(pThreadName, NULL, CStr("�������� ���������� ������ (") + ErrorText + ")"));
		::pProg->SetFlag(FLG_INIT_SCAN_IDB, SV_ERROR);
	}

	::pidInitScanIdb = 0;
	ThreadExit(0);
	return NULL;
}

void* SyncScanIdb(void* pStartThreadParams)
{// ������������� ��� ��ɣ�� ������ �� �������
#ifdef __sparc
	SetAbortProc(AbortProg);
#endif
	short 		SyncResult 	= 0;
	CStr		ErrorText;

	const char* pThreadName = "SyncScanDataIdb";

	::pidSyncScanIdb	= ThreadPID();

	::pProg->AddEvent(IDI_EVENT_INFO, MakeError(pThreadName, NULL, "����� ������"));

	long MainServerLen		= ::pProg->pSettings->MainServer.GetLength();
	long ReservServerLen	= ::pProg->pSettings->ReservServer.GetLength();

	::pProg->pScanIdb->SyncServerName = "";

	if(MainServerLen || ReservServerLen)
	{
		if(MainServerLen)
		{// �������� ������ ������
			SyncResult = ::pProg->pScanIdb->SyncIdb(::pProg->pSettings->MainServer, &ErrorText);
			if(SyncResult) ::pProg->pScanIdb->SyncServerName = ::pProg->pSettings->MainServer;
			else if(ReservServerLen) ::pProg->AddEvent(IDI_EVENT_ASTERISK, MakeError(pThreadName, NULL, ErrorText));
		}
		if(!SyncResult && ReservServerLen)
		{// ��������� ������ ������
			SyncResult 	= ::pProg->pScanIdb->SyncIdb(::pProg->pSettings->ReservServer, &ErrorText);
			::pProg->pScanIdb->SyncServerName = ::pProg->pSettings->ReservServer;
		}

		if(SyncResult)
		{
			::pProg->AddEvent(IDI_EVENT_INFO, MakeError(pThreadName, NULL, CStr("������� ������") + ". ������������� ��� ��������� � ������� " + ::pProg->pScanIdb->SyncServerName));
			::pProg->SetFlag(FLG_SYNC_SCAN_IDB, SV_READY);
		}
		else
		{
			::ErrorSyncScanIdb = ErrorText;
			::pProg->AddEvent(IDI_EVENT_ASTERISK, MakeError(pThreadName, NULL, (char*)(CStr("�������� ���������� ������ (") + ErrorText + ")") ));
			::pProg->SetFlag(FLG_SYNC_SCAN_IDB, SV_ERROR);
		}

		// ��������� ������������� ���
		::pProg->SetFlag(FLG_INIT_SCAN_IDB, SV_NEEDED);
	}
	else
	{
		::ErrorSyncScanIdb = "�� ������� ����� ��������� � ���������� �������� ������.";
		::pProg->AddEvent(IDI_EVENT_ERROR, MakeError(pThreadName, NULL, CStr("�������� ���������� ������ (") + MakeError(pThreadName, "????", ::ErrorSyncScanIdb)+")"));
		::pProg->SetFlag(FLG_SYNC_SCAN_IDB, SV_ERROR);
	}

	::pidSyncScanIdb = 0;

	ThreadExit(0);

	return NULL;
}

void* ScanData(void* pStartThreadParams)
{// ��ɣ� ������ �� �������
#ifdef __sparc
	SetAbortProc(AbortProg);
#endif

	CStr	ErrorText;

	const char* pThreadName = "ScanData";

	::pProg->SetFlag(FLG_STOP_SCAN_THREAD, SV_READY);

	::pidScanData = ThreadPID();
	::pProg->AddEvent(IDI_EVENT_INFO, MakeError(pThreadName, NULL, "����� ������"));
	if(!::pProg->ScanThread(pThreadName, &ErrorText) )
	{
		::ErrorScanData = CStr("�������� ���������� ������ (") + ErrorText + ")";
		::pProg->AddEvent(IDI_EVENT_ERROR, MakeError(pThreadName, NULL, CStr("�������� ���������� ������ (") + ErrorText + ")"));
		::pidScanData = 0;

		// ��������� ������ 
		::pProg->SetFlag(FLG_START_SCAN_THREAD, SV_NEEDED);
	}
	else ::pProg->AddEvent(IDI_EVENT_INFO, MakeError(pThreadName, NULL, "������� ������"));
	::pidScanData = 0;

	::pProg->SetFlag(FLG_STOP_SCAN_THREAD, SV_READY);

	ThreadExit(0);
	
	return NULL;
}

void* PrepareData(void* ptr)
{// ����� ���������� �������� �� ������� ������ ��� �������� � ������� ���ޣ���
#ifdef __sparc
	SetAbortProc(AbortProg);
#endif
	CStr	ErrorText;

	const char* pThreadName = "PrepareData";

	::pProg->SetFlag(FLG_STOP_PREPARE_DATA_THREAD, SV_READY);

	::pidPrepareData = ThreadPID();
	::pProg->AddEvent(IDI_EVENT_INFO, MakeError(pThreadName, NULL, "����� ������"));


	short Result = ::pProg->PrepareDataThread(pThreadName, &ErrorText);
	if(Result <= 0)
	{
		::ErrorPrepareData = CStr("�������� ���������� ������ (") + ErrorText + ")";
		::pProg->AddEvent(IDI_EVENT_ERROR, MakeError(pThreadName, NULL, CStr("�������� ���������� ������ (") + ErrorText + ")"));
		::pidPrepareData = 0;

		if(!Result)
		{// ��������� ������
			::pProg->SetFlag(FLG_START_PREPARE_DATA_THREAD, SV_NEEDED);
		}
	}
	else ::pProg->AddEvent(IDI_EVENT_INFO, MakeError(pThreadName, NULL, "������� ������"));
	::pidPrepareData = 0;

	::pProg->SetFlag(FLG_STOP_PREPARE_DATA_THREAD, SV_READY);
	ThreadExit(0);
	return NULL;
}

void* SyncExchangeThread(void* pStartThreadParams)
{
#ifdef __sparc
	SetAbortProc(AbortProg);
#endif

	::pProg->SetFlag(FLG_STOP_SYNC_EXCHANGE, SV_READY);
	::pProg->SetFlag(FLG_RUN_SYNC_EXCHANGE, SV_READY);

	::pidSyncExchange = ThreadPID();
	::pProg->AddEvent(IDI_EVENT_INFO, MakeError("SyncExchange", NULL, "����� ������"));

	double	dPrevCurrentTime	= GetCurrentTime(false);
	double	dPrevDeltaTime		= 0;

	while(true)
	{
		if(	::pProg->GetFlag(FLG_STOP_SYNC_EXCHANGE)!=SV_NEEDED && ::pProg->GetFlag(FLG_RUN_SYNC_EXCHANGE)!=SV_PROCESS )
		{// ���� ���� �������� �����������
			double 	dDeltaTime  		= TestCurrentTime(dPrevCurrentTime);

			if( dDeltaTime!=0 && (dPrevDeltaTime==0 || (dDeltaTime>0 && dPrevDeltaTime<0) || (dDeltaTime<0 && dPrevDeltaTime>0) ))
			{// ��������� ������� ������� !!! ��������� ���� �� ����������� �������� �������
				::pProg->AddEvent(IDI_EVENT_ASTERISK, MakeError("SyncExchange", NULL, CStr("���������� ��������� ����� �� ") + (int)(dDeltaTime) + " ����."));
			}
			dPrevDeltaTime	 = dDeltaTime;
			dPrevCurrentTime = GetCurrentTime(false);

			::pProg->SetFlag(FLG_RUN_SYNC_EXCHANGE, SV_PROCESS);

			// �������� ����� � SyncAgent
			::pProg->SyncScanShareMem();

			CrSleep(50000);
			::pProg->SetFlag(FLG_RUN_SYNC_EXCHANGE, SV_READY);
		}
		else if(::pProg->GetFlag(FLG_STOP_SYNC_EXCHANGE) == SV_NEEDED)
		{// ���� �����������
			break;
		}
		else ThreadYield();
	}

	::pProg->AddEvent(IDI_EVENT_INFO, MakeError("SyncExchange", NULL, "������� ������"));
	::pidSyncExchange = 0;
	::SetPrevCurrentTime(0);
	::pProg->SetFlag(FLG_STOP_SYNC_EXCHANGE, SV_READY);

	ThreadExit(0);

	return NULL;
}

void* InitCalc(void* pStartThreadParams)
{// ������������� ���ޣ���� �����
#ifdef __sparc
	SetAbortProc(AbortProg);
#endif

	const char* pThreadName = "InitCalc";

	::pidInitCalc = ThreadPID();
	::pProg->AddEvent(IDI_EVENT_INFO, MakeError(pThreadName, NULL, "����� ������"));

	::pProg->pCalc->Init();
    ::pProg->pAkz->init();

	::pProg->AddEvent(IDI_EVENT_INFO, MakeError(pThreadName, NULL, "������� ������. ������������� ���ޣ���� ����� ���������"));
	::pProg->SetFlag(FLG_INIT_CALC, SV_READY);
	::pidInitCalc = 0;

	::pProg->SetFlag(FLG_INIT_CORE_THREAD_DATA, SV_NEEDED);

	ThreadExit(0);

	return NULL;
}

void* InitCoreThreadData(void* pStartThreadParams)
{// ������������� ������ ��� ������� Rebuild � DifCalc
#ifdef __sparc
	SetAbortProc(AbortProg);
#endif

	const char* pThreadName = "InitCoreThreadData";

	::pidInitCore = ThreadPID();
	::pProg->AddEvent(IDI_EVENT_INFO, MakeError(pThreadName, NULL, "����� ������"));

    ::pProg->pAkz->initCalc();

	::pProg->AddEvent(IDI_EVENT_INFO, MakeError(pThreadName, NULL, "������� ������. ������������� ������ ��� ������� Rebuild � DifCalc ���������"));
	::pProg->SetFlag(FLG_INIT_CORE_THREAD_DATA, SV_READY);
	::pidInitCore = 0;

	::pProg->SetFlag(FLG_START_DIF_CALC, SV_NEEDED);

	::pidInitCore = 0;

	ThreadExit(0);

	return NULL;
}

void* CoreDifCalc(void* ptr)
{// ������������ ���ޣ�
	const char* pThreadName = "DifCalc";

	::pidCoreDifCalc = ThreadPID();
	::pProg->AddEvent(IDI_EVENT_INFO, MakeError(pThreadName, NULL, "����� ������"));

    ::pProg->pAkz->threadDU();

	::pProg->AddEvent(IDI_EVENT_INFO, MakeError(pThreadName, NULL, "������� ������"));

    ::pProg->SetFlag(FLG_STOP_DIF_CALC, SV_READY);
	::pidCoreDifCalc = 0;
	ThreadExit(0);
	return NULL;
}
