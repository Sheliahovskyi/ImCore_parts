#ifndef _IMCORE_IDB_H
#define _IMCORE_IDB_H

class CProg;

//////////////////////////////////////////////////////////////////////
// class CProgParamInfo
class CProgParamInfo : public CParamInfo
{// описание характеристик переменной для ИБД
public:
	unsigned char	Value[5];
	char			ChangeFlag;				// флаг изменения в последнем цикле опроса CF_
	char			CalcShortName[33];		// краткое символьное описание параметра в системе расчётов
	int				Index1;
	int				Index2;
	int				DataOffset;
public:
	float	GetValue(void){  return OutsideType==OST_ANALOG? *(float*)Value : (float)*Value; }
	char	GetStatus(void){ return *(Value + ValueByteSize);}
};

//////////////////////////////////////////////////////////////////////
// class CRegParamInfo
class CRegParamInfo : public CProgParamInfo
{// описание характеристик переменной для ИБД регистрации данных в сервере
public:
};

//////////////////////////////////////////////////////////////////////
// class CScanParamInfo
class CScanParamInfo : public CProgParamInfo
{// описание характеристик переменной для ИБД приёма данных от сервера
public:

};

//////////////////////////////////////////////////////////////////////
// ИБД
class CProgIdb : public CInfoDataBase
{
public:
	CProg*			pProg;				// указатель на приложение
	CStr			SyncServerName;		// имя сервера данных с которым проводилась синхронизация
	int				RejectParamCnt;		// количество отвернутых переменных

	CHAR_LST*		pCalcShortNameLst;
	LONG_LST*		pParamInfoLst;

	CProgParamInfo*	pLastChangeTeff;	// переменная, содержащая время последнего изменения Teff

public:
	CProgIdb(CProg* pProg, const char* pPathIdb, const char* pLocalListPrefix, const char* pCommonFilePrefix, long IdbStyle, const char* pCorrectiveFileName=NULL);
	virtual ~CProgIdb();

			CParamInfo*		GetParamInfoByCalcShortName(const char* pBShortName);
	virtual	bool			InitIdb(CStr* pStrError);
	virtual	CParamInfo*		MakeParamInfo(CParamInfo* pParamInfo, long PosInTable, long PosInFile);
};

//////////////////////////////////////////////////////////////////////
// ИБД регистрации данных в сервере
class CRegIdb : public CProgIdb
{
public:
	CRegIdb(CProg* pProg, const char* pIdbPath, const char* pLocalListPrefix, long IdbStyle);
	virtual ~CRegIdb();

			short			SyncIdb(const char* pServerName, CStr* pStrError);
	virtual	void			CreateParamLst(void);
	virtual	void			CreateParamTbl(void);
};

//////////////////////////////////////////////////////////////////////
// ИБД приёма данных от сервера
class CScanIdb : public CProgIdb
{
public:
	CScanIdb(CProg* pProg, const char* pIdbPath, const char* pLocalListPrefix, long IdbStyle);
	virtual ~CScanIdb();

			short			SyncIdb(const char* pServerName, CStr* pStrError);
	virtual	void			CreateParamLst(void);
	virtual	void			CreateParamTbl(void);
};

#endif
