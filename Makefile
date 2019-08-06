# Common Makefile (Linux)
# last edition 06.03.2019

MAKE_DATE = $(shell date +'%0e.%0m.%Y %R')
PROJECT_DIR = .

KVER = $(shell uname -r)
KMAJ = $(shell echo $(KVER) | sed -e 's/^\([0-9][0-9]*\)\.[0-9][0-9]*\.[0-9][0-9]*.*/\1/')
KMIN = $(shell echo $(KVER) | sed -e 's/^[0-9][0-9]*\.\([0-9][0-9]*\)\.[0-9][0-9]*.*/\1/')
MAKE_KERNEL = $(KMAJ).$(KMIN)

CXXCODEFLAGS = -DHAVE_DLFCN_H=1 -DTIME_WITH_SYS_TIME=1 -DHAVE_SYS_WAIT_H=1 -DHAVE_DIRENT_H=1 -DHAVE_UNISTD_H=1 -DHAVE_SYS_PARAM_H=1 -DHAVE_SYS_SELECT_H=1 -DHAVE_ZLIB_H=1 -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64
CXXSPECIFICFLAGS = -DMAKE_DATE="\"$(MAKE_DATE)\"" -DMAKE_KERNEL="\"$(KMAJ).$(KMIN)\"" -D_REENTRANT -D_POSIX_PTHREAD_SEMANTICS -DFAKE_ODBC
#-D_ENABLE_MKSEC

CXX = g++ -m32

CXXVERSION = $(shell $(CXX) --version)
ifeq ($(CXXVERSION), 2.96)
	CXXLANGFLAGS = -fwritable-strings
	CXXSPECIFICFLAGS += -D__USE_LARGEFILE64
	XLIBPATH = /usr/X11R6/lib
else
	XLIBPATH = /usr/xorg/lib
endif

ifneq ($(MAKE_KERNEL), 2.4)
	CXXSPECIFICFLAGS += -DUSE_NPTL
endif


ifeq (, $(findstring $(r), $(RELEASES)))
    r = z5
endif


ifeq ($(img), 1)
    CXXCODEFLAGS += -DHAVE_JPEG_H=1 -DHAVE_PNG_H=1 
endif

ifeq ($(r), su1)
    CXXSPECIFICFLAGS += -DNPP_SU -DNPP_SU1
endif
ifeq ($(r), su2)
    CXXSPECIFICFLAGS += -DNPP_SU -DNPP_SU2
endif
ifeq ($(r), z1)
    CXXSPECIFICFLAGS += -DNPP_Z -DNPP_Z1
endif
ifeq ($(r), z2)
    CXXSPECIFICFLAGS += -DNPP_Z -DNPP_Z2
endif
ifeq ($(r), z3)
    CXXSPECIFICFLAGS += -DNPP_Z -DNPP_Z3
endif
ifeq ($(r), z4)
    CXXSPECIFICFLAGS += -DNPP_Z -DNPP_Z4
endif
ifeq ($(r), z5)
    CXXSPECIFICFLAGS += -DNPP_Z -DNPP_Z5
endif
ifeq ($(r), z6)
    CXXSPECIFICFLAGS += -DNPP_Z -DNPP_Z6
endif
ifeq ($(r), hm1)
    CXXSPECIFICFLAGS += -DNPP_HM -DNPP_HM1
endif
ifeq ($(r), hm2)
    CXXSPECIFICFLAGS += -DNPP_HM -DNPP_HM2
endif
ifeq ($(r), upo)
    CXXSPECIFICFLAGS += -DNPP_Z
# -DNPP_SU2
endif

LIBSENSORVERSION = $(shell sensors --version | sed -e ' s/[^0-9.]//g; s/\([0-9].[0-9].[0-9]\)//; s/\(.[0-9].[0-9]\)//')

ifeq ($(LIBSENSORVERSION), 2)
    CXXSPECIFICFLAGS += -DSENS_V2
else
    CXXSPECIFICFLAGS += -DSENS_V3    
endif

ifeq ($(r),$(filter $(r),su1 su2 z1 z2 z3 z4 z5 upo))
    LDDIAGS += -lnetsnmp -lcrypto -lssl
endif

LDOPTIONS = -L. -lpthread -L$(XLIBPATH) -lX11 -lXext -lXtst -lXft -L/lib -L/usr/lib -lfontconfig -lcrypt

ifeq ($(wildcard /usr/lib/libjpeg.a),) 
    IMGLIBS += -ljpeg
else 
    IMGLIBS += /usr/lib/libjpeg.a
endif
ifeq ($(wildcard /usr/lib/libpng.a),) 
    IMGLIBS += -lpng
else 
    IMGLIBS += /usr/lib/libpng.a
endif

INCLUDES = \
	-I$(PROJECT_DIR)/libcurl/ \
	-I/usr/include \
	-I/usr/xorg/include \
	-I/usr/include/freetype2 \
	-I$(PROJECT_DIR)/libCrTools.a-dir/Includes/ \
	-I$(PROJECT_DIR)/libSrvAgentAPI.a-dir/Includes/ \
	-I$(PROJECT_DIR)/libSrvTools.a-dir/Includes/ \
	-I$(PROJECT_DIR)/libFox.a-dir/Includes/ \
	-I$(PROJECT_DIR)/libSrvTemplate.a-dir/Includes/ \
	-I$(PROJECT_DIR)/libBrowser.a-dir/Includes/ \
	-I$(PROJECT_DIR)/libSvrk.a-dir/Includes/ \
	-I$(PROJECT_DIR)/libSync.a-dir/Includes/ \
	-I$(PROJECT_DIR)/libDiagnTools.a-dir/Includes/ \
	-I$(PROJECT_DIR)/libDiagn.a-dir/Includes/ \
	-I$(PROJECT_DIR)/libVoyage.a-dir/Includes/ \
	-I$(PROJECT_DIR)/libpcap/	\
	-I$(PROJECT_DIR)/libAlgLib.a-dir/Includes \
	-I$(PROJECT_DIR)/ImCoreAkZ-dir/Includes

CXXWARNFLAGS = -std=gnu++11
CXXDEBUGFLAGS = -g
CXXOPTIMIZEFLAGS = -O0

CXXFLAGS = $(INCLUDES) $(CXXWARNFLAGS) $(CXXDEBUGFLAGS) $(CXXOPTIMIZEFLAGS) $(CXXCODEFLAGS) $(CXXLANGFLAGS) $(CXXSPECIFICFLAGS)

LD = $(CXX)

AR = ar
AROPTIONS = 

libs=libCrTools.a libSrvAgentAPI.a libFox libSrvTools.a libDiagn.a libSrvTemplate.a libBrowser.a libSvrk.a libSync.a libVoyage.a libDiagnTools.a
tools=SendMsg 
#UpdateSet
prog=SrvAgent SrvCore SrvBrowser ImGKso ArcMan
#ViBrowser ImGKso
gate=GateFkd GateIvs GateCruise GateAmur GateGcn GateRs GateBcn
#GateTempl GateRepl 
#GateLsb GateBod
#GateKso 
#GateIvv
diagn=ClientDiagn GateDiagn SrvDiagn
sync=SyncAgent SyncSrv 
#SyncEd
fc=FcBrowser

lib : $(libs)
prog : $(prog)
tools : $(tools)
gate : $(gate)
diagn : $(diagn)
sync : $(sync)
fc : $(fc)
all : lib tools prog sync fc gate #diagn 


TRG		= libCrTools.a
TRG_PATH	= $(TRG)-dir
TRG_OBJ_PATH	= $(TRG_PATH)/Objects
TRG_SRC		= $(TRG_OBJ_PATH)/CrCol.o\
		$(TRG_OBJ_PATH)/CrCore.o\
		$(TRG_OBJ_PATH)/CrCStr.o\
		$(TRG_OBJ_PATH)/CrFakeOdbc.o\
		$(TRG_OBJ_PATH)/CrGlobal.o\
		$(TRG_OBJ_PATH)/CrLst.o\
		$(TRG_OBJ_PATH)/CrNetApi.o\
		$(TRG_OBJ_PATH)/CrObj.o\
		$(TRG_OBJ_PATH)/CrOdbcApi.o\
		$(TRG_OBJ_PATH)/CrTbl.o\
		$(TRG_OBJ_PATH)/CrCrypt.o\
		$(TRG_OBJ_PATH)/CrTime.o
$(TRG) :	$(TRG_SRC)
		rm -f $@
		$(AR) -cr $(AROPTIONS) $@ $^
		ranlib $@
$(TRG_OBJ_PATH)/%.o : libCrTools.a-dir/Sources/%.cpp
	$(CXX) -c -o $@  -IlibCrTools.a-dir/Includes -IlibCrTools.a-dir/Sources $(CXXFLAGS) $<

TRG		= libSrvAgentAPI.a
TRG_PATH	= $(TRG)-dir
TRG_OBJ_PATH	= $(TRG_PATH)/Objects
TRG_SRC		= $(TRG_OBJ_PATH)/SrvAgentAPI.o
$(TRG) :	$(TRG_SRC)
		rm -f $@
		$(AR) -cr $(AROPTIONS) $@ $^
		ranlib $@
$(TRG_OBJ_PATH)/%.o : $(TRG_PATH)/Sources/%.cpp
	$(CXX) -c -o $@  -IlibSrvAgentAPI.a-dir/Includes -IlibSrvAgentAPI.a-dir/Sources $(CXXFLAGS) $<

.PHONY:		libFox
libFox :	libFox.a libFoxI.a

