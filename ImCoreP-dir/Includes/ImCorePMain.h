#pragma once

#ifndef _SRV_TOOLS_H
#include "SrvTools.h"
#endif
#ifndef _DIAGN_GATE_H
#include "DiagnGate.h"
#endif
#ifndef _IMCORE_P_H
#include "ImCoreP.h"
#endif
#ifndef _IMCORE_P_CLIENT_H
#include "ImCorePClient.h"
#endif
#ifndef _IMCORE_P_WND_H
#include "ImCorePWnd.h"
#endif
#ifndef _IMCORE_P_DIAG_H
#include "ImCorePDiag.h"
#endif
#ifndef _IMCORE_P_CALC_H
#include "ImCorePCalc.h"
#endif
#ifndef _FORECAST_H
#include "forecast.h"
#endif


class FORECAST;


class CProg;

////////////////////////////////////////////////////////
// иденификаторы семафоров
#define	SEM_SETTINGS					1	// установки
#define	SEM_PROTOCOL					2	// протокол
#define	SEM_DIGNOSTIC_TBL				3	// доступ к диагностической таблице
#define	SEM_GRANTS_2					4	// доступ к системе прав доступа
#define	SEM_CLIENT_LST					5	// доступ к списку клиентов
#define	MAX_SEM_CNT						20	// количество семафоров

////////////////////////////////////////////////////////
// иденификаторы глобальных флагов
#define	FLG_START_WND						1	// запустить управляющую консоль
#define	FLG_STOP_PROG						2	// остановить приложение
#define	FLG_START_DIAGNOSTIC_THREAD			3	// запустить поток внутренней диагностики
#define	FLG_RUN_DIAGNOSTIC					4	// идёт внутренняя диагностика
#define	FLG_STOP_DIAGNOSTIC_THREAD			5	// остановить поток внутренней диагностики
#define	FLG_START_DOUT_THREAD				6	// запустить поток выдачи диагностической информации
#define	FLG_RUN_DOUT_THREAD					7	// идёт выдача диагностической информации
#define	FLG_STOP_DOUT_THREAD				8	// остановить поток выдачи диагностической информации
#define	FLG_START_NET_THREAD				9	// запустить поток приёма сетевых соединений
#define	FLG_RUN_NET_THREAD					10	// идёт приём сетевых соединений
#define	FLG_STOP_NET_THREAD					11	// остановить поток приёма сетевых соединений
#define	FLG_START_SYNC_EXCHANGE				12	// запустить поток обмена с SyncAgent
#define	FLG_RUN_SYNC_EXCHANGE				13	// идёт обмен с SyncAgent
#define	FLG_STOP_SYNC_EXCHANGE				14	// остановить поток обмена с SyncAgent
#define FLG_FC_STATUS                       15  // запуск прогноза


////////////////////////////////////////////////////////
// значения StopStyle для функции StopGate
#define	SS_DIAGNOSTIC				0x00000001
#define	SS_CONSOLE					0x00000002
#define	SS_DOUT						0x00000004
#define	SS_SYNC_EXCHANGE			0x00000008
#define	SS_NETWORK					0x00000010
#define	SS_CLIENT					0x00000020
#define	SS_SINGLE_THREADS			0
#define	SS_ALL_THREADS				SS_SINGLE_THREADS|SS_DIAGNOSTIC|SS_CONSOLE|SS_DOUT|SS_SYNC_EXCHANGE|SS_NETWORK|SS_CLIENT

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
extern	int				pidDiagnostic;
extern	int				pidDout;
extern	int				pidSyncExchange;
extern	int				pidNetwork;

// глобальные тексты ошибок для постоянно действующих процессов
extern	CStr			ErrorDiagnostic;
extern	CStr			ErrorDout;
extern	CStr			ErrorNetwork;

// функции выполняемые отдельными потоками
extern	void*			ShowApp(void* ptr);								// запуск консоли
extern	void*			DiagnosticThread(void* ptr);					// диагностический поток
extern	void*			DoutThread(void* ptr);							// поток вывода диагностики
extern	void*		 	SyncExchangeThread(void* pStartThread);			// поток обмена с SyncAgent
extern	void*			NetThread(void* ptr);							// поток приёма сетевых соединений
extern	void*			PredictClientThread(void* pStartThreadParams);	// поток работы с клиентом

extern	CStr			MakeError(const char* pNameThread, const char*  pErrorNum, const char*  pMessageText);
extern	char*			MakeVersionStr(void);
