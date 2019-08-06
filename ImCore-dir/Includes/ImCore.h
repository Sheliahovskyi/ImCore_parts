#ifndef _IMCORE_H
#define _IMCORE_H

class	CProg;
class	CRegIdb;
class	CScanIdb;
class	CProgCalc;
class   ImCoreAkZ;

//////////////////////////////////////////////////////////////////////
// ������ ��� ����������� ������
class CRegisterServer
{
public:
	CStr		ServerName;				// ��� �������
	CTime		LastRegTime;			// ����� ���������� ������������������� ������
	CTime		SystemRegTime;			// ��������� ����� ���������� ������������������� ������
	CShareMem*	pRegRequest;			// ������ �������� ������� �� ����������� ������ � SrvCore
	CShareMem*	pRegAnswer;				// ����� �������� ������� �� ����������� ������ � SrvCore
	int			RegRequestID;			// ������������� �������� ������� �� ����������� ������ � SrvCore
	long		LastRegisterParamCnt;	// ���-�� ���������� � ��������� ������������������ ������
	LONG_LST*	pRegisterPackLst;		// ������� ������� �� �����������
	CStr		Error;					// ����� ������
	CTime		PrevPackTime;
	int			MutexID;
	// �������� ������ ����������� ������
	CStr		RegChannelName;
	CStr		RegThreadName;
	int			RegThreadPid;
	int			RegThreadFlgStart;
	int			RegThreadFlgRun;
	int			RegThreadFlgStop;

	bool		bFullRegData;
	double		LastRegFullDataTime;
	long		GlobalNum;
	bool		bFirst;

public:
	CRegisterServer(const char* pServerName);
	virtual ~CRegisterServer();
	bool 	Lock(){	  return MutexID ? ::MutexLock(MutexID)	  : false; }
	bool	Unlock(){ return MutexID ? ::MutexUnlock(MutexID) : false; }
	bool	IsLock(){ return MutexID ? ::MutexIsLock(MutexID) : false; }
	void	FreeRequest();
	void	AddPacket(unsigned char* pPacketData, int PacketSize, int MaxPackCnt);
	void	ClearPacket();
};


//////////////////////////////////////////////////////////////////////
// ������������� ��������� 
class CProgSettings : public CSettings
{
public:
	bool			bEnableSettings;			// ��������� �������� � �������������� ��������

	CStr			IdbPath;					// ���� � ������ �������������� ���� ������
	CStr			LogPath;					// ���� � log-������
	CStr			AlgPath;					// ���� � alg-������
	CStr			CfgPath;					// ���� � ���������������� ������
	CStr			ScriptPath;					// ���� � ��������
	CStr			FilePrefix;					// ������� ��� ������������ ������
	CStr			CalcDataPath;				// ���� � ������ ������ ��� ���������� ������ޣ���
    CStr			ImCoreDataPath;				// ���� � ������ ������ ��� ���������� ������ޣ���

	int				MaxEventsCnt;				// ������������ ���������� ������� � ���������
	bool			bEnablePuts;				// ���� ������ � ��������
	bool			bEnableDiagnostic;
	bool			bEnableRegisterData;		// ��������� ����������� ������ � �������
	bool			bEnableScanData;			// ��������� ��ɣ� ������ �� �������
	bool			bEnableRealTime;			// ��������� ����������� ������� � ��������� ��������
	bool			bEnableSelfSync;			// ��������� ��������������� ������������� ������ ���
	bool			bReadHistory;				// ��������� ������� ������ ����������� �� ������� BEACON
	bool			bSendHistory;				// ��������� ������� ������ ������������ � ������� BEACON
	bool			bEnableOFSTR_06;			// ��������� ������ �� � ����� 0,6 ���.
	bool			bSendToFTP;					// ��������� �������� ������ ������ �� FTP
	bool			bRecvFromFTP;				// ��������� ��ɣ� ������ ������ �� FTP

	CStr			NodeName;					// ��� ����
	CStr			ModuleName;					// ��� ������
	int				DiagInterval;				// ��������������� �������� (�������)
	CStr			CompIdent;

	CStr			MainServer;					// ��� ��������� ������� ������
	CStr			ReservServer;				// ��� ���������� ������� ������

	CStr			SrvAgentTaskName;			// ��� ������ ��� SrvAgenta
	int				AutoUnloginTimeout; 		// ������� ��� ��������������� ���������� ������������

	int				TeffCheckInterval;			// ������� �� ������ ������� ��� ����� Teff (�������)
	CStr			TeffInputDisplay;			// ������� ������ ��������� ����� �eff

	CStr			TacktShortName;				// ����������, ��� ��������� ������� ����� ����������� ������ �� ������� ���������� ������

	int				EmptyTacktCounter;			// ���������� ���������� ��������� ������ �� ������ ��������� ���������� ������

