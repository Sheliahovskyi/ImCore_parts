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
// ������������� ���������
#define	SEM_SETTINGS					1	// ���������
#define	SEM_PROTOCOL					2	// ��������
#define	SEM_DIGNOSTIC_TBL				3	// ������ � ��������������� �������
#define	SEM_GRANTS_2					4	// ������ � ������� ���� �������
#define	SEM_CLIENT_LST					5	// ������ � ������ ��������
#define	MAX_SEM_CNT						20	// ���������� ���������

////////////////////////////////////////////////////////
// ������������� ���������� ������
#define	FLG_START_WND						1	// ��������� ����������� �������
#define	FLG_STOP_PROG						2	// ���������� ����������
#define	FLG_START_DIAGNOSTIC_THREAD			3	// ��������� ����� ���������� �����������
#define	FLG_RUN_DIAGNOSTIC					4	// �ģ� ���������� �����������
#define	FLG_STOP_DIAGNOSTIC_THREAD			5	// ���������� ����� ���������� �����������
#define	FLG_START_DOUT_THREAD				6	// ��������� ����� ������ ��������������� ����������
#define	FLG_RUN_DOUT_THREAD					7	// �ģ� ������ ��������������� ����������
#define	FLG_STOP_DOUT_THREAD				8	// ���������� ����� ������ ��������������� ����������
#define	FLG_START_NET_THREAD				9	// ��������� ����� ��ɣ�� ������� ����������
#define	FLG_RUN_NET_THREAD					10	// �ģ� ��ɣ� ������� ����������
#define	FLG_STOP_NET_THREAD					11	// ���������� ����� ��ɣ�� ������� ����������
#define	FLG_START_SYNC_EXCHANGE				12	// ��������� ����� ������ � SyncAgent
#define	FLG_RUN_SYNC_EXCHANGE				13	// �ģ� ����� � SyncAgent
#define	FLG_STOP_SYNC_EXCHANGE				14	// ���������� ����� ������ � SyncAgent
#define FLG_FC_STATUS                       15  // ������ ��������


////////////////////////////////////////////////////////
// �������� StopStyle ��� ������� StopGate
#define	SS_DIAGNOSTIC				0x00000001
#define	SS_CONSOLE					0x00000002
#define	SS_DOUT						0x00000004
#define	SS_SYNC_EXCHANGE			0x00000008
#define	SS_NETWORK					0x00000010
#define	SS_CLIENT					0x00000020
#define	SS_SINGLE_THREADS			0
#define	SS_ALL_THREADS				SS_SINGLE_THREADS|SS_DIAGNOSTIC|SS_CONSOLE|SS_DOUT|SS_SYNC_EXCHANGE|SS_NETWORK|SS_CLIENT

struct	CStartThread
{// ��������� ������� ������
    int			ThreadID;					// ������������� ����������� ������
    int			MsgType;					// ��� ���������
    char		MsgText[MAX_MSG_LEN];		// ����� ���������
    char		UID[10];					// UID � ��������� ����
    int			AnswerQueueID;				// ������� ��� ������
    int			MsgID;						// ������������� ���������
};

extern	CProg*		pProg;
extern	CProgApp*	pProgApp;

//////////////////////////////////////////////////////////////////////
// �������

// ������� ������� ������
extern	void			StartThread(long SemID, const char* pThreadName, void* (*pFunc)(void*), void* pFuncParam=NULL);

// ������� ���������
extern	void			StopProg(CProg* pLocalProg, long StopStyle);

// �������
extern	void			AbortProg(int Signal);

// ���������� �������������� �������
extern	int				pidDiagnostic;
extern	int				pidDout;
extern	int				pidSyncExchange;
extern	int				pidNetwork;

// ���������� ������ ������ ��� ��������� ����������� ���������
extern	CStr			ErrorDiagnostic;
extern	CStr			ErrorDout;
extern	CStr			ErrorNetwork;

// ������� ����������� ���������� ��������
extern	void*			ShowApp(void* ptr);								// ������ �������
extern	void*			DiagnosticThread(void* ptr);					// ��������������� �����
extern	void*			DoutThread(void* ptr);							// ����� ������ �����������
extern	void*		 	SyncExchangeThread(void* pStartThread);			// ����� ������ � SyncAgent
extern	void*			NetThread(void* ptr);							// ����� ��ɣ�� ������� ����������
extern	void*			PredictClientThread(void* pStartThreadParams);	// ����� ������ � ��������

extern	CStr			MakeError(const char* pNameThread, const char*  pErrorNum, const char*  pMessageText);
extern	char*			MakeVersionStr(void);
