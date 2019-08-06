#include <ImCoreMain.h>

//////////////////////////////////////////////////////////////////////
// ИБД регистрации данных в сервере
CProgIdb::CProgIdb(CProg* pProg, const char* pPathIdb, const char* pLocalListPrefix, const char* pCommonFilePrefix, long IdbStyle, const char* pCorrectiveFileName)
		:CInfoDataBase(pPathIdb, pLocalListPrefix, pCommonFilePrefix, IdbStyle, pCorrectiveFileName)
{
	CProgIdb::pProg				= pProg;
	CProgIdb::RejectParamCnt	= 0;
	CProgIdb::pCalcShortNameLst	= new CHAR_LST(1000, 20);
	CProgIdb::pParamInfoLst		= new LONG_LST(1000);
	CProgIdb::pLastChangeTeff	= NULL;
}
CProgIdb::~CProgIdb(void)
{
	delete pCalcShortNameLst;
	delete pParamInfoLst;
}
bool CProgIdb::InitIdb(CStr* pStrError)
{
	bool bRetValue = false;
	if(Lock())
	{
		FreeGroup();  // очистим старые группы
		bRetValue = CInfoDataBase::InitIdb(pStrError);
		Unlock();

		int ParamCnt = pParamLst->GetCnt();
		for(int ParamPos=1 ; ParamPos<=ParamCnt ; ParamPos++)
		{
			CProgParamInfo* pProgParamInfo = (CProgParamInfo*)pParamLst->Get(ParamPos);
			if(StrLen(pProgParamInfo->CalcShortName))
			{
				pCalcShortNameLst->Add((long)pProgParamInfo->CalcShortName);
				pParamInfoLst->Add((long)pProgParamInfo);
			}
		}
		pLastChangeTeff = (CScanParamInfo*)GetParamInfoByName("TEFF_TM");
	}
	return bRetValue;
}
CParamInfo* CProgIdb::MakeParamInfo(CParamInfo* pParamInfo, long PosInTable, long PosInFile)
{
	CProgParamInfo* pProgParamInfo = (CProgParamInfo*)CInfoDataBase::MakeParamInfo(pParamInfo,PosInTable,PosInFile);
	if(pProgParamInfo)
	{
		pProgParamInfo->ChangeFlag	= 0;
		StrNCpy(pProgParamInfo->CalcShortName, pParamTbl->GetStr("CalcShortName", PosInTable), sizeof(pProgParamInfo->CalcShortName));
		pProgParamInfo->Index1	= (int)pParamTbl->GetNum("Index1", PosInTable);
		pProgParamInfo->Index2	= (int)pParamTbl->GetNum("Index2", PosInTable);

		pProgParamInfo->DataOffset	= -1;

		// если параметр в резерве то его отвергаем
		if(pProgParamInfo->Category == 3) pProgParamInfo->OutsideID = 0;

		if(pProgParamInfo->OutsideID) ::SetOutsideValue(pProgParamInfo, pProgParamInfo->Value, 0, STATUS_EMPTY);
		else pProgParamInfo = NULL;
	}
	CProgIdb::RejectParamCnt += !pProgParamInfo? 1 : 0;
	return pProgParamInfo;
}
CParamInfo* CProgIdb::GetParamInfoByCalcShortName(const char* pCalcShortName)
{
	CParamInfo* pRetValue = NULL;
	int FindPos = pCalcShortNameLst->Find((long)pCalcShortName);
	if(FindPos > 0) pRetValue = (CParamInfo*)pParamInfoLst->Get(FindPos);
	return pRetValue;
}


//////////////////////////////////////////////////////////////////////
// ИБД регистрации данных в сервере
CRegIdb::CRegIdb(CProg* pProg, const char* pIdbPath, const char* pLocalListPrefix, long IdbStyle)
		:CProgIdb(pProg, pIdbPath, pLocalListPrefix, ::pProg->pSettings->FilePrefix, IdbStyle){}