	bool			bUseIEDC;					// ������������� ����� � ����������
	double			MinPowerCalc;				// ����������� �������� �������� ��� ������ ���ޣ��� � ������� BEACON
	double			MinPowerDpzCalc;			// ����������� �������� �������� ��� ������ �ޣ�� ��� � ������� BEACON
	
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
	CRegIdb*				pRegIdb;					// ��� ����������� ������ � �������
	CScanIdb*				pScanIdb;					// ��� ��ɣ�� ������ �� �������
	CDiagnGate*				pDiagnGate;					// ���� ��� ������ ��������������� ������

/*	CTime					CurRegLayerTime;			// ����� ���������� ������������������� ������
	CShareMem*				pRegRequest;				// ������ �������� ������� �� ����������� ������ � �������� SrvCore
	CShareMem*				pRegAnswer;					// ����� �������� ������� �� ����������� ������ � �������� SrvCore
	int						RegRequestID;				// ������������� �������� ������� �� ����������� ������ � �������� SrvCore
	LONG_LST*				pRegPackLst;				// ������� �� �����������
*/
	CStr					CurScanServerName;			// ��� �������� ������� ������
	CTime					CurScanLayerTime;			// ����� ���������� ��������� ������
	CShareMem*				pScanRequest;				// ������ �������� ������� �� ��ɣ�� ������ �� ��������� SrvCore
	CShareMem*				pScanAnswer;				// ����� �������� ������� �� ��ɣ�� ������ �� ��������� SrvCore
	int						ScanRequestID;				// ������������� �������� ������� �� ��ɣ�� ������ �� ��������� SrvCore

//	CStr					MainSendHost;				// �������� ftp ����� ��� �������� ������ � ������� BEACON
//	CStr					ReservSendHost;				// ��������� ftp ����� ��� �������� ������ � ������� BEACON
//	CStr					ConnectSendURL;				// ������� ftp ����� ��� �������� ������ � ������� BEACON
//	CTime					LastSendDataTime;			// ����� ��������� �������� ������ � ������� BEACON"

//	CStr					MainRecvHost;				// �������� ftp ����� ��� ��ɣ�� ������ �� ������� BEACON
//	CStr					ReservRecvHost;				// ��������� ftp ����� ��� ��ɣ�� ������ �� ������� BEACON
//	CStr					ConnectRecvURL;				// ������� ftp ����� ��� ��ɣ�� ������ �� ������� BEACON

	LONG_LST*				pRegisterServersLst;		// ������ �������� ��� ����������� ������

//	CTime					LastRecvBcnTxtFileTime;		// ����� ���������� ��ɣ�� ��������� ����� �� ������� BEACON
//	CTime					LastRecvBcnMonitorTime;		// ����� ���������� ��ɣ�� monitor-����� �� ������� BEACON
//	CTime					LastRecvBcnUpdaterTime;		// ����� ���������� ��ɣ�� updater-����� �� ������� BEACON

//	CTime					TimeInBcnTxtFile;			// ����� � �������� ����� �������� �� ������� BEACON
//	CTime					TimeInBcnMonitor;			// ����� � monitor-����� �������� �� ������� BEACON
//	CTime					TimeInBcnUpdater;			// ����� � updater-����� �������� �� ������� BEACON

//	CTime					ChangeTimeBcnTxtFile;		// ����� ���������� ��������� ��������� ����� ������������ � ������� BEACON
//	CTime					ChangeTimeBcnMonitorFile;	// ����� ���������� ��������� monitor-����� ������������ � ������� BEACON
//	CTime					ChangeTimeBcnUpdaterFile;	// ����� ���������� ��������� updater-����� ������������ � ������� BEACON

    ImCoreAkZ*              pAkz;                       // ������ ���������� ���
	CProgCalc*				pCalc;                      // ������ ���������� ��
	CStr					TeffInputDisplay;

public:
	CProg(const char* pPath, const char* pTaskName);
	virtual~ CProg();

	void 			AddEvent(long EventType, const char* lpEventText, const char* pUID=NULL);

	void			InitRegisterServersLst();

	bool			SyncData(CStr* pError=NULL);
	bool			ScanThread(const char* pThreadName, CStr* pError=NULL);
	bool 			RegThread(CRegisterServer* pServer);
	short			SendDataToLocalThread(const char* pThreadName, CStr* pError=NULL);
	short			RecvDataFromLocalThread(const char* pThreadName, CStr* pError=NULL);
	short			PrepareDataThread(const char* pThreadName, CStr* pError=NULL);
	bool			MakeSendFile(const char* pFileName, CStr* pError=NULL);

	unsigned char*	MakeRegisterPackage(int DataSize, int ValuesCnt);
	void			AppendRegisterPackage(unsigned char*& pPackage, CRegParamInfo*	pParamInfo, unsigned char* pValue);
	void			SendRegisterPackage(CTime PacketTime, unsigned char* pPackage, int PackageSize);
	int				GetRegisterPackageParamCnt(unsigned char* pPackage){ return *(int*)(pPackage+sizeof(int)); }
	void			FreeRegisterPackage(unsigned char* pPackage);

	void			PrepareScanData();
//	void			PrepareDataFile(CStr FileName);
//	void			PrepareMonitorFile(CStr FileName);
//	void			PrepareUpdaterFile(CStr FileName);

	virtual	int		SyncGetModuleState(void);
	virtual	CStr	SyncGetModuleMenu(const char* pPrevMenuStr);
	virtual	void	GrantsChangeUserNotify(bool bAutoUnlogin=false);
};

#endif