TRG		= libFox.a
TRG_PATH	= $(TRG)-dir
TRG_OBJ_PATH	= $(TRG_PATH)/Objects
TRG_SRC		= $(TRG_OBJ_PATH)/FX4Splitter.o\
		$(TRG_OBJ_PATH)/FXApp.o\
		$(TRG_OBJ_PATH)/FXAccelTable.o\
		$(TRG_OBJ_PATH)/FXArrowButton.o\
		$(TRG_OBJ_PATH)/FXBMPIcon.o\
		$(TRG_OBJ_PATH)/FXBMPImage.o\
		$(TRG_OBJ_PATH)/FXBitmap.o\
		$(TRG_OBJ_PATH)/FXButton.o\
		$(TRG_OBJ_PATH)/FXCURCursor.o\
		$(TRG_OBJ_PATH)/FXCanvas.o\
		$(TRG_OBJ_PATH)/FXCharset.o\
		$(TRG_OBJ_PATH)/FXCheckButton.o\
		$(TRG_OBJ_PATH)/FXColorBar.o\
		$(TRG_OBJ_PATH)/FXColorDialog.o\
		$(TRG_OBJ_PATH)/FXColorNames.o\
		$(TRG_OBJ_PATH)/FXColorSelector.o\
		$(TRG_OBJ_PATH)/FXColorWell.o\
		$(TRG_OBJ_PATH)/FXColorWheel.o\
		$(TRG_OBJ_PATH)/FXComboBox.o\
		$(TRG_OBJ_PATH)/FXComposite.o\
		$(TRG_OBJ_PATH)/FXCursor.o\
		$(TRG_OBJ_PATH)/FXDC.o\
		$(TRG_OBJ_PATH)/FXDCPrint.o\
		$(TRG_OBJ_PATH)/FXDCWindow.o\
		$(TRG_OBJ_PATH)/FXDebugTarget.o\
		$(TRG_OBJ_PATH)/FXDelegator.o\
		$(TRG_OBJ_PATH)/FXDial.o\
		$(TRG_OBJ_PATH)/FXDialogBox.o\
		$(TRG_OBJ_PATH)/FXDict.o\
		$(TRG_OBJ_PATH)/FXDirBox.o\
		$(TRG_OBJ_PATH)/FXDirDialog.o\
		$(TRG_OBJ_PATH)/FXDirList.o\
		$(TRG_OBJ_PATH)/FXDirSelector.o\
		$(TRG_OBJ_PATH)/FXDocument.o\
		$(TRG_OBJ_PATH)/FXDragCorner.o\
		$(TRG_OBJ_PATH)/FXDrawable.o\
		$(TRG_OBJ_PATH)/FXDriveBox.o\
		$(TRG_OBJ_PATH)/FXException.o\
		$(TRG_OBJ_PATH)/FXFile.o\
		$(TRG_OBJ_PATH)/FXFileDialog.o\
		$(TRG_OBJ_PATH)/FXFileDict.o\
		$(TRG_OBJ_PATH)/FXFileList.o\
		$(TRG_OBJ_PATH)/FXFileSelector.o\
		$(TRG_OBJ_PATH)/FXFont.o\
		$(TRG_OBJ_PATH)/FXFontDialog.o\
		$(TRG_OBJ_PATH)/FXFontSelector.o\
		$(TRG_OBJ_PATH)/FXFrame.o\
		$(TRG_OBJ_PATH)/FXGIFCursor.o\
		$(TRG_OBJ_PATH)/FXGIFIcon.o\
		$(TRG_OBJ_PATH)/FXGIFImage.o\
		$(TRG_OBJ_PATH)/FXGroupBox.o\
		$(TRG_OBJ_PATH)/FXHeader.o\
		$(TRG_OBJ_PATH)/FXHorizontalFrame.o\
		$(TRG_OBJ_PATH)/FXICOIcon.o\
		$(TRG_OBJ_PATH)/FXICOImage.o\
		$(TRG_OBJ_PATH)/FXIcon.o\
		$(TRG_OBJ_PATH)/FXIconList.o\
		$(TRG_OBJ_PATH)/FXId.o\
		$(TRG_OBJ_PATH)/FXImage.o\
		$(TRG_OBJ_PATH)/FXImageView.o\
		$(TRG_OBJ_PATH)/FXInputDialog.o\
		$(TRG_OBJ_PATH)/FXLabel.o\
		$(TRG_OBJ_PATH)/FXList.o\
		$(TRG_OBJ_PATH)/FXListBox.o\
		$(TRG_OBJ_PATH)/FXMDIButton.o\
		$(TRG_OBJ_PATH)/FXMDIChild.o\
		$(TRG_OBJ_PATH)/FXMDIClient.o\
		$(TRG_OBJ_PATH)/FXMainWindow.o\
		$(TRG_OBJ_PATH)/FXMatrix.o\
		$(TRG_OBJ_PATH)/FXMenuButton.o\
		$(TRG_OBJ_PATH)/FXMenuCaption.o\
		$(TRG_OBJ_PATH)/FXMenuCascade.o\
		$(TRG_OBJ_PATH)/FXMenuCommand.o\
		$(TRG_OBJ_PATH)/FXMenuPane.o\
		$(TRG_OBJ_PATH)/FXMenuSeparator.o\
		$(TRG_OBJ_PATH)/FXMenuTitle.o\
		$(TRG_OBJ_PATH)/FXMenubar.o\
		$(TRG_OBJ_PATH)/FXMessageBox.o\
		$(TRG_OBJ_PATH)/FXObject.o\
		$(TRG_OBJ_PATH)/FXObjectList.o\
		$(TRG_OBJ_PATH)/FXOptionMenu.o\
		$(TRG_OBJ_PATH)/FXPCXIcon.o\
		$(TRG_OBJ_PATH)/FXPCXImage.o\
		$(TRG_OBJ_PATH)/FXPacker.o\
		$(TRG_OBJ_PATH)/FXPicker.o\
		$(TRG_OBJ_PATH)/FXPoint.o\
		$(TRG_OBJ_PATH)/FXPopup.o\
		$(TRG_OBJ_PATH)/FXPrintDialog.o\
		$(TRG_OBJ_PATH)/FXProgressBar.o\
		$(TRG_OBJ_PATH)/FXProgressDialog.o\
		$(TRG_OBJ_PATH)/FXRGBIcon.o\
		$(TRG_OBJ_PATH)/FXRGBImage.o\
		$(TRG_OBJ_PATH)/FXRadioButton.o\
		$(TRG_OBJ_PATH)/FXRecentFiles.o\
		$(TRG_OBJ_PATH)/FXRectangle.o\
		$(TRG_OBJ_PATH)/FXRegion.o\
		$(TRG_OBJ_PATH)/FXRegistry.o\
		$(TRG_OBJ_PATH)/FXReplaceDialog.o\
		$(TRG_OBJ_PATH)/FXRex.o\
		$(TRG_OBJ_PATH)/FXRootWindow.o\
		$(TRG_OBJ_PATH)/FXScrollArea.o\
		$(TRG_OBJ_PATH)/FXScrollWindow.o\
		$(TRG_OBJ_PATH)/FXScrollbar.o\
		$(TRG_OBJ_PATH)/FXSearchDialog.o\
		$(TRG_OBJ_PATH)/FXSeparator.o\
		$(TRG_OBJ_PATH)/FXSettings.o\
		$(TRG_OBJ_PATH)/FXShell.o\
		$(TRG_OBJ_PATH)/FXShutter.o\
		$(TRG_OBJ_PATH)/FXSize.o\
		$(TRG_OBJ_PATH)/FXSlider.o\
		$(TRG_OBJ_PATH)/FXSpinner.o\
		$(TRG_OBJ_PATH)/FXSplitter.o\
		$(TRG_OBJ_PATH)/FXStatusbar.o\
		$(TRG_OBJ_PATH)/FXStatusline.o\
		$(TRG_OBJ_PATH)/FXStream.o\
		$(TRG_OBJ_PATH)/FXString.o\
		$(TRG_OBJ_PATH)/FXStringDict.o\
		$(TRG_OBJ_PATH)/FXSwitcher.o\
		$(TRG_OBJ_PATH)/FXTGAIcon.o\
		$(TRG_OBJ_PATH)/FXTGAImage.o\
		$(TRG_OBJ_PATH)/FXTIFIcon.o\
		$(TRG_OBJ_PATH)/FXTIFImage.o\
		$(TRG_OBJ_PATH)/FXTabBar.o\
		$(TRG_OBJ_PATH)/FXTabBook.o\
		$(TRG_OBJ_PATH)/FXTabItem.o\
		$(TRG_OBJ_PATH)/FXText.o\
		$(TRG_OBJ_PATH)/FXTextField.o\
		$(TRG_OBJ_PATH)/FXToggleButton.o\
		$(TRG_OBJ_PATH)/FXToolbar.o\
		$(TRG_OBJ_PATH)/FXToolbarGrip.o\
		$(TRG_OBJ_PATH)/FXToolbarShell.o\
		$(TRG_OBJ_PATH)/FXToolbarTab.o\
		$(TRG_OBJ_PATH)/FXTooltip.o\
		$(TRG_OBJ_PATH)/FXTopWindow.o\
		$(TRG_OBJ_PATH)/FXTreeList.o\
		$(TRG_OBJ_PATH)/FXTreeListBox.o\
		$(TRG_OBJ_PATH)/FXURL.o\
		$(TRG_OBJ_PATH)/FXUndoList.o\
		$(TRG_OBJ_PATH)/FXVerticalFrame.o\
		$(TRG_OBJ_PATH)/FXVisual.o\
		$(TRG_OBJ_PATH)/FXWindow.o\
		$(TRG_OBJ_PATH)/FXXPMIcon.o\
		$(TRG_OBJ_PATH)/FXXPMImage.o\
		$(TRG_OBJ_PATH)/fxbmpio.o\
		$(TRG_OBJ_PATH)/fxcurio.o\
		$(TRG_OBJ_PATH)/fxfilematch.o\
		$(TRG_OBJ_PATH)/fxgifio.o\
		$(TRG_OBJ_PATH)/fxicoio.o\
		$(TRG_OBJ_PATH)/fxpcxio.o\
		$(TRG_OBJ_PATH)/fxpriv.o\
		$(TRG_OBJ_PATH)/fxquantize.o\
		$(TRG_OBJ_PATH)/fxrgbio.o\
		$(TRG_OBJ_PATH)/fxtargaio.o\
		$(TRG_OBJ_PATH)/fxtifio.o\
		$(TRG_OBJ_PATH)/fxutils.o\
		$(TRG_OBJ_PATH)/fxwinkbd.o\
		$(TRG_OBJ_PATH)/fxxpmio.o\
		$(TRG_OBJ_PATH)/FXDate.o\
		$(TRG_OBJ_PATH)/FXCalendar.o\
		$(TRG_OBJ_PATH)/FXCalendarView.o
$(TRG) :	$(TRG_SRC)
		rm -f $@
		$(AR) -cr $(AROPTIONS) $@ $^
		ranlib $@
$(TRG_OBJ_PATH)/%.o : libFox.a-dir/Sources/%.cpp
	$(CXX) -c -o $@  -IlibFox.a-dir/Includes -IlibFox.a-dir/Sources $(CXXFLAGS) $<

