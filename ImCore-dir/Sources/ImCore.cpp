#include <ImCoreMain.h>

//////////////////////////////////////////////////////
// настраиваемые параметры шлюза
CProgSettings::CProgSettings(FXRegistry* pProgRegistry) : CSettings(pProgRegistry)
{
	Add( new CBoolSetting( &bEnableSettings,		true, 					ENABLE_CHANGE,  "Mode",  	"EnableSettings",		 	" разрешить просмотр и редактирование настроек"));
	Add( new CBoolSetting( &bEnableDiagnostic,		true, 					DISABLE_CHANGE, "Mode",		"EnableDiagnostic",  		" разрешить выполнение внутренней диагностики"));
	Add( new CBoolSetting( &bEnableRegisterData,	true, 					DISABLE_CHANGE, "Mode",		"EnableRegisterData",  		" разрешить регистрацию данных в сервере ОБДРВ"));
	Add( new CBoolSetting( &bEnableScanData,		true, 					DISABLE_CHANGE, "Mode",		"EnableScanData",  			" разрешить прием текущих данных с сервера ОДБРВ"));
	Add( new CBoolSetting( &bEnableRealTime,		true, 					ENABLE_CHANGE, 	"Mode",		"EnableRealTime",  			" разрешить присвоение принятым и записываемым пакетам текущих данных системного времени"));
	Add( new CBoolSetting( &bEnableSelfSync,		false, 					DISABLE_CHANGE, "Mode",		"EnableSelfSync",	  		" разрешить самостоятельную синхронизацию файлов ИБД"));
	Add( new CBoolSetting( &bEnableOFSTR_06,		false, 					DISABLE_CHANGE, "Mode",		"EnableOFSTR_06",	  		" выполнять расчет АО в цикле 0,6 сек."));
	Add( new CBoolSetting( &bSendToFTP,				true, 					DISABLE_CHANGE, "Mode",		"EnableSendToFTP",	  		" разрешить передачу файлов данных по FTP"));
	Add( new CBoolSetting( &bRecvFromFTP,			true, 					DISABLE_CHANGE, "Mode",		"EnableRecvFromFTP",	  	" разрешить приём файлов данных по FTP"));

	Add( new CPathSetting( &LogPath, 				"$SRVHOME/log", 		DISABLE_CHANGE, "Path",		"LogPath", 					" путь к файлам журналов *.log"));
	Add( new CPathSetting( &IdbPath, 				"$SRVHOME/idb", 		DISABLE_CHANGE, "Path",		"IdbPath", 					" путь к файлам информационной базы данных *idb,..."));
	Add( new CPathSetting( &CfgPath, 				"$SRVHOME/cfg", 		DISABLE_CHANGE, "Path",		"CfgPath", 					" путь к  конфигурационным файлам"));
	Add( new CPathSetting( &ScriptPath, 			"$SRVHOME/script", 		DISABLE_CHANGE, "Path",		"ScriptPath", 				" путь к  скриптам"));
	Add( new CStrSetting(  &FilePrefix,				"", 					DISABLE_CHANGE, "Path",		"FilePrefix",				" префикс используемых файлов"));
    Add( new CPathSetting( &CalcDataPath, 			"$SRVHOME/idb/calc",	DISABLE_CHANGE, "Path",		"CalcPath", 				" путь к файлам данных для подсистемы физрасчётов"));
    Add( new CPathSetting( &ImCoreDataPath,			"$SRVHOME",             DISABLE_CHANGE, "Path",		"ImCorePath", 				" путь к файлам данных для подсистемы физрасчётов"));

	Add( new CIntSetting(  &MaxEventsCnt, 			1000, 					ENABLE_CHANGE,  "Log",  	"MaxEventCnt", 				" максимальное количество событий в журнале *.log"));
	Add( new CBoolSetting( &bEnablePuts,			true, 					ENABLE_CHANGE,  "Log",  	"EnableTerminalOutput", 	" разрешить дублирование событий из журнала в терминале"));

	Add( new CStrSetting(  &NodeName, 				"localhost", 			DISABLE_CHANGE, "Prog", 	"NodeName", 				" имя компьютерного узла"));
	Add( new CStrSetting(  &ModuleName,				"Подсистема физрасчётов",ENABLE_CHANGE,  "Prog", 	"ModuleName", 				" имя модуля (выводится на лепестке диагностики)"));
	Add( new CStrSetting(  &CompIdent,				"GB",					DISABLE_CHANGE, "Prog", 	"CompIdent", 				" префикс диагностических переменных для модуля"));
	Add( new CIntSetting(  &DiagInterval, 			1, 						ENABLE_CHANGE,  "Prog",		"DiagInterval", 			" иагностический интервал (сек.)"));
	Add( new CIntSetting(  &AutoUnloginTimeout, 	10,						ENABLE_CHANGE,  "Prog",		"AutoUnloginTimeout",		" timeout автоматического отключения пользователя (мин.)"));
	Add( new CStrSetting(  &MainServer,				"DS1",					DISABLE_CHANGE, "Prog", 	"MainServerName", 			" имя основного сервера ОБДРВ, зарегистрированное в SrvAgent"));
	Add( new CStrSetting(  &ReservServer,			"DS2",					DISABLE_CHANGE, "Prog", 	"ReservServerName", 		" имя резервного сервера ОБДРВ, зарегистрированное в SrvAgent"));
	Add( new CStrSetting(  &TacktShortName,			"",						ENABLE_CHANGE, 	"Prog",		"TacktShortName",			" переменная, при изменении которой будет выполняться расчет по приходу измеренных данных"));
	Add( new CIntSetting(  &EmptyTacktCounter,		3,						ENABLE_CHANGE, 	"Prog",		"EmptyTacktCounter",		" количество пропущеных стартовых тактов до начала обработки измеренных данных"));

	Add( new CBoolSetting( &bUseIEDC,				true, 					ENABLE_CHANGE,	"Calc",		"UseIEDC",	  				" использование токов с коррекцией"));
	Add( new CDoubleSetting(&MinPowerCalc, 			60,						ENABLE_CHANGE,	"Calc",		"MinPowerCalc",				" минимальное значение мощности для начала расчётов"));
	Add( new CDoubleSetting(&MinPowerDpzCalc, 		300,					ENABLE_CHANGE,	"Calc",		"MinPowerDpzCalc",			" минимальное значение мощности для начала учёта ДПЗ"));

	Add( new CIntSetting(  &TeffCheckInterval,	 	1800,					ENABLE_CHANGE,	"Teff",		"CheckInterval",			" уставка на период простоя для ввода Teff (секунды)"));
	Add( new CStrSetting(  &TeffInputDisplay,		"",						ENABLE_CHANGE,	"Teff",		"InputDisplay",				" дисплей вывода программы ввода Тeff"));
}
CProgSettings::~CProgSettings(void){}
bool CProgSettings::SetValue(const char* pSectionName, const char* pSettingName, const char* pStrValue)
{
	return CSettings::SetValue(pSectionName,pSettingName,pStrValue);
}


