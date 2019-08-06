#ifndef _IMCORE_H
#define _IMCORE_H

class	CProg;
class	CRegIdb;
class	CScanIdb;
class	CProgCalc;
class   ImCoreAkZ;

//////////////////////////////////////////////////////////////////////
// сервер для регистрации данных
class CRegisterServer
{
public:
	CStr		ServerName;				// имя сервера
	CTime		LastRegTime;			// время последнего зарегистрированного пакета
	CTime		SystemRegTime;			// системное время последнего зарегистрированного пакета
	CShareMem*	pRegRequest;			// данные текущего запроса по регистрации данных в SrvCore
	CShareMem*	pRegAnswer;				// ответ текущего запроса по регистрации данных в SrvCore
	int			RegRequestID;			// идентификатор текущего запроса по регистрации данных в SrvCore
	long		LastRegisterParamCnt;	// кол-во параметров в последнем зарегистрированном пакете
	LONG_LST*	pRegisterPackLst;		// очередь пакетов на регистрацию
	CStr		Error;					// текст ошибки
	CTime		PrevPackTime;
	int			MutexID;
	// атрибуты потока регистрации данных
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
// настраиваемые параметры 
class CProgSettings : public CSettings
{
public:
	bool			bEnableSettings;			// разрешить просмотр и редактирование настроек

	CStr			IdbPath;					// путь к файлам информационной базы данных
	CStr			LogPath;					// путь к log-файлам
	CStr			AlgPath;					// путь к alg-файлам
	CStr			CfgPath;					// путь к конфигурационным файлам
	CStr			ScriptPath;					// путь к скриптам
	CStr			FilePrefix;					// префикс для используемых файлов
	CStr			CalcDataPath;				// путь к файлам данных для подсистемы физрасчётов
    CStr			ImCoreDataPath;				// путь к файлам данных для подсистемы физрасчётов

	int				MaxEventsCnt;				// максимальное количество событий в протоколе
	bool			bEnablePuts;				// флаг вывода в терминал
	bool			bEnableDiagnostic;
	bool			bEnableRegisterData;		// разрешить регистрацию данных в сервере
	bool			bEnableScanData;			// разрешить приём данных от сервера
	bool			bEnableRealTime;			// разрешить регистрацию пакетов с системным временем
	bool			bEnableSelfSync;			// разрешить самостоятельную синхронизацию файлов ИБД
	bool			bReadHistory;				// сохранять историю данных принимаемых из системы BEACON
	bool			bSendHistory;				// сохранять историю данных передаваемых в систему BEACON
	bool			bEnableOFSTR_06;			// выполнять расчет АО в цикле 0,6 сек.
	bool			bSendToFTP;					// разрешить передачу файлов данных по FTP
	bool			bRecvFromFTP;				// разрешить приём файлов данных по FTP

	CStr			NodeName;					// имя узла
	CStr			ModuleName;					// имя модуля
	int				DiagInterval;				// диагностический интервал (секунды)
	CStr			CompIdent;

	CStr			MainServer;					// имя основного сервера данных
	CStr			ReservServer;				// имя резервного сервера данных

	CStr			SrvAgentTaskName;			// имя задачи для SrvAgenta
	int				AutoUnloginTimeout; 		// таймаут для автоматического отключения пользователя

	int				TeffCheckInterval;			// уставка на период простоя для ввода Teff (секунды)
	CStr			TeffInputDisplay;			// дисплей вывода программы ввода Тeff

	CStr			TacktShortName;				// переменная, при изменении которой будет выполняться расчет по приходу измеренных данных

	int				EmptyTacktCounter;			// количество пропущеных стартовых тактов до начала обработки измеренных данных

	bool			bUseIEDC;					// использование токов с коррекцией
	double			MinPowerCalc;				// минимальное значение мощности для начала расчётов в системе BEACON
	double			MinPowerDpzCalc;			// минимальное значение мощности для начала учёта ДПЗ в системе BEACON
	
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
	FXRegistry*				pRegistry;					// реестр приложения (хранение настроек)
	CProgSettings*			pSettings;					// настройки приложения
	CProtocol*				pProtocol;					// протокол работы приложения
	CRegIdb*				pRegIdb;					// ИБД регистрации данных в сервере
	CScanIdb*				pScanIdb;					// ИБД приёма данных от сервера
	CDiagnGate*				pDiagnGate;					// шлюз для вывода диагностических данных

/*	CTime					CurRegLayerTime;			// время последнего зарегистрированного пакета
	CShareMem*				pRegRequest;				// данные текущего запроса по регистрации данных в основном SrvCore
	CShareMem*				pRegAnswer;					// ответ текущего запроса по регистрации данных в основном SrvCore
	int						RegRequestID;				// идентификатор текущего запроса по регистрации данных в основном SrvCore
	LONG_LST*				pRegPackLst;				// очередь на регистрацию
*/
	CStr					CurScanServerName;			// имя текущего сервера данных
	CTime					CurScanLayerTime;			// время последнего принятого пакета
	CShareMem*				pScanRequest;				// данные текущего запроса по приёму данных из основного SrvCore
	CShareMem*				pScanAnswer;				// ответ текущего запроса по приёму данных из основного SrvCore
	int						ScanRequestID;				// идентификатор текущего запроса по приёму данных из основного SrvCore

//	CStr					MainSendHost;				// основной ftp адрес для передачи данных в систему BEACON
//	CStr					ReservSendHost;				// резервный ftp адрес для передачи данных в систему BEACON
//	CStr					ConnectSendURL;				// текущий ftp адрес для передачи данных в систему BEACON
//	CTime					LastSendDataTime;			// время последней передачи данных в систему BEACON"

//	CStr					MainRecvHost;				// основной ftp адрес для приёма данных от системы BEACON
//	CStr					ReservRecvHost;				// резервный ftp адрес для приёма данных от системы BEACON
//	CStr					ConnectRecvURL;				// текущий ftp адрес для приёма данных от системы BEACON

	LONG_LST*				pRegisterServersLst;		// список серверов для регистрации данных

//	CTime					LastRecvBcnTxtFileTime;		// время последнего приёма тектового файла от системы BEACON
//	CTime					LastRecvBcnMonitorTime;		// время последнего приёма monitor-файла от системы BEACON
//	CTime					LastRecvBcnUpdaterTime;		// время последнего приёма updater-файла от системы BEACON

//	CTime					TimeInBcnTxtFile;			// время в тектовом файле принятом от системы BEACON
//	CTime					TimeInBcnMonitor;			// время в monitor-файле принятом от системы BEACON
//	CTime					TimeInBcnUpdater;			// время в updater-файле принятом от системы BEACON

//	CTime					ChangeTimeBcnTxtFile;		// время последнего изменения тектового файла находящегося в системе BEACON
//	CTime					ChangeTimeBcnMonitorFile;	// время последнего изменения monitor-файла находящегося в системе BEACON
//	CTime					ChangeTimeBcnUpdaterFile;	// время последнего изменения updater-файла находящегося в системе BEACON

    ImCoreAkZ*              pAkz;                       // расчет параметров АкЗ
	CProgCalc*				pCalc;                      // расчет параметров РУ
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