TRG		= libFoxI.a
TRG_PATH	= libFox.a-dir
TRG_OBJ_PATH	= $(TRG_PATH)/ObjectsI
TRG_SRC		= $(TRG_OBJ_PATH)/FX4Splitter.o\
		$(TRG_OBJ_PATH)/FXApp.o\
		$(TRG_OBJ_PATH)/FXAccelTable.o\
		$(TRG_OBJ_PATH)/FXArrowButton.o\
		$(TRG_OBJ_PATH)/FXBMPIcon.o\
		$(TRG_OBJ_PATH)/FXBMPImage.o\
		$(TRG_OBJ_PATH)/FXBitmap.o\
		$(TRG_OBJ_PATH)/FXButton.o\
		$(TRG_OBJ_PATH)/FXCURCursor.o\
		$(TRG_OBJ_PATH)/FXCanvas.o\
		$(TRG_OBJ_PATH)/FXCharset.o\
		$(TRG_OBJ_PATH)/FXCheckButton.o\
		$(TRG_OBJ_PATH)/FXColorBar.o\
		$(TRG_OBJ_PATH)/FXColorDialog.o\
		$(TRG_OBJ_PATH)/FXColorNames.o\
		$(TRG_OBJ_PATH)/FXColorSelector.o\
		$(TRG_OBJ_PATH)/FXColorWell.o\
		$(TRG_OBJ_PATH)/FXColorWheel.o\
		$(TRG_OBJ_PATH)/FXComboBox.o\
		$(TRG_OBJ_PATH)/FXComposite.o\
		$(TRG_OBJ_PATH)/FXCursor.o\
		$(TRG_OBJ_PATH)/FXDC.o\
		$(TRG_OBJ_PATH)/FXDCPrint.o\
		$(TRG_OBJ_PATH)/FXDCWindow.o\
		$(TRG_OBJ_PATH)/FXDebugTarget.o\
		$(TRG_OBJ_PATH)/FXDelegator.o\
		$(TRG_OBJ_PATH)/FXDial.o\
		$(TRG_OBJ_PATH)/FXDialogBox.o\
		$(TRG_OBJ_PATH)/FXDict.o\
		$(TRG_OBJ_PATH)/FXDirBox.o\
		$(TRG_OBJ_PATH)/FXDirDialog.o\
		$(TRG_OBJ_PATH)/FXDirList.o\
		$(TRG_OBJ_PATH)/FXDirSelector.o\
		$(TRG_OBJ_PATH)/FXDocument.o\
		$(TRG_OBJ_PATH)/FXDragCorner.o\
		$(TRG_OBJ_PATH)/FXDrawable.o\
		$(TRG_OBJ_PATH)/FXDriveBox.o\
		$(TRG_OBJ_PATH)/FXException.o\
		$(TRG_OBJ_PATH)/FXFile.o\
		$(TRG_OBJ_PATH)/FXFileDialog.o\
		$(TRG_OBJ_PATH)/FXFileDict.o\
		$(TRG_OBJ_PATH)/FXFileList.o\
		$(TRG_OBJ_PATH)/FXFileSelector.o\
		$(TRG_OBJ_PATH)/FXFont.o\
		$(TRG_OBJ_PATH)/FXFontDialog.o\
		$(TRG_OBJ_PATH)/FXFontSelector.o\
		$(TRG_OBJ_PATH)/FXFrame.o\
		$(TRG_OBJ_PATH)/FXGIFCursor.o\
		$(TRG_OBJ_PATH)/FXGIFIcon.o\
		$(TRG_OBJ_PATH)/FXGIFImage.o\
		$(TRG_OBJ_PATH)/FXGroupBox.o\
		$(TRG_OBJ_PATH)/FXHeader.o\
		$(TRG_OBJ_PATH)/FXHorizontalFrame.o\
		$(TRG_OBJ_PATH)/FXICOIcon.o\
		$(TRG_OBJ_PATH)/FXICOImage.o\
		$(TRG_OBJ_PATH)/FXIcon.o\
		$(TRG_OBJ_PATH)/FXIconList.o\
		$(TRG_OBJ_PATH)/FXId.o\
		$(TRG_OBJ_PATH)/FXImage.o\
		$(TRG_OBJ_PATH)/FXImageView.o\
		$(TRG_OBJ_PATH)/FXInputDialog.o\
		$(TRG_OBJ_PATH)/FXJPGIcon.o\
		$(TRG_OBJ_PATH)/FXJPGImage.o\
		$(TRG_OBJ_PATH)/FXLabel.o\
		$(TRG_OBJ_PATH)/FXList.o\
		$(TRG_OBJ_PATH)/FXListBox.o\
		$(TRG_OBJ_PATH)/FXMDIButton.o\
		$(TRG_OBJ_PATH)/FXMDIChild.o\
		$(TRG_OBJ_PATH)/FXMDIClient.o\
		$(TRG_OBJ_PATH)/FXMainWindow.o\
		$(TRG_OBJ_PATH)/FXMatrix.o\
		$(TRG_OBJ_PATH)/FXMenuButton.o\
		$(TRG_OBJ_PATH)/FXMenuCaption.o\
		$(TRG_OBJ_PATH)/FXMenuCascade.o\
		$(TRG_OBJ_PATH)/FXMenuCommand.o\
		$(TRG_OBJ_PATH)/FXMenuPane.o\
		$(TRG_OBJ_PATH)/FXMenuSeparator.o\
		$(TRG_OBJ_PATH)/FXMenuTitle.o\
		$(TRG_OBJ_PATH)/FXMenubar.o\
		$(TRG_OBJ_PATH)/FXMessageBox.o\
		$(TRG_OBJ_PATH)/FXObject.o\
		$(TRG_OBJ_PATH)/FXObjectList.o\
		$(TRG_OBJ_PATH)/FXOptionMenu.o\
		$(TRG_OBJ_PATH)/FXPCXIcon.o\
		$(TRG_OBJ_PATH)/FXPCXImage.o\
		$(TRG_OBJ_PATH)/FXPNGIcon.o\
		$(TRG_OBJ_PATH)/FXPNGImage.o\
		$(TRG_OBJ_PATH)/FXPacker.o\
		$(TRG_OBJ_PATH)/FXPicker.o\
		$(TRG_OBJ_PATH)/FXPoint.o\
		$(TRG_OBJ_PATH)/FXPopup.o\
		$(TRG_OBJ_PATH)/FXPrintDialog.o\
		$(TRG_OBJ_PATH)/FXProgressBar.o\
		$(TRG_OBJ_PATH)/FXProgressDialog.o\
		$(TRG_OBJ_PATH)/FXRGBIcon.o\
		$(TRG_OBJ_PATH)/FXRGBImage.o\
		$(TRG_OBJ_PATH)/FXRadioButton.o\
		$(TRG_OBJ_PATH)/FXRecentFiles.o\
		$(TRG_OBJ_PATH)/FXRectangle.o\
		$(TRG_OBJ_PATH)/FXRegion.o\
		$(TRG_OBJ_PATH)/FXRegistry.o\
		$(TRG_OBJ_PATH)/FXReplaceDialog.o\
		$(TRG_OBJ_PATH)/FXRex.o\
		$(TRG_OBJ_PATH)/FXRootWindow.o\
		$(TRG_OBJ_PATH)/FXScrollArea.o\
		$(TRG_OBJ_PATH)/FXScrollWindow.o\
		$(TRG_OBJ_PATH)/FXScrollbar.o\
		$(TRG_OBJ_PATH)/FXSearchDialog.o\
		$(TRG_OBJ_PATH)/FXSeparator.o\
		$(TRG_OBJ_PATH)/FXSettings.o\
		$(TRG_OBJ_PATH)/FXShell.o\
		$(TRG_OBJ_PATH)/FXShutter.o\
		$(TRG_OBJ_PATH)/FXSize.o\
		$(TRG_OBJ_PATH)/FXSlider.o\
		$(TRG_OBJ_PATH)/FXSpinner.o\
		$(TRG_OBJ_PATH)/FXSplitter.o\
		$(TRG_OBJ_PATH)/FXStatusbar.o\
		$(TRG_OBJ_PATH)/FXStatusline.o\
		$(TRG_OBJ_PATH)/FXStream.o\
		$(TRG_OBJ_PATH)/FXString.o\
		$(TRG_OBJ_PATH)/FXStringDict.o\
		$(TRG_OBJ_PATH)/FXSwitcher.o\
		$(TRG_OBJ_PATH)/FXTGAIcon.o\
		$(TRG_OBJ_PATH)/FXTGAImage.o\
		$(TRG_OBJ_PATH)/FXTIFIcon.o\
		$(TRG_OBJ_PATH)/FXTIFImage.o\
		$(TRG_OBJ_PATH)/FXTabBar.o\
		$(TRG_OBJ_PATH)/FXTabBook.o\
		$(TRG_OBJ_PATH)/FXTabItem.o\
		$(TRG_OBJ_PATH)/FXText.o\
		$(TRG_OBJ_PATH)/FXTextField.o\
		$(TRG_OBJ_PATH)/FXToggleButton.o\
		$(TRG_OBJ_PATH)/FXToolbar.o\
		$(TRG_OBJ_PATH)/FXToolbarGrip.o\
		$(TRG_OBJ_PATH)/FXToolbarShell.o\
		$(TRG_OBJ_PATH)/FXToolbarTab.o\
		$(TRG_OBJ_PATH)/FXTooltip.o\
		$(TRG_OBJ_PATH)/FXTopWindow.o\
		$(TRG_OBJ_PATH)/FXTreeList.o\
		$(TRG_OBJ_PATH)/FXTreeListBox.o\
		$(TRG_OBJ_PATH)/FXURL.o\
		$(TRG_OBJ_PATH)/FXUndoList.o\
		$(TRG_OBJ_PATH)/FXVerticalFrame.o\
		$(TRG_OBJ_PATH)/FXVisual.o\
		$(TRG_OBJ_PATH)/FXWindow.o\
		$(TRG_OBJ_PATH)/FXXPMIcon.o\
		$(TRG_OBJ_PATH)/FXXPMImage.o\
		$(TRG_OBJ_PATH)/fxbmpio.o\
		$(TRG_OBJ_PATH)/fxcurio.o\
		$(TRG_OBJ_PATH)/fxfilematch.o\
		$(TRG_OBJ_PATH)/fxgifio.o\
		$(TRG_OBJ_PATH)/fxicoio.o\
		$(TRG_OBJ_PATH)/fxjpegio.o\
		$(TRG_OBJ_PATH)/fxpcxio.o\
		$(TRG_OBJ_PATH)/fxpngio.o\
		$(TRG_OBJ_PATH)/fxpriv.o\
		$(TRG_OBJ_PATH)/fxquantize.o\
		$(TRG_OBJ_PATH)/fxrgbio.o\
		$(TRG_OBJ_PATH)/fxtargaio.o\
		$(TRG_OBJ_PATH)/fxtifio.o\
		$(TRG_OBJ_PATH)/fxutils.o\
		$(TRG_OBJ_PATH)/fxwinkbd.o\
		$(TRG_OBJ_PATH)/fxxpmio.o\
		$(TRG_OBJ_PATH)/FXDate.o\
		$(TRG_OBJ_PATH)/FXCalendar.o\
		$(TRG_OBJ_PATH)/FXCalendarView.o
$(TRG) :	$(TRG_SRC)
		rm -f $@
		$(AR) -cr $(AROPTIONS) $@ $^
		ranlib $@
$(TRG_OBJ_PATH)/%.o : libFox.a-dir/Sources/%.cpp
	$(CXX) -c -o $@  -IlibFox.a-dir/Includes -IlibFox.a-dir/Sources $(CXXFLAGS) $<

TRG		= libSrvTools.a
TRG_PATH	= $(TRG)-dir
$(TRG) :	$(TRG_PATH)/Objects/SrvTools.o\
		$(TRG_PATH)/Objects/SrvToolsApp.o\
		$(TRG_PATH)/Objects/SrvToolsAlg.o\
		$(TRG_PATH)/Objects/SrvToolsAlgFrame.o\
		$(TRG_PATH)/Objects/SrvToolsCalc.o\
		$(TRG_PATH)/Objects/SrvToolsCore.o\
		$(TRG_PATH)/Objects/SrvToolsDiag.o\
		$(TRG_PATH)/Objects/SrvToolsDialog.o\
		$(TRG_PATH)/Objects/SrvToolsFileTbl.o\
		$(TRG_PATH)/Objects/SrvToolsFrame.o\
		$(TRG_PATH)/Objects/SrvToolsIdb.o\
		$(TRG_PATH)/Objects/SrvToolsImage.o\
		$(TRG_PATH)/Objects/SrvToolsProtocol.o\
		$(TRG_PATH)/Objects/SrvToolsSignal.o\
		$(TRG_PATH)/Objects/SrvToolsDrawTbl.o\
		$(TRG_PATH)/Objects/SrvToolsIconTbl.o\
		$(TRG_PATH)/Objects/SrvToolsTblWnd.o\
		$(TRG_PATH)/Objects/SrvToolsSettings.o\
		$(TRG_PATH)/Objects/SrvToolsWnd.o\
		$(TRG_PATH)/Objects/SrvToolsGrants.o
		rm -f $@
		$(AR) -cr $(AROPTIONS) $@ $^
		ranlib $@
