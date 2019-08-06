#include <ImCorePMain.h>
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
int		pidDiagnostic	= 0;
int		pidDout			= 0;
int		pidSyncExchange	= 0;
int		pidNetwork		= 0;

CStr	ErrorDiagnostic;
CStr	ErrorDout;
CStr	ErrorNetwork;

void	StopProg(CProg* pLocalProg, bool bStopNetThread, bool bStopDiag);

CStr	MakeError(const char* pNameThread, const char*  pErrorNum, const char*  pMessageText)
{// лепит сообщение 
	return MakeModuleError( pProg?pProg->GetTaskName():"ImCoreP", (pProg?(char*)pProg->pSettings->NodeName:NULL), pNameThread, pErrorNum, pMessageText);
}	

void AbortProg(int SignalNum)
{// аварийная функция Prog
	int	CurPid = ThreadPID();
//	printf("ImCoreP pid=%d, signal=%s\n", CurPid, GetSignalName(SignalNum));
#ifdef __sparc
	printf("ImCoreP pid=%d, signal=%s\n", CurPid, GetSignalName(SignalNum));
	SetAbortProc(AbortProg);
	if(SignalNum==SIGALRM || SignalNum==SIGCHLD || SignalNum==SIGWINCH) return;
#endif
	if(SignalNum==SIGHUP) return;

	if( ::pProg && ::pProg->pid==CurPid )
	{// если умер MAIN
		::bAbortProgram		= true;
		CProg*	pLocalProg	= ::pProg;
		pLocalProg->AddEvent(IDI_EVENT_ERROR, MakeError("Main", "60300", CStr("Аварийный останов потока. Сигнал ") + GetSignalName(SignalNum)));

		pLocalProg->SetFlag(FLG_STOP_PROG, SV_NEEDED);
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
	else if( ::pProg && ::pidNetwork==CurPid )
	{// поток приёма сетевых соединений
		if( !bAbortProgram && SignalNum!=SIGINT)
		{
			::pidNetwork = 0;
			if( SignalNum == SIGTERM )
			{// если поток останавливается принудительно из StopProg
				::pProg->AddEvent(IDI_EVENT_INFO, MakeError("Network", NULL, "Останов потока"));
			}
			else
			{
				::pProg->AddEvent(IDI_EVENT_ERROR, MakeError("Network", "60303", CStr("Аварийный останов потока. Сигнал ") + GetSignalName(SignalNum)));
				::pProg->SetFlag(FLG_START_NET_THREAD, SV_ERROR);
				// повторный запуск
				::pProg->SetFlag(FLG_START_NET_THREAD, SV_NEEDED);
			}
		}
		else return;
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
		CPredictClient* pClient = NULL;
		int ClientCnt = ::pProg->pClientLst->GetCnt();
		for(int ClientPos=1 ; ClientPos<=ClientCnt ; ClientPos++)
		{
			CPredictClient* pTmpClient = (CPredictClient*)::pProg->pClientLst->Get(ClientPos);
			if(pTmpClient->ThreadPid == CurPid){ pClient = pTmpClient; break; }
		}
		if(pClient)
		{
			if((!bAbortProgram && SignalNum!=SIGINT) || SignalNum==SIGALRM)
			{
				pClient->ThreadPid = 0;
				if(SignalNum==SIGTERM || SignalNum==SIGALRM)
				{// если поток останавливается принудительно
					::pProg->AddEvent(IDI_EVENT_INFO, MakeError(pClient->ThreadName, NULL, "Останов потока"));
				}
				else
				{
					::pProg->AddEvent(IDI_EVENT_ERROR, MakeError(pClient->ThreadName, "?????", CStr("Аварийный останов потока. Сигнал ") + GetSignalName(SignalNum)));
					::pProg->SetFlag(pClient->ThreadFlgStart, SV_ERROR);
					// повторный запуск
					::pProg->SetFlag(pClient->ThreadFlgStart, SV_NEEDED);
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

	if(StopStyle&SS_NETWORK && ::pidNetwork)
	{// остановим потока приёма сетевых соединений
		pLocalProg->SetFlag(FLG_STOP_NET_THREAD, SV_NEEDED);
		double  dStartTime 	= (double)::GetCurrentTime();
		while(::pidNetwork && (double)::GetCurrentTime()-dStartTime<=2000) ThreadYield();
		if(::pidNetwork) ThreadKill(::pidNetwork, SIGTERM);
		pLocalProg->SetFlag(FLG_STOP_NET_THREAD, SV_READY);
	}

	if(StopStyle & SS_CLIENT)
	{// остановим потоки работы с клиентами
		int ClientCnt = ::pProg->pClientLst->GetCnt();
		for(int ClientPos=1 ; ClientPos<=ClientCnt ; ClientPos++)
		{
			CPredictClient* pClient = (CPredictClient*)::pProg->pClientLst->Get(ClientPos);
			if(pClient->ThreadPid)
			{
				pLocalProg->SetFlag(pClient->ThreadFlgStop, SV_NEEDED);
				double dStartTime = (double)GetCurrentTime();
				while(pClient->ThreadPid && (double)GetCurrentTime()-dStartTime<=2000 ) ThreadYield();
				if(pClient->ThreadPid) ThreadKill(pClient->ThreadPid, SIGTERM);
				pLocalProg->SetFlag(pClient->ThreadFlgStop, SV_READY);
			}
		}
	}

	if(StopStyle&SS_DOUT && ::pidDout)
	{// остановим поток вывода диагностики
		pLocalProg->SetFlag(FLG_STOP_DOUT_THREAD, SV_NEEDED);
		double  dStartTime 	= (double)::GetCurrentTime();
		while(::pidDout && (double)::GetCurrentTime()-dStartTime<=2000) ThreadYield();
		if(::pidDout) ThreadKill(::pidDout, SIGTERM);
		pLocalProg->SetFlag(FLG_STOP_DOUT_THREAD, SV_READY);
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
	short Result = ::IsLoadTask(ThreadPID(false),"SRVHOME", "ImCoreP", argc>=2?argv[1]:NULL);

	if(Result == 1)
	{// если прошли проверку - продолжаем работать
		CProg*	pProg = new CProg(CStr(getenv("SRVHOME")) + "/ImCoreP", argc>=2?argv[1]:NULL);
		CProgSettings*	pSettings					= pProg->pSettings;
		int				PrevStartDiagnosticTime		= 0;
		int				PrevStartDoutTime			= 0;
		int				PrevStartSyncExchangeTime	= 0;
		int				PrevStartNetworkTime		= 0;

		bool			bEnableDirect = false;
		CStr			strUID(GrantsGetUID());
		CStartThread	ThreadParams;

		if( !::GrantsIsValid() || GrantsIsEnable(pSettings->NodeName,::pProg->GetTaskName(), strUID, "DirectModule") )
			bEnableDirect = true;
		else pProg->AddEvent(IDI_EVENT_ASTERISK, MakeError("Main", NULL, "Не достаточно прав для запуска программы"), (char*)strUID);
		
		if(bEnableDirect && pProg->MsgQueueID>=0 && pProg->SemArrayID>=0)
		{// если создали системные объекты

			// проинициализируем семафоры
			::pProg->SetFlag(FLG_START_DIAGNOSTIC_THREAD,	SV_NEEDED);
			::pProg->SetFlag(FLG_START_DOUT_THREAD,			SV_NEEDED);
			::pProg->SetFlag(FLG_START_SYNC_EXCHANGE,		SV_NEEDED);
			::pProg->SetFlag(FLG_START_NET_THREAD,			SV_NEEDED);

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

					if( ::pProg->GetFlag(FLG_START_NET_THREAD) == SV_NEEDED &&
						(CurTime > PrevStartNetworkTime + 3) )
					{// запуск потока приёма сетевых соединений
						PrevStartNetworkTime = CurTime;
						StartThread(FLG_START_NET_THREAD, "Network", NetThread);
					}

					if( ::pProg->GetFlag(FLG_START_WND)==SV_NEEDED )
					{// запуск консоли приложения
						CStartThread*	pStartThreadParams = new CStartThread;
						*pStartThreadParams = ThreadParams;
						sprintf(pStartThreadParams->MsgText, "%s %s", argv[0],(strlen(ThreadParams.MsgText)?ThreadParams.MsgText:" "));
						StartThread(FLG_START_WND, "Console", ShowApp, pStartThreadParams);
					} 

					if( ::pProg->GetFlag(FLG_START_SYNC_EXCHANGE) == SV_NEEDED &&
						(CurTime > PrevStartSyncExchangeTime + 3) )
					{// запустить поток обмена с SyncAgent
						PrevStartSyncExchangeTime = CurTime;
						StartThread(FLG_START_SYNC_EXCHANGE, "SyncExchange", SyncExchangeThread, NULL);
					}

					int ClientCnt = ::pProg->pClientLst->GetCnt();
					for(int ClientPos=1 ; ClientPos<=ClientCnt ; ClientPos++)
					{// запустим потоки обработки клиентов
						CPredictClient*	pClient = (CPredictClient*)::pProg->pClientLst->Get(ClientPos);
						if(!pClient->ThreadPid && ::pProg->GetFlag(pClient->ThreadFlgStart)==SV_NEEDED)
							StartThread(pClient->ThreadFlgStart, pClient->ThreadName, PredictClientThread, pClient);
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

		new CProgWnd(pLocApp);

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

void* NetThread(void* pStartThreadParams)
{// поток приёма сетевых соединений
#ifdef __sparc
	SetAbortProc(AbortProg);
#endif
	CStr	ErrorText;

	const char* pThreadName = "Network";

	::pProg->SetFlag(FLG_STOP_NET_THREAD, SV_READY);

	::pidNetwork = ThreadPID();
	::pProg->AddEvent(IDI_EVENT_INFO, MakeError(pThreadName, NULL, "Старт потока"));
	if(!::pProg->NetThread(pThreadName, &ErrorText) )
	{
		::ErrorNetwork = CStr("Прервано выполнение потока (") + ErrorText + ")";
		::pProg->AddEvent(IDI_EVENT_ERROR, MakeError(pThreadName, NULL, CStr("Прервано выполнение потока (") + ErrorText + ")"));
		::pidNetwork = 0;

		// повторный запуск
		::pProg->SetFlag(FLG_START_NET_THREAD, SV_NEEDED);
	}
	else ::pProg->AddEvent(IDI_EVENT_INFO, MakeError(pThreadName, NULL, "Останов потока"));
	::pidNetwork = 0;

	::pProg->SetFlag(FLG_STOP_NET_THREAD, SV_READY);

	ThreadExit(0);

	return NULL;
}

void* PredictClientThread(void* pStartThreadParams)
{// поток работы с клиентом
#ifdef __sparc
	SetAbortProc(AbortGate);
#endif
	CPredictClient* pClient = (CPredictClient*)pStartThreadParams;

	::pProg->SetFlag(pClient->ThreadFlgStop, SV_READY);
	pClient->ThreadPid = ThreadPID();
	::pProg->AddEvent(IDI_EVENT_INFO, MakeError(pClient->ThreadName, NULL, "Старт потока"));
	if(!pClient->ClientThread())
	{
		::pProg->AddEvent(IDI_EVENT_ERROR, MakeError(pClient->ThreadName, NULL, (char*)(CStr("Прервано выполнение потока (") + pClient->ThreadError + ")")));
		pClient->ThreadPid = 0;

		// повторный запуск регистрации
		::pProg->SetFlag(pClient->ThreadFlgStart, SV_NEEDED);
	}
	else ::pProg->AddEvent(IDI_EVENT_INFO, MakeError(pClient->ThreadName, NULL, "Останов потока"));
	pClient->ThreadPid = 0;

	::pProg->SetFlag(pClient->ThreadFlgStop, SV_READY);

	if(::pProg->pClientLst->Lock())
	{
		int FindPos = ::pProg->pClientLst->Find((long)pClient);
		if(FindPos > 0) ::pProg->pClientLst->Del(FindPos);
		::pProg->pClientLst->Unlock();
	}
	delete pClient;

	ThreadExit(0);

	return NULL;
}
