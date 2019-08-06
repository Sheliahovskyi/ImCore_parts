#ifndef _IMCORE_IDB_H
#define _IMCORE_IDB_H

class CProg;

//////////////////////////////////////////////////////////////////////
// class CProgParamInfo
class CProgParamInfo : public CParamInfo
{// �������� ������������� ���������� ��� ���
public:
	unsigned char	Value[5];
	char			ChangeFlag;				// ���� ��������� � ��������� ����� ������ CF_
	char			CalcShortName[33];		// ������� ���������� �������� ��������� � ������� ���ޣ���
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
{// �������� ������������� ���������� ��� ��� ����������� ������ � �������
public:
};

//////////////////////////////////////////////////////////////////////
// class CScanParamInfo
class CScanParamInfo : public CProgParamInfo
{// �������� ������������� ���������� ��� ��� ��ɣ�� ������ �� �������
public:

};

//////////////////////////////////////////////////////////////////////
// ���
class CProgIdb : public CInfoDataBase
{
public:
	CProg*			pProg;				// ��������� �� ����������
	CStr			SyncServerName;		// ��� ������� ������ � ������� ����������� �������������
	int				RejectParamCnt;		// ���������� ���������� ����������

	CHAR_LST*		pCalcShortNameLst;
	LONG_LST*		pParamInfoLst;

	CProgParamInfo*	pLastChangeTeff;	// ����������, ���������� ����� ���������� ��������� Teff

public:
	CProgIdb(CProg* pProg, const char* pPathIdb, const char* pLocalListPrefix, const char* pCommonFilePrefix, long IdbStyle, const char* pCorrectiveFileName=NULL);
	virtual ~CProgIdb();

			CParamInfo*		GetParamInfoByCalcShortName(const char* pBShortName);
	virtual	bool			InitIdb(CStr* pStrError);
	virtual	CParamInfo*		MakeParamInfo(CParamInfo* pParamInfo, long PosInTable, long PosInFile);
};

//////////////////////////////////////////////////////////////////////
// ��� ����������� ������ � �������
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
// ��� ��ɣ�� ������ �� �������
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