$(TRG_PATH)/Objects/%.o : $(TRG_PATH)/Sources/%.cpp
	$(CXX) -c -o $@  -IlibSrvTools.a-dir/Includes -IlibSrvTools.a-dir/Sources $(CXXFLAGS) $<

TRG		= libSrvTemplate.a
TRG_PATH	= $(TRG)-dir
$(TRG) :	$(TRG_PATH)/Objects/SrvTemplate.o
		rm -f $@
		$(AR) -cr $(AROPTIONS) $@ $^
		ranlib $@
$(TRG_PATH)/Objects/%.o : $(TRG_PATH)/Sources/%.cpp
	$(CXX) -c -o $@  -IlibSrvTemplate.a-dir/Includes -IlibSrvTemplate.a-dir/Sources $(CXXFLAGS) $<

TRG		= libDiagnTools.a
TRG_PATH	= $(TRG)-dir
$(TRG) :	$(TRG_PATH)/Objects/DiagnToolsBknt.o\
		$(TRG_PATH)/Objects/DiagnToolsNetDev.o\
		$(TRG_PATH)/Objects/DiagnToolsOs.o\
		$(TRG_PATH)/Objects/DiagnToolsProcess.o\
		$(TRG_PATH)/Objects/DiagnToolsRaid.o\
		$(TRG_PATH)/Objects/DiagnToolsResource.o\
		$(TRG_PATH)/Objects/DiagnToolsSensors.o\
		$(TRG_PATH)/Objects/DiagnToolsUps.o
		rm -f $@
		$(AR) -cr $(AROPTIONS) $@ $^
		ranlib $@
$(TRG_PATH)/Objects/%.o : $(TRG_PATH)/Sources/%.cpp
	$(CXX) -c -o $@  -IlibDiagnTools.a-dir/Includes -IlibDiagnTools.a-dir/Sources $(CXXFLAGS) $<

TRG		= libDiagn.a
TRG_PATH	= $(TRG)-dir
$(TRG) : 	$(TRG_PATH)/Objects/DiagnGate.o
		rm -f $@
		$(AR) -cr $(AROPTIONS) $@ $^
		ranlib $@
$(TRG_PATH)/Objects/%.o : $(TRG_PATH)/Sources/%.cpp
	$(CXX) -c -o $@  -IlibDiagn.a-dir/Includes -IlibDiagn.a-dir/Sources $(CXXFLAGS) $<

TRG		= libBrowser.a
TRG_PATH	= $(TRG)-dir
$(TRG) :	$(TRG_PATH)/Objects/libBrowserDiag.o\
		$(TRG_PATH)/Objects/libBrowser_LIST.o\
		$(TRG_PATH)/Objects/libBrowser_REP.o\
		$(TRG_PATH)/Objects/libBrowserTree.o\
		$(TRG_PATH)/Objects/libBrowserApp.o\
		$(TRG_PATH)/Objects/libBrowserDlg.o\
		$(TRG_PATH)/Objects/libBrowserMain.o\
		$(TRG_PATH)/Objects/libBrowser_RSA.o\
		$(TRG_PATH)/Objects/libBrowser_VID.o\
		$(TRG_PATH)/Objects/libBrowserArcInfo.o\
		$(TRG_PATH)/Objects/libBrowser_GRA.o\
		$(TRG_PATH)/Objects/libBrowserMakeVid.o\
		$(TRG_PATH)/Objects/libBrowser_RSM.o\
		$(TRG_PATH)/Objects/libBrowserVObj.o\
		$(TRG_PATH)/Objects/libBrowserBar.o\
		$(TRG_PATH)/Objects/libBrowserIdb.o\
		$(TRG_PATH)/Objects/libBrowserObj.o\
		$(TRG_PATH)/Objects/libBrowser_RTS.o\
		$(TRG_PATH)/Objects/libBrowserWnd.o\
		$(TRG_PATH)/Objects/libBrowser.o\
		$(TRG_PATH)/Objects/libBrowserImage.o\
		$(TRG_PATH)/Objects/libBrowserPropDlg.o\
		$(TRG_PATH)/Objects/libBrowserScan.o\
		$(TRG_PATH)/Objects/libBrowser_CTRL.o\
		$(TRG_PATH)/Objects/libBrowser_INFO.o\
		$(TRG_PATH)/Objects/libBrowserRegister.o\
		$(TRG_PATH)/Objects/libBrowser_SCH.o\
		$(TRG_PATH)/Objects/libBrowserTempl.o\
		$(TRG_PATH)/Objects/libBrowserXML.o\
		$(TRG_PATH)/Objects/libBrowserXObj.o\
		$(TRG_PATH)/Objects/libBrowser_RSI.o
		rm -f $@
		$(AR) -cr $(AROPTIONS) $@ $^
		ranlib $@
$(TRG_PATH)/Objects/%.o : $(TRG_PATH)/Sources/%.cpp
	$(CXX) -c -o $@  -IlibBrowser.a-dir/Includes -IlibBrowser.a-dir/Sources $(CXXFLAGS) $<

TRG		= libSvrk.a
TRG_PATH	= $(TRG)-dir
$(TRG) :	$(TRG_PATH)/Objects/libSvrkBrowser.o\
		$(TRG_PATH)/Objects/libSvrkImage.o\
		$(TRG_PATH)/Objects/libSvrkMain.o\
		$(TRG_PATH)/Objects/libSvrk_RSP.o\
		$(TRG_PATH)/Objects/libSvrkAlg.o\
		$(TRG_PATH)/Objects/libSvrkIdb.o\
		$(TRG_PATH)/Objects/libSvrk_INFO.o\
		$(TRG_PATH)/Objects/libSvrk_RMK.o\
		$(TRG_PATH)/Objects/libSvrk_FUEL.o\
		$(TRG_PATH)/Objects/libSvrk_GRA.o\
		$(TRG_PATH)/Objects/libSvrk_RSA.o\
		$(TRG_PATH)/Objects/libSvrk_RSM.o\
		$(TRG_PATH)/Objects/libSvrk_RTS.o\
		$(TRG_PATH)/Objects/libSvrk_OFSD.o
		rm -f $@
		$(AR) -cr $(AROPTIONS) $@ $^
		ranlib $@
$(TRG_PATH)/Objects/%.o : $(TRG_PATH)/Sources/%.cpp
	$(CXX) -c -o $@  -IlibSvrk.a-dir/Includes -IlibSvrk.a-dir/Sources $(CXXFLAGS) $<

TRG		= libSync.a
TRG_PATH	= $(TRG)-dir
$(TRG) :	$(TRG_PATH)/Objects/libSyncApp.o\
		$(TRG_PATH)/Objects/libSync.o\
		$(TRG_PATH)/Objects/libSyncFrame.o\
		$(TRG_PATH)/Objects/libSyncImage.o\
		$(TRG_PATH)/Objects/libSyncTbl.o
		rm -f $@
		$(AR) -cr $(AROPTIONS) $@ $^
		ranlib $@
$(TRG_PATH)/Objects/%.o : $(TRG_PATH)/Sources/%.cpp
	$(CXX) -c -o $@  -IlibSync.a-dir/Includes -IlibSync.a-dir/Sources $(CXXFLAGS) $<

TRG		= libVoyage.a
TRG_PATH	= $(TRG)-dir
$(TRG) :	$(TRG_PATH)/Objects/libVoyage.o
		rm -f $@
		$(AR) -cr $(AROPTIONS) $@ $^
		ranlib $@
$(TRG_PATH)/Objects/%.o : $(TRG_PATH)/Sources/%.cpp
	$(CXX) -c -o $@  -IlibVoyage.a-dir/Includes -IlibVoyage.a-dir/Sources $(CXXFLAGS) $<

TRG		= SendMsg
TRG_PATH	= $(TRG)-dir
$(TRG) :	$(TRG_PATH)/Objects/SendMsg.o\
		libCrTools.a
		$(CXX) -o $@ $^ $(LDOPTIONS)
$(TRG_PATH)/Objects/%.o : $(TRG_PATH)/Sources/%.cpp
	$(CXX) -c -o $@  -ISendMsg-dir/Includes -ISendMsg-dir/Sources $(CXXFLAGS) $<

TRG		= RecvUDP
TRG_PATH	= $(TRG)-dir
$(TRG) :	$(TRG_PATH)/Objects/RecvUDP.o\
		libCrTools.a
		$(CXX) -o $@ $^ $(LDOPTIONS)
$(TRG_PATH)/Objects/%.o : $(TRG_PATH)/Sources/%.cpp
	$(CXX) -c -o $@  -IRecvUDP-dir/Includes -IRecvUDP-dir/Sources $(CXXFLAGS) $<

TRG		= SendUDP
TRG_PATH	= $(TRG)-dir
$(TRG) :	$(TRG_PATH)/Objects/SendUDP.o\
		libCrTools.a
		$(CXX) -o $@ $^ $(LDOPTIONS)
$(TRG_PATH)/Objects/%.o : $(TRG_PATH)/Sources/%.cpp
	$(CXX) -c -o $@  -ISendUDP-dir/Includes -ISendUDP-dir/Sources $(CXXFLAGS) $<

TRG		= SendUDPLsb
TRG_PATH	= $(TRG)-dir
$(TRG) :	$(TRG_PATH)/Objects/SendUDPLsb.o\
		libCrTools.a
		$(CXX) -o $@ $^ $(LDOPTIONS)
$(TRG_PATH)/Objects/%.o : $(TRG_PATH)/Sources/%.cpp
	$(CXX) -c -o $@  -I$(TRG_PATH)/Includes -I$(TRG_PATH)/Sources $(CXXFLAGS) $<

TRG		= SrvTemplate
TRG_PATH	= $(TRG)-dir
$(TRG) :	$(TRG_PATH)/Objects/Template.o\
		libCrTools.a\
		libSrvTemplate.a\
		libSrvAgentAPI.a
		$(CXX) -o $@ $^ $(LDOPTIONS)
$(TRG_PATH)/Objects/%.o : $(TRG_PATH)/Sources/%.cpp
	$(CXX) -c -o $@  -ISrvTemplate-dir/Includes -ISrvTemplate-dir/Sources $(CXXFLAGS) $<

TRG		= MakeImportFile
TRG_PATH	= $(TRG)-dir
$(TRG) : 	$(TRG_PATH)/Objects/MakeImportFile.o\
		libCrTools.a\
		libSrvAgentAPI.a\
		libSrvTools.a
		$(CXX) -o $@ $^ $(LDOPTIONS)
$(TRG_PATH)/Objects/%.o : $(TRG_PATH)/Sources/%.cpp
	$(CXX) -c -o $@  -IMakeImportFile-dir/Includes -IMakeImportFile-dir/Sources $(CXXFLAGS) $<