CRegIdb::~CRegIdb(void){}
void CRegIdb::CreateParamLst()
{
	if(pParamLst) delete pParamLst;
	CRegIdb::pParamLst = new LST(DEFAULT_PARAM_CNT, sizeof(CRegParamInfo));
}
void CRegIdb::CreateParamTbl()
{
	if(CInfoDataBase::pParamTbl) delete CInfoDataBase::pParamTbl;

	// копируем основной файл во временный (чтобы не було записи в основной файл)
	CStr	MainFileName(CStr(pProg->pSettings->IdbPath)+"/" + pProg->pSettings->FilePrefix + "ImCoreReg.idb");

	CInfoDataBase::pParamTbl			= new CFileTbl(MainFileName);
	CInfoDataBase::pParamTbl->ObjName	= "Перечень переменных";
	CInfoDataBase::pParamTbl->MakeColumns();

	CInfoDataBase::pShortNameCol = CInfoDataBase::pParamTbl->AddCol("ShortName", "", SQL_C_CHAR, 16,0);
	CInfoDataBase::pParamTbl->AddCol("OutsideID",	"", SQL_C_LONG, 	0, 0, CLS_NO_SAVE|CLS_NOLOAD);
	CInfoDataBase::pParamTbl->AddCol("MUnit", 		"", SQL_C_SHORT,	0, 0, CLS_NO_SAVE|CLS_NOLOAD);
	CInfoDataBase::pParamTbl->AddCol("Category",	"", SQL_C_SHORT,	0, 0, CLS_NO_SAVE|CLS_NOLOAD);
	CInfoDataBase::pParamTbl->AddCol("Style", 		"", SQL_C_LONG, 	0, 0, CLS_NO_SAVE|CLS_NOLOAD);
	CInfoDataBase::pParamTbl->AddCol("FullName",	"",	SQL_C_CHAR,		60,0, CLS_NO_SAVE|CLS_NOLOAD);
//	CInfoDataBase::pParamTbl->AddCol("NVG",			"", SQL_DOUBLE,		0, 0, CLS_NO_SAVE|CLS_NOLOAD);
//	CInfoDataBase::pParamTbl->AddCol("VVG",			"", SQL_DOUBLE,		0, 0, CLS_NO_SAVE|CLS_NOLOAD);
//	CInfoDataBase::pParamTbl->AddCol("NAG",			"", SQL_DOUBLE,		0, 0, CLS_NO_SAVE|CLS_NOLOAD);
//	CInfoDataBase::pParamTbl->AddCol("VAG",			"", SQL_DOUBLE,		0, 0, CLS_NO_SAVE|CLS_NOLOAD);
	CInfoDataBase::pParamTbl->AddCol("CalcShortName","", SQL_C_CHAR, 	16,0);
	CInfoDataBase::pParamTbl->AddCol("Index1",		"", SQL_C_SHORT,	0, 0);
	CInfoDataBase::pParamTbl->AddCol("Index2",		"", SQL_C_SHORT,	0, 0);
	CInfoDataBase::pParamTbl->AddCol("OpType",		"", SQL_C_CHAR,		6, 0);
	CInfoDataBase::pParamTbl->AddCol("MultiCoef",	"", SQL_C_FLOAT,	0, 0);

	// чтобы таблицы были открыты всегда
	OpenParamTbl(NULL);
}
short CRegIdb::SyncIdb(const char* pServerName, CStr* pStrError)
{// синхронизиует ИБД с сервером данных
	short 		RetValue = 1;
	CStr		ErrorText;

	if( ::pProg->pSettings->bEnableSelfSync )
	{// старая синхронизация файлов ИБД
		CHAR_LST	NeededSyncLst(10, 100);
		CHAR_LST	NeededNameLst(10, 100);

		NeededNameLst.Add((long)(char*)(CStr(pProg->pSettings->FilePrefix)+"ImCoreReg.idb"));

		RetValue =::SrvVerifyIdbFiles(&NeededNameLst, &NeededSyncLst, pProg->pSettings->IdbPath, CStr("ImCoreReg ")+pProg->pSettings->NodeName, pServerName, &ErrorText);

		for( int FilePos=1; FilePos<=NeededSyncLst.GetCnt(); FilePos++ )
		{// если требуется синхронизация файлов
			char*	pSyncFileName = (char*)NeededSyncLst.Get(FilePos);
			if( SrvSyncIdbFile(pProg->pSettings->IdbPath, pSyncFileName, CStr("ImCoreReg ")+pProg->pSettings->NodeName, pServerName, &ErrorText) )
			{
				RetValue = 2; pProg->AddEvent( IDI_EVENT_INFO, MakeError("SyncRegIdb", NULL, CStr("Синхронизирован файл ИБД ") + pSyncFileName));
			}
			else{ RetValue = 0; break; }
		}
	}

	if( pParamTbl && pParamTbl->IsOpenFile() && (RetValue=pParamTbl->CheckColumns(&ErrorText))==1)
	{// синхронизация атрибутов переменных

		// создадим таблицу для синхронизации атрибутов переменных
		if( CInfoDataBase::pSyncParamTbl ) delete CInfoDataBase::pSyncParamTbl;
		CInfoDataBase::pSyncParamTbl = CreateSyncParamTbl(pParamTbl);

		// вгружаем с сервера атрибуты переменных
		if(CInfoDataBase::pSyncParamTbl && !pSyncParamTbl->GetParamsInfo(CStr("ImCoreReg ")+pProg->pSettings->NodeName,pServerName, &ErrorText) )
			RetValue = 0;
	}

	if( !RetValue )
	{
		if(pStrError)  *pStrError = ErrorText;
		else			puts((char*)ErrorText);
	}

	return RetValue;
}


