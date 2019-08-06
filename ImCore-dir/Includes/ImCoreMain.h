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
// ������������� ���������
#define	SEM_SETTINGS					1	// ���������
#define	SEM_PROTOCOL					2	// ��������
#define	SEM_DIGNOSTIC_TBL				3	// ������ � ��������������� �������
#define	SEM_BUSY_REG_IDB				4
#define	SEM_BUSY_SCAN_IDB				5
#define	SEM_REG_PACK_LST				6
#define	SEM_GRANTS_2					7	// ������ � ������� ���� �������
#define	SEM_CALCULATIONS				8	// ���������� ��������
#define	SEM_SETTINGS					1	// ��������� SrvCore
#define	MAX_SEM_CNT						20	// ���������� ���������

////////////////////////////////////////////////////////
// ������������� ���������� ������
#define	FLG_START_WND						1	// ��������� ����������� �������
#define	FLG_STOP_PROG						2	// ���������� ����������
#define	FLG_SYNC_REG_IDB					3	// ������������� ��� ����������� ������ � ������
#define	FLG_INIT_REG_IDB					4	// ������������� ��� ����������� ������ � ������
#define	FLG_SYNC_SCAN_IDB					5	// ������������� ��� ��ɣ�� ������ �� �������
#define	FLG_INIT_SCAN_IDB					6	// ������������� ��� ����������� ������ � ������
#define	FLG_START_SCAN_THREAD				7	// ��������� ����� ��ɣ�� ������ �� �������
#define	FLG_RUN_SCAN_THREAD					8	// ���� ��ɣ� ������ �� �������
#define	FLG_STOP_SCAN_THREAD				9	// ���������� ����� ��ɣ�� ������ �� �������
#define	FLG_START_PREPARE_DATA_THREAD		10	// ��������� ����� ������� �������� ������ �� ������� BEACON
#define	FLG_RUN_PREPARE_DATA_THREAD			11	// ���� ������ �������� ������ �� ������� BEACON
#define	FLG_STOP_PREPARE_DATA_THREAD		12	// ���������� ����� ������� �������� ������ �� ������� BEACON
#define	FLG_START_DIAGNOSTIC_THREAD			13	// ��������� ����� ���������� �����������
#define	FLG_RUN_DIAGNOSTIC					14	// �ģ� ���������� �����������
#define	FLG_STOP_DIAGNOSTIC_THREAD			15	// ���������� ����� ���������� �����������
#define	FLG_START_DOUT_THREAD				16	// ��������� ����� ������ ��������������� ����������
#define	FLG_RUN_DOUT_THREAD					17	// �ģ� ������ ��������������� ����������
#define	FLG_STOP_DOUT_THREAD				18	// ���������� ����� ������ ��������������� ����������
#define	FLG_INIT_CALC						19	// ������������� ���ޣ���� �����
#define	FLG_INPUT_TEFF						20	// ���� Teff
#define	FLG_START_SYNC_EXCHANGE				21	// ��������� ����� ������ � SyncAgent
#define	FLG_RUN_SYNC_EXCHANGE				22	// �ģ� ����� � SyncAgent
#define	FLG_STOP_SYNC_EXCHANGE				23	// ���������� ����� ������ � SyncAgent
#define	FLG_INPUT_TEFF_USER_CALL			24	// ���� ����������������� ������ ������� ����� Teff
#define	FLG_UPDATE_CONSTANTS				25	// ���������� ��������� �� Constants.idb

#define	FLG_START_DIF_CALC					29	// ��������� ����� ������������� ���ޣ��
#define	FLG_RUN_DIF_CALC					30	// �ģ� ������������ ���ޣ�
#define	FLG_STOP_DIF_CALC					31	// ���������� ����� ������������� ���ޣ��
#define	FLG_INIT_CORE_THREAD_DATA			32	// ������������� ������ ��� ������� Rebuild � DifCalc

#define	FLG_CHANGE_SCAN_DATA				40	// ���� ��������� ������� ������
#define	FLG_CHANGE_REBUILD_DATA				41	// ���� ��������� ������ ������������� ����
#define	FLG_CHANGE_DIF_DATA					42	// ���� ��������� ������ ������������� ���ޣ��




////////////////////////////////////////////////////////
// �������� StopStyle ��� ������� StopProg
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

// ���������� ������ ������ ��� ��������� ����������� ���������
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

// ������� ����������� ���������� ��������
extern	void*			ShowApp(void* ptr);						// ������ �������
extern	void*			SyncRegIdb(void* ptr);					// ������������� ��� ����������� ������ � �������
extern	void*			InitRegIdb(void* ptr);					// ������������� ��� ����������� ������ � �������
extern	void*			RegData(void* ptr);						// ����������� ������ � ������
extern	void*			SyncScanIdb(void* ptr);					// ������������� ��� ��ɣ�� ������ �� �������
extern	void*			InitScanIdb(void* ptr);					// ������������� ��� ��ɣ�� ������ �� �������
extern	void* 			ScanData(void* ptr);					// ��ɣ� ������ �� �������
extern	void*			DiagnosticThread(void* ptr);			// ��������������� �����
extern	void*			DoutThread(void* ptr);					// ����� ������ �����������
extern	void*		 	SyncExchangeThread(void* pStartThread);	// ����� ������ � SyncAgent
extern	void* 			PrepareData(void* ptr);					// ����� ������� �������� ������ �� ������� BEACON
extern	void*			InitCalc(void* ptr);					// ������������� ���ޣ���� �����
extern	void*			CoreDifCalc(void* ptr);					// ������������ ���ޣ�
extern	void*			InitCoreThreadData(void* ptr);			// ������������� ������ ��� ������� Rebuild � DifCalc

extern	CStr			MakeError(const char* pNameThread, const char*  pErrorNum, const char*  pMessageText);
extern	char*			MakeVersionStr(void);

#endif