TRG		= ImGKso
TRG_PATH	= $(TRG)-dir
$(TRG) : 	$(TRG_PATH)/Objects/ImGKso.o\
		$(TRG_PATH)/Objects/ImGKsoApp.o\
		$(TRG_PATH)/Objects/ImGKsoDiag.o\
		$(TRG_PATH)/Objects/ImGKsoFilManager.o\
		$(TRG_PATH)/Objects/ImGKsoFrame.o\
		$(TRG_PATH)/Objects/ImGKsoIdb.o\
		$(TRG_PATH)/Objects/ImGKsoMain.o\
		$(TRG_PATH)/Objects/ImGKsoRegister.o\
		$(TRG_PATH)/Objects/ImGKsoScan.o\
		$(TRG_PATH)/Objects/ImGKsoSignals.o\
		$(TRG_PATH)/Objects/ImGKsoWnd.o\
		libCrTools.a\
		libSrvAgentAPI.a\
		libSrvTools.a\
		libFox.a
		$(CXX) -o $@ $^ $(LDOPTIONS)
$(TRG_PATH)/Objects/%.o : $(TRG_PATH)/Sources/%.cpp
	$(CXX) -c -o $@  -IImGKso-dir/Includes -IImGKso-dir/Sources $(CXXFLAGS) $<

TRG		= UpdateSet
TRG_PATH	= $(TRG)-dir
$(TRG) :	$(TRG_PATH)/Objects/UpdateSet.o\
		$(TRG_PATH)/Objects/UpdateSetBbox.o\
		$(TRG_PATH)/Objects/UpdateSetXml.o\
		libSrvTools.a\
		libCrTools.a\
		libSrvAgentAPI.a\
		libFox.a
		$(CXX) -o $@ $^ $(LDOPTIONS)
$(TRG_PATH)/Objects/%.o : $(TRG_PATH)/Sources/%.cpp
	$(CXX) -c -o $@  -IUpdateSet-dir/Includes -IUpdateSet-dir/Sources $(CXXFLAGS) $<

TRG		= ToDat
TRG_PATH	= $(TRG)-dir
$(TRG) :	$(TRG_PATH)/Objects/ToDat.o\
		libSrvTools.a\
		libCrTools.a\
		libSrvAgentAPI.a\
		libFox.a
		$(CXX) -o $@ $^ $(LDOPTIONS)
$(TRG_PATH)/Objects/%.o : $(TRG_PATH)/Sources/%.cpp
	$(CXX) -c -o $@  -IToDat-dir/Includes -IToDat-dir/Sources $(CXXFLAGS) $<

TRG		= Expo
TRG_PATH	= $(TRG)-dir
$(TRG) :	$(TRG_PATH)/Objects/Expo.o\
		libSrvTools.a\
		libCrTools.a\
		libSrvAgentAPI.a\
		libFox.a
		$(CXX) -o $@ $^ $(LDOPTIONS)
$(TRG_PATH)/Objects/%.o : $(TRG_PATH)/Sources/%.cpp
	$(CXX) -c -o $@  -IExpo-dir/Includes -IExpo-dir/Sources $(CXXFLAGS) $<

TRG		= SrvAgent
TRG_PATH	= $(TRG)-dir
$(TRG) :	$(TRG_PATH)/Objects/SrvAgent.o\
		$(TRG_PATH)/Objects/SrvAgentApp.o\
		$(TRG_PATH)/Objects/SrvAgentDiag.o\
		$(TRG_PATH)/Objects/SrvAgentDialog.o\
		$(TRG_PATH)/Objects/SrvAgentMain.o\
		$(TRG_PATH)/Objects/SrvAgentServer.o\
		$(TRG_PATH)/Objects/SrvAgentWnd.o\
		libCrTools.a\
		libDiagn.a\
		libSrvAgentAPI.a\
		libSrvTools.a\
		libFox.a
		$(CXX) -o $@ $^ $(LDOPTIONS)
$(TRG_PATH)/Objects/%.o : $(TRG_PATH)/Sources/%.cpp
	$(CXX) -c -o $@  -ISrvAgent-dir/Includes -ISrvAgent-dir/Sources $(CXXFLAGS) $<

TRG		= SrvCore
TRG_PATH	= $(TRG)-dir
$(TRG) :	$(TRG_PATH)/Objects/SrvCore.o\
		$(TRG_PATH)/Objects/SrvCoreApp.o\
		$(TRG_PATH)/Objects/SrvCoreDiag.o\
		$(TRG_PATH)/Objects/SrvCoreFileMan.o\
		$(TRG_PATH)/Objects/SrvCoreDialog.o\
		$(TRG_PATH)/Objects/SrvCoreFlush.o\
		$(TRG_PATH)/Objects/SrvCoreFrame.o\
		$(TRG_PATH)/Objects/SrvCoreIdb.o\
		$(TRG_PATH)/Objects/SrvCoreNet.o\
		$(TRG_PATH)/Objects/SrvCoreMain.o\
		$(TRG_PATH)/Objects/SrvCoreReader.o\
		$(TRG_PATH)/Objects/SrvCoreRegister.o\
		$(TRG_PATH)/Objects/SrvCoreSignal.o\
		$(TRG_PATH)/Objects/SrvCoreSync.o\
		$(TRG_PATH)/Objects/SrvCoreStat.o\
		$(TRG_PATH)/Objects/SrvCoreWnd.o\
		libCrTools.a\
		libDiagn.a\
		libSrvTools.a\
		libSrvAgentAPI.a\
		libFox.a
		$(CXX) -o $@ $^ $(LDOPTIONS)
$(TRG_PATH)/Objects/%.o : $(TRG_PATH)/Sources/%.cpp
	$(CXX) -c -o $@  -ISrvCore-dir/Includes -ISrvCore-dir/Sources $(CXXFLAGS) $<

TRG		= SrvBrowser
TRG_PATH	= $(TRG)-dir
$(TRG) :	$(TRG_PATH)/Objects/SrvBrowser.o\
		$(TRG_PATH)/Objects/SrvBrowserMakeVid.o\
		$(TRG_PATH)/Objects/SrvBrowserMain.o\
		$(TRG_PATH)/Objects/SrvBrowserForecast.o\
		libBrowser.a\
		libCrTools.a\
		libSvrk.a\
		libSrvTools.a\
		libSrvAgentAPI.a\
		libDiagn.a\
		libSrvTemplate.a\
		libFoxI.a\
		$(IMGLIBS)
		$(CXX) -o $@ $^ $(LDOPTIONS) -lcurl
$(TRG_PATH)/Objects/%.o : $(TRG_PATH)/Sources/%.cpp
	$(CXX) -c -o $@  -ISrvBrowser-dir/Includes -ISrvBrowser-dir/Sources $(CXXFLAGS) $<

TRG		= ViBrowser
TRG_PATH	= $(TRG)-dir
$(TRG) :	$(TRG_PATH)/Objects/ViBrowser.o\
		$(TRG_PATH)/Objects/ViBrowserMain.o\
		$(TRG_PATH)/Objects/ViBrowserDIA.o\
		$(TRG_PATH)/Objects/ViBrowserSPE.o\
		$(TRG_PATH)/Objects/ViBrowserMakeVid.o\
		$(TRG_PATH)/Objects/ViBrowserVObj.o\
		$(TRG_PATH)/Objects/ViBrowserSOSP.o\
		$(TRG_PATH)/Objects/ViBrowserSVSHD.o\
		$(TRG_PATH)/Objects/ViBrowserSVKD.o\
		$(TRG_PATH)/Objects/ViBrowserORB.o\
		$(TRG_PATH)/Objects/ViBrowserCRC.o\
		$(TRG_PATH)/Objects/ViBrowserSPR.o\
		libBrowser.a\
		libCrTools.a\
		libSvrk.a\
		libSrvTools.a\
		libSrvAgentAPI.a\
		libDiagn.a\
		libSrvTemplate.a\
		libFoxI.a\
		$(IMGLIBS)
		$(CXX) -o $@ $^ $(LDOPTIONS)
$(TRG_PATH)/Objects/%.o : $(TRG_PATH)/Sources/%.cpp
	$(CXX) -c -o $@  -IViBrowser-dir/Includes -IViBrowser-dir/Sources $(CXXFLAGS) $<

TRG		= GateFkd
TRG_PATH	= $(TRG)-dir
$(TRG) :	$(TRG_PATH)/Objects/GateFkd.o\
		$(TRG_PATH)/Objects/GateFkdApp.o\
		$(TRG_PATH)/Objects/GateFkdDiag.o\
		$(TRG_PATH)/Objects/GateFkdIdb.o\
		$(TRG_PATH)/Objects/GateFkdMain.o\
		$(TRG_PATH)/Objects/GateFkdProcess.o\
		$(TRG_PATH)/Objects/GateFkdScan.o\
		$(TRG_PATH)/Objects/GateFkdSignals.o\
		$(TRG_PATH)/Objects/GateFkdWnd.o\
		libCrTools.a\
		libDiagn.a\
		libSrvAgentAPI.a\
		libSrvTools.a\
		libFox.a
		$(CXX) -o $@ $^ $(LDOPTIONS)
$(TRG_PATH)/Objects/%.o : $(TRG_PATH)/Sources/%.cpp
	$(CXX) -c -o $@  -IGateFkd-dir/Includes -IGateFkd-dir/Sources $(CXXFLAGS) $<

TRG		= GateIvs
TRG_PATH	= $(TRG)-dir
$(TRG) :	$(TRG_PATH)/Objects/GateIvs.o\
		$(TRG_PATH)/Objects/GateIvsDiag.o\
		$(TRG_PATH)/Objects/GateIvsIdb.o\
		$(TRG_PATH)/Objects/GateIvsExchange.o\
		$(TRG_PATH)/Objects/GateIvsMain.o\
		$(TRG_PATH)/Objects/GateIvsScan.o\
		$(TRG_PATH)/Objects/GateIvsSend.o\
		$(TRG_PATH)/Objects/GateIvsRecv.o\
		$(TRG_PATH)/Objects/GateIvsRecvEth.o\
		$(TRG_PATH)/Objects/GateIvsRecvPrepare.o\
		$(TRG_PATH)/Objects/GateIvsRegister.o\
		$(TRG_PATH)/Objects/GateIvsSignals.o\
		$(TRG_PATH)/Objects/GateIvsWnd.o\
		libCrTools.a\
		libSrvAgentAPI.a\
		libSrvTools.a\
		libDiagn.a\
		libFox.a\
		libpcap/libpcap.a
		$(CXX) -o $@ $^ $(LDOPTIONS)
$(TRG_PATH)/Objects/%.o : $(TRG_PATH)/Sources/%.cpp
	$(CXX) -c -o $@  -IGateIvs-dir/Includes -IGateIvs-dir/Sources $(CXXFLAGS) $<