//////////////////////////////////////////////////////////////////////
// ИБД приёма данных от сервера
CScanIdb::CScanIdb(CProg* pProg, const char* pIdbPath, const char* pLocalListPrefix, long IdbStyle)
		 :CProgIdb(pProg, pIdbPath, pLocalListPrefix, ::pProg->pSettings->FilePrefix, IdbStyle){}
CScanIdb::~CScanIdb(void){}
void CScanIdb::CreateParamLst()
{
	if(pParamLst) delete pParamLst;
	CScanIdb::pParamLst = new LST(DEFAULT_PARAM_CNT, sizeof(CScanParamInfo));
}
void CScanIdb::CreateParamTbl()
{
	if(CInfoDataBase::pParamTbl) delete CInfoDataBase::pParamTbl;

	// копируем основной файл во временный (чтобы не було записи в основной файл)
	CStr	MainFileName(CStr(pProg->pSettings->IdbPath)+"/" + pProg->pSettings->FilePrefix + "ImCoreScan.idb");

	CInfoDataBase::pParamTbl			= new CFileTbl(MainFileName);
	CInfoDataBase::pParamTbl->ObjName	= "Перечень переменных";
	CInfoDataBase::pParamTbl->MakeColumns();

	CInfoDataBase::pShortNameCol = CInfoDataBase::pParamTbl->AddCol("ShortName", "", SQL_C_CHAR, 16,0);
	CInfoDataBase::pParamTbl->AddCol("OutsideID",	"", SQL_C_LONG, 	0, 0, CLS_NO_SAVE|CLS_NOLOAD);
	CInfoDataBase::pParamTbl->AddCol("MUnit", 		"", SQL_C_SHORT,	0, 0, CLS_NO_SAVE|CLS_NOLOAD);
	CInfoDataBase::pParamTbl->AddCol("Category",	"", SQL_C_SHORT,	0, 0, CLS_NO_SAVE|CLS_NOLOAD);
	CInfoDataBase::pParamTbl->AddCol("Style", 		"", SQL_C_LONG, 	0, 0, CLS_NO_SAVE|CLS_NOLOAD);
	CInfoDataBase::pParamTbl->AddCol("FullName",	"",	SQL_C_CHAR,		60,0, CLS_NO_SAVE|CLS_NOLOAD);
//	CInfoDataBase::pParamTbl->AddCol("NVG",			"", SQL_DOUBLE,		0, 0, CLS_NO_SAVE|CLS_NOLOAD);
//	CInfoDataBase::pParamTbl->AddCol("VVG",			"", SQL_DOUBLE,		0, 0, CLS_NO_SAVE|CLS_NOLOAD);
//	CInfoDataBase::pParamTbl->AddCol("NAG",			"", SQL_DOUBLE,		0, 0, CLS_NO_SAVE|CLS_NOLOAD);
//	CInfoDataBase::pParamTbl->AddCol("VAG",			"", SQL_DOUBLE,		0, 0, CLS_NO_SAVE|CLS_NOLOAD);
	CInfoDataBase::pParamTbl->AddCol("CalcShortName","", SQL_C_CHAR, 	16,0);
	CInfoDataBase::pParamTbl->AddCol("Index1",		"", SQL_C_SHORT,	0, 0);
	CInfoDataBase::pParamTbl->AddCol("Index2",		"", SQL_C_SHORT,	0, 0);
	CInfoDataBase::pParamTbl->AddCol("OpType",		"", SQL_C_CHAR,		6, 0);
	CInfoDataBase::pParamTbl->AddCol("MultiCoef",	"", SQL_C_FLOAT,	0, 0);

	// чтобы таблицы были открыты всегда
	OpenParamTbl(NULL);
}
short CScanIdb::SyncIdb(const char* pServerName, CStr* pStrError)
{// синхронизиует ИБД с сервером данных
	short 		RetValue = 1;
	CStr		ErrorText;

	if( ::pProg->pSettings->bEnableSelfSync )
	{// старая синхронизация файлов ИБД
		CHAR_LST	NeededSyncLst(10, 100);
		CHAR_LST	NeededNameLst(10, 100);

		NeededNameLst.Add((long)(char*)(CStr(pProg->pSettings->FilePrefix)+"ImCoreScan.idb"));

		RetValue =::SrvVerifyIdbFiles(&NeededNameLst, &NeededSyncLst, pProg->pSettings->IdbPath, CStr("ImCoreScan ")+pProg->pSettings->NodeName, pServerName, &ErrorText);

		for( int FilePos=1; FilePos<=NeededSyncLst.GetCnt(); FilePos++ )
		{// если требуется синхронизация файлов
			char*	pSyncFileName = (char*)NeededSyncLst.Get(FilePos); 
			if( SrvSyncIdbFile(pProg->pSettings->IdbPath, pSyncFileName, CStr("ImCoreScan ")+pProg->pSettings->NodeName, pServerName, &ErrorText) )
			{
				RetValue = 2; pProg->AddEvent( IDI_EVENT_INFO, MakeError("SyncScanIdb", NULL, CStr("Синхронизирован файл ИБД ") + pSyncFileName));
			}
			else{ RetValue = 0; break; }
		}
	}

	if( pParamTbl && pParamTbl->IsOpenFile() && (RetValue=pParamTbl->CheckColumns(&ErrorText))==1)
	{// синхронизация атрибутов переменных

		// создадим таблицу для синхронизации атрибутов переменных
		if( CInfoDataBase::pSyncParamTbl ) delete CInfoDataBase::pSyncParamTbl;
		CInfoDataBase::pSyncParamTbl = CreateSyncParamTbl(pParamTbl);

		// вгружаем с сервера атрибуты переменных
		if(CInfoDataBase::pSyncParamTbl && !pSyncParamTbl->GetParamsInfo(CStr("ImCoreScan ")+pProg->pSettings->NodeName,pServerName, &ErrorText) )
			RetValue = 0;
	}

	if( !RetValue )
	{
		if(pStrError)  *pStrError = ErrorText;
		else			puts((char*)ErrorText);
	}

	return RetValue;
}




