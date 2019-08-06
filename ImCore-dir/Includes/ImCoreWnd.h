#ifndef _IMCORE_WND_H
#define _IMCORE_WND_H


///////////////////////////////////////////////////////////////////////////
// класс приложения
class CProgApp : public CApp
{
	FXDECLARE(CProgApp)
public:

public:
	CProgApp();
	CProgApp(char* pDisplayName);
	virtual ~CProgApp(void);
};


///////////////////////////////////////////////////////////////////////
// фрейм для сигналов/списков
class CProgFrame : public SignalFrame
{
	FXDECLARE(CProgFrame)
public:
	enum
	{
		ID_FIRST=SignalFrame::ID_LAST,
		ID_LAST
	};
public:
	CProgFrame(){}
	CProgFrame(CApp* pApp, CInfoDataBase* pIdb, CSignalFrameTbl* pSignalTbl, FXComposite* p,long FrameStyle,FXuint opts=0,FXint x=0,FXint y=0,FXint w=0,FXint h=0,FXint pl=DEFAULT_SPACING,FXint pr=DEFAULT_SPACING,FXint pt=DEFAULT_SPACING,FXint pb=DEFAULT_SPACING,FXint hs=DEFAULT_SPACING,FXint vs=DEFAULT_SPACING);
	virtual~ CProgFrame();
};


///////////////////////////////////////////////////////////////////////////
// таблица переменных для регистрации данных в сервере
class CRegSignalTbl : public CSignalFrameTbl
{
public:
//	CCol*	pPortIDCol;
//	CCol*	pMBusAddrCol;
//	CCol*	pRegNumCol;
//	CCol*	pDataTypeCol;

public:
	CRegSignalTbl(void);
	virtual ~CRegSignalTbl(void);
	
	virtual	void		MakeColumns(void);	
	virtual	char*		GetColShowValue(CCol* pCol, long Pos);
//	virtual	FXColor		GetCellTextColor(CCol* pCol, long Row, bool bSelect, bool bCurrent, short PrintFlag=PF_SCREEN);
//	virtual	FXIcon*		GetCellIcon(CCol* pCol, long Row, bool bSelect, bool bCurrent, short PrintFlag=PF_SCREEN);
	virtual	short		GetValueStatus(long OutsideID);
};

///////////////////////////////////////////////////////////////////////////
// таблица переменных для приёма данных от сервера
class CScanSignalTbl : public CSignalFrameTbl
{
public:
//	CCol*	pPortIDCol;
//	CCol*	pMBusAddrCol;
//	CCol*	pRegNumCol;
//	CCol*	pDataTypeCol;

public:
	CScanSignalTbl(void);
	virtual ~CScanSignalTbl(void);

	virtual	void		MakeColumns(void);
	virtual	char*		GetColShowValue(CCol* pCol, long Pos);
//	virtual	FXColor		GetCellTextColor(CCol* pCol, long Row, bool bSelect, bool bCurrent, short PrintFlag=PF_SCREEN);
//	virtual	FXIcon*		GetCellIcon(CCol* pCol, long Row, bool bSelect, bool bCurrent, short PrintFlag=PF_SCREEN);
	virtual	short		GetValueStatus(long OutsideID);
};

//////////////////////////////////////////////////////////////////////////////
// класс окна
class CBaseProgWnd : public CMainWindow
{
	FXDECLARE(CBaseProgWnd)

public:
	FXTabItem*		pDiagTabItem;
public:
	CBaseProgWnd(){};
	CBaseProgWnd(bool bHideMode, CApp* pApp,const FXString& name,FXIcon *ic=NULL,FXIcon *mi=NULL,FXuint opts=DECOR_ALL,FXint x=0,FXint y=0,FXint w=0,FXint h=0,FXint hs=4,FXint vs=4);
	virtual ~CBaseProgWnd(void);
};

//////////////////////////////////////////////////////////////////////////////
// класс основного окна
class CProgWnd : public CBaseProgWnd
{
	FXDECLARE(CProgWnd)
	
public:
	FXTabBook*			pTabBook;
	FXLabel*				pTimeSection;
	FXLabel*				pServerSection;

	CRegSignalTbl*		pRegSignalTbl;
	SignalFrame*			pRegSignalFrame;

	CScanSignalTbl*		pScanSignalTbl;
	SignalFrame*			pScanSignalFrame;

	FXTreeList*			pDiagTreeList;
	CSettingsFrame*		pSettingsFrame;

	// Messages
	enum
	{
		ID_ABOUT=CMainWindow::ID_LAST,
		ID_DIAG_LIST,
		ID_DIAG_UPDATE,
		ID_UPDATE_VALUE_COL,
		ID_START_COMMAND,
		// ...
		ID_END_COMMAND,
		ID_LAST
    };
	
public:
	CProgWnd(){};
	CProgWnd(CProgApp* pApp);
	virtual ~CProgWnd(void);

	virtual	void	create();
	virtual	bool	Detach();
	virtual	void	ReadRegistry();
	virtual	void	WriteRegistry();
	virtual	long	RunCommand(int CommandID, void* pCommandData);
};

//////////////////////////////////////////////////////////////////////////////
// класс окна для ввода Teff
class CInputTeffWnd : public CBaseProgWnd
{
	FXDECLARE(CInputTeffWnd)

public:
	FXTextField*		pInputField;
	FXCheckButton*	pAutoCalcCheck;
	FXTextField*		pStopField;
	FXTextField*		pStartField;
	FXTextField*		pDownTimeField;

	FXTimer*			pTimer1;
	enum
	{// Messages
		ID_INPUT_TEFF=CBaseProgWnd::ID_LAST,
		ID_EXIT,
		ID_TIMER1,
		ID_LAST
    };

public:
	CInputTeffWnd(){};
	CInputTeffWnd(CProgApp* pApp);
	virtual ~CInputTeffWnd(void);

			long 	onTimeout1(FXObject* pObj, FXSelector ID, void* ptr);

	virtual	void	create();
//	virtual	bool	Detach();
	virtual	void	ReadRegistry(){}
	virtual	void	WriteRegistry(){}
	virtual	long	RunCommand(int CommandID, void* pCommandData);
};

#endif
