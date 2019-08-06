#ifndef _IMCORE_CALC_H
#define _IMCORE_CALC_H

#define	KNI_CNT					64
#define	DPZ_CNT					7
#define	TVS_CNT					163
#define	PIN_CNT					331
#define	TVEL_CNT				312
#define	TVS_TP_CNT				95
#define	SUZ_CNT					61
#define	SUZG_CNT				10
#define	L07_CNT					7
#define	L24_CNT					24
#define	L16_CNT					16
#define	PT_CNT					4

#pragma pack(1)	// ������������ �� 1
struct Val
{
	float	fValue;
	char	cStatus;
};
struct InputHeader
{
	CTime	Time;
};
struct InputData
{
	Val	SuzG[SUZG_CNT];
	Val	Suz[SUZ_CNT];
	Val	Nakz;
	Val	Ied[KNI_CNT][DPZ_CNT];
	Val	Stpt[PT_CNT];
	Val	Pakz;
	Val	Thn[PT_CNT];
	Val	Tp[TVS_TP_CNT];
	Val	Yaf_s;
};
struct InputBuff
{
	InputHeader	Header;
	InputData	Data;
};
#pragma pack()



//////////////////////////////////////////////////////////////////////
//
class WaterSteamProperty
{
public:
	WaterSteamProperty(void){};
	~WaterSteamProperty(void){};

	static double getWaterEnthalpy(double pressure, double temperature);	// 3.19.01	���
	static double getWaterTemperature(double pressure, double enthalpy);	// 3.19.02	���
	static double getWaterDensity(double pressure, double temperature);		// 3.19.03	���
	static double getSteamEnthalpyS(double pressureSatturation);			// 3.19.05	���
	static double getWaterEnthalpyS(double pressureSatturation);			// 3.19.06	���
	static double getWaterBoilingHeatS(double pressureSatturation);			// 3.19.07	���
	static double getWaterTemperatureS(double pressureSatturation);			// 3.19.10	���

//	static double getWaterEnthalpy2(double pressure, double temperature);
//	static double getPressureSatturation(double pressure, double temperature);
};


class CProgCalc;

class CProgCalcParamInfo :  public LONG_LST
{
public:
	CInfoDataBase*	pIdb;
	int				SizeX;
	int				SizeY;
	float			MultyCoef;

public:
	CProgCalcParamInfo(CInfoDataBase* pIdb, long SizeX, long SizeY, float MultyCoef);
	virtual	~CProgCalcParamInfo(void);

	CProgParamInfo*	GetParamInfo(int Pos=0);
	CProgParamInfo*	GetParamInfo(int PosX, int PosY);

	void			Read(float* pValue, char* pStatus);
	void			ReadValue(float* pValue);
	void			ReadStatus(char* pStatus);

	void			Write(float* pValue, char* pStatus, bool bAlwaysWrite=false);
	void			WriteValue(float* pValue, char cStatus=STATUS_NORMAL, bool bAlwaysWrite=false);
	void			WriteStatus(char cStatus);
	void			WriteStatus(char* pStatus);
};


/*struct IdbValue
{
public:
	double	dValue;
	char	cStatus;
	char*	pCalcName;

	IdbValue(double dValue=0.0, char cStatus=STATUS_UNDEF){ IdbValue::dValue=dValue; IdbValue::cStatus=cStatus; IdbValue::pCalcName=NULL;}

	double	v(){ return dValue; }
	char	s(){ return cStatus; }
	bool	ok(){ return cStatus<STATUS_UNDEF ? true : false; }
};


class CCalcConstTbl : public CFileTbl
{
public:
	CCol* pConstNameCol;
	CCol* pConstValueCol;
	CCol* pConstMUnitCol;
	CCol* pFullNameCol;

public:

	CCalcConstTbl(void);
	virtual ~CCalcConstTbl();

			void		AppendConst(const char* pConstName, double ConstValue, const char* pStrMUnit, const char* pFullName);
			int			GetConstPos(const char* pConstName);
			double		GetConstValue(const char* pConstName);

			bool		Load(char* pFileName, CStr* pStrError=NULL);
	virtual	void		MakeColumns(void);
};


class CCalcParamsTbl : public CFileTbl
{
public:
	CCol* pIdbNameCol;
	CCol* pParamNameCol;
	CCol* pFullNameCol;
	CCol* pTemplateCol;
	CCol* pSizeXCol;
	CCol* pSizeYCol;
	CCol* pShiftXCol;
	CCol* pMultyCoefCol;

public:

	CCalcParamsTbl(void);
	virtual ~CCalcParamsTbl();

			void		AppendParam(const char* pIdbName, const char* pParamName, const char* pTemplate, int SizeX, int SizeY, float MultyCoef, int ShiftX, const char* pFullName);
			int			GetParamPos(const char* pParamName);

			bool		Load(char* pFileName, CStr* pStrError=NULL);
	virtual	void		MakeColumns(void);
};
*/