TRG		= GateLsb
TRG_PATH	= $(TRG)-dir
$(TRG) :	$(TRG_PATH)/Objects/GateLsb.o\
		$(TRG_PATH)/Objects/GateLsbDiag.o\
		$(TRG_PATH)/Objects/GateLsbExchange.o\
		$(TRG_PATH)/Objects/GateLsbIdb.o\
		$(TRG_PATH)/Objects/GateLsbMain.o\
		$(TRG_PATH)/Objects/GateLsbRecv.o\
		$(TRG_PATH)/Objects/GateLsbRecvPrepare.o\
		$(TRG_PATH)/Objects/GateLsbRegister.o\
		$(TRG_PATH)/Objects/GateLsbSignals.o\
		$(TRG_PATH)/Objects/GateLsbWnd.o\
		libCrTools.a\
		libSrvAgentAPI.a\
		libSrvTools.a\
		libDiagn.a\
		libFox.a
		$(CXX) -o $@ $^ $(LDOPTIONS)
$(TRG_PATH)/Objects/%.o : $(TRG_PATH)/Sources/%.cpp
	$(CXX) -c -o $@  -IGateLsb-dir/Includes -IGateLsb-dir/Sources $(CXXFLAGS) $<

TRG		= GateBod
TRG_PATH	= $(TRG)-dir
$(TRG) :	$(TRG_PATH)/Objects/GateBod.o\
		$(TRG_PATH)/Objects/GateBodApp.o\
		$(TRG_PATH)/Objects/GateBodDiag.o\
		$(TRG_PATH)/Objects/GateBodIdb.o\
		$(TRG_PATH)/Objects/GateBodMain.o\
		$(TRG_PATH)/Objects/GateBodRegister.o\
		$(TRG_PATH)/Objects/GateBodScan.o\
		$(TRG_PATH)/Objects/GateBodSignals.o\
		$(TRG_PATH)/Objects/GateBodWnd.o\
		libCrTools.a\
		libSrvAgentAPI.a\
		libSrvTools.a\
		libDiagn.a\
		libFox.a
		$(CXX) -o $@ $^ $(LDOPTIONS)
$(TRG_PATH)/Objects/%.o : $(TRG_PATH)/Sources/%.cpp
	$(CXX) -c -o $@  -IGateBod-dir/Includes -IGateBod-dir/Sources $(CXXFLAGS) $<

TRG		= GateTempl
TRG_PATH	= $(TRG)-dir
$(TRG) :	$(TRG_PATH)/Objects/GateTempl.o\
		$(TRG_PATH)/Objects/GateTemplDiag.o\
		$(TRG_PATH)/Objects/GateTemplIdb.o\
		$(TRG_PATH)/Objects/GateTemplMain.o\
		$(TRG_PATH)/Objects/GateTemplProcess.o\
		$(TRG_PATH)/Objects/GateTemplRegister.o\
		$(TRG_PATH)/Objects/GateTemplScan.o\
		$(TRG_PATH)/Objects/GateTemplSignals.o\
		$(TRG_PATH)/Objects/GateTemplTbl.o\
		$(TRG_PATH)/Objects/GateTemplWnd.o\
		libCrTools.a\
		libSrvAgentAPI.a\
		libSrvTools.a\
		libSrvTemplate.a\
		libDiagn.a\
		libFox.a
		$(CXX) -o $@ $^ $(LDOPTIONS)
$(TRG_PATH)/Objects/%.o : $(TRG_PATH)/Sources/%.cpp
	$(CXX) -c -o $@  -IGateTempl-dir/Includes -IGateTempl-dir/Sources $(CXXFLAGS) $<

TRG		= GateRepl
TRG_PATH	= $(TRG)-dir
$(TRG) :	$(TRG_PATH)/Objects/GateRepl.o\
		$(TRG_PATH)/Objects/GateReplDiag.o\
		$(TRG_PATH)/Objects/GateReplIdb.o\
		$(TRG_PATH)/Objects/GateReplMain.o\
		$(TRG_PATH)/Objects/GateReplRegister.o\
		$(TRG_PATH)/Objects/GateReplScan.o\
		$(TRG_PATH)/Objects/GateReplWnd.o\
		libCrTools.a\
		libSrvAgentAPI.a\
		libSrvTools.a\
		libSrvTemplate.a\
		libDiagn.a\
		libFox.a
		$(CXX) -o $@ $^ $(LDOPTIONS)
$(TRG_PATH)/Objects/%.o : $(TRG_PATH)/Sources/%.cpp
	$(CXX) -c -o $@  -IGateRepl-dir/Includes -IGateRepl-dir/Sources $(CXXFLAGS) $<

TRG		= GateIvv
TRG_PATH	= $(TRG)-dir
$(TRG) :	$(TRG_PATH)/Objects/GateIvv.o\
		$(TRG_PATH)/Objects/GateIvvDiag.o\
		$(TRG_PATH)/Objects/GateIvvIdb.o\
		$(TRG_PATH)/Objects/GateIvvMain.o\
		$(TRG_PATH)/Objects/GateIvvProcess.o\
		$(TRG_PATH)/Objects/GateIvvRegister.o\
		$(TRG_PATH)/Objects/GateIvvScan.o\
		$(TRG_PATH)/Objects/GateIvvSignals.o\
		$(TRG_PATH)/Objects/GateIvvWnd.o\
		libCrTools.a\
		libSrvAgentAPI.a\
		libSrvTools.a\
		libDiagn.a\
		libFox.a
		$(CXX) -o $@ $^ $(LDOPTIONS)
$(TRG_PATH)/Objects/%.o : $(TRG_PATH)/Sources/%.cpp
	$(CXX) -c -o $@  -IGateIvv-dir/Includes -IGateIvv-dir/Sources $(CXXFLAGS) $<

TRG		= GateKso
TRG_PATH	= $(TRG)-dir
$(TRG) : 	$(TRG_PATH)/Objects/GateKso.o\
		$(TRG_PATH)/Objects/GateKsoApp.o\
		$(TRG_PATH)/Objects/GateKsoMain.o\
		$(TRG_PATH)/Objects/GateKsoRegister.o\
		$(TRG_PATH)/Objects/GateKsoSignals.o\
		$(TRG_PATH)/Objects/GateKsoFrame.o\
		$(TRG_PATH)/Objects/GateKsoObd.o\
		$(TRG_PATH)/Objects/GateKsoScan.o\
		$(TRG_PATH)/Objects/GateKsoWnd.o\
		$(TRG_PATH)/Objects/GateKsoDiag.o\
		$(TRG_PATH)/Objects/GateKsoIdb.o\
		$(TRG_PATH)/Objects/GateKsoChangeSmooth.o\
		$(TRG_PATH)/Objects/GateKsoSendPZ.o\
		$(TRG_PATH)/Objects/GateKsoPulse.o\
		$(TRG_PATH)/Objects/GateKsoSendCP.o\
		libCrTools.a\
		libSrvTools.a\
		libSrvAgentAPI.a\
		libDiagn.a\
		libFox.a
		$(CXX) -o $@ $^ $(LDOPTIONS) -lJTC -lMSKU_M_OBD -lOB
$(TRG_PATH)/Objects/%.o : $(TRG_PATH)/Sources/%.cpp
	$(CXX) -c -o $@  -IGateKso-dir/Includes -IGateKso-dir/Sources $(CXXFLAGS) $<

TRG		= ClientDiagn
TRG_PATH	= $(TRG)-dir
$(TRG) : 	$(TRG_PATH)/Objects/ClientDiagnApp.o\
		$(TRG_PATH)/Objects/ClientDiagn.o\
		$(TRG_PATH)/Objects/ClientDiagnDout.o\
		$(TRG_PATH)/Objects/ClientDiagnFrame.o\
		$(TRG_PATH)/Objects/ClientDiagnIdb.o\
		$(TRG_PATH)/Objects/ClientDiagnInput.o\
		$(TRG_PATH)/Objects/ClientDiagnMain.o\
		$(TRG_PATH)/Objects/ClientDiagnScan.o\
		$(TRG_PATH)/Objects/ClientDiagnSignals.o\
		$(TRG_PATH)/Objects/ClientDiagnWnd.o\
		libCrTools.a\
		libSrvTools.a\
		libFox.a\
		libSrvAgentAPI.a\
		libDiagn.a\
		libDiagnTools.a
		$(CXX) -o $@ $^ $(LDOPTIONS) -lBKnt -lsensors
$(TRG_PATH)/Objects/%.o : $(TRG_PATH)/Sources/%.cpp
	$(CXX) -c -o $@  -IClientDiagn-dir/Includes -IClientDiagn-dir/Sources $(CXXFLAGS) $<

TRG		= GateDiagn
TRG_PATH	= $(TRG)-dir
$(TRG) :	$(TRG_PATH)/Objects/GateDiagnApp.o\
		$(TRG_PATH)/Objects/GateDiagnComp.o\
		$(TRG_PATH)/Objects/GateDiagn.o\
		$(TRG_PATH)/Objects/GateDiagnFrame.o\
		$(TRG_PATH)/Objects/GateDiagnIdb.o\
		$(TRG_PATH)/Objects/GateDiagnMain.o\
		$(TRG_PATH)/Objects/GateDiagnScan.o\
		$(TRG_PATH)/Objects/GateDiagnSignals.o\
		$(TRG_PATH)/Objects/GateDiagnWnd.o\
		$(TRG_PATH)/Objects/GateDiagnNode.o\
		$(TRG_PATH)/Objects/GateDiagnSso.o\
		libCrTools.a\
		libSrvTools.a\
		libFox.a\
		libSrvAgentAPI.a\
		libDiagn.a
		$(CXX) -o $@ $^ $(LDOPTIONS) -lMSKU_M_OBD -lOB -lJTC
$(TRG_PATH)/Objects/%.o : $(TRG_PATH)/Sources/%.cpp
	$(CXX) -c -o $@  -IGateDiagn-dir/Includes -IGateDiagn-dir/Sources $(CXXFLAGS) $<

TRG		= SrvDiagn
TRG_PATH	= $(TRG)-dir
$(TRG) :	$(TRG_PATH)/Objects/SrvDiagn.o\
		$(TRG_PATH)/Objects/SrvDiagnApp.o\
		$(TRG_PATH)/Objects/SrvDiagnGate.o\
		$(TRG_PATH)/Objects/SrvDiagnSignals.o\
		$(TRG_PATH)/Objects/SrvDiagnIdb.o\
		$(TRG_PATH)/Objects/SrvDiagnRegister.o\
		$(TRG_PATH)/Objects/SrvDiagnSnmp.o\
		$(TRG_PATH)/Objects/SrvDiagnFrame.o\
		$(TRG_PATH)/Objects/SrvDiagnMain.o\
		$(TRG_PATH)/Objects/SrvDiagnScan.o\
		$(TRG_PATH)/Objects/SrvDiagnWnd.o\
		libCrTools.a\
		libSrvTools.a\
		libFox.a\
		libSrvAgentAPI.a\
		libDiagn.a
		$(CXX) -o $@ $^ $(LDOPTIONS) $(LDDIAGS)
$(TRG_PATH)/Objects/%.o : $(TRG_PATH)/Sources/%.cpp
	$(CXX) -c -o $@  -ISrvDiagn-dir/Includes -ISrvDiagn-dir/Sources $(CXXFLAGS) $<

