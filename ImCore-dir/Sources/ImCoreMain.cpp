#include <ImCoreMain.h>
#include <X11/X.h>
#include <X11/Xlib.h>

char* MakeVersionStr(void)
{
	static char Version[120]; sprintf(Version, "%s.1 сборка %s, ядро %s", CrMakeVersion(), MAKE_DATE, MAKE_KERNEL); return Version;
}

CProg*		pProg			= NULL;
CProgApp*	pProgApp		= NULL;
bool		bAbortProgram	= false;

// глобальные идентификаторы потоков  
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
{// лепит сообщение 
	return MakeModuleError( pProg?pProg->GetTaskName():"ImCore", (pProg?(char*)pProg->pSettings->NodeName:NULL), pNameThread, pErrorNum, pMessageText);
}	

void AbortProg(int SignalNum)
{// аварийная функция Prog
	int	CurPid = ThreadPID();
//	printf("ImiCore pid=%d, signal=%s\n", CurPid, GetSignalName(SignalNum));
#ifdef __sparc
	printf("ImCore pid=%d, signal=%s\n", CurPid, GetSignalName(SignalNum));
	SetAbortProc(AbortProg);
	if(SignalNum==SIGALRM || SignalNum==SIGCHLD || SignalNum==SIGWINCH) return;
#endif
	if(SignalNum==SIGHUP) return;

	if( ::pProg && ::pProg->pid==CurPid )
	{// если умер MAIN
		::bAbortProgram		= true;
		CProg*	pLocalProg	= ::pProg;
		pLocalProg->AddEvent(IDI_EVENT_ERROR, MakeError("Main", "60300", CStr("Аварийный останов потока. Сигнал ") + GetSignalName(SignalNum)));

		pLocalProg->SetFlag(FLG_STOP_PROG,SV_NEEDED);
		::StopProg(pLocalProg, SS_ALL_THREADS);
		delete pLocalProg;
		::pProg = NULL;
	}
	else if( ::pProg && ::pProgApp && ::pProgApp->pid==CurPid )
	{// сдохла консоль
		if(SignalNum != SIGINT)
		{// запрет выхода по Ctrl-С
			CProgApp* 	pLocalApp	= ::pProgApp;
			::pProgApp				= NULL;
			if(SignalNum == SIGTERM)
			{// если поток останавливается принудительно из StopProg
				::pProg->AddEvent(IDI_EVENT_INFO, MakeError("Console", NULL, "Останов потока"));
				::pProg->SetFlag(FLG_START_WND, SV_READY);
			}
			else
			{
				::pProg->SetFlag(FLG_START_WND, SV_ERROR);
				::pProg->AddEvent(IDI_EVENT_ERROR, MakeError("Console", "60301", CStr("Аварийный останов потока. Сигнал ") + GetSignalName(SignalNum)));
			}
			pLocalApp->Abort(); delete pLocalApp;
			::pProg->SetFlag(FLG_STOP_PROG, SV_NEEDED);
		}
		else return;
	}
	else if( ::pProg && ::pidScanData==CurPid )
	{// прием данных
		if( !bAbortProgram && SignalNum!=SIGINT)
		{
			::pidScanData = 0;
			if(  SignalNum == SIGTERM )
			{// если поток останавливается принудительно из StopProg
				::pProg->AddEvent(IDI_EVENT_INFO, MakeError("ScanData", NULL, "Останов потока"));
			}
			else
			{
				::pProg->AddEvent(IDI_EVENT_ERROR, MakeError("ScanData", "60302", CStr("Аварийный останов потока. Сигнал ") + GetSignalName(SignalNum)));
				::pProg->SetFlag(FLG_START_SCAN_THREAD, SV_ERROR);
				// повторный запуск
				::pProg->SetFlag(FLG_START_SCAN_THREAD, SV_NEEDED);
			}
		}
		else return;
	}
	else if( ::pProg && ::pidDiagnostic==CurPid )
	{// диагностический поток
		if( !bAbortProgram && SignalNum!=SIGINT)
		{	
			::pidDiagnostic = 0;
			if(  SignalNum == SIGTERM )
			{// если поток останавливается принудительно из StopProg
				::pProg->AddEvent(IDI_EVENT_INFO, MakeError("Diagnostic", NULL, "Останов потока"));
			}
			else
			{	
				::pProg->AddEvent(IDI_EVENT_ERROR, MakeError("Diagnostic", "60303", CStr("Аварийный останов потока. Сигнал ") + GetSignalName(SignalNum)));
				::pProg->SetFlag(FLG_START_DIAGNOSTIC_THREAD, SV_ERROR);
				// повторный запуск
				::pProg->SetFlag(FLG_START_DIAGNOSTIC_THREAD, SV_NEEDED);
			}	
		}
		else return;
	}
	else if( ::pProg && ::pidDout==CurPid )
	{// поток вывода диагностики
		if( !bAbortProgram && SignalNum!=SIGINT)
		{	
			::pidDout = 0;
			if(  SignalNum == SIGTERM )
			{// если поток останавливается принудительно из StopProg
				::pProg->AddEvent(IDI_EVENT_INFO, MakeError("Dout", NULL, "Останов потока"));
			}
			else
			{	
				::pProg->AddEvent(IDI_EVENT_ERROR, MakeError("Dout", "60303", CStr("Аварийный останов потока. Сигнал ") + GetSignalName(SignalNum)));
				::pProg->SetFlag(FLG_START_DOUT_THREAD, SV_ERROR);
				// повторный запуск
				::pProg->SetFlag(FLG_START_DOUT_THREAD, SV_NEEDED);
			}	
		}
		else return;
	}
	else if( ::pProg && ::pidPrepareData==CurPid )
	{// поток разбора принятых данных от системы BEACON
		if( !bAbortProgram && SignalNum!=SIGINT)
		{
			::pidPrepareData = 0;
			if( SignalNum == SIGTERM )
			{// если поток останавливается принудительно из StopProg
				::pProg->AddEvent(IDI_EVENT_INFO, MakeError("PrepareData", NULL, "Останов потока"));
			}
			else
			{
				::pProg->AddEvent(IDI_EVENT_ERROR, MakeError("PrepareData", "?????", CStr("Аварийный останов потока. Сигнал ") + GetSignalName(SignalNum)));
				// повторный запуск
				::pProg->SetFlag(FLG_START_PREPARE_DATA_THREAD, SV_NEEDED);
			}
		}
		else return;
	}
	else if( ::pProg && ::pidInitCore==CurPid )
	{// поток
		if( !bAbortProgram && SignalNum!=SIGINT)
		{
			::pidInitCore = 0;
			if( SignalNum == SIGTERM )
			{// если поток останавливается принудительно из StopProg
				::pProg->AddEvent(IDI_EVENT_INFO, MakeError("InitCoreThreadData", NULL, "Останов потока"));
			}
			else
			{
				::pProg->AddEvent(IDI_EVENT_ERROR, MakeError("InitCoreThreadData", "?????", CStr("Аварийный останов потока. Сигнал ") + GetSignalName(SignalNum)));
				// повторный запуск
				::pProg->SetFlag(FLG_INIT_CORE_THREAD_DATA, SV_NEEDED);
			}
		}
		else return;
	}
	else if( ::pProg && ::pidCoreDifCalc==CurPid )
	{// поток диффузионного расчёта
		if( !bAbortProgram && SignalNum!=SIGINT)
		{
			::pidCoreDifCalc = 0;
			if( SignalNum == SIGTERM )
			{// если поток останавливается принудительно из StopProg
				::pProg->AddEvent(IDI_EVENT_INFO, MakeError("DifCalc", NULL, "Останов потока"));
			}
			else
			{
				::pProg->AddEvent(IDI_EVENT_ERROR, MakeError("DifCalc", "?????", CStr("Аварийный останов потока. Сигнал ") + GetSignalName(SignalNum)));
				// повторный запуск
				::pProg->SetFlag(FLG_START_DIF_CALC, SV_NEEDED);
			}
		}
		else return;
	}
	else if( ::pProg && ::pidSyncRegIdb==CurPid )
	{// синхронизация ИБД регистрации данных в сервере
		::pidSyncRegIdb = 0;
		::ErrorSyncRegIdb = CStr("Аварийный останов потока. Сигнал ") + GetSignalName(SignalNum);
		::pProg->AddEvent(IDI_EVENT_ERROR, MakeError("SyncRegIdb", "60312", ::ErrorSyncRegIdb));
		::pProg->SetFlag(FLG_SYNC_REG_IDB, SV_ERROR);
	}
	else if( ::pProg && ::pidInitRegIdb==CurPid )
	{// инициализация ИБД  регистрации данных в сервере
		::pidInitRegIdb = 0;
		::ErrorInitRegIdb = CStr("Аварийный останов потока. Сигнал ")+GetSignalName(SignalNum);
		::pProg->AddEvent(IDI_EVENT_ERROR, MakeError("InitRegIdb", "60313", ::ErrorInitRegIdb));
		::pProg->SetFlag(FLG_INIT_REG_IDB, SV_ERROR);
	}
	else if( ::pProg && ::pidInitCalc==CurPid )
	{// инициализация расчётной части
		::pidInitCalc = 0;
		::ErrorInitCalc = CStr("Аварийный останов потока. Сигнал ")+GetSignalName(SignalNum);
		::pProg->AddEvent(IDI_EVENT_ERROR, MakeError("InitCalc", "60313", ::ErrorInitCalc));
		::pProg->SetFlag(FLG_INIT_CALC, SV_ERROR);
	}
	else if( ::pProg && ::pidSyncScanIdb==CurPid )
	{// синхронизация ИБД приёма данных от сервера
		::pidSyncScanIdb = 0;
		::ErrorSyncScanIdb = CStr("Аварийный останов потока. Сигнал ") + GetSignalName(SignalNum);
		::pProg->AddEvent(IDI_EVENT_ERROR, MakeError("SyncScanIdb", "?????", ::ErrorSyncScanIdb));
		::pProg->SetFlag(FLG_SYNC_SCAN_IDB, SV_ERROR);
	}
	else if( ::pProg && ::pidInitScanIdb==CurPid )
	{// инициализация ИБД приёма данных от сервера
		::pidInitScanIdb = 0;
		::ErrorInitScanIdb = CStr("Аварийный останов потока. Сигнал ")+GetSignalName(SignalNum);
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
			{// если поток останавливается принудительно из StopSrvCore
				::pProg->AddEvent(IDI_EVENT_INFO, MakeError("SyncExchange", NULL, "Останов потока"));
			}
			else
			{
				::pProg->AddEvent(IDI_EVENT_ERROR, MakeError("SyncExchange", "60314", CStr("Аварийный останов потока. Сигнал ")+GetSignalName(SignalNum)));
				// повторный запуск
				::pProg->SetFlag(FLG_START_SYNC_EXCHANGE, SV_NEEDED);
			}
		}
	}	
	else if(::pProg)
	{
		// регистрация данных
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
				{// если поток останавливается принудительно
					::pProg->AddEvent(IDI_EVENT_INFO, MakeError(pRegServer->RegThreadName, NULL, "Останов потока"));
				}
				else
				{
					::pProg->AddEvent(IDI_EVENT_ERROR, MakeError(pRegServer->RegThreadName, "?????", CStr("Аварийный останов потока. Сигнал ") + GetSignalName(SignalNum)));
					::pProg->SetFlag(pRegServer->RegThreadFlgStart, SV_ERROR);
					// повторный запуск
					::pProg->SetFlag(pRegServer->RegThreadFlgStart, SV_NEEDED);
				}
			}
			else return;
		}
	}

	ThreadExit(0);
}