//////////////////////////////////////////////////////////////////////
//
class CNedQuotaStruct
{
public:
	int		NumKni;		// ����� ���
	int		NumDpz;		// ����� ���
	int		NumTvs;		// ����� ���
	int		NumPrLoDpz;	// ����� ������ � ����� ���
	int		CntPrDpz;	// ���������� ����� � ���
	double	QuotaPr1;	// ���� ������ ������
	double	QuotaPr2;	// ���� ������ ������
	double	QuotaPr3;	// ���� ������� ������
};





//////////////////////////////////////////////////////////////////////
//
class CTvsFuelStruct
{
public:
	int				NumTvs;				// ����� ���
	int				FuelId;				// ��� �������
	char			FuelName[20];		// ������������ �������
	unsigned char	ChanType;			// ��� ������ (� ������� TvelTypes)
	unsigned char	TvegType;			// ��� ���� (� ������� TvelTypes)
	unsigned char	PinTypes[PIN_CNT]; 	// ���� ����/����/������� � ���� ���
};


//////////////////////////////////////////////////////////////////////
//
class CQlRangeStruct
{
public:
	int				Tvel;				// ������� ����
	int				Tveg;				// ������� ����
	double			Sl;					// ������� ��������� ����/����, ���*���./�� U
	double			QlRange;			// ���������� ��������, ��/��
};




class DPZ_DELAY
{// ���� ������������ ����� ��� �� ��������
private:
	double lam1;
	double lam2;
	double sig1;
	double sig2;
	double PP, QQ, mm1, mm2;
	double aa1, aa2, kk1, kk2, bb1, bb2, ee1, ee2;	//��������� ��� ��������� ������� �� ���� (2)
	double yy, yy1, fff1;				//��������� ��� ��������� ������� �� ���� � ����������� (2)
	double ddt;					//���������� ������� ����� ����������� �����

public:

	double Tok1,Tok2;				//���� ���
	double Flux;					//������������������ ��� ���
	double alf;						//�������� ����������� ����������
	double IPrev;					//���������� �������� ���� ���

//////////////////////////////////////////////////////////////////////////////////
	DPZ_DELAY();
	~DPZ_DELAY();

	void	Init(void);
	double	Bur_2_fl(double Q2, short T1, double h1, double gg1);//�������� ������� �� ���� c �����������
};


//////////////////////////////////////////////////////////////////////
// ����� ���������� ������ � ������������ ���ޣ��
class CProgCalc : public CObj
{
public:
	// ������� ������ ����� Monitor
	int		MonRecord1[8];
	float	MonRecord2[TVS_CNT][L24_CNT];				// KV24
	float	MonRecord3[TVS_CNT][L24_CNT];
	float	MonRecord4[6]; // ���� 5 2017.06.29

	// ������� ������ ����� Updater
	int		UpdRecord1[8];
	float	UpdRecord2[TVS_CNT][L24_CNT];				// KV24 duplex ??
	float	UpdRecord3[TVS_CNT][L24_CNT];
	float	UpdRecord4[TVS_CNT][PIN_CNT][L24_CNT];		// KK
	float	UpdRecord5[TVS_CNT][L24_CNT];				// ��������� ���������
	float	UpdRecord6[TVS_CNT][PIN_CNT];				// ������� ��������� ����/����
	float	UpdRecord7[TVS_CNT][L24_CNT];				// IOD24
	float	UpdRecord8[TVS_CNT][L24_CNT];				// XE24
	float	UpdRecord9[TVS_CNT][L24_CNT];				// PM24
	float	UpdRecord10[TVS_CNT][L24_CNT];				// SM24
	float	UpdRecord11[KNI_CNT][DPZ_CNT];				// SED
	float	UpdRecord12[11]; // ���� 7 2017.06.29

	CProg* 			pProg;				// ��������� �� ����
	CCalcConstTbl*	pConstTbl;			// ������� �������� ��� �������
	CCalcParamsTbl*	pParamsTbl;			// ������� ����� � ���������� ��� �������

	float			Conv7x24Array[L07_CNT][L24_CNT];	// ������ ��� �������������� 24 �������� ���� � 7 �������
	float			ConvKV7x24Array[L07_CNT][L24_CNT];	// ������ ��� �������������� 24 �������� ���� � 7 ������� (�������� ��� KV)
	CNedQuotaStruct	NedQuotaArray[KNI_CNT][DPZ_CNT]; 	// ������ ��� ������� ���� KV24 ��� ��������� �������� ���
	CTvsFuelStruct	TvsFuelArray[TVS_CNT];				// ������ ����� �������
	LST*			pTvelQlRangeLst;					// ������ �������� ������ ��� ����
	LST*			pTvegQlRangeLst;					// ������ �������� ������ ��� ����

	double			LINEQ[TVS_CNT][PIN_CNT][L24_CNT];	// �������� ������ � �������� ���� � �������� ������ - ��� ������� ���� ������� �� DNBR

	float			QVCAS[TVS_CNT];						// �������� ���

	// ��������� ��� ������� ����������������� ����� ���
	CTime			PrevCalcIEDCTime;			// ����� ����������� ������� ���� IEDC
	DPZ_DELAY		DpzDelay[KNI_CNT][DPZ_CNT];	// ������ �������� ����� ���


//	double			PrevIED[KNI_CNT][DPZ_CNT];	// ���������� �������� ����� ���
//	double			IEDC_y[KNI_CNT][DPZ_CNT];
//	double			IEDC_k1[KNI_CNT][DPZ_CNT];
//	double			IEDC_k2[KNI_CNT][DPZ_CNT];


	CProgCalcParamInfo*	pIED;				// ������ ���������� �� ���� IED � ��� (���� ��� ��������)
	CProgCalcParamInfo*	pKVED;				// ������ ���������� �� ���� KVED � ���
	CProgCalcParamInfo*	pIEDC;				// ������ ���������� �� ���� IED� � ��� (���� ��� � ����������)
	CProgCalcParamInfo*	pZE_SCAN;			// ������ ���������� �� ���� ZE � scan ���
	CProgCalcParamInfo*	pZE_REG; 			// ������ ���������� �� ���� ZE � reg ���
	CProgCalcParamInfo*	pQED;				// ������ ���������� �� ���� QED � ���
	CProgCalcParamInfo*	pRSED;				// ������ ���������� �� ���� SED � register ���
	CProgCalcParamInfo*	pSSED;				// ������ ���������� �� ���� SED � scan ���
	CProgCalcParamInfo*	pIOD;				// ������ ���������� �� ���� IOD � ���
	CProgCalcParamInfo*	pKV7L;				// ������ ���������� �� ���� KV7L � ���
	CProgCalcParamInfo*	pPM;				// ������ ���������� �� ���� PM � ���
	CProgCalcParamInfo*	pSM;				// ������ ���������� �� ���� SM � ���
	CProgCalcParamInfo*	pXE;				// ������ ���������� �� ���� XE � ���
	CProgCalcParamInfo*	pSL;				// ������ ���������� �� ���� SL � ���
	CProgCalcParamInfo*	pQVMF;				// ������ ���������� �� ���� QVMF � ���
	CProgCalcParamInfo*	pUsQVMF;			// ������ ���������� �� ���� usQVMF � ���
	CProgCalcParamInfo*	pDNBR;				// ������ ���������� �� ���� DNBR � ���
	CProgCalcParamInfo*	pKQ;				// ������ ���������� �� ���� KQ � ���
	CProgCalcParamInfo*	pQVCAS;				// ������ ���������� �� ���� KQ � ���
	CProgCalcParamInfo*	pDNBT;				// ������ ���������� �� ���� DNBT � ���
	CProgCalcParamInfo*	pSLCAS;				// ������ ���������� �� ���� SLCAS � ���
	CProgCalcParamInfo*	pSLTCAS;			// ������ ���������� �� ���� SLTCAS � ���
	CProgCalcParamInfo*	pSLNTCAS;			// ������ ���������� �� ���� SLNTCAS � ���
	CProgCalcParamInfo*	pKC;				// ������ ���������� �� ���� KC � ���
	CProgCalcParamInfo*	pOFSTCAS;			// ������ ���������� �� ���� OFSTCAS � ���
	CProgCalcParamInfo*	pYCTOUT;			// ������ ���������� �� ���� YCTOUT � ���
	CProgCalcParamInfo*	pYCTDT;				// ������ ���������� �� ���� YCTDT � ���
	CProgCalcParamInfo*	pYC00T_TP;			// ������ ���������� �� ���� YC00T_TP � ���
	CProgCalcParamInfo*	pYC00DT_TP;			// ������ ���������� �� ���� YC00DT_TP � ���
	CProgCalcParamInfo*	pSZ;				// ������ ���������� �� ���� SZ � ���

	IdbValue		PrevPzn;
	IdbValue		PrevTzn1;
	IdbValue		PrevTzn2;
	IdbValue		PrevTzn3;
	IdbValue		PrevPpg10;
	IdbValue		PrevPpg20;
	IdbValue		PrevPpg30;
	IdbValue		PrevPpg40;
	IdbValue		PrevTgn10;
	IdbValue		PrevTgn20;
	IdbValue		PrevTgn30;
	IdbValue		PrevTgn40;
	IdbValue		PrevThn10;
	IdbValue		PrevThn20;
	IdbValue		PrevThn30;
	IdbValue		PrevThn40;

	IdbValue		PrevVPzn;
	IdbValue		PrevVTzn1;
	IdbValue		PrevVTzn2;
	IdbValue		PrevVTzn3;
	IdbValue		PrevVPpg10;
	IdbValue		PrevVPpg20;
	IdbValue		PrevVPpg30;
	IdbValue		PrevVPpg40;
	IdbValue		PrevVTgn10;
	IdbValue		PrevVTgn20;
	IdbValue		PrevVTgn30;
	IdbValue		PrevVTgn40;
	IdbValue		PrevVThn10;
	IdbValue		PrevVThn20;
	IdbValue		PrevVThn30;
	IdbValue		PrevVThn40;


	IdbValue		PrevH08;		// ���������� ��������� ��������� ����� �� ������ ������ �� � BEACON
	IdbValue		PrevH09;
	IdbValue		PrevH10;
	IdbValue		PrevPOWER;


	CTime			PrevCalcZETime;		// ����� ����������� ������� ���� ZE
	double			PrevZE[KNI_CNT][DPZ_CNT];		// ��������� �������� ���� ZE
	char			stPrevIED[KNI_CNT][DPZ_CNT];	// ��������� ������� ���� IED

	CTime			PrevCalcTeffTime;	// ����� ����������� ������� TEFF
	double			PrevTEFF;			// ��������� �������� ���� TEFF

	bool			bStartAutoCalc;		// �������������� ���ޣ� TEFF � ZE

public:
	CProgCalc(CProg* pProg);
	~CProgCalc();

	void			ReadFromIdb(const char* pCalcName, IdbValue* pIdbValue, bool bEnableMessage=true);
	void			ReadFromIdb(const char* pCalcName, float* pValue, char* pStatus, bool bEnableMessage=true);
	void			ReadFromIdb(const char* pCalcName, double* pValue, char* pStatus, bool bEnableMessage=true);
	float			ReadValueFromIdb(const char* pCalcName, bool bEnableMessage=true);
	char			ReadStatusFromIdb(const char* pCalcName, bool bEnableMessage=true);

	void			WriteToIdb(const char* pCalcName, IdbValue* pIdbValue);
	void			WriteToIdb(const char* pCalcName, float Value, char cStatus);
	void			WriteValueToIdb(const char* pCalcName, float Value);
	void			WriteStatusToIdb(const char* pCalcName, char cStatus);

	void			WriteToIdb(const char* pCalcName, double Value, char cStatus);
	void			WriteValueToIdb(const char* pCalcName, double Value);

	float*			Convert24To7(float (&p24)[TVS_CNT][L24_CNT], float (&p7)[TVS_CNT][L07_CNT]);
	float*			ConvertKV24To7(float (&p24)[TVS_CNT][L24_CNT], float (&p7)[TVS_CNT][L07_CNT]);

	CProgParamInfo* 	ConnectWithIdb(CInfoDataBase* pIdb, const char* pCalcName, char* pShortName, bool bEnableMesaage);
	char*  			MakeIdbName(char* pTemplate, char* pRetBuff, int X, int Y, int ShiftX=0);
	CProgCalcParamInfo*	MakeCalcParamInfo(const char* pFieldName, bool bEnableMesaage=true);

	void 			Init(void);
	bool			LoadConv7x24Array(char* pFileName, CStr* pStrError=NULL);
	bool			LoadConvKV7x24Array(char* pFileName, CStr* pStrError=NULL);
	bool			LoadNedQuotaArray(char* pFileName, CStr* pStrError=NULL);
	bool			LoadTvsFuelArray(char* pDirName, CStr* pStrError=NULL);

