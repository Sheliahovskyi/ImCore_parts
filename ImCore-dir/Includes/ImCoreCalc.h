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

#pragma pack(1)	// выравнивание на 1
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

	static double getWaterEnthalpy(double pressure, double temperature);	// 3.19.01	ОАВ
	static double getWaterTemperature(double pressure, double enthalpy);	// 3.19.02	ОАВ
	static double getWaterDensity(double pressure, double temperature);		// 3.19.03	ОАВ
	static double getSteamEnthalpyS(double pressureSatturation);			// 3.19.05	ОАВ
	static double getWaterEnthalpyS(double pressureSatturation);			// 3.19.06	ОАВ
	static double getWaterBoilingHeatS(double pressureSatturation);			// 3.19.07	ОАВ
	static double getWaterTemperatureS(double pressureSatturation);			// 3.19.10	ОАВ

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
	int		NumKni;		// НОМЕР КНИ
	int		NumDpz;		// НОМЕР ДПЗ
	int		NumTvs;		// НОМЕР ТВС
	int		NumPrLoDpz;	// НОМЕР ПРИЗМЫ С НИЗОМ ДПЗ
	int		CntPrDpz;	// КОЛИЧЕСТВО ПРИЗМ С ДПЗ
	double	QuotaPr1;	// ДОЛЯ ПЕРВОЙ ПРИЗМЫ
	double	QuotaPr2;	// ДОЛЯ ВТОРОЙ ПРИЗМЫ
	double	QuotaPr3;	// ДОЛЯ ТРЕТЬЕЙ ПРИЗМЫ
};





//////////////////////////////////////////////////////////////////////
//
class CTvsFuelStruct
{
public:
	int				NumTvs;				// номер ТВС
	int				FuelId;				// тип топлива
	char			FuelName[20];		// наименование топлива
	unsigned char	ChanType;			// тип канала (в массиве TvelTypes)
	unsigned char	TvegType;			// тип ТВЭГ (в массиве TvelTypes)
	unsigned char	PinTypes[PIN_CNT]; 	// типы ТВЭЛ/ТВЭГ/КАНАЛОВ в этой ТВС
};


//////////////////////////////////////////////////////////////////////
//
class CQlRangeStruct
{
public:
	int				Tvel;				// признак ТВЭЛ
	int				Tveg;				// признак ТВЭГ
	double			Sl;					// среднее выгорание ТВЭЛ/ТВЭГ, МВт*сут./кг U
	double			QlRange;			// допустимая мощность, Вт/см
};




class DPZ_DELAY
{// Учет запаздывания токов ДПЗ по ШИКАЛОВУ
private:
	double lam1;
	double lam2;
	double sig1;
	double sig2;
	double PP, QQ, mm1, mm2;
	double aa1, aa2, kk1, kk2, bb1, bb2, ee1, ee2;	//константы для алгоритма расчета по току (2)
	double yy, yy1, fff1;				//константы для алгоритма расчета по току с фильтрацией (2)
	double ddt;					//промежуток времени между измерениями токов

public:

	double Tok1,Tok2;				//токи ДПЗ
	double Flux;					//откорректированный ток ДПЗ
	double alf;						//величина мнгновенной компоненты
	double IPrev;					//предыдущее значение тока ДПЗ

//////////////////////////////////////////////////////////////////////////////////
	DPZ_DELAY();
	~DPZ_DELAY();

	void	Init(void);
	double	Bur_2_fl(double Q2, short T1, double h1, double gg1);//алгоритм расчета по току c фильтрацией
};


//////////////////////////////////////////////////////////////////////
// класс содержащий данные и производящий расчёты
class CProgCalc : public CObj
{
public:
	// входные данные файла Monitor
	int		MonRecord1[8];
	float	MonRecord2[TVS_CNT][L24_CNT];				// KV24
	float	MonRecord3[TVS_CNT][L24_CNT];
	float	MonRecord4[6]; // было 5 2017.06.29

	// входные данные файла Updater
	int		UpdRecord1[8];
	float	UpdRecord2[TVS_CNT][L24_CNT];				// KV24 duplex ??
	float	UpdRecord3[TVS_CNT][L24_CNT];
	float	UpdRecord4[TVS_CNT][PIN_CNT][L24_CNT];		// KK
	float	UpdRecord5[TVS_CNT][L24_CNT];				// послойное выгорание
	float	UpdRecord6[TVS_CNT][PIN_CNT];				// среднее выгорание ТВЭЛ/ТВЭГ
	float	UpdRecord7[TVS_CNT][L24_CNT];				// IOD24
	float	UpdRecord8[TVS_CNT][L24_CNT];				// XE24
	float	UpdRecord9[TVS_CNT][L24_CNT];				// PM24
	float	UpdRecord10[TVS_CNT][L24_CNT];				// SM24
	float	UpdRecord11[KNI_CNT][DPZ_CNT];				// SED
	float	UpdRecord12[11]; // было 7 2017.06.29

	CProg* 			pProg;				// указатель на шлюз
	CCalcConstTbl*	pConstTbl;			// таблица констант для расчета
	CCalcParamsTbl*	pParamsTbl;			// таблица полей и переменных для расчета

