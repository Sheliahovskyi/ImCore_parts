#include "ImCoreP.h"

//////////////////////////////////////////////////////
// настраиваемые параметры шлюза
CProgSettings::CProgSettings(FXRegistry* pGateRegistry) : CSettings(pGateRegistry)
{
	Add( new CBoolSetting( &bEnableSettings,		true, 					ENABLE_CHANGE,  "Mode",  	"EnableSettings",		 	" разрешить просмотр и редактирование настроек"));
	Add( new CBoolSetting( &bEnableDiagnostic,		true, 					DISABLE_CHANGE, "Mode",		"EnableDiagnostic",  		" разрешить выполнение внутренней диагностики"));

	Add( new CPathSetting( &LogPath, 				"$SRVHOME/log", 		DISABLE_CHANGE, "Path",		"LogPath", 					" путь к файлам журналов *.log"));
	Add( new CPathSetting( &CfgPath, 				"$SRVHOME/cfg", 		DISABLE_CHANGE, "Path",		"CfgPath", 					" путь к конфигурационным файлам"));
	Add( new CStrSetting(  &FilePrefix,				"", 					DISABLE_CHANGE, "Path",		"FilePrefix",				" префикс используемых файлов"));

	Add( new CIntSetting(  &MaxEventsCnt, 			1000, 					ENABLE_CHANGE,  "Log",  	"MaxEventCnt", 				" максимальное количество событий в журнале *.log"));
	Add( new CBoolSetting( &bEnablePuts,			true, 					ENABLE_CHANGE,  "Log",  	"EnableTerminalOutput", 	" разрешить дублирование событий из журнала в терминале"));

	Add( new CStrSetting(  &NodeName, 				"localhost", 			DISABLE_CHANGE, "ImCoreP", 	"NodeName", 				" имя компьютерного узла"));
	Add( new CStrSetting(  &ModuleName,				"Программа прогнозных расчётов",ENABLE_CHANGE,  "ImCoreP", 	"ModuleName", 		" имя модуля (выводится на лепестке диагностики)"));
	Add( new CStrSetting(  &CompIdent,				"GB",					DISABLE_CHANGE, "ImCoreP", 	"CompIdent", 				" префикс диагностических переменных для модуля"));
	Add( new CIntSetting(  &DiagInterval, 			1, 						ENABLE_CHANGE,  "ImCoreP",	"DiagInterval", 			" диагностический интервал (сек.)"));
	Add( new CIntSetting(  &AutoUnloginTimeout, 	10,						ENABLE_CHANGE,  "ImCoreP",	"AutoUnloginTimeout",		" timeout автоматического отключения пользователя (мин.)"));
	Add( new CStrSetting(  &SrvHost,				"0.0.0.0",				DISABLE_CHANGE,	"ImCoreP",	"Host", 					" адрес для приёма заданий прогноза"));
	Add( new CIntSetting(  &SrvPort, 				20000,					DISABLE_CHANGE, "ImCoreP",	"Port", 					" порт для приёма заданий прогноза"));
}
CProgSettings::~CProgSettings(void){}
bool CProgSettings::SetValue(const char* pSectionName, const char* pSettingName, const char* pStrValue)
{
	return CSettings::SetValue(pSectionName,pSettingName,pStrValue);
}


