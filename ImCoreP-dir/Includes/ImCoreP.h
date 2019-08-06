#ifndef _IMCORE_P_H
#define _IMCORE_P_H
#include "ImCorePMain.h"

class	CProg;
class	CProgCalc;


//////////////////////////////////////////////////////////////////////
// настраиваемые параметры 
class CProgSettings : public CSettings
{
public:
	bool			bEnableSettings;			// разрешить просмотр и редактирование настроек

	CStr			LogPath;					// путь к log-файлам
	CStr			CfgPath;					// путь к конфигурационным файлам
	CStr			FilePrefix;					// префикс для используемых файлов

	int				MaxEventsCnt;				// максимальное количество событий в протоколе
	bool			bEnablePuts;				// флаг вывода в терминал
	bool			bEnableDiagnostic;

	CStr			NodeName;					// имя узла
	CStr			ModuleName;					// имя модуля
	int				DiagInterval;				// диагностический интервал (секунды)
	CStr			CompIdent;

	CStr			SrvHost;					// адрес для приёма заданий прогноза
	int				SrvPort;					// порт для приёма заданий прогноза

	int				AutoUnloginTimeout; 		// таймаут для автоматического отключения пользователя
	
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
	CDiagnGate*				pDiagnGate;					// шлюз для вывода диагностических данных

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