	float			Conv7x24Array[L07_CNT][L24_CNT];	// массив для преобразования 24 слойного поля в 7 слойное
	float			ConvKV7x24Array[L07_CNT][L24_CNT];	// массив для преобразования 24 слойного поля в 7 слойное (отдельно для KV)
	CNedQuotaStruct	NedQuotaArray[KNI_CNT][DPZ_CNT]; 	// массив для свертки поля KV24 для коррекции смешений ДПЗ
	CTvsFuelStruct	TvsFuelArray[TVS_CNT];				// массив типов топлива
	LST*			pTvelQlRangeLst;					// список лимитных кривых для ТВЭЛ
	LST*			pTvegQlRangeLst;					// список лимитных кривых для ТВЭГ

	double			LINEQ[TVS_CNT][PIN_CNT][L24_CNT];	// тепловые потоки с отрезков ТВЭЛ в пределах призмы - для расчета форм фактора по DNBR

	float			QVCAS[TVS_CNT];						// мощности ТВС

	// параметры для расчета скорректированных токов ДПЗ
	CTime			PrevCalcIEDCTime;			// время предыдущего расчета поля IEDC
	DPZ_DELAY		DpzDelay[KNI_CNT][DPZ_CNT];	// массив задержек токов ДПЗ


//	double			PrevIED[KNI_CNT][DPZ_CNT];	// предыдущие значения токов ДПЗ
//	double			IEDC_y[KNI_CNT][DPZ_CNT];
//	double			IEDC_k1[KNI_CNT][DPZ_CNT];
//	double			IEDC_k2[KNI_CNT][DPZ_CNT];


	CProgCalcParamInfo*	pIED;				// массив указателей на поле IED в ИБД (токи ДПЗ исходные)
	CProgCalcParamInfo*	pKVED;				// массив указателей на поле KVED в ИБД
	CProgCalcParamInfo*	pIEDC;				// массив указателей на поле IEDС в ИБД (токи ДПЗ с коррекцией)
	CProgCalcParamInfo*	pZE_SCAN;			// массив указателей на поле ZE в scan ИБД
	CProgCalcParamInfo*	pZE_REG; 			// массив указателей на поле ZE в reg ИБД
	CProgCalcParamInfo*	pQED;				// массив указателей на поле QED в ИБД
	CProgCalcParamInfo*	pRSED;				// массив указателей на поле SED в register ИБД
	CProgCalcParamInfo*	pSSED;				// массив указателей на поле SED в scan ИБД
	CProgCalcParamInfo*	pIOD;				// массив указателей на поле IOD в ИБД
	CProgCalcParamInfo*	pKV7L;				// массив указателей на поле KV7L в ИБД
	CProgCalcParamInfo*	pPM;				// массив указателей на поле PM в ИБД
	CProgCalcParamInfo*	pSM;				// массив указателей на поле SM в ИБД
	CProgCalcParamInfo*	pXE;				// массив указателей на поле XE в ИБД
	CProgCalcParamInfo*	pSL;				// массив указателей на поле SL в ИБД
	CProgCalcParamInfo*	pQVMF;				// массив указателей на поле QVMF в ИБД
	CProgCalcParamInfo*	pUsQVMF;			// массив указателей на поле usQVMF в ИБД
	CProgCalcParamInfo*	pDNBR;				// массив указателей на поле DNBR в ИБД
	CProgCalcParamInfo*	pKQ;				// массив указателей на поле KQ в ИБД
	CProgCalcParamInfo*	pQVCAS;				// массив указателей на поле KQ в ИБД
	CProgCalcParamInfo*	pDNBT;				// массив указателей на поле DNBT в ИБД
	CProgCalcParamInfo*	pSLCAS;				// массив указателей на поле SLCAS в ИБД
	CProgCalcParamInfo*	pSLTCAS;			// массив указателей на поле SLTCAS в ИБД
	CProgCalcParamInfo*	pSLNTCAS;			// массив указателей на поле SLNTCAS в ИБД
	CProgCalcParamInfo*	pKC;				// массив указателей на поле KC в ИБД
	CProgCalcParamInfo*	pOFSTCAS;			// массив указателей на поле OFSTCAS в ИБД
	CProgCalcParamInfo*	pYCTOUT;			// массив указателей на поле YCTOUT в ИБД
	CProgCalcParamInfo*	pYCTDT;				// массив указателей на поле YCTDT в ИБД
	CProgCalcParamInfo*	pYC00T_TP;			// массив указателей на поле YC00T_TP в ИБД
	CProgCalcParamInfo*	pYC00DT_TP;			// массив указателей на поле YC00DT_TP в ИБД
	CProgCalcParamInfo*	pSZ;				// массив указателей на поле SZ в ИБД

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


	IdbValue		PrevH08;		// предыдущие состояния положения групп на момент сброса их в BEACON
	IdbValue		PrevH09;
	IdbValue		PrevH10;
	IdbValue		PrevPOWER;


	CTime			PrevCalcZETime;		// время предыдущего расчета поля ZE
	double			PrevZE[KNI_CNT][DPZ_CNT];		// последние значения поля ZE
	char			stPrevIED[KNI_CNT][DPZ_CNT];	// последние статусы поля IED

	CTime			PrevCalcTeffTime;	// время предыдущего расчета TEFF
	double			PrevTEFF;			// последние значения поля TEFF

	bool			bStartAutoCalc;		// автоматический досчёт TEFF и ZE

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
