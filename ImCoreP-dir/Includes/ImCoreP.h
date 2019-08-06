#ifndef _IMCORE_P_H
#define _IMCORE_P_H
#include "ImCorePMain.h"

class	CProg;
class	CProgCalc;


//////////////////////////////////////////////////////////////////////
// ������������� ��������� 
class CProgSettings : public CSettings
{
public:
	bool			bEnableSettings;			// ��������� �������� � �������������� ��������

	CStr			LogPath;					// ���� � log-������
	CStr			CfgPath;					// ���� � ���������������� ������
	CStr			FilePrefix;					// ������� ��� ������������ ������

	int				MaxEventsCnt;				// ������������ ���������� ������� � ���������
	bool			bEnablePuts;				// ���� ������ � ��������
	bool			bEnableDiagnostic;

	CStr			NodeName;					// ��� ����
	CStr			ModuleName;					// ��� ������
	int				DiagInterval;				// ��������������� �������� (�������)
	CStr			CompIdent;

	CStr			SrvHost;					// ����� ��� ��ɣ�� ������� ��������
	int				SrvPort;					// ���� ��� ��ɣ�� ������� ��������

	int				AutoUnloginTimeout; 		// ������� ��� ��������������� ���������� ������������
	
public:
	CProgSettings(FXRegistry* pRegistry);
	~CProgSettings(void);

    virtual bool SetValue(const char* pSectionName, const char* pSettingName, const char* pStrValue);
};


//////////////////////////////////////////////////////////////////////
// 
class CProg : public CCore
{
public:
	FXRegistry*				pRegistry;					// ������ ���������� (�������� ��������)
	CProgSettings*			pSettings;					// ��������� ����������
	CProtocol*				pProtocol;					// �������� ������ ����������
	CDiagnGate*				pDiagnGate;					// ���� ��� ������ ��������������� ������

	int						SrvSocket;
	LONG_LST*				pClientLst;

public:
	CProg(const char* pPath, const char* pTaskName);
	virtual~ CProg();

	void 			AddEvent(long EventType, const char* lpEventText, const char* pUID=NULL);

	bool			NetThread(const char* pThreadName, CStr* pError=NULL);

	virtual	int		SyncGetModuleState(void);
	virtual	CStr	SyncGetModuleMenu(const char* pPrevMenuStr);
	virtual	void	GrantsChangeUserNotify(bool bAutoUnlogin=false);
};

#endif
