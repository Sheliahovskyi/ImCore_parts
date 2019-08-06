#include <ImCorePMain.h>

///////////////////////////////////////////////////////////////////////
// фрейм для переменных
/*FXIMPLEMENT(CProgFrame, SignalFrame, NULL, 0)
CProgFrame::CProgFrame(CApp* pApp, CInfoDataBase* pIdb, CSignalFrameTbl* pSignalTbl, FXComposite* p, long FrameStyle, FXuint opts, FXint x, FXint y, FXint w, FXint h, FXint pl, FXint pr, FXint pt, FXint pb, FXint hs, FXint vs)
		  :SignalFrame(pApp, pIdb,pSignalTbl,p,FrameStyle|MODE_SHOW_SWID,opts,x,y,w,h,pl,pr,pt,pb,hs,vs){}
CProgFrame::~CProgFrame(){}
*/

///////////////////////////////////////////////////////////////////////////
// приложение
FXIMPLEMENT(CProgApp, CApp, NULL, 0)
CProgApp::CProgApp() : CApp( (char*)(CStr(::pProg->GetTaskName()) + "Console.ini"),NULL){}
CProgApp::CProgApp(char* pDisplayName) : CApp( (char*)(CStr(::pProg->GetTaskName()) + "Console.ini"),pDisplayName){}
CProgApp::~CProgApp(void){}


///////////////////////////////////////////////////////////////////////////
// таблица переменных для регистрации данных в сервере
/*CRegSignalTbl::CRegSignalTbl() : CSignalFrameTbl(STS_CREATE_VALUE_COL){}
CRegSignalTbl::~CRegSignalTbl(){}
void CRegSignalTbl::MakeColumns()
{
	CSignalFrameTbl::MakeColumns();
	AddCol("OpType", 		"Тип операции", 		SQL_C_CHAR,   6, 100);
	AddCol("MultiCoef", 	"Коэффициент", 			SQL_C_FLOAT,  0, 100);
}
char* CRegSignalTbl::GetColShowValue(CCol* pCol, long Pos)
{
	const char*	pStr = "";
	if( pCol == pValueCol )
	{// надо отрисовать текущее значение переменной
		int 			OutsideID  = pOutsideIDCol->Get(Pos);
		int				InfoBasePos= ::pProg->pRegIdb->OutsideIdToInfoBasePos(OutsideID);
		CProgParamInfo*	pParamInfo = (InfoBasePos?(CProgParamInfo*)::pProg->pRegIdb->pParamLst->Get(InfoBasePos):NULL);
		if(pParamInfo) pStr = GetShowSignalValue(pParamInfo, (unsigned char*)pParamInfo->Value, 15, 7);
	}
	else pStr = CSignalFrameTbl::GetColShowValue(pCol, Pos);
	return (char*)pStr;
}
short CRegSignalTbl::GetValueStatus(long OutsideID)
{
	short			cStatus		= STATUS_NORMAL;
	int				ParamPos   	= ::pProg->pRegIdb->OutsideIdToInfoBasePos(OutsideID);
	CProgParamInfo*	pParamInfo 	= ParamPos ? ((CProgParamInfo*)::pProg->pRegIdb->pParamLst->Get(ParamPos)) : NULL;
	if(pParamInfo)	cStatus		= *(pParamInfo->Value + pParamInfo->ValueByteSize);
	return cStatus;
}
*/


