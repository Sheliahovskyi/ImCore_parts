#ifndef _IMCORE_MAIN_H
#define _IMCORE_MAIN_H

#ifndef _SRV_TOOLS_H
#include "SrvTools.h"
#endif
#ifndef _DIAGN_GATE_H
#include "DiagnGate.h"
#endif
#ifndef _IMCORE_IDB_H
#include "ImCoreIdb.h"
#endif
#ifndef _IMCORE_H
#include "ImCore.h"
#endif
#ifndef _IMCORE_WND_H
#include "ImCoreWnd.h"
#endif
#ifndef _IMCORE_DIAG_H
#include "ImCoreDiag.h"
#endif
#ifndef _IMCORE_CALC_H
#include "ImCoreCalc.h"
#endif
#ifndef IMCOREAKZ_H
#include "ImCoreAkZ.h"
#endif

#define MSG_INPUT_TEFF					10
#define MSG_UPDATE_CONSTANTS			11

////////////////////////////////////////////////////////
// иденификаторы семафоров
#define	SEM_SETTINGS					1	// установки
#define	SEM_PROTOCOL					2	// протокол
#define	SEM_DIGNOSTIC_TBL				3	// доступ к диагностической таблице
#define	SEM_BUSY_REG_IDB				4
#define	SEM_BUSY_SCAN_IDB				5
#define	SEM_REG_PACK_LST				6
#define	SEM_GRANTS_2					7	// доступ к системе прав доступа
#define	SEM_CALCULATIONS				8	// блокировка расчетов
#define	SEM_SETTINGS					1	// установки SrvCore
#define	MAX_SEM_CNT						20	// количество семафоров

////////////////////////////////////////////////////////
// иденификаторы глобальных флагов
#define	FLG_START_WND						1	// запустить управляющую консоль
#define	FLG_STOP_PROG						2	// остановить приложение
#define	FLG_SYNC_REG_IDB					3	// синхронизация ИБД регистрации данных в сервер
#define	FLG_INIT_REG_IDB					4	// инициализация ИБД регистрации данных в сервер
#define	FLG_SYNC_SCAN_IDB					5	// синхронизация ИБД приёма данных от сервера
#define	FLG_INIT_SCAN_IDB					6	// инициализация ИБД регистрации данных в сервер
#define	FLG_START_SCAN_THREAD				7	// запустить поток приёма данных от сервера
#define	FLG_RUN_SCAN_THREAD					8	// идет приём данных от сервера
#define	FLG_STOP_SCAN_THREAD				9	// остановить поток приёма данных от сервера
#define	FLG_START_PREPARE_DATA_THREAD		10	// запустить поток разбора принятых данных от системы BEACON
#define	FLG_RUN_PREPARE_DATA_THREAD			11	// идет разбор принятых данных от системы BEACON
#define	FLG_STOP_PREPARE_DATA_THREAD		12	// остановить поток разбора принятых данных от системы BEACON
#define	FLG_START_DIAGNOSTIC_THREAD			13	// запустить поток внутренней диагностики
#define	FLG_RUN_DIAGNOSTIC					14	// идёт внутренняя диагностика
#define	FLG_STOP_DIAGNOSTIC_THREAD			15	// остановить поток внутренней диагностики
#define	FLG_START_DOUT_THREAD				16	// запустить поток выдачи диагностической информации
#define	FLG_RUN_DOUT_THREAD					17	// идёт выдача диагностической информации
#define	FLG_STOP_DOUT_THREAD				18	// остановить поток выдачи диагностической информации
#define	FLG_INIT_CALC						19	// инициализация расчётной части
#define	FLG_INPUT_TEFF						20	// ввод Teff
#define	FLG_START_SYNC_EXCHANGE				21	// запустить поток обмена с SyncAgent
#define	FLG_RUN_SYNC_EXCHANGE				22	// идёт обмен с SyncAgent
#define	FLG_STOP_SYNC_EXCHANGE				23	// остановить поток обмена с SyncAgent
#define	FLG_INPUT_TEFF_USER_CALL			24	// флаг пользовательского вызова диалога ввода Teff
#define	FLG_UPDATE_CONSTANTS				25	// подгрузить константы из Constants.idb

#define	FLG_START_DIF_CALC					29	// запустить поток диффузионного расчёта
#define	FLG_RUN_DIF_CALC					30	// идёт диффузионный расчёт
#define	FLG_STOP_DIF_CALC					31	// остановить поток диффузионного расчёта
#define	FLG_INIT_CORE_THREAD_DATA			32	// инициализация данных для потоков Rebuild и DifCalc

#define	FLG_CHANGE_SCAN_DATA				40	// флаг изменения текущих данных
#define	FLG_CHANGE_REBUILD_DATA				41	// флаг изменения данных востановления поля
#define	FLG_CHANGE_DIF_DATA					42	// флаг изменения данных диффузионного расчёта