void   StopProg(CProg* pLocalProg, long StopStyle)
{// культурно останавливает приложение
	if(::pProgApp && StopStyle&SS_CONSOLE)
	{// закроем консоль
		
		CrSleep(1000000); // чтобы успели закрыться дочерние окна
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
	{// если запущен процесс сбора данных, то остановим его
		pLocalProg->SetFlag(FLG_STOP_SCAN_THREAD, SV_NEEDED);
		double  dStartTime 	= (double)::GetCurrentTime();
		while(::pidScanData && (double)::GetCurrentTime()-dStartTime<=2000 ) ThreadYield();
		if(::pidScanData) ThreadKill(::pidScanData, SIGTERM);
		pLocalProg->SetFlag(FLG_STOP_SCAN_THREAD, SV_READY);
	}

	if(StopStyle & SS_REG_DATA)
	{// если запущен процесс регистрации данных, то остановим его
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
	{// остановим процесс разбора принятых данных от системы BEACON
		pLocalProg->SetFlag(FLG_STOP_PREPARE_DATA_THREAD, SV_NEEDED);
		double dStartTime = (double)GetCurrentTime();
		while(::pidPrepareData && (double)GetCurrentTime()-dStartTime<=2000 ) ThreadYield();
		if(::pidPrepareData) ThreadKill(::pidPrepareData, SIGTERM);
		pLocalProg->SetFlag(FLG_STOP_PREPARE_DATA_THREAD, SV_READY);
	}

	if(StopStyle&SS_DOUT && ::pidDout)
	{// остановим поток вывода диагностики
		pLocalProg->SetFlag(FLG_STOP_DOUT_THREAD, SV_NEEDED);
		double  dStartTime 	= (double)::GetCurrentTime();
		while(::pidDout && (double)::GetCurrentTime()-dStartTime<=2000) ThreadYield();
		if(::pidDout) ThreadKill(::pidDout, SIGTERM);
		pLocalProg->SetFlag(FLG_STOP_DOUT_THREAD, SV_READY);
	}

	if(StopStyle&SS_CORE_THREAD_DATA && ::pidInitCore)
	{// остановим процесс
		pLocalProg->SetFlag(FLG_INIT_CORE_THREAD_DATA, SV_NEEDED);
		double dStartTime = (double)GetCurrentTime();
		while(::pidInitCore && (double)GetCurrentTime()-dStartTime<=2000 ) ThreadYield();
		if(::pidInitCore) ThreadKill(::pidInitCore, SIGTERM);
		pLocalProg->SetFlag(FLG_INIT_CORE_THREAD_DATA, SV_READY);
	}

	if(StopStyle&SS_DIF_CALC && ::pidCoreDifCalc)
	{// остановим процесс диффузионного расчёта
		pLocalProg->SetFlag(FLG_STOP_DIF_CALC, SV_NEEDED);
		double dStartTime = (double)GetCurrentTime();
		while(::pidCoreDifCalc && (double)GetCurrentTime()-dStartTime<=2000 ) ThreadYield();
		if(::pidCoreDifCalc) ThreadKill(::pidCoreDifCalc, SIGTERM);
		pLocalProg->SetFlag(FLG_STOP_DIF_CALC, SV_READY);
	}

	if(StopStyle&SS_DIAGNOSTIC && ::pidDiagnostic)
	{// остановим диагностический поток
		pLocalProg->SetFlag(FLG_STOP_DIAGNOSTIC_THREAD, SV_NEEDED);
		double  dStartTime 	= (double)::GetCurrentTime();
		while(::pidDiagnostic && (double)::GetCurrentTime()-dStartTime<=2000) ThreadYield();
		if(::pidDiagnostic) ThreadKill(::pidDiagnostic, SIGTERM);
		pLocalProg->SetFlag(FLG_STOP_DIAGNOSTIC_THREAD, SV_READY);
	}
	
	if(StopStyle&SS_SYNC_EXCHANGE && ::pidSyncExchange)
	{// остановим поток обмена с SyncAgent (последним)
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
	if(StopStyle==FullStyle ) pLocalProg->AddEvent(IDI_EVENT_INFO, MakeError(NULL, NULL, "--- Останов программы ---"));
}

void StartThread(long SemID, const char* pThreadName, void* (*pFunc)(void*), void* pFuncParam)
{// стартуем поток 
	CStr Error;
	::pProg->SetFlag(SemID, SV_PROCESS);
	if(ThreadCreate(pThreadName, pFunc, pFuncParam, &Error))
	{// если неудачный запуск потока, писнем в протокол
		::pProg->AddEvent(IDI_EVENT_ERROR, MakeError("Main", "60307", Error));
		::pProg->SetFlag(SemID, SV_ERROR);
	}
}
int main(int argc, char *argv[])
{
	if(argc>1 && ParseCommandLine(argc, argv, MakeVersionStr)) return 0;

	if(!XInitThreads()) puts(MakeError("Main", "60398", "XInitThreads error"));
	ThreadSetConcurrency(30);

	// устанавливаем обработчик сигналов
	SetAbortProc(AbortProg);
	short Result = ::IsLoadTask(ThreadPID(false),"SRVHOME", "ImCore", argc>=2?argv[1]:NULL);

	if(Result == 1)
	{// если прошли проверку - продолжаем работать
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
		else pProg->AddEvent(IDI_EVENT_ASTERISK,MakeError("Main", NULL, "Не достаточно прав для запуска программы"),(char*)strUID);
		
		if(bEnableDirect && pProg->MsgQueueID>=0 && pProg->SemArrayID>=0)
		{// если создали системные объекты

			// проинициализируем семафоры
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
 				// просмотрим очередь сообщений программы и выполняем обработку
				ThreadParams.MsgType = pProg->ScanMsgQueue(ThreadParams.MsgText, ThreadParams.UID, &ThreadParams.AnswerQueueID, &ThreadParams.MsgID);

				if( ThreadParams.MsgType > 0 )
				{// просмотр очереди сообщений

					if(ThreadParams.MsgType == MSG_CLOSE)
					{// завершить выполнение программы
						::pProg->AddEvent(IDI_EVENT_INFO,MakeError("Main", NULL, "Поступила команда на останов программы "),ThreadParams.UID);
						if( GrantsIsEnable(::pProg->pSettings->NodeName, ::pProg->GetTaskName(), ThreadParams.UID, "DirectModule") )
						{
							::MsgSendAnswer(NULL, ThreadParams.AnswerQueueID, MSGA_WAIT, ThreadParams.MsgType, ThreadParams.MsgID);
							::pProg->SetFlag(FLG_STOP_PROG, SV_NEEDED);
						}
						else
						{
							CStr	EventText("Недостаточно прав для останова программы");
							pProg->AddEvent(IDI_EVENT_ASTERISK,MakeError("Main", NULL, EventText),ThreadParams.UID);
							::MsgSendAnswer(EventText, ThreadParams.AnswerQueueID, MSGA_READY, ThreadParams.MsgType, ThreadParams.MsgID);
						}
					} 
					else if( ThreadParams.MsgType == MSG_DISPLAY )
					{// запустить управляющую консоль
						if( pProg->pSettings->bEnablePuts )
							pProg->AddEvent(IDI_EVENT_INFO,MakeError("Main", NULL, CStr("Поступила команда на запуск консоли")),ThreadParams.UID);
						
						if( GrantsIsEnable(::pProg->pSettings->NodeName, ::pProg->GetTaskName(), ThreadParams.UID, "DirectConsole") )
						{
							// выполним смену текущего пользователя
							GrantsChangeUser(ThreadParams.UID,pProg);
							CMainWindow* pMainWindow = GetMainWindow();
							
							if(pMainWindow && pProg->GetFlag(FLG_START_WND) == SV_PROCESS)
							{// покажем основное окно
								CApp*	pApp = (CApp*)pMainWindow->getApp();
								CStr	DisplayName( pApp?(char*)pApp->DisplayName:"");
								::MsgSendAnswer( CStr("Консоль открыта ранее на ")+DisplayName, ThreadParams.AnswerQueueID, MSGA_READY, ThreadParams.MsgType, ThreadParams.MsgID);
								
								if(pMainWindow->isIconified()) pMainWindow->deiconify();
								pMainWindow->ActivateWindow();
							}
							else
							{
								::pProg->SetFlag(FLG_START_WND, SV_NEEDED);
								
								CStr	MsgText("Инициировано открытие консоли"); if( strlen(ThreadParams.MsgText) ) MsgText += CStr(" на ") + ThreadParams.MsgText;
								::MsgSendAnswer(MsgText, ThreadParams.AnswerQueueID, MSGA_READY, ThreadParams.MsgType, ThreadParams.MsgID);
							}
						}
						else
						{// отказано в доступе
							CStr	EventText("Недостаточно прав для открытия консоли");
							pProg->AddEvent(IDI_EVENT_ASTERISK,MakeError("Main", NULL, EventText),ThreadParams.UID);
							::MsgSendAnswer(EventText, ThreadParams.AnswerQueueID, MSGA_READY, ThreadParams.MsgType, ThreadParams.MsgID);
						} 							
					}
					else if( ThreadParams.MsgType == MSG_CLOSE_DISPLAY )
					{// закрыть управляющую консоль
						if( pProg->pSettings->bEnablePuts )
							pProg->AddEvent(IDI_EVENT_INFO,MakeError("Main", NULL, CStr("Поступила команда на закрытие консоли")),ThreadParams.UID);
						
						if( GrantsIsEnable(::pProg->pSettings->NodeName, ::pProg->GetTaskName(), ThreadParams.UID, "DirectConsole") )
						{
							if(::pProgApp)
							{
								StopProg(::pProg, SS_CONSOLE);
								::MsgSendAnswer("Выполнено закрытие консоли", ThreadParams.AnswerQueueID, MSGA_READY, ThreadParams.MsgType, ThreadParams.MsgID);
							}
							else ::MsgSendAnswer("Консоль закрыта ранее", ThreadParams.AnswerQueueID, MSGA_READY, ThreadParams.MsgType, ThreadParams.MsgID);
						}
						else
						{// отказано в доступе
							CStr	EventText( "Недостаточно прав для закрытия консоли");							
							pProg->AddEvent(IDI_EVENT_ASTERISK,MakeError("Main", NULL, EventText),ThreadParams.UID);
							::MsgSendAnswer(EventText, ThreadParams.AnswerQueueID, MSGA_READY, ThreadParams.MsgType, ThreadParams.MsgID);
						}
					}
					else if( ThreadParams.MsgType == MSG_SET_SETTING )
					{// установить новое значение для настроек *.ini файла
						if( pProg->pSettings->bEnablePuts )
							pProg->AddEvent(IDI_EVENT_INFO,MakeError("Main", NULL, CStr("Поступила команда на изменение настроек ") + ThreadParams.MsgText),ThreadParams.UID);
						if( GrantsIsEnable(::pProg->pSettings->NodeName, ::pProg->GetTaskName(), ThreadParams.UID, "EditSettings") )
						{
							::pProg->pSettings->PrepareMsg(ThreadParams.MsgText);
							::MsgSendAnswer(CStr("Выполнено изменение настройки ") + ThreadParams.MsgText, ThreadParams.AnswerQueueID, MSGA_READY, ThreadParams.MsgType, ThreadParams.MsgID);
						}
						else
						{
							CStr	EventText("Недостаточно прав для изменения настроек");							
							pProg->AddEvent(IDI_EVENT_ASTERISK,MakeError("Main", NULL, EventText),ThreadParams.UID);
							::MsgSendAnswer(EventText, ThreadParams.AnswerQueueID, MSGA_READY, ThreadParams.MsgType, ThreadParams.MsgID);
						} 
					}
					else if( ThreadParams.MsgType == MSG_INPUT_TEFF )
					{// запустить управляющую консоль
						if( pProg->pSettings->bEnablePuts )
							pProg->AddEvent(IDI_EVENT_INFO,MakeError("Main", NULL, CStr("Поступила команда на ввод Teff")),ThreadParams.UID);

						if(::pProgApp) StopProg(::pProg, SS_CONSOLE);

						::pProg->TeffInputDisplay = ThreadParams.MsgText;
						::pProg->SetFlag(FLG_INPUT_TEFF_USER_CALL,	SV_NEEDED);
						::pProg->SetFlag(FLG_INPUT_TEFF,			SV_NEEDED);
						SendMQMessage(::pProg->ExeName, ::pProg->GetTaskName(), MSG_DISPLAY, ThreadParams.MsgText);

						CStr MsgText("Инициирован ввод Teff"); if( strlen(ThreadParams.MsgText) ) MsgText += CStr(" на ") + ThreadParams.MsgText;
						::MsgSendAnswer(MsgText, ThreadParams.AnswerQueueID, MSGA_READY, ThreadParams.MsgType, ThreadParams.MsgID);
					}
					else if( ThreadParams.MsgType == MSG_UPDATE_CONSTANTS )
					{// загрузить с диска constants.idb
						if( pProg->pSettings->bEnablePuts )
							pProg->AddEvent(IDI_EVENT_INFO,MakeError("Main", NULL, CStr("Поступила команда на обновление constants.idb")),ThreadParams.UID);

						::pProg->SetFlag(FLG_UPDATE_CONSTANTS,	SV_NEEDED);

						CStr MsgText("Инициировано обновление constants.idb");
						::MsgSendAnswer(MsgText, ThreadParams.AnswerQueueID, MSGA_READY, ThreadParams.MsgType, ThreadParams.MsgID);
					}
					else ::MsgSendAnswer(CStr("Команда ") + ThreadParams.MsgType + " не поддерживается", ThreadParams.AnswerQueueID, MSGA_READY, ThreadParams.MsgType, ThreadParams.MsgID);
				}// конец просмотра очереди сообщений

				// ***************************************************************
				{// ДИСПЕТЧЕР ПОТОКОВ
					long  CurTime = CrTime();
					if( ::pProg->GetFlag(FLG_STOP_PROG)==SV_NEEDED )
					{// требуется прервать выполнение программы

						// культурно остановиться
						StopProg(::pProg, SS_ALL_THREADS);

						// Ответ в очередь ответов 
						if( ThreadParams.AnswerQueueID ) ::MsgSendAnswer("Останов завершен", ThreadParams.AnswerQueueID, MSGA_READY, ThreadParams.MsgType, ThreadParams.MsgID);

						// ВЫХОД ИЗ ОСНОВНОГО ЦИКЛА
						break; 
					}

					if( ::pProg->GetFlag(FLG_START_DIAGNOSTIC_THREAD) == SV_NEEDED &&
						(::pProg->pSettings->bEnableDiagnostic) &&
						(CurTime > PrevStartDiagnosticTime + 3) )
					{// запуск дигностического потока
						PrevStartDiagnosticTime = CurTime;
						StartThread(FLG_START_DIAGNOSTIC_THREAD, "Diagnostic", DiagnosticThread);
					}

					if( ::pProg->GetFlag(FLG_START_DOUT_THREAD) == SV_NEEDED &&
						(CurTime > PrevStartDoutTime + 3) )
					{// запуск потока вывода диагностики
						PrevStartDoutTime = CurTime;
						StartThread(FLG_START_DOUT_THREAD, "Dout", DoutThread);
					}

					if( ::pProg->GetFlag(FLG_START_WND)==SV_NEEDED &&
						(!::pProg->pSettings->bEnableScanData || ::pProg->GetFlag(FLG_INIT_SCAN_IDB)==SV_READY)  &&
						(!::pProg->pSettings->bEnableRegisterData || ::pProg->GetFlag(FLG_INIT_REG_IDB)==SV_READY))
					{// запуск консоли приложения
						CStartThread*	pStartThreadParams = new CStartThread;
						*pStartThreadParams = ThreadParams;
						sprintf(pStartThreadParams->MsgText, "%s %s", argv[0],(strlen(ThreadParams.MsgText)?ThreadParams.MsgText:" "));
						StartThread(FLG_START_WND, "Console", ShowApp, pStartThreadParams);
					} 

					if( ::pProg->GetFlag(FLG_INIT_CORE_THREAD_DATA)==SV_NEEDED &&
						::pProg->GetFlag(FLG_INIT_CALC)==SV_READY)
					{// запуск
						StartThread(FLG_INIT_CORE_THREAD_DATA, "InitCoreThreadData", InitCoreThreadData, NULL);
					}

					if( ::pProg->GetFlag(FLG_START_DIF_CALC)==SV_NEEDED &&
						::pProg->GetFlag(FLG_INIT_CORE_THREAD_DATA)==SV_READY &&
						::pProg->GetFlag(FLG_INIT_CALC)==SV_READY)
					{// запуск
						StartThread(FLG_START_DIF_CALC, "CoreDifCalc", CoreDifCalc, NULL);
					}

					if(::pProg->pSettings->bEnableRegisterData)
					{
						if( ::pProg->GetFlag(FLG_SYNC_REG_IDB)==SV_NEEDED )
						{// синхронизация ИБД регистрации данных в сервере
							StartThread(FLG_SYNC_REG_IDB, "SyncRegIdb", SyncRegIdb);
						}
						if( ::pProg->GetFlag(FLG_INIT_REG_IDB)==SV_NEEDED )
						{// инициализация ИБД регистрации данных в сервере
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
						{// синхронизация ИБД приёма данных от сервера
							StartThread(FLG_SYNC_SCAN_IDB, "SyncScanIdb", SyncScanIdb);
						}
						if( ::pProg->GetFlag(FLG_INIT_SCAN_IDB)==SV_NEEDED )
						{// инициализация ИБД приёма данных от сервера
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
					{// инициализация расчётной части
						StartThread(FLG_INIT_CALC, "InitCalc", InitCalc);
					}

					if( ::pProg->GetFlag(FLG_START_SYNC_EXCHANGE) == SV_NEEDED &&
						(CurTime > PrevStartSyncExchangeTime + 3) )
					{// запустить поток обмена с SyncAgent
						PrevStartSyncExchangeTime = CurTime;
						StartThread(FLG_START_SYNC_EXCHANGE, "SyncExchange", SyncExchangeThread, NULL);
					}
				}// КОНЕЦ ДИСПЕТЧЕРА


				{// дадим дыхнуть остальным программам, если нам делать нечего
					ThreadYield();
				}
			}
		}
		else
		{
			printf("MsgQueueID=%d, SemArrayID=%d\n", pProg->MsgQueueID, pProg->SemArrayID); // сообщения были выданы в протокол ранее
		}

		delete pProg;
	}
	else if( Result == 2 )
	{// если повторный запуск программы
		puts(MakeError("Main", "60308", "Программа уже запущена"));
	}
	else if( Result == 3 )
	{// неверно установлена переменная SRVHOME
		puts(MakeError("Main", "60309", "Неверно установлена переменная окружения SRVHOME"));
	}
	else if( Result == -1 )
	{// ошибки при чтении из канала
		puts(MakeError("Main", "60310", CStr("Ошибка при обработке команды pgrep - ") + strerror(errno)));
	}

	return 0;
}




///////////////////////////////////////////////////////////////////////////
// запуск ProgApp
void* ShowApp(void* pStartThreadParams)
{
#ifdef __sparc
	SetAbortProc(AbortProg);
#endif
	bool bInputTeff = ::pProg->GetFlag(FLG_INPUT_TEFF)==SV_NEEDED  ? true : false;

	CStartThread*  pStartParams 	= (CStartThread*)pStartThreadParams;
	// формируем массив параметров командной строки
	char	Path[256];	Path[0]	= 0;
	char	Host[256];	Host[0]	= 0; sscanf(pStartParams->MsgText, "%s %s", Path, Host);
	int		Argc				= 3;
	const char*	ppArgv[4];

	if(!StrLen(Host))
	{// нет хоста стартуем на текущем терминале
		StrCpy(Host, getenv("DISPLAY"));
	}		

	if(!StrStr(Host, ":")) StrCat(Host, ":0.0");

	ppArgv[0] = Path;
	ppArgv[1] = "-display";
	ppArgv[2] = Host;
	ppArgv[3] = "\0" ;

	delete pStartParams; // т.к. рождено по new  в Main

	if(CheckXServer(Host))
	{
		// установим глобальный флаг выполнения App потоком
		::fxthreadapp	= 1;
		// создаем приложение
		CProgApp* pLocApp = new CProgApp(Host);
		pLocApp->init(Argc, (char**)ppArgv);

		if(bInputTeff)	new CInputTeffWnd(pLocApp);
		else			new CProgWnd(pLocApp);

		pLocApp->create();
		::pProgApp = pLocApp;
		::pProg->AddEvent(IDI_EVENT_INFO,MakeError("Console", NULL, "Старт потока"));
		::pProgApp->run();
		::pProg->AddEvent(IDI_EVENT_INFO, MakeError("Console", NULL, "Останов потока"));
		delete ::pProgApp;
		::pProgApp = NULL;
	}
	else ::pProg->AddEvent(IDI_EVENT_ERROR, MakeError("Console", NULL, CStr("Прервано выполнение потока. Дисплей \"")+Host+"\" недоступен." ));
	::pProg->SetFlag(FLG_START_WND, SV_READY);
	
	ThreadExit(0);
	
	return NULL;
}

void* InitRegIdb(void* pStartThreadParams)
{// инициализация ИБД регистрации данных в сервере
#ifdef __sparc
	SetAbortProc(AbortProg);
#endif

	CStr	ErrorText;

	const char* pThreadName = "InitRegIdb";

	::pidInitRegIdb = ThreadPID();
	::pProg->AddEvent(IDI_EVENT_INFO, MakeError(pThreadName, NULL, "Старт потока"));

	if( ::pProg->pRegIdb->InitIdb(&ErrorText) )
	{
		::pProg->AddEvent(IDI_EVENT_INFO, MakeError(pThreadName, NULL, CStr("Останов потока. Инициализация ИБД выполнена. Загружено ") + ::pProg->pRegIdb->pParamLst->GetCnt() + ", отвергнуто " + ::pProg->pRegIdb->RejectParamCnt + " переменных"));
		::pProg->SetFlag(FLG_INIT_REG_IDB, SV_READY);
	}
	else
	{
		::ErrorInitRegIdb = ErrorText;
		::pProg->AddEvent(IDI_EVENT_ERROR, MakeError(pThreadName, NULL, CStr("Прервано выполнение потока (") + ErrorText + ")"));
		::pProg->SetFlag(FLG_INIT_REG_IDB, SV_ERROR);
	}

	::pidInitRegIdb = 0;
	
	ThreadExit(0);
	
	return NULL;
}

void* SyncRegIdb(void* pStartThreadParams)
{// синхронизация ИБД регистрации данных в сервере
#ifdef __sparc
	SetAbortProc(AbortProg);
#endif
	short 		SyncResult 	= 0;
	CStr		ErrorText;

	const char* pThreadName = "SyncRegDataIdb";

	::pidSyncRegIdb	= ThreadPID();

	::pProg->AddEvent(IDI_EVENT_INFO, MakeError(pThreadName, NULL, "Старт потока"));

	long MainServerLen		= ::pProg->pSettings->MainServer.GetLength();
	long ReservServerLen	= ::pProg->pSettings->ReservServer.GetLength();

	::pProg->pRegIdb->SyncServerName = "";

	if(MainServerLen || ReservServerLen)
	{
		if(MainServerLen)
		{// основной сервер данных
			SyncResult = ::pProg->pRegIdb->SyncIdb(::pProg->pSettings->MainServer, &ErrorText);
			if(SyncResult) ::pProg->pRegIdb->SyncServerName = ::pProg->pSettings->MainServer;
			else if(ReservServerLen) ::pProg->AddEvent(IDI_EVENT_ASTERISK, MakeError(pThreadName, NULL, ErrorText));
		}
		if(!SyncResult && ReservServerLen)
		{// резервный сервер данных
			SyncResult 	= ::pProg->pRegIdb->SyncIdb(::pProg->pSettings->ReservServer, &ErrorText);
			::pProg->pRegIdb->SyncServerName = ::pProg->pSettings->ReservServer;
		}

		if(SyncResult)
		{
			::pProg->AddEvent(IDI_EVENT_INFO, MakeError(pThreadName, NULL, CStr("Останов потока") + ". Синхронизация ИБД выполнена с сервера " + ::pProg->pRegIdb->SyncServerName));
			::pProg->SetFlag(FLG_SYNC_REG_IDB, SV_READY);
		}
		else
		{
			::ErrorSyncRegIdb = ErrorText;
			::pProg->AddEvent(IDI_EVENT_ASTERISK, MakeError(pThreadName, NULL, (char*)(CStr("Прервано выполнение потока (") + ErrorText + ")") ));
			::pProg->SetFlag(FLG_SYNC_REG_IDB, SV_ERROR);
		}

		// запускаем инициализацию ИБД
		::pProg->SetFlag(FLG_INIT_REG_IDB, SV_NEEDED);
	}
	else
	{
		::ErrorSyncRegIdb = "Не указаны имена основного и резервного серверов данных.";
		::pProg->AddEvent(IDI_EVENT_ERROR, MakeError(pThreadName, NULL, CStr("Прервано выполнение потока (") + MakeError(pThreadName, "????", ::ErrorSyncRegIdb)+")"));
		::pProg->SetFlag(FLG_SYNC_REG_IDB, SV_ERROR);
	}

	::pidSyncRegIdb = 0;

	ThreadExit(0);

	return NULL;
}

void* RegData(void* pStartThreadParams)
{// регистрация и сохранение данных
#ifdef __sparc
	SetAbortProc(AbortProg);
#endif

	CRegisterServer* pServer = (CRegisterServer*)pStartThreadParams;

	::pProg->SetFlag(pServer->RegThreadFlgStop, SV_READY);
	pServer->RegThreadPid = ThreadPID();
	::pProg->AddEvent(IDI_EVENT_INFO, MakeError(pServer->RegThreadName, NULL, "Старт потока"));
	if(!::pProg->RegThread(pServer))
	{
		::pProg->AddEvent(IDI_EVENT_ERROR, MakeError(pServer->RegThreadName, NULL, (char*)(CStr("Прервано выполнение потока (") + pServer->Error + ")")));
		pServer->RegThreadPid = 0;

		// повторный запуск регистрации
		::pProg->SetFlag(pServer->RegThreadFlgStart, SV_NEEDED);
	}
	else ::pProg->AddEvent(IDI_EVENT_INFO, MakeError(pServer->RegThreadName, NULL, "Останов потока"));
	pServer->RegThreadPid = 0;

	::pProg->SetFlag(pServer->RegThreadFlgStop, SV_READY);

	ThreadExit(0);

	return NULL;
}

void* InitScanIdb(void* pStartThreadParams)
{// инициализация ИБД приёма данных от сервера
#ifdef __sparc
	SetAbortProc(AbortProg);
#endif

	CStr	ErrorText;

	const char* pThreadName = "InitScanIdb";

	::pidInitScanIdb = ThreadPID();
	::pProg->AddEvent(IDI_EVENT_INFO, MakeError(pThreadName, NULL, "Старт потока"));

	if( ::pProg->pScanIdb->InitIdb(&ErrorText) )
	{
		::pProg->AddEvent(IDI_EVENT_INFO, MakeError(pThreadName, NULL, CStr("Останов потока. Инициализация ИБД выполнена. Загружено ") + ::pProg->pScanIdb->pParamLst->GetCnt() + ", отвергнуто " + ::pProg->pScanIdb->RejectParamCnt + " переменных"));
		::pProg->SetFlag(FLG_INIT_SCAN_IDB, SV_READY);
	}
	else
	{
		::ErrorInitScanIdb = ErrorText;
		::pProg->AddEvent(IDI_EVENT_ERROR, MakeError(pThreadName, NULL, CStr("Прервано выполнение потока (") + ErrorText + ")"));
		::pProg->SetFlag(FLG_INIT_SCAN_IDB, SV_ERROR);
	}

	::pidInitScanIdb = 0;
	ThreadExit(0);
	return NULL;
}

void* SyncScanIdb(void* pStartThreadParams)
{// синхронизация ИБД приёма данных от сервера
#ifdef __sparc
	SetAbortProc(AbortProg);
#endif
	short 		SyncResult 	= 0;
	CStr		ErrorText;

	const char* pThreadName = "SyncScanDataIdb";

	::pidSyncScanIdb	= ThreadPID();

	::pProg->AddEvent(IDI_EVENT_INFO, MakeError(pThreadName, NULL, "Старт потока"));

	long MainServerLen		= ::pProg->pSettings->MainServer.GetLength();
	long ReservServerLen	= ::pProg->pSettings->ReservServer.GetLength();

	::pProg->pScanIdb->SyncServerName = "";

	if(MainServerLen || ReservServerLen)
	{
		if(MainServerLen)
		{// основной сервер данных
			SyncResult = ::pProg->pScanIdb->SyncIdb(::pProg->pSettings->MainServer, &ErrorText);
			if(SyncResult) ::pProg->pScanIdb->SyncServerName = ::pProg->pSettings->MainServer;
			else if(ReservServerLen) ::pProg->AddEvent(IDI_EVENT_ASTERISK, MakeError(pThreadName, NULL, ErrorText));
		}
		if(!SyncResult && ReservServerLen)
		{// резервный сервер данных
			SyncResult 	= ::pProg->pScanIdb->SyncIdb(::pProg->pSettings->ReservServer, &ErrorText);
			::pProg->pScanIdb->SyncServerName = ::pProg->pSettings->ReservServer;
		}

		if(SyncResult)
		{
			::pProg->AddEvent(IDI_EVENT_INFO, MakeError(pThreadName, NULL, CStr("Останов потока") + ". Синхронизация ИБД выполнена с сервера " + ::pProg->pScanIdb->SyncServerName));
			::pProg->SetFlag(FLG_SYNC_SCAN_IDB, SV_READY);
		}
		else
		{
			::ErrorSyncScanIdb = ErrorText;
			::pProg->AddEvent(IDI_EVENT_ASTERISK, MakeError(pThreadName, NULL, (char*)(CStr("Прервано выполнение потока (") + ErrorText + ")") ));
			::pProg->SetFlag(FLG_SYNC_SCAN_IDB, SV_ERROR);
		}

		// запускаем инициализацию ИБД
		::pProg->SetFlag(FLG_INIT_SCAN_IDB, SV_NEEDED);
	}
	else
	{
		::ErrorSyncScanIdb = "Не указаны имена основного и резервного серверов данных.";
		::pProg->AddEvent(IDI_EVENT_ERROR, MakeError(pThreadName, NULL, CStr("Прервано выполнение потока (") + MakeError(pThreadName, "????", ::ErrorSyncScanIdb)+")"));
		::pProg->SetFlag(FLG_SYNC_SCAN_IDB, SV_ERROR);
	}

	::pidSyncScanIdb = 0;

	ThreadExit(0);

	return NULL;
}

void* ScanData(void* pStartThreadParams)
{// приём данных от сервера
#ifdef __sparc
	SetAbortProc(AbortProg);
#endif

	CStr	ErrorText;

	const char* pThreadName = "ScanData";

	::pProg->SetFlag(FLG_STOP_SCAN_THREAD, SV_READY);

	::pidScanData = ThreadPID();
	::pProg->AddEvent(IDI_EVENT_INFO, MakeError(pThreadName, NULL, "Старт потока"));
	if(!::pProg->ScanThread(pThreadName, &ErrorText) )
	{
		::ErrorScanData = CStr("Прервано выполнение потока (") + ErrorText + ")";
		::pProg->AddEvent(IDI_EVENT_ERROR, MakeError(pThreadName, NULL, CStr("Прервано выполнение потока (") + ErrorText + ")"));
		::pidScanData = 0;

		// повторный запуск 
		::pProg->SetFlag(FLG_START_SCAN_THREAD, SV_NEEDED);
	}
	else ::pProg->AddEvent(IDI_EVENT_INFO, MakeError(pThreadName, NULL, "Останов потока"));
	::pidScanData = 0;

	::pProg->SetFlag(FLG_STOP_SCAN_THREAD, SV_READY);

	ThreadExit(0);
	
	return NULL;
}

void* PrepareData(void* ptr)
{// поток подготовки принятых от сервера данных для передачи в систему расчётов
#ifdef __sparc
	SetAbortProc(AbortProg);
#endif
	CStr	ErrorText;

	const char* pThreadName = "PrepareData";

	::pProg->SetFlag(FLG_STOP_PREPARE_DATA_THREAD, SV_READY);

	::pidPrepareData = ThreadPID();
	::pProg->AddEvent(IDI_EVENT_INFO, MakeError(pThreadName, NULL, "Старт потока"));


	short Result = ::pProg->PrepareDataThread(pThreadName, &ErrorText);
	if(Result <= 0)
	{
		::ErrorPrepareData = CStr("Прервано выполнение потока (") + ErrorText + ")";
		::pProg->AddEvent(IDI_EVENT_ERROR, MakeError(pThreadName, NULL, CStr("Прервано выполнение потока (") + ErrorText + ")"));
		::pidPrepareData = 0;

		if(!Result)
		{// повторный запуск
			::pProg->SetFlag(FLG_START_PREPARE_DATA_THREAD, SV_NEEDED);
		}
	}
	else ::pProg->AddEvent(IDI_EVENT_INFO, MakeError(pThreadName, NULL, "Останов потока"));
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
	::pProg->AddEvent(IDI_EVENT_INFO, MakeError("SyncExchange", NULL, "Старт потока"));

	double	dPrevCurrentTime	= GetCurrentTime(false);
	double	dPrevDeltaTime		= 0;

	while(true)
	{
		if(	::pProg->GetFlag(FLG_STOP_SYNC_EXCHANGE)!=SV_NEEDED && ::pProg->GetFlag(FLG_RUN_SYNC_EXCHANGE)!=SV_PROCESS )
		{// если надо передать диагностику
			double 	dDeltaTime  		= TestCurrentTime(dPrevCurrentTime);

			if( dDeltaTime!=0 && (dPrevDeltaTime==0 || (dDeltaTime>0 && dPrevDeltaTime<0) || (dDeltaTime<0 && dPrevDeltaTime>0) ))
			{// произошел перевод времени !!! принимаем меры по компенсации перевода времени
				::pProg->AddEvent(IDI_EVENT_ASTERISK, MakeError("SyncExchange", NULL, CStr("Переведено системное время на ") + (int)(dDeltaTime) + " мсек."));
			}
			dPrevDeltaTime	 = dDeltaTime;
			dPrevCurrentTime = GetCurrentTime(false);

			::pProg->SetFlag(FLG_RUN_SYNC_EXCHANGE, SV_PROCESS);

			// выполним обмен с SyncAgent
			::pProg->SyncScanShareMem();

			CrSleep(50000);
			::pProg->SetFlag(FLG_RUN_SYNC_EXCHANGE, SV_READY);
		}
		else if(::pProg->GetFlag(FLG_STOP_SYNC_EXCHANGE) == SV_NEEDED)
		{// надо выгружаться
			break;
		}
		else ThreadYield();
	}

	::pProg->AddEvent(IDI_EVENT_INFO, MakeError("SyncExchange", NULL, "Останов потока"));
	::pidSyncExchange = 0;
	::SetPrevCurrentTime(0);
	::pProg->SetFlag(FLG_STOP_SYNC_EXCHANGE, SV_READY);

	ThreadExit(0);

	return NULL;
}

void* InitCalc(void* pStartThreadParams)
{// инициализация расчётной части
#ifdef __sparc
	SetAbortProc(AbortProg);
#endif

	const char* pThreadName = "InitCalc";

	::pidInitCalc = ThreadPID();
	::pProg->AddEvent(IDI_EVENT_INFO, MakeError(pThreadName, NULL, "Старт потока"));

	::pProg->pCalc->Init();
    ::pProg->pAkz->init();

	::pProg->AddEvent(IDI_EVENT_INFO, MakeError(pThreadName, NULL, "Останов потока. Инициализация расчётной части выполнена"));
	::pProg->SetFlag(FLG_INIT_CALC, SV_READY);
	::pidInitCalc = 0;

	::pProg->SetFlag(FLG_INIT_CORE_THREAD_DATA, SV_NEEDED);

	ThreadExit(0);

	return NULL;
}

void* InitCoreThreadData(void* pStartThreadParams)
{// инициализация данных для потоков Rebuild и DifCalc
#ifdef __sparc
	SetAbortProc(AbortProg);
#endif

	const char* pThreadName = "InitCoreThreadData";

	::pidInitCore = ThreadPID();
	::pProg->AddEvent(IDI_EVENT_INFO, MakeError(pThreadName, NULL, "Старт потока"));

    ::pProg->pAkz->initCalc();

	::pProg->AddEvent(IDI_EVENT_INFO, MakeError(pThreadName, NULL, "Останов потока. Инициализация данных для потоков Rebuild и DifCalc выполнена"));
	::pProg->SetFlag(FLG_INIT_CORE_THREAD_DATA, SV_READY);
	::pidInitCore = 0;

	::pProg->SetFlag(FLG_START_DIF_CALC, SV_NEEDED);

	::pidInitCore = 0;

	ThreadExit(0);

	return NULL;
}

void* CoreDifCalc(void* ptr)
{// диффузионный расчёт
	const char* pThreadName = "DifCalc";

	::pidCoreDifCalc = ThreadPID();
	::pProg->AddEvent(IDI_EVENT_INFO, MakeError(pThreadName, NULL, "Старт потока"));

    ::pProg->pAkz->threadDU();

	::pProg->AddEvent(IDI_EVENT_INFO, MakeError(pThreadName, NULL, "Останов потока"));

    ::pProg->SetFlag(FLG_STOP_DIF_CALC, SV_READY);
	::pidCoreDifCalc = 0;
	ThreadExit(0);
	return NULL;
}