//////////////////////////////////////////////////////////////////////
// 
CProg::CProg(const char* pPath, const char* pTaskName) : CCore(pPath, MAX_SEM_CNT, pTaskName,STYLE_OFFICE_2003|STYLE_ENABLE_MUTEX_ARRAY)
{
	CStr	StrError;

	// присвоим значение глобальной переменной
	::pProg 					= this;

	// создаем реестр
	CProg::pRegistry			= new FXRegistry( (char*)(CStr(GetTaskName()) + ".ini"), NULL);

	CProg::pDiagTbl->SetLockOptions(SemArrayID, SEM_DIGNOSTIC_TBL,bEnableMutexArray);

	// создаем и загружаем настройки
	CProg::pSettings			= new CProgSettings(pRegistry);
	CProg::pSettings->SetLockOptions(SemArrayID, SEM_SETTINGS,bEnableMutexArray);
	CProg::pSettings->Load();
	CCore::pAutoUnloginTimeout	= &pSettings->AutoUnloginTimeout;
	CCore::pNodeName			= &pSettings->NodeName;
	if( pSettings->NodeName=="localhost" ) pSettings->NodeName = ::NetGetHostName();

	// слепим протокол
	CStr LogName				= CStr(pSettings->LogPath) + "/" + pProg->pSettings->FilePrefix + "ImCore.log";
	CProg::pProtocol			= new CProtocol(LogName, (long*)&pSettings->MaxEventsCnt, &pSettings->bEnablePuts,true);
	CProg::pProtocol->MakeColumns();
	CProg::pProtocol->SetLockOptions(SemArrayID, SEM_PROTOCOL,bEnableMutexArray);
	if( !CProg::pProtocol->ReadAllFile(&StrError) ) AddEvent(IDI_EVENT_ERROR, StrError);

	// заявим о запуске программы
	AddEvent(IDI_EVENT_INFO, MakeError(NULL, NULL, CStr("--- Старт программы. Версия ")+MakeVersionStr()+" ---"));

	// закинем в протокол сделанные ранее ошибки
	if(CProg::MsgQueueID < 0)	AddEvent(IDI_EVENT_ERROR, MakeError("Main", "61000", CStr("Ошибка создания очереди сообщений - ") + strerror(errno)));
	if(CProg::SemArrayID < 0)	AddEvent(IDI_EVENT_ERROR, MakeError("Main", "61001", CStr("Ошибка создания массива семафоров - ") + strerror(errno)));
	
	// создаем ИБД регистрации данных в сервере
	CProg::pRegIdb	= new CRegIdb(this, pSettings->IdbPath, "ImCoreReg", IDBS_ALL_LOCAL);
	CProg::pRegIdb->CreateParamTbl();
	CProg::pRegIdb->SetLockOptions(SemArrayID, SEM_BUSY_REG_IDB, bEnableMutexArray);

	// создаем ИБД приёма данных от сервера
	CProg::pScanIdb	= new CScanIdb(this, pSettings->IdbPath, "ImCoreScan", IDBS_ALL_LOCAL);
	CProg::pScanIdb->CreateParamTbl();
	CProg::pScanIdb->SetLockOptions(SemArrayID, SEM_BUSY_SCAN_IDB, bEnableMutexArray);

	CProg::pDiagnGate		= NULL;
	CProg::pDiagnGate		= new CDiagnGate();

	CProg::pScanRequest		= NULL;
	CProg::pScanAnswer		= NULL;
	CProg::ScanRequestID	= 0;

	CProg::pCalc				= new CProgCalc(this);

printf("START CREATE IMCORE\n");
    CProg::pAkz                 = new ImCoreAkZ(pSettings->ImCoreDataPath);

	CProg::pCalc->SetLockOptions(SemArrayID, SEM_CALCULATIONS, bEnableMutexArray);

	// инициализация списка серверов для регистрации данных
	CProg::pRegisterServersLst = new LONG_LST(5);	InitRegisterServersLst();

	// инициализация таблицы едениц измерения
	InitMUnitTbl(CStr(pSettings->IdbPath) + "/" + pProg->pSettings->FilePrefix + "MUnit.idb");
	
	// инициализация таблицы источников
	InitSWIDTbl(CStr(pSettings->IdbPath) + "/" + pProg->pSettings->FilePrefix + "SWID.idb");
	
	// Выполняем инициализацию системы прав доступа
	::GrantsInit(SemArrayID, SEM_GRANTS_2, bEnableMutexArray);
}
CProg::~CProg()
{
	delete pRegistry;

	for(int RegSrvPos=pRegisterServersLst->GetCnt() ; RegSrvPos>=1 ; RegSrvPos--)
	{// перечень серверов для регистрации данных
		delete (CRegisterServer*)pRegisterServersLst->Get(RegSrvPos);
	}
	delete pRegisterServersLst;

	if( ScanRequestID )	::SrvFreeRequest(ScanRequestID);
	if( pScanRequest  )	::SrvFreeMem(pScanRequest);
	if( pScanAnswer   )	::SrvFreeMem(pScanAnswer);

	if(pDiagnGate) delete pDiagnGate;

	::GrantsFree();

	delete pScanIdb;
	delete pRegIdb;
	delete pSettings;
	delete pProtocol;	pProtocol = NULL;

	delete pCalc;

    delete pAkz;
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
	return (GetFlag(FLG_STOP_PROG)!=SV_NEEDED && GetFlag(FLG_INIT_REG_IDB)==SV_READY && GetFlag(FLG_INIT_SCAN_IDB)==SV_READY)
			? SYNC_MS_RUN : SYNC_MS_BUSY;
}
CStr	CProg::SyncGetModuleMenu(const char* pPrevMenuStr)
{
	int		PrevMenuItemID	= StrToLong(pPrevMenuStr);
	CStr	MenuStr;
	
	if( 	 PrevMenuItemID==0 ) 					MenuStr = MakeModuleMenuStr(MSG_DISPLAY,"Открыть консоль","DirectConsole","Укажите ност для вывода дисплея$HOSTNAME");
	else if( PrevMenuItemID==MSG_DISPLAY )			MenuStr = MakeModuleMenuStr(MSG_CLOSE_DISPLAY,"Закрыть консоль","DirectConsole");
	else if( PrevMenuItemID==MSG_CLOSE_DISPLAY )	MenuStr = MakeModuleMenuStr(MSG_INPUT_TEFF,"Ввод Teff","DirectConsole","Укажите ност для вывода диалога ввода Teff$HOSTNAME");
	 
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

	AddEvent(IDI_EVENT_ASTERISK,MakeError("Console", NULL, Message));
}
void CProg::InitRegisterServersLst()
{
	int RegSrvPos = 1;
	if(pSettings->MainServer.GetLength())
	{
		CRegisterServer* pRegServer = new CRegisterServer(pSettings->MainServer);
		pRegServer->RegThreadFlgStart	= 1007 + 10*RegSrvPos;
		pRegServer->RegThreadFlgRun		= 1008 + 10*RegSrvPos;
		pRegServer->RegThreadFlgStop	= 1009 + 10*RegSrvPos;
		SetFlag(pRegServer->RegThreadFlgStart,	SV_NEEDED);
		SetFlag(pRegServer->RegThreadFlgRun,	SV_READY);
		SetFlag(pRegServer->RegThreadFlgStop,	SV_READY);
		pRegisterServersLst->Add((long)pRegServer);
		RegSrvPos++;
	}
	if(pSettings->ReservServer.GetLength())
	{
		CRegisterServer* pRegServer = new CRegisterServer(pSettings->ReservServer);
		pRegServer->RegThreadFlgStart	= 1007 + 10*RegSrvPos;
		pRegServer->RegThreadFlgRun		= 1008 + 10*RegSrvPos;
		pRegServer->RegThreadFlgStop	= 1009 + 10*RegSrvPos;
		SetFlag(pRegServer->RegThreadFlgStart,	SV_NEEDED);
		SetFlag(pRegServer->RegThreadFlgRun,	SV_READY);
		SetFlag(pRegServer->RegThreadFlgStop,	SV_READY);
		pRegisterServersLst->Add((long)pRegServer);
		RegSrvPos++;
	}
}