TRG		= FcBrowser
TRG_PATH	= $(TRG)-dir
$(TRG) :	$(TRG_PATH)/Objects/FcBrowser.o\
		$(TRG_PATH)/Objects/FcBrowserFrame.o\
		$(TRG_PATH)/Objects/FcBrowserRecv.o\
		$(TRG_PATH)/Objects/FcBrowserMain.o\
		$(TRG_PATH)/Objects/FcBrowserWnd.o\
		libBrowser.a\
		libCrTools.a\
		libSvrk.a\
		libSrvTools.a\
		libSrvAgentAPI.a\
		libDiagn.a\
		libSrvTemplate.a\
		libFoxI.a\
		libVoyage.a\
		$(IMGLIBS)
		$(CXX) -o $@ $^ $(LDOPTIONS)
$(TRG_PATH)/Objects/%.o : $(TRG_PATH)/Sources/%.cpp
	$(CXX) -c -o $@  -IFcBrowser-dir/Includes -IFcBrowser-dir/Sources $(CXXFLAGS) $<

TRG		= Autonom
TRG_PATH	= $(TRG)-dir
$(TRG) :	$(TRG_PATH)/Objects/Autonom.o\
		$(TRG_PATH)/Objects/AutonomMain.o\
		libCrTools.a\
		libSrvTools.a\
		libSrvAgentAPI.a\
		libFox.a\
		libVoyage.a
		$(CXX) -o $@ $^ $(LDOPTIONS)
$(TRG_PATH)/Objects/%.o : $(TRG_PATH)/Sources/%.cpp
	$(CXX) -c -o $@  -IAutonom-dir/Includes -IAutonom-dir/Sources $(CXXFLAGS) $<

TRG		= GateCruise
TRG_PATH	= $(TRG)-dir
$(TRG) :	$(TRG_PATH)/Objects/GateCruise.o\
		$(TRG_PATH)/Objects/GateCruiseApp.o\
		$(TRG_PATH)/Objects/GateCruiseIdb.o\
		$(TRG_PATH)/Objects/GateCruiseCalc.o\
		$(TRG_PATH)/Objects/GateCruiseCommand.o\
		$(TRG_PATH)/Objects/GateCruiseSignals.o\
		$(TRG_PATH)/Objects/GateCruiseMain.o\
		$(TRG_PATH)/Objects/GateCruiseWnd.o\
		$(TRG_PATH)/Objects/GateCruiseRegister.o\
		$(TRG_PATH)/Objects/GateCruiseDiag.o\
		$(TRG_PATH)/Objects/GateCruiseScan.o\
		$(TRG_PATH)/Objects/GateCruiseExchange.o\
		$(TRG_PATH)/Objects/GateCruiseStruct.o\
		libCrTools.a\
		libDiagn.a\
		libSrvAgentAPI.a\
		libSrvTools.a\
		libVoyage.a\
		libFox.a
		$(CXX) -o $@ $^ $(LDOPTIONS)
$(TRG_PATH)/Objects/%.o : $(TRG_PATH)/Sources/%.cpp
	$(CXX) -c -o $@  -IGateCruise-dir/Includes -IGateCruise-dir/Sources $(CXXFLAGS) $<

TRG		= TblEdit
TRG_PATH	= $(TRG)-dir
$(TRG) :	$(TRG_PATH)/Objects/TblEdit.o\
		$(TRG_PATH)/Objects/TblEditWnd.o\
		libSrvTools.a\
		libSrvAgentAPI.a\
		libFox.a\
		libCrTools.a
		$(CXX) -o $@ $^ $(LDOPTIONS)
$(TRG_PATH)/Objects/%.o : $(TRG_PATH)/Sources/%.cpp
	$(CXX) -c -o $@  -ITblEdit-dir/Includes -ITblEdit-dir/Sources $(CXXFLAGS) $<

TRG		= SyncSrv
TRG_PATH	= $(TRG)-dir
$(TRG) :	$(TRG_PATH)/Objects/SyncSrv.o\
		$(TRG_PATH)/Objects/SyncSrvDisp.o\
		$(TRG_PATH)/Objects/SyncSrvMain.o\
		$(TRG_PATH)/Objects/SyncSrvNet.o\
		$(TRG_PATH)/Objects/SyncSrvTbl.o\
		$(TRG_PATH)/Objects/SyncSrvWnd.o\
		libCrTools.a\
		libSrvAgentAPI.a\
		libSrvTools.a\
		libFox.a\
		libSync.a
		$(CXX) -o $@ $^ $(LDOPTIONS)
$(TRG_PATH)/Objects/%.o : $(TRG_PATH)/Sources/%.cpp
	$(CXX) -c -o $@  -ISyncSrv-dir/Includes -ISyncSrv-dir/Sources $(CXXFLAGS) $<

TRG		= SyncAgent
TRG_PATH	= $(TRG)-dir
$(TRG) :	$(TRG_PATH)/Objects/SyncAgent.o\
		$(TRG_PATH)/Objects/SyncAgentDisp.o\
		$(TRG_PATH)/Objects/SyncAgentMain.o\
		$(TRG_PATH)/Objects/SyncAgentNet.o\
		$(TRG_PATH)/Objects/SyncAgentTbl.o\
		$(TRG_PATH)/Objects/SyncAgentWnd.o\
		libCrTools.a\
		libSrvAgentAPI.a\
		libSrvTools.a\
		libFox.a\
		libDiagn.a\
		libSync.a
		$(CXX) -o $@ $^ $(LDOPTIONS)
$(TRG_PATH)/Objects/%.o : $(TRG_PATH)/Sources/%.cpp
	$(CXX) -c -o $@  -ISyncAgent-dir/Includes -ISyncAgent-dir/Sources $(CXXFLAGS) $<

TRG		= SyncEd
TRG_PATH	= $(TRG)-dir
$(TRG) :	$(TRG_PATH)/Objects/Editor.o\
		$(TRG_PATH)/Objects/EditorDlg.o\
		$(TRG_PATH)/Objects/EditorConsole.o\
		$(TRG_PATH)/Objects/EditorImage.o\
		$(TRG_PATH)/Objects/EditorMain.o\
		$(TRG_PATH)/Objects/EditorModules.o\
		$(TRG_PATH)/Objects/EditorTree.o\
		libSrvTools.a\
		libSrvAgentAPI.a\
		libFox.a\
		libCrTools.a\
		libSync.a
		$(CXX) -o $@ $^ $(LDOPTIONS)
$(TRG_PATH)/Objects/%.o : $(TRG_PATH)/Sources/%.cpp
	$(CXX) -c -o $@  -ISyncEd-dir/Includes -ISyncEd-dir/Sources $(CXXFLAGS) $<

TRG		= ArcMan
TRG_PATH	= $(TRG)-dir
$(TRG) :	$(TRG_PATH)/Objects/ArcMan.o\
		$(TRG_PATH)/Objects/ArcManApp.o\
		$(TRG_PATH)/Objects/ArcManDiag.o\
		$(TRG_PATH)/Objects/ArcManDisp.o\
		$(TRG_PATH)/Objects/ArcManDlg.o\
		$(TRG_PATH)/Objects/ArcManFrame.o\
		$(TRG_PATH)/Objects/ArcManMain.o\
		$(TRG_PATH)/Objects/ArcManRecord.o\
		$(TRG_PATH)/Objects/ArcManTbl.o\
		$(TRG_PATH)/Objects/ArcManWnd.o\
		libCrTools.a\
		libSrvTools.a\
		libSrvAgentAPI.a\
		libFox.a
		$(CXX) -o $@ $^ $(LDOPTIONS) -lburn -lcdio
$(TRG_PATH)/Objects/%.o : $(TRG_PATH)/Sources/%.cpp
	$(CXX) -c -o $@  -IArcMan-dir/Includes -IArcMan-dir/Sources $(CXXFLAGS) $<

TRG		= GateAmur
TRG_PATH	= $(TRG)-dir
$(TRG) :	$(TRG_PATH)/Objects/GateAmur.o\
		$(TRG_PATH)/Objects/GateAmurDiag.o\
		$(TRG_PATH)/Objects/GateAmurMain.o\
		$(TRG_PATH)/Objects/GateAmurRecv.o\
		$(TRG_PATH)/Objects/GateAmurWnd.o\
		$(TRG_PATH)/Objects/GateAmurFileFrame.o\
		libCrTools.a\
		libSrvTools.a\
		libSrvAgentAPI.a\
		libDiagn.a\
		libFox.a
		$(CXX) -o $@ $^ $(LDOPTIONS)
$(TRG_PATH)/Objects/%.o : $(TRG_PATH)/Sources/%.cpp
	$(CXX) -c -o $@  -IGateAmur-dir/Includes -IGateAmur-dir/Sources $(CXXFLAGS) $<


TRG		= SimSrv
TRG_PATH	= $(TRG)-dir
$(TRG) :	$(TRG_PATH)/Objects/SimSrv.o\
		$(TRG_PATH)/Objects/SimSrvDiag.o\
		$(TRG_PATH)/Objects/SimSrvIdb.o\
		$(TRG_PATH)/Objects/SimSrvMain.o\
		$(TRG_PATH)/Objects/SimSrvNet.o\
		$(TRG_PATH)/Objects/SimSrvWnd.o\
		libCrTools.a\
		libSrvTools.a\
		libSrvAgentAPI.a\
		libFox.a
		$(CXX) -o $@ $^ $(LDOPTIONS)
$(TRG_PATH)/Objects/%.o : $(TRG_PATH)/Sources/%.cpp
	$(CXX) -c -o $@  -ISimSrv-dir/Includes -ISimSrv-dir/Sources $(CXXFLAGS) $<

TRG		= GateRs
TRG_PATH	= $(TRG)-dir
$(TRG) :	$(TRG_PATH)/Objects/GateRs.o\
		$(TRG_PATH)/Objects/GateRsWnd.o\
		$(TRG_PATH)/Objects/GateRsExchange.o\
		$(TRG_PATH)/Objects/GateRsDiag.o\
		$(TRG_PATH)/Objects/GateRsIdb.o\
		$(TRG_PATH)/Objects/GateRsMain.o\
		$(TRG_PATH)/Objects/GateRsScanPort.o\
		$(TRG_PATH)/Objects/GateRsSendTime.o\
		$(TRG_PATH)/Objects/GateRsReg.o\
		libCrTools.a\
		libSrvTools.a\
		libFox.a\
		libSrvAgentAPI.a\
		libDiagn.a\
		/usr/lib/libmodbus.a
		$(CXX) -o $@ $^ $(LDOPTIONS)
$(TRG_PATH)/Objects/%.o : $(TRG_PATH)/Sources/%.cpp
	$(CXX) -c -o $@  -IGateRs-dir/Includes -IGateRs-dir/Sources $(CXXFLAGS) $<

TRG		= GateGcn
TRG_PATH	= $(TRG)-dir
$(TRG) :	$(TRG_PATH)/Objects/GateGcn.o\
		$(TRG_PATH)/Objects/GateGcnDiag.o\
		$(TRG_PATH)/Objects/GateGcnExchange.o\
		$(TRG_PATH)/Objects/GateGcnIdb.o\
		$(TRG_PATH)/Objects/GateGcnMain.o\
		$(TRG_PATH)/Objects/GateGcnMaps.o\
		$(TRG_PATH)/Objects/GateGcnMapsScan.o\
		$(TRG_PATH)/Objects/GateGcnReg.o\
		$(TRG_PATH)/Objects/GateGcnScan.o\
		$(TRG_PATH)/Objects/GateGcnWnd.o\
		libCrTools.a\
		libSrvTools.a\
		libFox.a\
		libSrvAgentAPI.a\
		libDiagn.a
		$(CXX) -o $@ $^ $(LDOPTIONS) -lMAPS