////////////////////////////////////////////////////////
// значения StopStyle для функции StopProg
#define	SS_INIT_REG_IDB				0x00000001
#define	SS_SYNC_REG_IDB				0x00000002
#define	SS_INIT_SCAN_IDB			0x00000004
#define	SS_SYNC_SCAN_IDB			0x00000008
#define	SS_DIAGNOSTIC				0x00000010
#define	SS_CONSOLE					0x00000020
#define	SS_DOUT						0x00000040
#define	SS_SYNC_EXCHANGE			0x00000080
#define	SS_SCAN_DATA				0x00000100
#define	SS_SEND_DATA				0x00000200
#define	SS_RECV_DATA				0x00000400
#define	SS_PREPARE_DATA				0x00000800
#define	SS_INIT_CALC				0x00001000
#define	SS_REG_DATA					0x00002000
#define	SS_DIF_CALC					0x00004000
#define	SS_CORE_THREAD_DATA			0x00008000
#define	SS_SINGLE_THREADS			SS_INIT_REG_IDB|SS_SYNC_REG_IDB|SS_INIT_SCAN_IDB|SS_SYNC_SCAN_IDB|SS_INIT_CALC|SS_CORE_THREAD_DATA
#define	SS_ALL_THREADS				SS_SINGLE_THREADS|SS_DIAGNOSTIC|SS_CONSOLE|SS_DOUT|SS_SYNC_EXCHANGE|SS_SCAN_DATA|SS_SEND_DATA|SS_RECV_DATA|SS_PREPARE_DATA|SS_REG_DATA|SS_DIF_CALC

struct	CStartThread
{// параметры запуска потока 
	int			ThreadID;					// идентификатор запущенного потока
	int			MsgType;					// тип сообщения
	char		MsgText[MAX_MSG_LEN];		// текст сообщения
	char		UID[10];					// UID в текстовом виде
	int			AnswerQueueID;				// очередь для ответа    
	int			MsgID;						// идентификатор сообщения    
};

extern	CProg*		pProg;
extern	CProgApp*	pProgApp;

//////////////////////////////////////////////////////////////////////
// функции

// функция запуска потока
extern	void			StartThread(long SemID, const char* pThreadName, void* (*pFunc)(void*), void* pFuncParam=NULL);

// функция остановки 
extern	void			StopProg(CProg* pLocalProg, long StopStyle);

// функция 
extern	void			AbortProg(int Signal);

// глобальные идентификаторы потоков  
extern	int				pidSyncRegIdb;
extern	int				pidInitRegIdb;
extern	int				pidSyncScanIdb;
extern	int				pidInitScanIdb;
extern	int				pidScanData;
extern	int				pidDiagnostic;
extern	int				pidDout;
extern	int				pidSyncExchange;
extern	int				pidPrepareData;
extern	int				pidInitCalc;
extern	int				pidInitCore;
extern	int				pidCoreDifCalc;

// глобальные тексты ошибок для постоянно действующих процессов
extern	CStr			ErrorSyncRegIdb;
extern	CStr			ErrorInitRegIdb;
extern	CStr			ErrorSyncScanIdb;
extern	CStr			ErrorInitScanIdb;
extern	CStr			ErrorScanData;
extern	CStr			ErrorDiagnostic;
extern	CStr			ErrorDout;
extern	CStr			ErrorPrepareData;
extern	CStr			ErrorInitCalc;
extern	CStr			ErrorInitCore;
extern	CStr			ErrorCoreDifCalc;

// функции выполняемые отдельными потоками
extern	void*			ShowApp(void* ptr);						// запуск консоли
extern	void*			SyncRegIdb(void* ptr);					// синхронизация ИБД регистрации данных в сервере
extern	void*			InitRegIdb(void* ptr);					// инициализация ИБД регистрации данных в сервере
extern	void*			RegData(void* ptr);						// регистрация данных в сервер
extern	void*			SyncScanIdb(void* ptr);					// синхронизация ИБД приёма данных от сервера
extern	void*			InitScanIdb(void* ptr);					// инициализация ИБД приёма данных от сервера
extern	void* 			ScanData(void* ptr);					// приём данных от сервера
extern	void*			DiagnosticThread(void* ptr);			// диагностический поток
extern	void*			DoutThread(void* ptr);					// поток вывода диагностики
extern	void*		 	SyncExchangeThread(void* pStartThread);	// поток обмена с SyncAgent
extern	void* 			PrepareData(void* ptr);					// поток разбора принятых данных от системы BEACON
extern	void*			InitCalc(void* ptr);					// инициализация расчётной части
extern	void*			CoreDifCalc(void* ptr);					// диффузионный расчёт
extern	void*			InitCoreThreadData(void* ptr);			// инициализация данных для потоков Rebuild и DifCalc

extern	CStr			MakeError(const char* pNameThread, const char*  pErrorNum, const char*  pMessageText);
extern	char*			MakeVersionStr(void);

#endif