///////////////////////////////////////////////////////////////////////////
// таблица переменных для приёма данных от сервера
/*CScanSignalTbl::CScanSignalTbl() : CSignalFrameTbl(STS_CREATE_VALUE_COL){}
CScanSignalTbl::~CScanSignalTbl(){}
void CScanSignalTbl::MakeColumns()
{
	CSignalFrameTbl::MakeColumns();
	AddCol("OpType", 		"Тип операции", 		SQL_C_CHAR,   6, 100);
	AddCol("MultiCoef", 	"Коэффициент", 			SQL_C_FLOAT,  0, 100);
}
char* CScanSignalTbl::GetColShowValue(CCol* pCol, long Pos)
{
	const char*	pStr = "";
	if( pCol == pValueCol )
	{// надо отрисовать текущее значение переменной
		int 			OutsideID  = pOutsideIDCol->Get(Pos);
		int				InfoBasePos= ::pProg->pScanIdb->OutsideIdToInfoBasePos(OutsideID);
		CProgParamInfo*	pParamInfo = (InfoBasePos?(CProgParamInfo*)::pProg->pScanIdb->pParamLst->Get(InfoBasePos):NULL);
		if(pParamInfo) pStr = GetShowSignalValue(pParamInfo, (unsigned char*)pParamInfo->Value, 15, 7);
	}
	else pStr = CSignalFrameTbl::GetColShowValue(pCol, Pos);
	return (char*)pStr;
}
short CScanSignalTbl::GetValueStatus(long OutsideID)
{
	short			cStatus		= STATUS_NORMAL;
	int				ParamPos   	= ::pProg->pScanIdb->OutsideIdToInfoBasePos(OutsideID);
	CProgParamInfo*	pParamInfo 	= ParamPos ? ((CProgParamInfo*)::pProg->pScanIdb->pParamLst->Get(ParamPos)) : NULL;
	if(pParamInfo)	cStatus		= *(pParamInfo->Value + pParamInfo->ValueByteSize);
	return cStatus;
}
*/
///////////////////////////////////////////////////////////////////////////
// основное окно приложения
FXDEFMAP(CProgWnd) CProgWndMap[]=
{
	FXMAPFUNCS(SEL_COMMAND,	CProgWnd::ID_START_COMMAND,	 CProgWnd::ID_END_COMMAND, CMainWindow::onCmdParam)
};
FXIMPLEMENT(CProgWnd, CMainWindow, CProgWndMap, ARRAYNUMBER(CProgWndMap))
CProgWnd::CProgWnd(CApp* pApp) : CMainWindow(false,pApp,"",NULL,NULL,DECOR_ALL,0,0,600,300)
{
	CProgWnd::pTabBook			= NULL;
	CProgWnd::pDiagTabItem 		= NULL;
//	CProgWnd::pRegSignalTbl		= NULL;
//	CProgWnd::pRegSignalFrame	= NULL;
//	CProgWnd::pScanSignalTbl	= NULL;
//	CProgWnd::pScanSignalFrame	= NULL;
	CProgWnd::pDiagTreeList		= NULL;
	CProgWnd::pDiagTabItem		= NULL;

	CStr Title(::pProg->GetTaskName());
	if(::pProg->pSettings->NodeName.GetLength())	Title += CStr(" ")+pProg->pSettings->NodeName;
	if(::pProg->pSettings->ModuleName.GetLength())	Title += CStr(" ")+pProg->pSettings->ModuleName;
	setTitle((char*)Title);

	pTimeSection						= new FXLabel(pStatusBar,"",NULL,FRAME_SUNKEN|LAYOUT_RIGHT|LAYOUT_FILL_Y|LAYOUT_FIX_WIDTH|ICON_BEFORE_TEXT,0,0,150);
	pServerSection						= new FXLabel(pStatusBar,"",NULL,FRAME_SUNKEN|LAYOUT_RIGHT|LAYOUT_FILL_Y|LAYOUT_FIX_WIDTH|ICON_BEFORE_TEXT,0,0,140);

	// создадим общий контекст
	FXHorizontalFrame*	pContents		= new FXHorizontalFrame(this,LAYOUT_SIDE_TOP|FRAME_NONE|LAYOUT_FILL_X|LAYOUT_FILL_Y|PACK_UNIFORM_WIDTH);

	// создадим панель с закладками
	pTabBook							= new FXTabBook(pContents, NULL, 0, LAYOUT_FILL_X|LAYOUT_FILL_Y|LAYOUT_RIGHT);

	// закладка диагностики
	pDiagTabItem	= new FXTabItem(pTabBook, " Диагностика ", ::GetIcon(IDI_DIAG_GREEN_1));
	FXVerticalFrame*	pDiagFrame		= new FXVerticalFrame(pTabBook, FRAME_THICK|FRAME_RAISED);
	FXVerticalFrame*	pTreeListFrame	= new FXVerticalFrame(pDiagFrame, LAYOUT_FILL_X|LAYOUT_FILL_Y|FRAME_SUNKEN|FRAME_THICK, 0,0,0,0, 0,0,0,0);
	pDiagTreeList	= new FXTreeList(pTreeListFrame,0,this,ID_DIAG_LIST,LAYOUT_FILL_X|LAYOUT_FILL_Y|TREELIST_SHOWS_LINES|TREELIST_SHOWS_BOXES|TREELIST_ROOT_BOXES|TREELIST_SINGLESELECT);

//	// закладка с переменными для приёма данных от сервера
//	new FXTabItem(pTabBook, " ОБРВ->ПФР ", ::GetIcon(IDI_VARS));
//	pScanSignalTbl		= new CScanSignalTbl();	pScanSignalTbl->MakeColumns();	pScanSignalTbl->Caption	= "ScanSignalTbl";
//	pScanSignalFrame	= new SignalFrame(pApp, ::pProg->pScanIdb, pScanSignalTbl, pTabBook, 0, FRAME_THICK|FRAME_RAISED);
//	pScanSignalFrame->pStatusBar = pStatusBar;
//
//	// закладка с переменными для регистрации данных в сервере
//	new FXTabItem(pTabBook, " ПФР->ОБДРВ ", ::GetIcon(IDI_VARS));
//	pRegSignalTbl		= new CRegSignalTbl();	pRegSignalTbl->MakeColumns();	pRegSignalTbl->Caption	= "RegSignalTbl";
//	pRegSignalFrame		= new SignalFrame(pApp, ::pProg->pRegIdb, pRegSignalTbl, pTabBook, 0, FRAME_THICK|FRAME_RAISED);
//	pRegSignalFrame->pStatusBar = pStatusBar;

	// закладка с настройками
	if( ::pProg->pSettings->bEnableSettings )
	{
										  	  new FXTabItem(pTabBook, " Установки ", ::GetIcon(IDI_DEBUG));
		FXVerticalFrame*	pVerticalFrame	= new FXVerticalFrame(pTabBook, FRAME_LINE);
							pSettingsFrame  = new CSettingsFrame(pApp, ::pProg->pSettings, NULL, pVerticalFrame, LAYOUT_FILL_X|LAYOUT_FILL_Y, 0,0,0,0, 0,0,0,0);
							pSettingsFrame->bEnableChange 	= GrantsIsEnable(::pProg->pSettings->NodeName, ::pProg->GetTaskName(), GrantsGetUID(), "EditSettings");
	}
	else pSettingsFrame = NULL;

	// закладка с протоколом работы приложения
	new FXTabItem(pTabBook, " Протокол ", ::GetIcon(IDI_PROTOCOL));
	FXVerticalFrame*	pEventFrame		= new FXVerticalFrame(pTabBook, FRAME_THICK|FRAME_RAISED);
	CTblWnd*			pProtocolWnd	= new CTblWnd(TWS_NVGT_BAR, pEventFrame, FRAME_LINE|LAYOUT_FILL_X|LAYOUT_FILL_Y, 0,0,0,0, 0,0,0,0);
	pProg->pProtocol->bEnableChange = GrantsIsEnable(::pProg->pSettings->NodeName, ::pProg->GetTaskName(), GrantsGetUID(), "EditProtocol");
	pProtocolWnd->AttachTbl((CDrawTbl*)pProg->pProtocol->pProtocolTbl);
	CMainWindow::pMainProtocol = pProg->pProtocol;
}
CProgWnd::~CProgWnd(){}
void CProgWnd::create()
{
	CMainWindow::create();

	// подключим дерево диагностики к диагностическому потоку
	if(::pProg->pDiagTbl->Lock())
	{
		::pProg->pDiagTbl->AttachDiagList(pDiagTreeList);
		::pProg->pDiagTbl->Unlock();
	}
	// откроем первый элемент дерева
	FXTreeItem* pRootItem = pDiagTreeList->getFirstItem();
	if(pRootItem) pDiagTreeList->expandTree(pRootItem);
}
bool CProgWnd::Detach()
{
	if(::pProg->pDiagTbl->Lock())
	{// отключим дерево диагностики от диагностического потока
		::pProg->pDiagTbl->DetachDiagList();
		::pProg->pDiagTbl->Unlock();
	}

//	CStr Error;
//	if(pRegSignalFrame)
//	{// сохраним состояние элементов дерева и само дерево
//		if(!::pProg->pRegIdb->SaveAllList(&Error, pRegSignalFrame)) MessageBox(Error, MBOX_OK, ::GetIcon(IDI_WARNING));
//	}
//	if(pRegSignalTbl) delete pRegSignalTbl;

//	if(pScanSignalFrame)
//	{// сохраним состояние элементов дерева и само дерево
//		if(!::pProg->pScanIdb->SaveAllList(&Error, pScanSignalFrame)) MessageBox(Error, MBOX_OK, ::GetIcon(IDI_WARNING));
//	}
//	if(pScanSignalTbl) delete pScanSignalTbl;

	return CMainWindow::Detach();
}
void CProgWnd::ReadRegistry()
{
	CMainWindow::ReadRegistry();
//	if(pScanSignalFrame) pScanSignalFrame->ReadRegistry("ScanSignalList", "ScanSplitterPos", "ScanSignalFilter");
//	if(pRegSignalFrame) pRegSignalFrame->ReadRegistry("RegSignalList", "RegSplitterPos", "RegSignalFilter");
	if(pSettingsFrame ) pSettingsFrame->ReadRegistry();
}
void CProgWnd::WriteRegistry()
{
//	if(pScanSignalFrame)	pScanSignalFrame->WriteRegistry("ScanSignalList", "ScanSplitterPos", "ScanSignalFilter");
//	if(pRegSignalFrame)		pRegSignalFrame->WriteRegistry("RegSignalList", "RegSplitterPos", "RegSignalFilter");
	if(pSettingsFrame)		pSettingsFrame->WriteRegistry();
	CMainWindow::WriteRegistry();
}
long CProgWnd::RunCommand(int CommandID, void* pCommandData)
{
	switch(CommandID)
	{
		case ID_DIAG_UPDATE:
			if(bLiveWnd)
			{
				if(pCommandData) pDiagTabItem->setIcon((FXIcon*)pCommandData);
//				if(!::pProg->CurScanLayerTime.IsNull()) pTimeSection->setText((char*)TimeToStr(&::pProg->CurScanLayerTime, MTD_TIMESTAMP));
//				else									pTimeSection->setText("");
//				pServerSection->setText((char*)::pProg->CurScanServerName);
			}
		break;
//		case ID_UPDATE_VALUE_COL:
//			if(bLiveWnd)
//			{
//				if(pScanSignalTbl)	pScanSignalTbl->UpdateValueCol();
//				if(pRegSignalTbl)	pRegSignalTbl->UpdateValueCol();
//			}
//		break;
		default: return CMainWindow::RunCommand(CommandID, pCommandData);
	}
	return true;
}