$(TRG_PATH)/Objects/%.o : $(TRG_PATH)/Sources/%.cpp
	$(CXX) -c -o $@  -IGateGcn-dir/Includes -IGateGcn-dir/Sources $(CXXFLAGS) $<


TRG		= GateSdor
TRG_PATH	= $(TRG)-dir
$(TRG) :	$(TRG_PATH)/Objects/GateSdor.o\
		$(TRG_PATH)/Objects/GateSdorCalc.o\
		$(TRG_PATH)/Objects/GateSdorDiag.o\
		$(TRG_PATH)/Objects/GateSdorIdb.o\
		$(TRG_PATH)/Objects/GateSdorMain.o\
		$(TRG_PATH)/Objects/GateSdorRegister.o\
		$(TRG_PATH)/Objects/GateSdorScan.o\
		$(TRG_PATH)/Objects/GateSdorWnd.o\
		libCrTools.a\
		libSrvAgentAPI.a\
		libSrvTools.a\
		libDiagn.a\
		libFox.a
		$(CXX) -o $@ $^ $(LDOPTIONS)
$(TRG_PATH)/Objects/%.o : $(TRG_PATH)/Sources/%.cpp
	$(CXX) -c -o $@  -IGateSdor-dir/Includes -IGateSdor-dir/Sources $(CXXFLAGS) $<

TRG		= GateBcn
TRG_PATH	= $(TRG)-dir
$(TRG) :	$(TRG_PATH)/Objects/GateBcn.o\
		$(TRG_PATH)/Objects/GateBcnDiag.o\
		$(TRG_PATH)/Objects/GateBcnIdb.o\
		$(TRG_PATH)/Objects/GateBcnMain.o\
		$(TRG_PATH)/Objects/GateBcnReg.o\
		$(TRG_PATH)/Objects/GateBcnScan.o\
		$(TRG_PATH)/Objects/GateBcnWnd.o\
		$(TRG_PATH)/Objects/GateBcnCalc.o\
		$(TRG_PATH)/Objects/GateBcnData.o\
		$(TRG_PATH)/Objects/GateBcnPrepare.o\
		libCrTools.a\
		libSrvAgentAPI.a\
		libSrvTools.a\
		libDiagn.a\
		libFox.a
		$(CXX) -o $@ $^ $(LDOPTIONS) -lcurl

$(TRG_PATH)/Objects/%.o : $(TRG_PATH)/Sources/%.cpp
	$(CXX) -c -o $@  -IGateBcn-dir/Includes -IGateBcn-dir/Sources $(CXXFLAGS) $<

TRG		= libAlgLib.a
TRG_PATH	= $(TRG)-dir
$(TRG) :	$(TRG_PATH)/Objects/alglibinternal.o \
		$(TRG_PATH)/Objects/alglibmisc.o \
		$(TRG_PATH)/Objects/ap.o \
		$(TRG_PATH)/Objects/dataanalysis.o \
		$(TRG_PATH)/Objects/diffequations.o \
		$(TRG_PATH)/Objects/fasttransforms.o \
		$(TRG_PATH)/Objects/integration.o \
		$(TRG_PATH)/Objects/interpolation.o \
		$(TRG_PATH)/Objects/linalg.o \
		$(TRG_PATH)/Objects/optimization.o \
		$(TRG_PATH)/Objects/solvers.o \
		$(TRG_PATH)/Objects/specialfunctions.o \
		$(TRG_PATH)/Objects/statistics.o 
		rm -f $@
		$(AR) -cr $(AROPTIONS) $@ $^
		ranlib $@

$(TRG_PATH)/Objects/%.o : $(TRG_PATH)/Sources/%.cpp
	$(CXX) -c -o $@  -IlibAlgLib.a-dir/Includes -IlibAlgLib.a-dir/Sources $(CXXFLAGS) $<

TRG		= ImCoreAkZ
TRG_PATH	= $(TRG)-dir
$(TRG) :	$(TRG_PATH)/Objects/core.o \
		$(TRG_PATH)/Objects/prism.o \
		$(TRG_PATH)/Objects/crod.o \
		$(TRG_PATH)/Objects/assembly.o \
		$(TRG_PATH)/Objects/node.o \
		$(TRG_PATH)/Objects/cubic_spline.o \
		$(TRG_PATH)/Objects/xs.o \
		$(TRG_PATH)/Objects/suz.o \
		$(TRG_PATH)/Objects/aprox_polynom.o \
		$(TRG_PATH)/Objects/kni.o \
		$(TRG_PATH)/Objects/time_imcore.o \
		$(TRG_PATH)/Objects/core_reactivity_coef.o \
                $(TRG_PATH)/Objects/test_writer.o \
                $(TRG_PATH)/Objects/archive.o
		rm -f $@
		$(AR) -cr $(AROPTIONS) $@ $^
		ranlib $@

$(TRG_PATH)/Objects/%.o : $(TRG_PATH)/Sources/%.cpp
	$(CXX) -c -o $@  -IImCoreAkZ-dir/Includes -IImCoreAkZ-dir/Sources $(CXXFLAGS) $<

TRG		= ImCore
TRG_PATH	= $(TRG)-dir
$(TRG) :	$(TRG_PATH)/Objects/ImCore.o\
		$(TRG_PATH)/Objects/ImCoreDiag.o\
		$(TRG_PATH)/Objects/ImCoreIdb.o\
		$(TRG_PATH)/Objects/ImCoreMain.o\
		$(TRG_PATH)/Objects/ImCoreReg.o\
		$(TRG_PATH)/Objects/ImCoreScan.o\
		$(TRG_PATH)/Objects/ImCoreWnd.o\
		$(TRG_PATH)/Objects/ImCoreCalc.o\
		$(TRG_PATH)/Objects/ImCorePrepare.o\
		$(TRG_PATH)/Objects/ImCoreAkZ.o\
		ImCoreAkZ\
		libCrTools.a\
		libAlgLib.a\
		libSrvAgentAPI.a\
		libSrvTools.a\
		libDiagn.a\
		libFox.a 

		$(CXX) -o $@ $^ $(LDOPTIONS) -fopenmp
$(TRG_PATH)/Objects/%.o : $(TRG_PATH)/Sources/%.cpp
	$(CXX) -c -o $@  -IImCore-dir/Includes -IImCore-dir/Sources $(CXXFLAGS) $< -fopenmp


TRG		= ImCoreP
TRG_PATH	= $(TRG)-dir
$(TRG) :	$(TRG_PATH)/Objects/ImCorePCalc.o\
		$(TRG_PATH)/Objects/ImCorePClient.o\
		$(TRG_PATH)/Objects/ImCoreP.o\
		$(TRG_PATH)/Objects/ImCorePDiag.o\
		$(TRG_PATH)/Objects/ImCorePMain.o\
		$(TRG_PATH)/Objects/ImCorePNet.o\
		$(TRG_PATH)/Objects/ImCorePWnd.o\
		$(TRG_PATH)/Objects/forecast.o\
		ImCoreAkZ\
		libCrTools.a\
		libAlgLib.a\
		libSrvTools.a\
		libSrvAgentAPI.a\
		libDiagn.a\
		libFox.a

		$(CXX) -o $@ $^ $(LDOPTIONS) -fopenmp

$(TRG_PATH)/Objects/%.o : $(TRG_PATH)/Sources/%.cpp
	$(CXX) -c -o $@  -IImCoreP-dir/Includes -IImCoreP-dir/Sources $(CXXFLAGS) $< -fopenmp

TRG		= ImCorePtest
TRG_PATH	= $IImCoreP-dir
$(TRG) :	$(TRG_PATH)/Objects/maintest.o\
                $(TRG_PATH)/Objects/forecast.o\
                $(TRG_PATH)/Objects/ImCorePCalc.o\
                $(TRG_PATH)/Objects/ImCorePClient.o\
                $(TRG_PATH)/Objects/ImCoreP.o\
                $(TRG_PATH)/Objects/ImCorePDiag.o\
                $(TRG_PATH)/Objects/ImCorePMain.o\
                $(TRG_PATH)/Objects/ImCorePNet.o\
                $(TRG_PATH)/Objects/ImCorePWnd.o\
		ImCoreAkZ\
		libCrTools.a\
		libAlgLib.a\
		libSrvTools.a\
		libSrvAgentAPI.a\
		libDiagn.a\
		libFox.a

		$(CXX) -o $@ $^ $(LDOPTIONS) -fopenmp
$(TRG_PATH)/Objects/%.o : $(TRG_PATH)/Sources/%.cpp
	$(CXX) -c -o $@  -IImCoreP-dir/Includes -IImCoreP-dir/Sources $(CXXFLAGS) $< -fopenmp




#:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
.PHONY: objdirs
TargetDirs = $(shell ls -d *-dir)
objects:	$(addsuffix /Objects, $(TargetDirs))
%-dir/Objects:	%-dir
		mkdir $@

.PHONY: clean
clean:
	rm -f lib*.a
	rm -f SendMsg
	rm -f SrvTemplate
	rm -f ImGKso
	rm -f SrvAgent
	rm -f SrvCore
	rm -f SrvBrowser 
	rm -f GateFkd
	rm -f GateIvs
	rm -f GateIvv
	rm -f GateLsb
	rm -f GateTempl
	rm -f GateHort
	rm -f GateKso
	rm -f GateBod
	rm -f ClientDiagn
	rm -f GateDiagn
	rm -f SrvDiagn
	rm -f GateCruise
	rm -f GateRepl
	rm -f SyncAgent
	rm -f SyncSrv
	rm -f Editor
	rm -f SyncEd
	rm -f ViBrowser
	rm -f FcBrowser
	rm -f Autonom
	rm -f UpdateSet
	rm -f ArcMan
	rm -f GateAmur
	rm -f SimSrv
	rm -f GateGcn
	rm -f GateRs
	rm -f GateSdor
	rm -f GateBcn
	rm -f ToDat
	rm -f ImCore
	rm -f ImCoreP
	find . -name '*.o' |xargs rm -f 

.PHONY: strip
strip:
	strip ClientDiagn
	strip GateTempl
	strip SendMsg
	strip FcBrowser
	strip SrvAgent
	strip GateCruise
	strip SrvBrowser
	strip GateDiagn
	strip SrvCore
	strip GateFkd
	strip SrvDiagn
	strip GateIvs
	strip GateIvv
	strip GateKso
	strip GateBod
	strip SyncAgent
	strip GateRepl
	strip SyncSrv
	strip SyncEd
	strip GateLsb
	strip Autonom
	strip UpdateSet
	strip ArcMan
	strip GateAmur
	strip SimSrv
	strip GateGcn
	strip GateRs
	strip GateSdor
	strip GateBcn
	strip ToDat
	strip ImCore
	strip ImCoreP