	void			Calculation(int ChangeDataFlag);

	double			CalcConstUQL(double TvelSL, bool bTveg);
	void 			Calc_NED(void);
//	void 			Calc_NAKZ(void);
	void 			Calc_wNAKZ(void);
	void 			Calc_MODE(void);
	void			Calc_ZE(void);
	void			Calc_IEDC(void);
	void 			Calc_PZ2_QL(int ChangeDataFlag);
	void			Calc_PZ2_DNBR(int ChangeDataFlag);
	void			Calc_AppendFields(int ChangeDataFlag);
	void			WriteSimpleFieds(int ChangeDataFlag);
	void			CalcStartTEFF();
	void 			Calc_HH();
	void 			Calc_OFSTR();

	void 			Calc_Tn(IdbValue* pResult, IdbValue* pTs, IdbValue* pTp1, IdbValue* pTp2, double kTs, double kTp1, double kTp2);
	void			Calc_DtPt(IdbValue* pResult, IdbValue* pThn,  IdbValue* pTgn);
	void 			Calc_ThnTgnDt(int Step);

	void 			Calc_StPt(IdbValue* pResult, IdbValue* pOn,  IdbValue* pOff, IdbValue* pDt, IdbValue* pDp, double DT0, double DP0);
	void			Calc_StPt();

	void			Calc_Avg(IdbValue* pResult, LONG_LST*	pIdbValueLst, LONG_LST*	pStPtValueLst, bool bAllDost=false);
	void 			Calc_Sum(IdbValue* pResult, LONG_LST*	pIdbValueLst, bool bAllDost=false);
	void			Calc_Avg();

	void			Calc_Gpt(IdbValue* pGVpt, IdbValue* pGMpt,  IdbValue* pDpt, IdbValue* pStPt, IdbValue* pFpt, IdbValue* pDPpt, IdbValue* pThn, double YcpIn, double A0,  double A1,  double A2, double KCIO, double HOBI, double Density0, double Frequency_0, double GcnRes);
	void			Calc_Gpt();

	void			Calc_Gpg(IdbValue* pGMpg, IdbValue* pGVpg, IdbValue* pDpg, IdbValue* pTpg, IdbValue* pATpg, IdbValue* pPpg, IdbValue* pAPpg, double	Density_0);
	void 			Calc_Gpg();

	void			Calc_Gpvd(IdbValue* pGMpvd, IdbValue* pGVpvd,  IdbValue* pDpvd, IdbValue* pTpvd, IdbValue* pPpvd, double	Density_0);
	void 			Calc_Gpvd();

	void			Calc_ATn(IdbValue* pATn, LONG_LST* pTnLst, LONG_LST* pGMptLst, LONG_LST* pSptLst, int Kgcnm, int DostTnCnt, int GMPtDostCnt);
	void 			Calc_AThnAtgnADTpt();

	void 			Calc_DTtpADTtp();

	void			Calc_V(IdbValue* pNewV, IdbValue* pPrevV, IdbValue* pNewParam, IdbValue* pPrevParam, int N, int K);
	void			Calc_V();

	void			Calc_Npt(IdbValue* pNpt, IdbValue* pGMpt, IdbValue* pThn, IdbValue* pIhn,  IdbValue* pTgn, IdbValue* pIgn, double Pzn, double DPzn);
	void			Calc_N1k();

	void			Calc_Npg(IdbValue* pNpg, IdbValue* pGMpg, IdbValue* pPpg, IdbValue* pISpg, IdbValue* pAPpg, IdbValue* pPpw, IdbValue* pIpg, IdbValue* pTpw, IdbValue* pAPpw, IdbValue* pATpw, IdbValue* pNgcn, IdbValue* pGppgl, IdbValue* pGppol, IdbValue* pGnpgl, IdbValue* pGnpol);
	void			Calc_N2k();


	void			Calc_Npvd(IdbValue* pNpvd, IdbValue* pGMpvd, IdbValue* pAPpg, IdbValue* pISpvd, IdbValue* pPpvd, IdbValue* pIpvd, IdbValue* pTpvd);
	void			Calc_Npvd();

	void			Calc_Nik();

	void			Calc_TSgn();

	void			Calc_TSpg();

	void			Calc_SZa(float*	pSuz, char* pStSuz, int* pSuzGr, IdbValue* pSZa, int GrNum);
	void			Calc_SZa();

	void			Calc_Cbor();

	void 			Calc_Kpd();
};

#endif
