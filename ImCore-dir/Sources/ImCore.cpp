#include <ImCoreMain.h>

//////////////////////////////////////////////////////
// ������������� ��������� �����
CProgSettings::CProgSettings(FXRegistry* pProgRegistry) : CSettings(pProgRegistry)
{
	Add( new CBoolSetting( &bEnableSettings,		true, 					ENABLE_CHANGE,  "Mode",  	"EnableSettings",		 	" ��������� �������� � �������������� ��������"));
	Add( new CBoolSetting( &bEnableDiagnostic,		true, 					DISABLE_CHANGE, "Mode",		"EnableDiagnostic",  		" ��������� ���������� ���������� �����������"));
	Add( new CBoolSetting( &bEnableRegisterData,	true, 					DISABLE_CHANGE, "Mode",		"EnableRegisterData",  		" ��������� ����������� ������ � ������� �����"));
	Add( new CBoolSetting( &bEnableScanData,		true, 					DISABLE_CHANGE, "Mode",		"EnableScanData",  			" ��������� ����� ������� ������ � ������� �����"));
	Add( new CBoolSetting( &bEnableRealTime,		true, 					ENABLE_CHANGE, 	"Mode",		"EnableRealTime",  			" ��������� ���������� �������� � ������������ ������� ������� ������ ���������� �������"));
	Add( new CBoolSetting( &bEnableSelfSync,		false, 					DISABLE_CHANGE, "Mode",		"EnableSelfSync",	  		" ��������� ��������������� ������������� ������ ���"));
	Add( new CBoolSetting( &bEnableOFSTR_06,		false, 					DISABLE_CHANGE, "Mode",		"EnableOFSTR_06",	  		" ��������� ������ �� � ����� 0,6 ���."));
	Add( new CBoolSetting( &bSendToFTP,				true, 					DISABLE_CHANGE, "Mode",		"EnableSendToFTP",	  		" ��������� �������� ������ ������ �� FTP"));
	Add( new CBoolSetting( &bRecvFromFTP,			true, 					DISABLE_CHANGE, "Mode",		"EnableRecvFromFTP",	  	" ��������� ��ɣ� ������ ������ �� FTP"));

	Add( new CPathSetting( &LogPath, 				"$SRVHOME/log", 		DISABLE_CHANGE, "Path",		"LogPath", 					" ���� � ������ �������� *.log"));
	Add( new CPathSetting( &IdbPath, 				"$SRVHOME/idb", 		DISABLE_CHANGE, "Path",		"IdbPath", 					" ���� � ������ �������������� ���� ������ *idb,..."));
	Add( new CPathSetting( &CfgPath, 				"$SRVHOME/cfg", 		DISABLE_CHANGE, "Path",		"CfgPath", 					" ���� �  ���������������� ������"));
	Add( new CPathSetting( &ScriptPath, 			"$SRVHOME/script", 		DISABLE_CHANGE, "Path",		"ScriptPath", 				" ���� �  ��������"));
	Add( new CStrSetting(  &FilePrefix,				"", 					DISABLE_CHANGE, "Path",		"FilePrefix",				" ������� ������������ ������"));
    Add( new CPathSetting( &CalcDataPath, 			"$SRVHOME/idb/calc",	DISABLE_CHANGE, "Path",		"CalcPath", 				" ���� � ������ ������ ��� ���������� ������ޣ���"));
    Add( new CPathSetting( &ImCoreDataPath,			"$SRVHOME",             DISABLE_CHANGE, "Path",		"ImCorePath", 				" ���� � ������ ������ ��� ���������� ������ޣ���"));

	Add( new CIntSetting(  &MaxEventsCnt, 			1000, 					ENABLE_CHANGE,  "Log",  	"MaxEventCnt", 				" ������������ ���������� ������� � ������� *.log"));
	Add( new CBoolSetting( &bEnablePuts,			true, 					ENABLE_CHANGE,  "Log",  	"EnableTerminalOutput", 	" ��������� ������������ ������� �� ������� � ���������"));

	Add( new CStrSetting(  &NodeName, 				"localhost", 			DISABLE_CHANGE, "Prog", 	"NodeName", 				" ��� ������������� ����"));
	Add( new CStrSetting(  &ModuleName,				"���������� ������ޣ���",ENABLE_CHANGE,  "Prog", 	"ModuleName", 				" ��� ������ (��������� �� �������� �����������)"));
	Add( new CStrSetting(  &CompIdent,				"GB",					DISABLE_CHANGE, "Prog", 	"CompIdent", 				" ������� ��������������� ���������� ��� ������"));
	Add( new CIntSetting(  &DiagInterval, 			1, 						ENABLE_CHANGE,  "Prog",		"DiagInterval", 			" �������������� �������� (���.)"));
	Add( new CIntSetting(  &AutoUnloginTimeout, 	10,						ENABLE_CHANGE,  "Prog",		"AutoUnloginTimeout",		" timeout ��������������� ���������� ������������ (���.)"));
	Add( new CStrSetting(  &MainServer,				"DS1",					DISABLE_CHANGE, "Prog", 	"MainServerName", 			" ��� ��������� ������� �����, ������������������ � SrvAgent"));
	Add( new CStrSetting(  &ReservServer,			"DS2",					DISABLE_CHANGE, "Prog", 	"ReservServerName", 		" ��� ���������� ������� �����, ������������������ � SrvAgent"));
	Add( new CStrSetting(  &TacktShortName,			"",						ENABLE_CHANGE, 	"Prog",		"TacktShortName",			" ����������, ��� ��������� ������� ����� ����������� ������ �� ������� ���������� ������"));
	Add( new CIntSetting(  &EmptyTacktCounter,		3,						ENABLE_CHANGE, 	"Prog",		"EmptyTacktCounter",		" ���������� ���������� ��������� ������ �� ������ ��������� ���������� ������"));

	Add( new CBoolSetting( &bUseIEDC,				true, 					ENABLE_CHANGE,	"Calc",		"UseIEDC",	  				" ������������� ����� � ����������"));
	Add( new CDoubleSetting(&MinPowerCalc, 			60,						ENABLE_CHANGE,	"Calc",		"MinPowerCalc",				" ����������� �������� �������� ��� ������ ���ޣ���"));
	Add( new CDoubleSetting(&MinPowerDpzCalc, 		300,					ENABLE_CHANGE,	"Calc",		"MinPowerDpzCalc",			" ����������� �������� �������� ��� ������ �ޣ�� ���"));

	Add( new CIntSetting(  &TeffCheckInterval,	 	1800,					ENABLE_CHANGE,	"Teff",		"CheckInterval",			" ������� �� ������ ������� ��� ����� Teff (�������)"));
	Add( new CStrSetting(  &TeffInputDisplay,		"",						ENABLE_CHANGE,	"Teff",		"InputDisplay",				" ������� ������ ��������� ����� �eff"));
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

	// �������� �������� ���������� ����������
	::pProg 					= this;

	// ������� ������
	CProg::pRegistry			= new FXRegistry( (char*)(CStr(GetTaskName()) + ".ini"), NULL);

	CProg::pDiagTbl->SetLockOptions(SemArrayID, SEM_DIGNOSTIC_TBL,bEnableMutexArray);

	// ������� � ��������� ���������
	CProg::pSettings			= new CProgSettings(pRegistry);
	CProg::pSettings->SetLockOptions(SemArrayID, SEM_SETTINGS,bEnableMutexArray);
	CProg::pSettings->Load();
	CCore::pAutoUnloginTimeout	= &pSettings->AutoUnloginTimeout;
	CCore::pNodeName			= &pSettings->NodeName;
	if( pSettings->NodeName=="localhost" ) pSettings->NodeName = ::NetGetHostName();

	// ������ ��������
	CStr LogName				= CStr(pSettings->LogPath) + "/" + pProg->pSettings->FilePrefix + "ImCore.log";
	CProg::pProtocol			= new CProtocol(LogName, (long*)&pSettings->MaxEventsCnt, &pSettings->bEnablePuts,true);
	CProg::pProtocol->MakeColumns();
	CProg::pProtocol->SetLockOptions(SemArrayID, SEM_PROTOCOL,bEnableMutexArray);
	if( !CProg::pProtocol->ReadAllFile(&StrError) ) AddEvent(IDI_EVENT_ERROR, StrError);

	// ������ � ������� ���������
	AddEvent(IDI_EVENT_INFO, MakeError(NULL, NULL, CStr("--- ����� ���������. ������ ")+MakeVersionStr()+" ---"));

	// ������� � �������� ��������� ����� ������
	if(CProg::MsgQueueID < 0)	AddEvent(IDI_EVENT_ERROR, MakeError("Main", "61000", CStr("������ �������� ������� ��������� - ") + strerror(errno)));
	if(CProg::SemArrayID < 0)	AddEvent(IDI_EVENT_ERROR, MakeError("Main", "61001", CStr("������ �������� ������� ��������� - ") + strerror(errno)));
	
	// ������� ��� ����������� ������ � �������
	CProg::pRegIdb	= new CRegIdb(this, pSettings->IdbPath, "ImCoreReg", IDBS_ALL_LOCAL);
	CProg::pRegIdb->CreateParamTbl();
	CProg::pRegIdb->SetLockOptions(SemArrayID, SEM_BUSY_REG_IDB, bEnableMutexArray);

	// ������� ��� ��ɣ�� ������ �� �������
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

	// ������������� ������ �������� ��� ����������� ������
	CProg::pRegisterServersLst = new LONG_LST(5);	InitRegisterServersLst();

	// ������������� ������� ������ ���������
	InitMUnitTbl(CStr(pSettings->IdbPath) + "/" + pProg->pSettings->FilePrefix + "MUnit.idb");
	
	// ������������� ������� ����������
	InitSWIDTbl(CStr(pSettings->IdbPath) + "/" + pProg->pSettings->FilePrefix + "SWID.idb");
	
	// ��������� ������������� ������� ���� �������
	::GrantsInit(SemArrayID, SEM_GRANTS_2, bEnableMutexArray);
}
CProg::~CProg()
{
	delete pRegistry;

	for(int RegSrvPos=pRegisterServersLst->GetCnt() ; RegSrvPos>=1 ; RegSrvPos--)
	{// �������� �������� ��� ����������� ������
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
{// ��������� ����� ������� � ��������
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
	
	if( 	 PrevMenuItemID==0 ) 					MenuStr = MakeModuleMenuStr(MSG_DISPLAY,"������� �������","DirectConsole","������� ���� ��� ������ �������$HOSTNAME");
	else if( PrevMenuItemID==MSG_DISPLAY )			MenuStr = MakeModuleMenuStr(MSG_CLOSE_DISPLAY,"������� �������","DirectConsole");
	else if( PrevMenuItemID==MSG_CLOSE_DISPLAY )	MenuStr = MakeModuleMenuStr(MSG_INPUT_TEFF,"���� Teff","DirectConsole","������� ���� ��� ������ ������� ����� Teff$HOSTNAME");
	 
	return MenuStr;
}

void	CProg::GrantsChangeUserNotify(bool	bAutoUnlogin)
{
	CProgWnd* pMainWindow 	= ::pProgApp ? (CProgWnd*)::pProgApp->getMainWindow() : NULL;
	if( pMainWindow && pMainWindow->bLiveWnd ) pMainWindow->pSettingsFrame->bEnableChange = GrantsIsEnable(pSettings->NodeName, GetTaskName(), GrantsGetUID(), "EditSettings");
	pProtocol->bEnableChange	= GrantsIsEnable(pSettings->NodeName, GetTaskName(), GrantsGetUID(), "EditProtocol");

	CStr	Message;
	if( bAutoUnlogin )	Message = CStr("��������� �������������� ������������ ������������ ") + GrantsGetUserName( (char*)GrantsGetPrevUID() ) + " �� " + GrantsGetUserName( (char*)GrantsGetUID() );
	else 				Message = CStr("��������� ������������ ������������ ") + GrantsGetUserName( (char*)GrantsGetPrevUID() ) + " �� " + GrantsGetUserName( (char*)GrantsGetUID() );

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