//////////////////////////////////////////////////////////////////////
// class CProg
CProg::CProg(const char* pPath, const char* pTaskName) : CCore(pPath, MAX_SEM_CNT, pTaskName,STYLE_OFFICE_2003|STYLE_ENABLE_MUTEX_ARRAY)
{
	CStr	StrError;

	// присвоим значение глобальной переменной
	::pProg 					= this;
	CProg::SrvSocket			= -1;

	// создаем реестр
	CProg::pRegistry			= new FXRegistry( (char*)(CStr(GetTaskName()) + ".ini"), NULL);

	CProg::pDiagTbl->SetLockOptions(SemArrayID, SEM_DIGNOSTIC_TBL, bEnableMutexArray);

	// создаем и загружаем настройки
	CProg::pSettings			= new CProgSettings(pRegistry);
	CProg::pSettings->SetLockOptions(SemArrayID, SEM_SETTINGS,bEnableMutexArray);
	CProg::pSettings->Load();
	CCore::pAutoUnloginTimeout	= &pSettings->AutoUnloginTimeout;
	CCore::pNodeName			= &pSettings->NodeName;
	if( pSettings->NodeName=="localhost" ) pSettings->NodeName = ::NetGetHostName();

	// слепим протокол
	CStr LogName				= CStr(pSettings->LogPath) + "/" + pSettings->FilePrefix + "ImCoreP.log";
	CProg::pProtocol			= new CProtocol(LogName, (long*)&pSettings->MaxEventsCnt, &pSettings->bEnablePuts,true);
	CProg::pProtocol->MakeColumns();
	CProg::pProtocol->SetLockOptions(SemArrayID, SEM_PROTOCOL,bEnableMutexArray);
	if( !CProg::pProtocol->ReadAllFile(&StrError) ) AddEvent(IDI_EVENT_ERROR, StrError);

	// заявим о запуске программы
	AddEvent(IDI_EVENT_INFO, MakeError(NULL, NULL, CStr("--- Старт программы. Версия ")+MakeVersionStr()+" ---"));

	// закинем в протокол сделанные ранее ошибки
	if(CProg::MsgQueueID < 0)	AddEvent(IDI_EVENT_ERROR, MakeError("Main", "61000", CStr("Ошибка создания очереди сообщений - ") + strerror(errno)));
	if(CProg::SemArrayID < 0)	AddEvent(IDI_EVENT_ERROR, MakeError("Main", "61001", CStr("Ошибка создания массива семафоров - ") + strerror(errno)));
	
	CProg::pDiagnGate		= NULL;
	CProg::pDiagnGate		= new CDiagnGate();

//	CProg::pCalc				= new CProgCalc(this);
//	CProg::pCalc->SetLockOptions(SemArrayID, SEM_CALCULATIONS, bEnableMutexArray);

	// Выполняем инициализацию системы прав доступа
	::GrantsInit(SemArrayID, SEM_GRANTS_2, bEnableMutexArray);

	CProg::pClientLst		= new LONG_LST(10);
	CProg::pClientLst->SetLockOptions(SemArrayID, SEM_CLIENT_LST, bEnableMutexArray);
}
CProg::~CProg()
{
	for(int ClientPos=pClientLst->GetCnt() ; ClientPos>0 ; ClientPos--)
		delete (CPredictClient*)pClientLst->Get(ClientPos);
	delete pClientLst;

	delete pRegistry;

	if(pDiagnGate) delete pDiagnGate;

	::GrantsFree();

	delete pSettings;
	delete pProtocol;	pProtocol = NULL;

	//delete pCalc;
}
void CProg::AddEvent(long EventType, const char* lpEventText, const char* pUID)
{// добавляет новое событие в протокол
	if(pProtocol && SemArrayID>=0)
	{
		if(pProtocol->Lock())
		{
			pProtocol->AddEvent(EventType, lpEventText,pUID);
			pProtocol->Unlock();
		}
	}
}
int		CProg::SyncGetModuleState(void)
{
	return (GetFlag(FLG_STOP_PROG)!=SV_NEEDED) ? SYNC_MS_RUN : SYNC_MS_BUSY;
}
CStr	CProg::SyncGetModuleMenu(const char* pPrevMenuStr)
{
	int		PrevMenuItemID	= StrToLong(pPrevMenuStr);
	CStr	MenuStr;
	
	if( 	 PrevMenuItemID==0 ) 					MenuStr = MakeModuleMenuStr(MSG_DISPLAY,"Открыть консоль","DirectConsole","Укажите ност для вывода дисплея$HOSTNAME");
	else if( PrevMenuItemID==MSG_DISPLAY )			MenuStr = MakeModuleMenuStr(MSG_CLOSE_DISPLAY,"Закрыть консоль","DirectConsole");
	 
	return MenuStr;
}

void	CProg::GrantsChangeUserNotify(bool	bAutoUnlogin)
{
	CProgWnd* pMainWindow 	= ::pProgApp ? (CProgWnd*)::pProgApp->getMainWindow() : NULL;
	if( pMainWindow && pMainWindow->bLiveWnd ) pMainWindow->pSettingsFrame->bEnableChange = GrantsIsEnable(pSettings->NodeName, GetTaskName(), GrantsGetUID(), "EditSettings");
	pProtocol->bEnableChange	= GrantsIsEnable(pSettings->NodeName, GetTaskName(), GrantsGetUID(), "EditProtocol");

	CStr	Message;
	if( bAutoUnlogin )	Message = CStr("Выполнено автоматическое переключение пользователя ") + GrantsGetUserName( (char*)GrantsGetPrevUID() ) + " на " + GrantsGetUserName( (char*)GrantsGetUID() );
	else 				Message = CStr("Выполнено переключение пользователя ") + GrantsGetUserName( (char*)GrantsGetPrevUID() ) + " на " + GrantsGetUserName( (char*)GrantsGetUID() );

	AddEvent(IDI_EVENT_ASTERISK, MakeError("Console", NULL, Message));
}
