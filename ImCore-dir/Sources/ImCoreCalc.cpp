#include <ImCoreMain.h>

// ��������� ������
#define 	STPT_ON				0
#define 	STPT_OFF			1
#define 	STPT_UNDEF			3

void FillOffset(CProgParamInfo* pParamInfo)
{
	InputData*	pInData = NULL;
	CStr		CalcShortName(pParamInfo->CalcShortName);
	if(CalcShortName == "INPUT.SUZ")		pParamInfo->DataOffset = (int)&pInData->Suz[pParamInfo->Index1];
	else if(CalcShortName == "INPUT.IED")	pParamInfo->DataOffset = (int)&pInData->Ied[pParamInfo->Index1][pParamInfo->Index2];
	else if(CalcShortName == "INPUT.TP")	pParamInfo->DataOffset = (int)&pInData->Tp[pParamInfo->Index1];
	else if(CalcShortName == "INPUT.SUZG")	pParamInfo->DataOffset = (int)&pInData->SuzG[pParamInfo->Index1];
	else if(CalcShortName == "INPUT.NAKZ")	pParamInfo->DataOffset = (int)&pInData->Nakz;
	else if(CalcShortName == "INPUT.PAKZ")	pParamInfo->DataOffset = (int)&pInData->Pakz;
	else if(CalcShortName == "INPUT.THN")	pParamInfo->DataOffset = (int)&pInData->Thn[pParamInfo->Index1];
	else if(CalcShortName == "INPUT.YAF_S")	pParamInfo->DataOffset = (int)&pInData->Yaf_s;
	else if(CalcShortName == "INPUT.STPT")	pParamInfo->DataOffset = (int)&pInData->Stpt[pParamInfo->Index1];
//printf("%s Index1=%d, Index2=%d, Offset=%d\n", pParamInfo->CalcShortName, pParamInfo->Index1, pParamInfo->Index2, pParamInfo->DataOffset);
}


//////////////////////////////////////////////////////////////////////
//
double WaterSteamProperty::getWaterEnthalpy(double pressure, double temperature)
{// 3.19.1 ������ ��������� ���� �� ��������� �������� � �����������,
	// Enthalpy - ���/��
	// pressure - ���
	// temperature - ��.�

	double 	IndexArray[20][16] =
	{
		{0,		3,	4,	5,	6,	7,	8,	9,	10,	11,	12,	13,	14,	15,	16,	17},
		{180,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0},
		{190,	1,	1,	1,	1,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0},
		{200,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1},
		{210,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1},
		{220,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1},
		{230,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1},
		{240,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1},
		{250,	-1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1},
		{260,	-1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1},
		{270,	-1,	-1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1},
		{280,	-1,	-1,	-1,	2,	2,	2,	2,	1,	1,	1,	1,	1,	1,	1,	1},
		{290,	-1,	-1,	-1,	-1,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2},
		{300,	-1,	-1,	-1,	-1,	-1,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2},
		{310,	-1,	-1,	-1,	-1,	-1,	-1,	2,	2,	2,	2,	2,	2,	2,	2,	2},
		{320,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	3,	3,	3,	3,	3,	3,	3,	3},
		{330,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	3,	3,	3,	3,	3,	3},
		{340,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	4,	4,	4,	4,	4},
		{350,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	4,	4,	4},
		{360,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	4}
	};

	double	B[6][10] =
	{
		{0,				0,				0,				0,				0,				0,				0,				0,				0,				0},
		{4.189871e+002,	7.516658e-001,	4.218026e+002,	1.943512e-004,	6.760350e+000,	-2.477820e-001,	7.354649e-005,	-4.720858e-002,	-3.589461e-006, 3.630056e+000 },
		{4.089590e+002,	1.873655e-001,	4.563168e+002,	-5.632079e-003,	-3.387597e+001,	9.942254e-001,	4.811609e-003,	-7.602892e-001,	9.201130e-005,  2.032594e+001 },
		{6.962865e+001,	-1.300134e+001,	1.122978e+003,	-1.467056e-001,	-4.695918e+002,	1.775954e+001,	9.275094e-002,	-6.093478e+000,	-2.776575e-004, 1.151097e+002 },
		{-3.537755e+003,-7.086473e+001,	6.542013e+003,	-6.659196e-001,	-3.198507e+003,	7.938396e+001,	3.504944e-001,	-2.238377e+001,	-8.319714e-004,	5.754534e+002 },
		{-3.190392e+004,-7.275816e+002,	4.676686e+004,	-2.892199e+000,	-2.214788e+004,	6.725098e+002,	1.218665e+000,	-1.555131e+002,	4.578863e-003,  3.540542e+003 }
	};


	double 	p 	= pressure;
	double 	t 	= (temperature-100)*0.01;

	// ������ ������ �������
	int	row = 0;	int col = 0;
	for( ; row<20; row++)	if( temperature	<= IndexArray[row][0] )	break;
	for( ; col<16; col++)	if( pressure	<= IndexArray[0][col] )	break;

	int		i = 1 + IndexArray[row][col];

	return B[i][0] + B[i][1]*p + B[i][2]*t + B[i][3]*p*p + B[i][4]*t*t + B[i][5]*p*t + B[i][6]*t*p*p + B[i][7]*t*t*p + B[i][8]*p*p*p + B[i][9]*t*t*t;
}


double WaterSteamProperty::getWaterTemperature(double pressure, double enthalpy)
{// 3.19.2 ������ ����������� ���� �� ��������� �������� � ���������
 //	enthalpy - ��������� �  ���/��
 //	pressure - �������� (���)

	double B[3][10] =
	{
		{-6.078829e-002,	2.383560e+002,	-2.258354e-001,	6.283081e+000,	1.213277e-004,	5.979954e-002,	1.169905e-001,	-4.404609e-004,	-1.222070e+001,	-4.008193e-006},
		{2.738385e+001,		1.464506e+002,	7.838471e-001,	1.095507e+002,	6.651068e-003,	-2.075013e+000,	1.259500e+000,	-7.536731e-003,	-5.121273e+001,	1.260223e-005},
		{2.917282e+002,		-4.984173e+002,	1.064360e+001,	6.303373e+002,	7.888833e-002,	-1.717805e+001,	6.917002e+000,	-4.849208e-002,	-1.902250e+002,	-3.275220e-004}
	};
	double C[3] = {1.496798e+003, -3.748929e+000, 5.892857e-002};


	double	I = enthalpy/1000.0;
	double	p = pressure;
	int	  	i = 2;

	if( enthalpy < 990.2 + 0.2357*(p-3) ) 			i=0;
	else if( enthalpy < C[0] + C[1]*p + C[2]*p*p )	i=1;

	return B[i][0] + B[i][1] * I + B[i][2] * p + B[i][3] * I*I + B[i][4] * p*p + B[i][5] * p*I + B[i][6] * I*I*p + B[i][7] * I*p*p + B[i][8] * I*I*I + B[i][9] * p*p*p;
}

double WaterSteamProperty::getWaterDensity(double pressure, double temperature)
{// 3.19.3 ������ ��������� ���� �� ��������� �������� � �����������
	// density - �/�3
	// pressure - ���
	// temperature - ��.�

	double 	IndexArray[20][17] =
	{
		{0,		2,	3,	4,	5,	6,	7,	8,	9,	10,	11,	12,	13,	14,	15,	16,	17},
		{190,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0},
		{200,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	0,	0},
		{210,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1},
		{220,	2,	2,	2,	2,	2,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1},
		{230,	-1,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2},
		{240,	-1,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2},
		{250,	-1,	-1,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2},
		{260,	-1,	-1,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2},
		{270,	-1,	-1,	-1,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2},
		{280,	-1,	-1,	-1,	-1,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2},
		{290,	-1,	-1,	-1,	-1,	-1,	3,	3,	3,	3,	3,	3,	3,	2,	2,	2,	2},
		{300,	-1,	-1,	-1,	-1,	-1,	-1,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3},
		{310,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	3,	3,	3,	3,	3,	3,	3,	3,	3},
		{320,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	3,	3,	3,	3,	3,	3,	3,	3},
		{330,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	3,	3,	3,	3,	3,	3},
		{340,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	4,	4,	4,	4,	4},
		{350,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	4,	4,	4},
		{360,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	4}
	};
	double B[7][10] =
	{
		{0,				0,				0,				0,				0,				0,				0,				0,				0,				0},
		{9.584496e-001,	4.699430e-004,	-7.187481e-002,	-1.728091e-007,	-2.499884e-002,	1.417731e-004,	-1.265861e-006,	1.235399e-004,	-2.842992e-008,	2.746196e-003},
		{9.589838e-001,	3.128704e-004,	-7.366099e-002,	-4.205745e-005,	-2.324085e-002,	1.247682e-003,	4.661889e-005,	-8.253917e-004,	-2.958431e-007,	1.380274e-003},
		{9.921471e-001,	1.847344e-003,	-1.620672e-001,	1.179501e-005,	6.018831e-002,	-2.266342e-003,	-1.281064e-005,	1.236537e-003,	3.234230e-008,	-2.676232e-002},
		{1.796867e+000,	2.916249e-002,	-1.592217e+000,	2.742959e-004,	9.071298e-001,	-3.420583e-002,	-1.577909e-004,	1.053468e-002,	3.689857e-007,	-1.938434e-001},
		{2.856414e+001,	6.612431e-001,	-3.990309e+001,	3.080937e-003,	1.913875e+001,	-6.156810e-001,	-1.402400e-003,	1.441131e-001,	1.020221e-006,	-3.079259e+000},
		{9.983955e+002,	4.084095e-001,	-2.516245e+001,	-2.538698e-001,	-3.452552e+001,	9.090084e-001,	3.718426e-002,	-7.426022e-001,	1.340274e-001,	4.296386e+000}
	};


	// ������ ������ �������
	int	row = 0;	int col = 0;
	for( ; row<20; row++)	if( temperature	<= IndexArray[row][0] )	break;
	for( ; col<16; col++)	if( pressure	<= IndexArray[0][col] )	break;

	double	p = pressure;
	int		i = 1 + IndexArray[row][col];
	double	t = (temperature - 100)*0.01;
	//	t = (T - 100)*0.01 ��� i<=5
	//	t = (T - 20)*0.01 ��� i==6

	return  B[i][0] + B[i][1]*p + B[i][2]*t + B[i][3]*p*p + B[i][4]*t*t + B[i][5]*p*t + B[i][6]*t*p*p + B[i][7]*t*t*p + B[i][8]*p*p*p + B[i][9]*t*t*t;
}

double WaterSteamProperty::getSteamEnthalpyS(double pressureSatturation)
{// 3.19.5 ������ ��������� ���� �� ��������� ��������  �� ����� ���������
	// Enthalpy - ���/��
	// pressureSatturation - ���

	double A[2][6] =
	{
		{2.706619e+003, 1.173400e+002, -6.279778e+001, 1.892918e+001, -3.123276e+000, 2.128271e-001},
		{2.767375e+003, 2.904673e+001, -7.589837e+000, 7.698688e-001, -4.527300e-002, 1.095000e-003}
	};

	int		i = pressureSatturation<3.9 ? 0 : 1;
	double	p = pressureSatturation;

	return A[i][0] + A[i][1]*p + A[i][2]*p*p + A[i][3]*p*p*p + A[i][4]*p*p*p*p + A[i][5]*p*p*p*p*p;
}


double WaterSteamProperty::getWaterEnthalpyS(double pressureSatturation)
{// 3.19.6 ������ ��������� ���� �� ��������� �������� �� ����� ���������,
	// Enthalpy - ���/��
	// pressureSatturation - ���

	double B[6] = {7.108600e+002, 1.268899e+002, -1.050211e+001, 6.899313e-001, -2.517568e-002, 4.015837e-004};
	double p	= pressureSatturation;

	return B[0] + B[1]*p + B[2]*p*p + B[3]*p*p*p + B[4]*p*p*p*p + B[5]*p*p*p*p*p;
}


double WaterSteamProperty::getWaterBoilingHeatS(double pressureSatturation)
{// 3.19.7 ������ �������� ������� ���������������  �� ��������� ��������  �� ����� ���������

	// BoilingHeat - �������� ������� ��������������� � ���/��
	// pressureSatturation - ���


	double IndexArray[8][2] =
	{
		{5.4, 	0.0},
		{7.4,	1.0},
		{9.4,	2.0},
		{11.8,	3.0},
		{14.8,	4.0},
		{16.6,	5.0},
		{18.8,	6.0},
		{19.4,	7.0}
	};

	double C[8][3] =
	{
		{1.639937e+003,	-7.994286e+000,	2.285714e-001},
		{1.639546e+003,	-7.025322e+000,	1.477038e-002},
		{1.640985e+003,	-7.008886e+000,	1.156498e-002},
		{1.645045e+003,	-6.844691e+000,	5.749405e-003},
		{1.600123e+003,	-5.381950e+000,	-6.048669e-003},
		{1.487497e+003,	-2.988409e+000,	-1.875000e-002},
		{1.205720e+003,	 1.834403e+000,	-3.940435e-002},
		{-8.401905e+002, 3.114107e+001,	-1.443452e-001}
	};


	// ���������� ������
	int	   i = 0;
	for(int pos=0; pos<8; pos++ )
		if( pressureSatturation <= IndexArray[pos][0] )
			{ i = IndexArray[pos][1]; 	break; }

	double p = (pressureSatturation - 5)*10;

	return C[i][0] + C[i][1]*p + C[i][2]*p*p;
}

double WaterSteamProperty::getWaterTemperatureS(double pressureSatturation)
{// 3.19.10 ������ ����������� �� ��������� ��������  �� ����� ���������

	// pressureSatturation - ���
	// temperature - ��.�

	double B[6][6] =
	{
		{1.271650e+002, 6.729754e+001,	-1.723450e+001,	2.925688e+000,	-2.627797e-001,	9.487179e-003},
		{1.872902e+002,	1.988320e+001,	-1.024484e+000,	2.862995e-002,	-1.456876e-005,	-1.153846e-005},
		{6.768200e+001,	3.988330e+002,	-9.116101e+002,	1.325458e+003,	-1.000198e+003,	2.997436e+002}
	};

	double 	p = pressureSatturation;
	int		i = 1;
	if( 	 p < 1 ) i=2;
	else if( p < 8 ) i=1;

	return  B[i][0] + B[i][1]*p + B[i][2]*p*p + B[i][3]*p*p*p + B[i][4]*p*p*p*p + B[i][5]*p*p*p*p*p;
}




//////////////////////////////////////////////////////////////////////
//
CProgCalcParamInfo::CProgCalcParamInfo(CInfoDataBase* pIdb, long SizeX, long SizeY, float MultyCoef) : LONG_LST(SizeX*SizeY,0,"")
{
	CProgCalcParamInfo::pIdb 		= pIdb;
	CProgCalcParamInfo::SizeX 		= SizeX;
	CProgCalcParamInfo::SizeY 		= SizeY;
	CProgCalcParamInfo::MultyCoef 	= MultyCoef;

	SetCnt(SizeX*SizeY);
}
CProgCalcParamInfo::~CProgCalcParamInfo(void){}

CProgParamInfo*	CProgCalcParamInfo::GetParamInfo(int Pos)
{// Pos - ������ �� ����
	CProgParamInfo* pParamInfo = NULL;

	if( 	 ItemCnt==1 ) 				pParamInfo = (CProgParamInfo*)Get(1);
	else if( Pos>=0 && Pos<ItemCnt )	pParamInfo = (CProgParamInfo*)Get(Pos+1);

	return pParamInfo;
}

CProgParamInfo*	CProgCalcParamInfo::GetParamInfo(int PosX, int PosY)
{// PosX � PosY - ������ �� ����
	CProgParamInfo* 	pParamInfo 	= NULL;
	int				Pos			= PosX*SizeY + PosY;

	if( 	 ItemCnt==1 ) 				pParamInfo = (CProgParamInfo*)Get(1);
	else if( Pos>=0 && Pos<ItemCnt )	pParamInfo = (CProgParamInfo*)Get(Pos+1);

	return pParamInfo;
}

void	CProgCalcParamInfo::Read(float* pValue, char* pStatus)
{// �������� �������� �� ��� � pValue � � pStatus
	for( int PosX=0; PosX<SizeX; PosX++ )
	{
		for( int PosY=0; PosY<SizeY; PosY++ )
		{
			CProgParamInfo*	pBcnParamInfo = GetParamInfo(PosX,PosY);
			if(pBcnParamInfo )
			{
				if( ::IsAnalog(pBcnParamInfo->OutsideType) )
						*(pValue+PosX*SizeY+PosY) 	=  *(float*)pBcnParamInfo->Value / CProgCalcParamInfo::MultyCoef;
				else	*(pValue+PosX*SizeY+PosY) 	=  (float)*pBcnParamInfo->Value;

				*(pStatus+PosX*SizeY+PosY) 	=  *(pBcnParamInfo->Value+pBcnParamInfo->ValueByteSize);
			}
			else
			{
				*(pValue+PosX*SizeY+PosY) 	=  0.0;
				*(pStatus+PosX*SizeY+PosY) 	=  STATUS_UNDEF;
			}
		}
	}
}

void	CProgCalcParamInfo::ReadValue(float* pValue)
{// �������� �������� �� ��� � pValue
	for( int PosX=0; PosX<SizeX; PosX++ )
	{
		for( int PosY=0; PosY<SizeY; PosY++ )
		{
			CProgParamInfo*	pBcnParamInfo = GetParamInfo(PosX,PosY);
			if( pBcnParamInfo )
			{
				if( ::IsAnalog(pBcnParamInfo->OutsideType) )
					 *(pValue+PosX*SizeY+PosY) =  *(float*)pBcnParamInfo->Value/CProgCalcParamInfo::MultyCoef;
				else *(pValue+PosX*SizeY+PosY) = (float)*pBcnParamInfo->Value;
			}
		}
	}
}
void	CProgCalcParamInfo::ReadStatus(char* pStatus)
{// �������� �������� �� ��� � pValue
	for( int PosX=0; PosX<SizeX; PosX++ )
	{
		for( int PosY=0; PosY<SizeY; PosY++ )
		{
			CProgParamInfo*	pBcnParamInfo = GetParamInfo(PosX,PosY);
			*(pStatus+PosX*SizeY+PosY) =  pBcnParamInfo ? *(pBcnParamInfo->Value+pBcnParamInfo->ValueByteSize) : STATUS_UNDEF;
		}
	}
}
void	CProgCalcParamInfo::Write(float* pValue, char* pStatus, bool bAlwaysWrite)
{// �������� �������� �� pValue � pStatus � ���
	for( int PosX=0; PosX<SizeX; PosX++ )
	{
		for( int PosY=0; PosY<SizeY; PosY++ )
		{
			CProgParamInfo*	pBcnParamInfo = GetParamInfo(PosX,PosY);
			if(pBcnParamInfo )
			{
				if( ::IsAnalog(pBcnParamInfo->OutsideType) )
				{
					float	NewValue = *(pValue+PosX*SizeY+PosY)*CProgCalcParamInfo::MultyCoef;

//					if( bAlwaysWrite || *(float*)pBcnParamInfo->Value != NewValue )
					{
						*(float*)pBcnParamInfo->Value = NewValue;
						pBcnParamInfo->ChangeFlag |= CF_VALUE;
					}
				}
				else
				{
					char NewValue = (char)(int)*(pValue+PosX*SizeY+PosY);
//					if( bAlwaysWrite || *pBcnParamInfo->Value != NewValue )
					{
						*pBcnParamInfo->Value = NewValue;
						pBcnParamInfo->ChangeFlag |= CF_VALUE;
					}
				}

				if( bAlwaysWrite || *(char*)(pBcnParamInfo->Value+pBcnParamInfo->ValueByteSize) != *(pStatus+PosX*SizeY+PosY) )
				{
					*(char*)(pBcnParamInfo->Value+pBcnParamInfo->ValueByteSize) = *(pStatus+PosX*SizeY+PosY);
					pBcnParamInfo->ChangeFlag |= CF_STATUS;
				}
			}
		}
	}
}
void	CProgCalcParamInfo::WriteValue(float* pValue, char cStatus, bool bAlwaysWrite)
{// �������� �������� �� pValue � ���
	for( int PosX=0; PosX<SizeX; PosX++ )
	{
		for( int PosY=0; PosY<SizeY; PosY++ )
		{
			CProgParamInfo*	pBcnParamInfo = GetParamInfo(PosX,PosY);
			if(pBcnParamInfo )
			{
				if( ::IsAnalog(pBcnParamInfo->OutsideType) )
				{
					float	NewValue = *(pValue+PosX*SizeY+PosY)*CProgCalcParamInfo::MultyCoef;

					if( bAlwaysWrite || *(float*)pBcnParamInfo->Value != NewValue )
					{
						*(float*)pBcnParamInfo->Value = NewValue;
						pBcnParamInfo->ChangeFlag |= CF_VALUE;
					}

					if( bAlwaysWrite || *(char*)(pBcnParamInfo->Value+pBcnParamInfo->ValueByteSize) != cStatus )
					{// �������� ���������� ������ �� ���������
						*(char*)(pBcnParamInfo->Value+pBcnParamInfo->ValueByteSize) = cStatus;
						pBcnParamInfo->ChangeFlag |= CF_STATUS;
					}
				}
				else
				{
					char	NewValue = (char)(int)*(pValue+PosX*SizeY+PosY);

					if( *pBcnParamInfo->Value != NewValue )
					{
						*pBcnParamInfo->Value = NewValue;
						pBcnParamInfo->ChangeFlag |= CF_VALUE;
					}

					if( *(char*)(pBcnParamInfo->Value+pBcnParamInfo->ValueByteSize) != cStatus )
					{// �������� ���������� ������ �� ���������
						*(char*)(pBcnParamInfo->Value+pBcnParamInfo->ValueByteSize) = cStatus;
						pBcnParamInfo->ChangeFlag |= CF_STATUS;
					}
				}
			}
		}
	}
}
void	CProgCalcParamInfo::WriteStatus(char cStatus)
{// ������������� � ���� ���������� � ��� ���� ������
	for( int PosX=0; PosX<SizeX; PosX++ )
	{
		for( int PosY=0; PosY<SizeY; PosY++ )
		{
			CProgParamInfo*	pBcnParamInfo = GetParamInfo(PosX,PosY);
			if(pBcnParamInfo )
			{
				if( *(char*)(pBcnParamInfo->Value+pBcnParamInfo->ValueByteSize) != cStatus )
				{
					*(char*)(pBcnParamInfo->Value+pBcnParamInfo->ValueByteSize) = cStatus;
					pBcnParamInfo->ChangeFlag |= CF_STATUS;
				}
			}
		}
	}
}

void	CProgCalcParamInfo::WriteStatus(char* pStatus)
{
	for( int PosX=0; PosX<SizeX; PosX++ )
	{
		for( int PosY=0; PosY<SizeY; PosY++ )
		{
			CProgParamInfo*	pBcnParamInfo = GetParamInfo(PosX,PosY);
			if(pBcnParamInfo )
			{
				if( *(char*)(pBcnParamInfo->Value+pBcnParamInfo->ValueByteSize) != *(pStatus+PosX*SizeY+PosY) )
				{
					*(char*)(pBcnParamInfo->Value+pBcnParamInfo->ValueByteSize) = *(pStatus+PosX*SizeY+PosY);
					pBcnParamInfo->ChangeFlag |= CF_STATUS;
				}
			}
		}
	}
}





/*
//////////////////////////////////////////////////////////////////////
// ������� ��������
CCalcConstTbl::CCalcConstTbl(void) : CFileTbl("")
{
	CCalcConstTbl::pConstNameCol	= NULL;
	CCalcConstTbl::pConstValueCol	= NULL;
	CCalcConstTbl::pConstMUnitCol	= NULL;
	CCalcConstTbl::pFullNameCol		= NULL;
}
CCalcConstTbl::~CCalcConstTbl(){}

void CCalcConstTbl::MakeColumns()
{
	CTbl::MakeColumns();

	CCalcConstTbl::pConstNameCol 	= AddCol("ConstName", 	"", 				SQL_C_CHAR, 32, 90);
	CCalcConstTbl::pConstValueCol 	= AddCol("ConstValue", 	"", 				SQL_C_FLOAT, 0, 70);
	CCalcConstTbl::pConstMUnitCol 	= AddCol("ConstMUnit", 	"", 				SQL_C_CHAR, 16, 90);
	CCalcConstTbl::pFullNameCol		= AddCol("FullName", 	"",					SQL_C_CHAR, 100,250);
}

int		CCalcConstTbl::GetConstPos(const char* pConstName)
{
	int	RetValue = 0;

	if( pConstNameCol ) RetValue = pConstNameCol->FindStr(pConstName);

	return RetValue;
}
double	CCalcConstTbl::GetConstValue(const char* pConstName)
{
	double	ConstValue 	= -1;
	int 	ConstPos 	= GetConstPos(pConstName);
	if( 	ConstPos && pConstValueCol )
		ConstValue = pConstValueCol->GetNum(ConstPos);

	return ConstValue;
}


void	CCalcConstTbl::AppendConst(const char* pConstName, double ConstValue, const char* pStrMUnit, const char* pFullName )
{// ���������� �������
	int	ConstPos 	= GetConstPos((char*)pConstName);
	if( !ConstPos )
	{
		int	RowPos = AddRow();
		pConstNameCol->SetStr(RowPos, pConstName);
		pConstValueCol->SetNum(RowPos, ConstValue);
		pConstMUnitCol->SetStr(RowPos, pStrMUnit);
		pFullNameCol->SetStr(RowPos, pFullName);
	}
}



bool	CCalcConstTbl::Load(char* pFileName, CStr* pStrError)
{// ��������/������������� ������
	FileName = pFileName;
	SetRowCnt(0);

	CStr	Error;

// ��������! ������ ����� �������� �� ���������

	if(OpenFile(O_RDWR, &Error))
	{// �������� ������ �� �����
		if(CheckColumns(&Error))
			for(long RecordPos=1 ; RecordPos<=GetRecordCnt() ; RecordPos++)
				if( ReadRecord(RecordPos, 0, &Error) )
					CopyRow(AddRow(), 0 );

		CloseFile();
	}
	else Error.Empty();

	if( Error.GetLength() )
	{
		if( pStrError ) *pStrError = CStr(FileName) + " - " + Error;
		else 			puts(Error);
	}

	return !Error.GetLength() ? true : false;
}



////////////////////////////////////////////////////////////////////////////////////////////////////
// ������� ���������� ��� ������� (���������� � ����)
CCalcParamsTbl::CCalcParamsTbl(void) : CFileTbl("")
{
	CCalcParamsTbl::pIdbNameCol		= NULL;
	CCalcParamsTbl::pParamNameCol	= NULL;
	CCalcParamsTbl::pFullNameCol	= NULL;
	CCalcParamsTbl::pTemplateCol	= NULL;
	CCalcParamsTbl::pSizeXCol		= NULL;
	CCalcParamsTbl::pSizeYCol		= NULL;
	CCalcParamsTbl::pShiftXCol		= NULL;
	CCalcParamsTbl::pMultyCoefCol	= NULL;
}
CCalcParamsTbl::~CCalcParamsTbl(){}

void CCalcParamsTbl::MakeColumns()
{
	CTbl::MakeColumns();

	CCalcParamsTbl::pIdbNameCol		= AddCol("IdbName", 	"", 				SQL_C_CHAR,  6, 60);
	CCalcParamsTbl::pParamNameCol	= AddCol("ParamName", 	"", 				SQL_C_CHAR, 20, 90);
	CCalcParamsTbl::pFullNameCol	= AddCol("FullName", 	"", 				SQL_C_CHAR, 100, 150);
	CCalcParamsTbl::pTemplateCol	= AddCol("Template", 	"", 				SQL_C_CHAR, 16, 90);
	CCalcParamsTbl::pSizeXCol		= AddCol("SizeX", 		"", 				SQL_C_LONG,  0, 60);
	CCalcParamsTbl::pSizeYCol		= AddCol("SizeY", 		"", 				SQL_C_LONG,  0, 60);
	CCalcParamsTbl::pShiftXCol		= AddCol("ShiftX", 		"", 				SQL_C_LONG,  0, 60);
	CCalcParamsTbl::pMultyCoefCol	= AddCol("MultyCoef",	"", 				SQL_C_FLOAT, 0, 60);
}

int		CCalcParamsTbl::GetParamPos(const char* pParamName)
{
	int	RetValue = 0;

	if( pParamNameCol ) RetValue = pParamNameCol->FindStr(pParamName);

	return RetValue;
}
void	CCalcParamsTbl::AppendParam(const char* pIdbName, const char* pParamName, const char* pTemplate, int SizeX, int SizeY, float MultyCoef, int ShiftX, const char* pFullName)
{// ���������� �������
	int	FieldPos 	= GetParamPos((char*)pParamName);
	if( !FieldPos )
	{
		int	RowPos = AddRow();
		pIdbNameCol->SetStr(RowPos, pIdbName);
		pParamNameCol->SetStr(RowPos, pParamName);
		pFullNameCol->SetStr(RowPos, pFullName);
		pTemplateCol->SetStr(RowPos, pTemplate);
		pSizeXCol->SetNum(RowPos, SizeX);
		pSizeYCol->SetNum(RowPos, SizeY);
		pShiftXCol->SetNum(RowPos, ShiftX);
		pMultyCoefCol->SetNum(RowPos, MultyCoef);
	}
}

bool	CCalcParamsTbl::Load(char* pFileName, CStr* pStrError)
{// ��������/������������� ������
	FileName = pFileName;
	SetRowCnt(0);

	CStr	Error;

	if(OpenFile(O_RDWR, &Error))
	{// �������� ������ �� �����
		if(CheckColumns(&Error))
			for(long RecordPos=1 ; RecordPos<=GetRecordCnt() ; RecordPos++)
				if( ReadRecord(RecordPos, 0, &Error) )
					CopyRow(AddRow(), 0 );

		CloseFile();
	}
	else Error.Empty();

//puts(CStr("CCalcParamsTbl::Load Row=") + GetRowCnt() + " file=" + pFileName);

	if( Error.GetLength() )
	{
		if( pStrError ) *pStrError = CStr(FileName) + " - " + Error;
		else 			puts(Error);
	}

	{// ������� ������������ - ��������
		int ParamPos = 0;

		ParamPos = CCalcParamsTbl::GetParamPos("IOD"); 		if( ParamPos ) pMultyCoefCol->SetNum(ParamPos,1.0E-13);
		ParamPos = CCalcParamsTbl::GetParamPos("PM");  		if( ParamPos ) pMultyCoefCol->SetNum(ParamPos,1.0E-14);
		ParamPos = CCalcParamsTbl::GetParamPos("SM");  		if( ParamPos ) pMultyCoefCol->SetNum(ParamPos,1.0E-14);
		ParamPos = CCalcParamsTbl::GetParamPos("XE"); 		if( ParamPos ) pMultyCoefCol->SetNum(ParamPos,1.0E-13);
		ParamPos = CCalcParamsTbl::GetParamPos("SL"); 		if( ParamPos ) pMultyCoefCol->SetNum(ParamPos,1.0);
		ParamPos = CCalcParamsTbl::GetParamPos("SLCAS"); 	if( ParamPos ) pMultyCoefCol->SetNum(ParamPos,1.0);
	}

	return !Error.GetLength() ? true : false;
}
*/





//////////////////////////////////////////////////////////////////////
// ����� ���������� ������ � ������������ ���ޣ��
CProgCalc::CProgCalc(CProg* pProg) : pProg(pProg)
{
	CProgCalc::pConstTbl	= new CCalcConstTbl();
	CProgCalc::pConstTbl->MakeColumns();

	CProgCalc::pParamsTbl	= new CCalcParamsTbl();
	CProgCalc::pParamsTbl->MakeColumns();

	CProgCalc::pTvelQlRangeLst	= new LST(10,sizeof(CQlRangeStruct));
	CProgCalc::pTvegQlRangeLst	= new LST(10,sizeof(CQlRangeStruct));

	MemSet(MonRecord1,0,sizeof(MonRecord1));
	MemSet(MonRecord2,0,sizeof(MonRecord2));
	MemSet(MonRecord3,0,sizeof(MonRecord3));
	MemSet(MonRecord4,0,sizeof(MonRecord4));

	MemSet(UpdRecord1,0,sizeof(UpdRecord1));
	MemSet(UpdRecord2,0,sizeof(UpdRecord2));
	MemSet(UpdRecord3,0,sizeof(UpdRecord3));
	MemSet(UpdRecord4,0,sizeof(UpdRecord4));
	MemSet(UpdRecord5,0,sizeof(UpdRecord5));
	MemSet(UpdRecord6,0,sizeof(UpdRecord6));
	MemSet(UpdRecord7,0,sizeof(UpdRecord7));
	MemSet(UpdRecord8,0,sizeof(UpdRecord8));
	MemSet(UpdRecord9,0,sizeof(UpdRecord9));
	MemSet(UpdRecord10,0,sizeof(UpdRecord10));
	MemSet(UpdRecord11,0,sizeof(UpdRecord11));
	MemSet(UpdRecord12,0,sizeof(UpdRecord12));

	MemSet(Conv7x24Array,0,sizeof(Conv7x24Array));
	MemSet(ConvKV7x24Array,0,sizeof(ConvKV7x24Array));
	MemSet(NedQuotaArray,0,sizeof(NedQuotaArray));
	MemSet(TvsFuelArray,0,sizeof(TvsFuelArray));
	MemSet(QVCAS,0,sizeof(QVCAS));
	MemSet(PrevZE,0,sizeof(PrevZE));
	MemSet(stPrevIED,0,sizeof(stPrevIED));

	PrevTEFF	= 0.0;
	bStartAutoCalc	= true;

	pIED 	= NULL;
	pKVED	= NULL;
	pIEDC 	= NULL;
	pZE_SCAN= NULL;
	pZE_REG = NULL;
	pQED 	= NULL;
	pRSED	= NULL;
	pSSED	= NULL;
	pIOD	= NULL;
	pKV7L 	= NULL;
	pPM		= NULL;
	pSM		= NULL;
	pXE		= NULL;
	pSL		= NULL;
	pQVMF	= NULL;
	pUsQVMF	= NULL;
	pDNBR	= NULL;
	pKQ		= NULL;
	pQVCAS	= NULL;
	pDNBT	= NULL;
	pSLCAS	= NULL;
	pSLTCAS	= NULL;
	pSLNTCAS= NULL;
	pKC		= NULL;
	pOFSTCAS= NULL;
	pYCTOUT = NULL;
	pYCTDT  = NULL;
	pYC00T_TP	= NULL;
	pYC00DT_TP	= NULL;
	pSZ		= NULL;
}
CProgCalc::~CProgCalc()
{
	delete pConstTbl;
	delete pParamsTbl;
	delete pTvelQlRangeLst;
	delete pTvegQlRangeLst;


	if( pIED ) 		delete pIED;
	if( pKVED ) 	delete pKVED;
	if( pIEDC ) 	delete pIEDC;
	if( pZE_SCAN ) 	delete pZE_SCAN;
	if( pZE_REG ) 	delete pZE_REG;
	if( pQED ) 		delete pQED;
	if( pRSED ) 	delete pRSED;
	if( pSSED ) 	delete pSSED;
	if( pIOD ) 		delete pIOD;
	if( pKV7L ) 	delete pKV7L;
	if( pPM ) 		delete pPM;
	if( pSM ) 		delete pSM;
	if( pXE ) 		delete pXE;
	if( pSL )		delete pSL;
	if( pQVMF ) 	delete pQVMF;
	if( pUsQVMF ) 	delete pUsQVMF;
	if( pDNBR ) 	delete pDNBR;
	if( pKQ )		delete pKQ;
	if( pQVCAS )	delete pQVCAS;
	if( pDNBT )		delete pDNBT;
	if( pSLCAS )	delete pSLCAS;
	if( pSLTCAS )	delete pSLTCAS;
	if( pSLNTCAS )	delete pSLNTCAS;
	if( pKC )		delete pKC;
	if( pOFSTCAS )	delete pOFSTCAS;
	if( pYCTOUT )	delete pYCTOUT;
	if( pYCTDT )	delete pYCTDT;
	if( pYC00T_TP )	delete pYC00T_TP;
	if( pYC00DT_TP )delete pYC00DT_TP;
	if( pSZ )		delete pSZ;
}

bool	CProgCalc::LoadNedQuotaArray(char* pFileName, CStr*	pStrError)
{
	CStr	Error;


	CFileTbl*	pNedQuotaTbl = new CFileTbl(pFileName);

	if( pNedQuotaTbl->OpenFile(O_RDWR, &Error) )
	{
		pNedQuotaTbl->CreateColumnsFromFile();
		pNedQuotaTbl->ReadAllFile(&Error);

//int CpyCnt=0;
		for( int RowPos=1; RowPos<=pNedQuotaTbl->GetRowCnt(); RowPos++ )
		{
			CNedQuotaStruct	DpzQuota;

			DpzQuota.NumKni		= (int)pNedQuotaTbl->GetNum("NumKni",RowPos)-1;
			DpzQuota.NumDpz		= (int)pNedQuotaTbl->GetNum("NumDpz",RowPos)-1;
			DpzQuota.NumTvs		= (int)pNedQuotaTbl->GetNum("NumTvs",RowPos)-1;
			DpzQuota.NumPrLoDpz	= (int)pNedQuotaTbl->GetNum("NumPrLoDpz",RowPos);
			DpzQuota.CntPrDpz	= (int)pNedQuotaTbl->GetNum("CntPrDpz",RowPos);
			DpzQuota.QuotaPr1	= pNedQuotaTbl->GetNum("QuotaPr1",RowPos);
			DpzQuota.QuotaPr2	= pNedQuotaTbl->GetNum("QuotaPr2",RowPos);
			DpzQuota.QuotaPr3	= pNedQuotaTbl->GetNum("QuotaPr3",RowPos);

			if( DpzQuota.NumKni>=0 && DpzQuota.NumKni<KNI_CNT && DpzQuota.NumDpz>=0 && DpzQuota.NumDpz<DPZ_CNT )
			{
				MemCpy(&NedQuotaArray[DpzQuota.NumKni][DpzQuota.NumDpz],&DpzQuota,sizeof(DpzQuota));
//puts(CStr("LoadNedQuotaArray NumKni=") + DpzQuota.NumKni + " NumDpz=" + DpzQuota.NumDpz + " CpyCnt=" + (++CpyCnt));
			}
		}
	}
	delete pNedQuotaTbl;

	if( Error.GetLength() )
	{
		if( pStrError ) *pStrError = CStr(pFileName) + " - " + Error;
		else 			puts(Error);
	}

	return !Error.GetLength() ? true : false;
}


bool	CProgCalc::LoadTvsFuelArray(char* pDirName, CStr*	pStrError)
{
	CStr	Error;
	char*	pErrorFileName	= NULL;

	CFileTbl*	pFuelUranTbl 	= new CFileTbl(CStr(pDirName) + "fuel_uran.idb");
	CFileTbl*	pFuelTvsTbl 	= new CFileTbl(CStr(pDirName) + "fuel_tvs.idb");
	CFileTbl*	pFuelTvelTbl 	= new CFileTbl(CStr(pDirName) + "fuel_tvel.idb");
	CFileTbl*	pQlRangesTbl 	= new CFileTbl(CStr(pDirName) + "fuel_ql_range.idb");

	// ��������� �������
	pErrorFileName = pFuelUranTbl->FileName;
	if( !Error.GetLength() && pFuelUranTbl->OpenFile(O_RDWR, &Error) )
	{
		pFuelUranTbl->CreateColumnsFromFile();		pFuelUranTbl->ReadAllFile(&Error);
	}


	pErrorFileName = pFuelTvsTbl->FileName;
	if( !Error.GetLength() && pFuelTvsTbl->OpenFile(O_RDWR, &Error) )
	{
		pFuelTvsTbl->CreateColumnsFromFile();	pFuelTvsTbl->ReadAllFile(&Error);
	}

	pErrorFileName = pFuelTvelTbl->FileName;
	if( !Error.GetLength() && pFuelTvelTbl->OpenFile(O_RDWR, &Error) )
	{
		pFuelTvelTbl->CreateColumnsFromFile();	pFuelTvelTbl->ReadAllFile(&Error);
	}

	pErrorFileName = pQlRangesTbl->FileName;
	if( !Error.GetLength() && pQlRangesTbl->OpenFile(O_RDWR, &Error) )
	{
		pQlRangesTbl->CreateColumnsFromFile();	pQlRangesTbl->ReadAllFile(&Error);
	}


	if( !Error.GetLength() )
	{// ��������� ������ �������� ���

		for( int RowPos=1; RowPos<=pFuelTvsTbl->GetRowCnt(); RowPos++ )
		{
			int	NumTvs = (int)pFuelTvsTbl->GetNum("NTvs", RowPos)-1;
			if( NumTvs>=0 && NumTvs<TVS_CNT )
			{
				CTvsFuelStruct&  TvsFuel = TvsFuelArray[NumTvs];
				TvsFuel.NumTvs		= NumTvs;
				TvsFuel.FuelId		= (int)pFuelTvsTbl->GetNum("FID", RowPos);
				TvsFuel.ChanType	= 0;
				TvsFuel.TvegType	= -1;

				StrCpy(TvsFuel.FuelName, pFuelTvsTbl->GetStr("FSym", RowPos));

				int	FuelUranPos = pFuelUranTbl->FindNum("FID",TvsFuel.FuelId);
				if( FuelUranPos )
				{// �� ���� ������� ��������� ����� ��� ���� ��� ���� � ������� pFuelTvelTbl
					for( int RowPos=FuelUranPos; RowPos<=pFuelUranTbl->GetRowCnt(); RowPos++ )
					{
						if( (int)pFuelUranTbl->GetNum("FID",RowPos)==TvsFuel.FuelId )
						{
							if( (int)pFuelUranTbl->GetNum("Tveg",RowPos)==1 )
							{
								TvsFuel.TvegType = (int)pFuelUranTbl->GetNum("TvelType",RowPos);
								break;
							}
						}
						else break;
					}
				}


				int	FirstTvelPos = pFuelTvelTbl->FindNum("FID",TvsFuel.FuelId);

				if( FirstTvelPos )
				{// �� ���� ������� �������� ������� ������
					for( int RowPos=FirstTvelPos; RowPos<=pFuelTvelTbl->GetRowCnt(); RowPos++ )
					{
						if( (int)pFuelTvelTbl->GetNum("FID", RowPos)==TvsFuel.FuelId )
						{
							int	PinPos = (int)pFuelTvelTbl->GetNum("TvelPos", RowPos)-1;

							if( PinPos>=0 && PinPos<PIN_CNT )
							{
								TvsFuel.PinTypes[PinPos] = (int)pFuelTvelTbl->GetNum("TvelType", RowPos);
							}
						}
						else break;
					}
				}
			}
		}
	}


	if( !Error.GetLength() )
	{// �������� �������� ������ ��� ���� � ����
		for( int RowPos=1; RowPos<=pQlRangesTbl->GetRowCnt(); RowPos++ )
		{
			CQlRangeStruct	QlRange;
			QlRange.Tvel 	= (int)pQlRangesTbl->GetNum("Tvel",RowPos);
			QlRange.Tveg 	= (int)pQlRangesTbl->GetNum("Tveg",RowPos);
			QlRange.Sl 		= pQlRangesTbl->GetNum("Sl",RowPos);
			QlRange.QlRange	= pQlRangesTbl->GetNum("QlRange",RowPos);

			if( 	 QlRange.Tvel==1 ) pTvelQlRangeLst->AddPtr(&QlRange);
			else if( QlRange.Tveg==1 ) pTvegQlRangeLst->AddPtr(&QlRange);
		}
	}

	delete pFuelUranTbl;
	delete pFuelTvsTbl;
	delete pFuelTvelTbl;
	delete pQlRangesTbl;

	if( Error.GetLength() )
	{
		if( pStrError ) *pStrError = CStr(pErrorFileName) + " - " + Error;
		else 			puts(Error);
	}

	return !Error.GetLength() ? true : false;
}



bool	CProgCalc::LoadConv7x24Array(char* pFileName, CStr*	pStrError)
{
	CStr	Error;


	CFileTbl*	p24x7Tbl = new CFileTbl(pFileName);

	if( p24x7Tbl->OpenFile(O_RDWR, &Error) )
	{
		p24x7Tbl->CreateColumnsFromFile();
		p24x7Tbl->ReadAllFile(&Error);

		for( int i=0; i<L07_CNT; i++)
		{
			CCol*	pCol = p24x7Tbl->GetColByName(CStr("d")+(i+1));
			for( int j=0; pCol && j<L24_CNT; j++)
				Conv7x24Array[i][j] = pCol->GetNum(j+1);
		}
	}
	delete p24x7Tbl;


	if( Error.GetLength() )
	{
		if( pStrError ) *pStrError = CStr(pFileName) + " - " + Error;
		else 			puts(Error);
	}

	return !Error.GetLength() ? true : false;
}


bool	CProgCalc::LoadConvKV7x24Array(char* pFileName, CStr*	pStrError)
{
	CStr	Error;


	CFileTbl*	p24x7Tbl = new CFileTbl(pFileName);

	if( p24x7Tbl->OpenFile(O_RDWR, &Error) )
	{
		p24x7Tbl->CreateColumnsFromFile();
		p24x7Tbl->ReadAllFile(&Error);

		for( int i=0; i<L07_CNT; i++)
		{
			CCol*	pCol = p24x7Tbl->GetColByName(CStr("d")+(i+1));
			for( int j=0; pCol && j<L24_CNT; j++)
				ConvKV7x24Array[i][j] = pCol->GetNum(j+1);
		}
	}
	delete p24x7Tbl;


	if( Error.GetLength() )
	{
		if( pStrError ) *pStrError = CStr(pFileName) + " - " + Error;
		else 			puts(Error);
	}

	return !Error.GetLength() ? true : false;
}




void CProgCalc::Init(void)
{
	CStr	StrError;

	// ��������� ������� ��������� ��������
	if( !pConstTbl->Load(CStr(pProg->pSettings->CalcDataPath) + "/" + pProg->pSettings->FilePrefix + "ImCore_constants.idb", &StrError) )
		pProg->AddEvent(IDI_EVENT_ERROR, MakeError("Calc", "0001", CStr(StrError)));

	// ��������� ������� ����� ��������
	if( !pParamsTbl->Load(CStr(pProg->pSettings->CalcDataPath) + "/" + pProg->pSettings->FilePrefix + "ImCore_calc_params.idb", &StrError) )
		pProg->AddEvent(IDI_EVENT_ERROR, MakeError("Calc", "0002", CStr(StrError)));

	if( !LoadConv7x24Array(CStr(pProg->pSettings->CalcDataPath) + "/" + pProg->pSettings->FilePrefix + "M247.idb", &StrError) )
		pProg->AddEvent(IDI_EVENT_ERROR, MakeError("Calc", "0003", CStr(StrError)));


	if( !LoadConvKV7x24Array(CStr(pProg->pSettings->CalcDataPath) + "/" + pProg->pSettings->FilePrefix + "M247KV.idb", &StrError) )
		pProg->AddEvent(IDI_EVENT_ERROR, MakeError("Calc", "0003", CStr(StrError)));

	if( !LoadNedQuotaArray(CStr(pProg->pSettings->CalcDataPath) + "/" + pProg->pSettings->FilePrefix + "ned_quota.idb", &StrError) )
		pProg->AddEvent(IDI_EVENT_ERROR, MakeError("Calc", "0004", CStr(StrError)));

	if( !LoadTvsFuelArray(CStr(pProg->pSettings->CalcDataPath) + "/", &StrError) )
		pProg->AddEvent(IDI_EVENT_ERROR, MakeError("Calc", "0005", CStr(StrError)));

	if(::pProg->pRegIdb && ::pProg->pRegIdb->Lock())
	{
		int ParamCnt = ::pProg->pRegIdb->pParamInfoLst->GetCnt();
		for(int ParamPos=1 ; ParamPos<=ParamCnt ; ParamPos++)
		{
			CProgParamInfo* pParamInfo = (CProgParamInfo*)::pProg->pRegIdb->pParamInfoLst->Get(ParamPos);
			if(StrLen(pParamInfo->CalcShortName))
			{
				FillOffset(pParamInfo);
			}
		}
		::pProg->pRegIdb->Unlock();
	}

	if(::pProg->pScanIdb && ::pProg->pScanIdb->Lock())
	{
		int ParamCnt = ::pProg->pScanIdb->pParamInfoLst->GetCnt();
		for(int ParamPos=1 ; ParamPos<=ParamCnt ; ParamPos++)
		{
			CProgParamInfo* pParamInfo = (CProgParamInfo*)::pProg->pScanIdb->pParamInfoLst->Get(ParamPos);
			if(StrLen(pParamInfo->CalcShortName)) FillOffset(pParamInfo);
		}
		::pProg->pScanIdb->Unlock();
	}

	// �������� ������ �� ����
	pIED 	= MakeCalcParamInfo("IEDS");
	pKVED	= MakeCalcParamInfo("KVED");
	pIEDC 	= MakeCalcParamInfo("IEDC");
	pZE_SCAN= MakeCalcParamInfo("ZE_SCAN");
	pZE_REG = MakeCalcParamInfo("ZE_REG");
	pQED 	= MakeCalcParamInfo("QED");
	pRSED 	= MakeCalcParamInfo("SED");
	pSSED 	= MakeCalcParamInfo("SSED");
	pIOD 	= MakeCalcParamInfo("IOD");
	pKV7L 	= MakeCalcParamInfo("KV7L");
	pPM 	= MakeCalcParamInfo("PM");
	pSM 	= MakeCalcParamInfo("SM");
	pXE 	= MakeCalcParamInfo("XE");
	pQVMF 	= MakeCalcParamInfo("QVMF");
	pUsQVMF	= MakeCalcParamInfo("usQVMF");
	pDNBR 	= MakeCalcParamInfo("DNBR");
	pKQ 	= MakeCalcParamInfo("KQ");
	pQVCAS 	= MakeCalcParamInfo("QVCAS");
	pDNBT 	= MakeCalcParamInfo("DNBT");
	pSLCAS 	= MakeCalcParamInfo("SLCAS");
	pSLTCAS = MakeCalcParamInfo("SLTCAS");
	pSLNTCAS= MakeCalcParamInfo("SLNTCAS");
	pSL 	= MakeCalcParamInfo("SL");
	pKC 	= MakeCalcParamInfo("KC");
	pOFSTCAS= MakeCalcParamInfo("OFSTCAS");
	pYCTOUT = MakeCalcParamInfo("YCTOUT");
	pYCTDT 	= MakeCalcParamInfo("YCTDT");
	pYC00T_TP	= MakeCalcParamInfo("YC00T_TP");
	pYC00DT_TP	= MakeCalcParamInfo("YC00DT_TP");
	pSZ 	= MakeCalcParamInfo("SZ");
}

float* CProgCalc::Convert24To7(float (&p24)[TVS_CNT][L24_CNT], float (&p7)[TVS_CNT][L07_CNT])
{// ������� �������������� ���� 163�24 � ���� 163�7

	MemSet(p7,0,sizeof(p7));

	for(int TvsPos=0 ; TvsPos<TVS_CNT ; TvsPos++)
		for(int L07Pos=0 ; L07Pos<L07_CNT ; L07Pos++)
			for(int L24Pos=0 ; L24Pos<L24_CNT ; L24Pos++)
				p7[TvsPos][L07Pos] += p24[TvsPos][L24Pos]*Conv7x24Array[L07Pos][L24Pos];

	return (float*)p7;
}

float* CProgCalc::ConvertKV24To7(float (&p24)[TVS_CNT][L24_CNT], float (&p7)[TVS_CNT][L07_CNT])
{// ������� �������������� ���� 163�24 � ���� 163�7

	MemSet(p7,0,sizeof(p7));

	for(int TvsPos=0 ; TvsPos<TVS_CNT ; TvsPos++)
		for(int L07Pos=0 ; L07Pos<L07_CNT ; L07Pos++)
			for(int L24Pos=0 ; L24Pos<L24_CNT ; L24Pos++)
				p7[TvsPos][L07Pos] += p24[TvsPos][L24Pos]*ConvKV7x24Array[L07Pos][L24Pos];

	return (float*)p7;
}



void	CProgCalc::ReadFromIdb(const char* pCalcName, IdbValue* pIdbValue, bool bEnableMessage)
{
	ReadFromIdb(pCalcName, &pIdbValue->dValue, &pIdbValue->cStatus, bEnableMessage);
	pIdbValue->pCalcName = (char*)pCalcName; // ��� ���������� �������
//puts(CStr("CProgCalc::ReadFromIdb ") + pCalcName + " Value=" + pIdbValue->dValue);
}


void	CProgCalc::ReadFromIdb(const char* pCalcName, float* pValue, char* pStatus, bool bEnableMessage)
{
	CProgCalcParamInfo*	pCalcVar = MakeCalcParamInfo(pCalcName, bEnableMessage);
	pCalcVar->Read(pValue, pStatus);  delete pCalcVar;
}

void	CProgCalc::ReadFromIdb(const char* pCalcName, double* pValue, char* pStatus,bool bEnableMessage)
{
	float fValue = 0;
	ReadFromIdb(pCalcName,&fValue,pStatus, bEnableMessage);
	*pValue = fValue;
}


float	CProgCalc::ReadValueFromIdb(const char* pCalcName, bool bEnableMessage)
{
	CProgCalcParamInfo*	pCalcVar = MakeCalcParamInfo(pCalcName,bEnableMessage);
	float			RetValue = 0;

	pCalcVar->ReadValue(&RetValue);	delete pCalcVar;

	return RetValue;
}

char	CProgCalc::ReadStatusFromIdb(const char* pCalcName,bool bEnableMessage)
{
	CProgCalcParamInfo*	pCalcVar  = MakeCalcParamInfo(pCalcName,bEnableMessage);
	char			RetStatus = STATUS_UNDEF;

	pCalcVar->ReadStatus(&RetStatus);	delete pCalcVar;

	return RetStatus;
}
void	CProgCalc::WriteToIdb(const char* pCalcName, IdbValue* pIdbValue)
{
	WriteToIdb(pCalcName, pIdbValue->dValue, pIdbValue->cStatus);
}

void	CProgCalc::WriteToIdb(const char* pCalcName, float fValue, char cStatus)
{
	CProgCalcParamInfo*	pCalcVar = MakeCalcParamInfo(pCalcName);

	pCalcVar->Write(&fValue, &cStatus);  delete pCalcVar;
}
void	CProgCalc::WriteValueToIdb(const char* pCalcName, float fValue)
{
	CProgCalcParamInfo*	pCalcVar = MakeCalcParamInfo(pCalcName);

	pCalcVar->WriteValue(&fValue);  delete pCalcVar;
}
void	CProgCalc::WriteStatusToIdb(const char* pCalcName, char cStatus)
{
	CProgCalcParamInfo*	pCalcVar = MakeCalcParamInfo(pCalcName);

	pCalcVar->WriteStatus(&cStatus);  delete pCalcVar;
}
void	CProgCalc::WriteToIdb(const char* pCalcName, double dValue, char cStatus)
{
	float	fValue = (float)dValue;
	if( !isfinite(fValue) ){ fValue = 0.0; cStatus=STATUS_UNDEF; }
	WriteToIdb(pCalcName,fValue,cStatus);
}
void	CProgCalc::WriteValueToIdb(const char* pCalcName, double dValue)
{
	float	fValue = (float)dValue;
	if( !isfinite(fValue) ){ fValue = 0.0; }
	WriteValueToIdb(pCalcName,fValue);
}


char*  CProgCalc::MakeIdbName(char* pTemplate, char* pRetBuff, int X, int Y, int ShiftX)
{
	char	Mask[50];	Mask[0] = '\0';
	char*	XPtr 	= StrStr(pTemplate, "x");
	char*	YPtr 	= StrStr(pTemplate, "y");
	int		XLen 	= 0;	while(XPtr && *(XPtr + XLen) == 'x' ) XLen++;
	int		YLen 	= 0;	while(YPtr && *(YPtr + YLen) == 'y' ) YLen++;

	if(XPtr && YPtr && XPtr<YPtr)
	{// ���� � X � Y (X ������� �)
		// �������
		StrNCpy(pRetBuff, pTemplate, XPtr-pTemplate);
		// ������ ��� X
		sprintf(Mask, "%0*d", XLen, X+ShiftX);	strcat(pRetBuff, Mask);
		// ��������
		if(YPtr-XPtr-XLen){ StrNCpy(Mask, XPtr+XLen, YPtr-XPtr-XLen); strcat(pRetBuff, Mask); }
		// ������ ��� Y
		sprintf(Mask, "%0*d", YLen, Y);	strcat(pRetBuff, Mask);
		// ��������
		strcat(pRetBuff, YPtr+YLen);
	}
	else if(XPtr && YPtr && YPtr<XPtr)
	{// ���� � X � Y (� ������� X)
		StrNCpy(pRetBuff, pTemplate, YPtr-pTemplate);
		// ������ ��� Y
		sprintf(Mask, "%0*d", YLen, Y);	strcat(pRetBuff, Mask);
		// ��������
		if(XPtr-YPtr-YLen){ StrNCpy(Mask, YPtr+YLen, XPtr-YPtr-YLen); strcat(pRetBuff, Mask); }
		// ������ ��� X
		sprintf(Mask, "%0*d", XLen, X+ShiftX);	strcat(pRetBuff, Mask);
		// ��������
		strcat(pRetBuff, XPtr+XLen);
	}
	else if(XPtr)
	{// ������ X
		// �������
		StrNCpy(pRetBuff, pTemplate, XPtr-pTemplate);
		// ������ ��� X
		sprintf(Mask, "%0*d", XLen, X+ShiftX);	strcat(pRetBuff, Mask);
		// ��������
		strcat(pRetBuff, XPtr+XLen);
	}
	else if( YPtr )
	{// ������ Y
		// �������
		StrNCpy(pRetBuff, pTemplate, YPtr-pTemplate);
		// ������ ��� X
		sprintf(Mask, "%0*d", YLen, Y);	strcat(pRetBuff, Mask);
		// ��������
		strcat(pRetBuff, YPtr+YLen);
	}
	else
	{// ��� ���������
		StrCpy(pRetBuff, pTemplate);
	}

//puts(CStr("MakeIdbName ") + pTemplate + " x=" + X + " y=" + Y + " " + pRetBuff);
	return pRetBuff;
}

CProgParamInfo* CProgCalc::ConnectWithIdb(CInfoDataBase* pIdb, const char* pCalcName, char* pShortName, bool bEnableMesaage)
{
//puts(CStr("ConnectWithIdb pCalcName=") + pCalcName + " pShortName=" + pShortName);

	CProgParamInfo* pParamInfo = (CProgParamInfo*)pIdb->GetParamInfoByName(pShortName);
	if( !pParamInfo && bEnableMesaage )
	{
		pProg->AddEvent(IDI_EVENT_ERROR, MakeError("Calc", "0010", CStr("��� ") + pCalcName + " ��� ������������ � ��� �� " + pShortName));
//		puts(CStr("*** ") + (int)pIdb->GetParamInfoByName("XE00101"));
	}
	return pParamInfo;
}


CProgCalcParamInfo*	CProgCalc::MakeCalcParamInfo(const char* pCalcName, bool bEnableMesaage)
{
//printf("MakeCalcParamInfo %s\n", pCalcName);

	int 			ParamPos 		= pParamsTbl->GetParamPos(pCalcName);
	CProgCalcParamInfo*	pCalcParamInfo	= NULL;
	CStr			CalcName(pCalcName);

	if( ParamPos )
	{

//printf("1 %p\n", pParamsTbl->pIdbNameCol);
		char* 			pIdbName 	= pParamsTbl->pIdbNameCol->GetStr(ParamPos);
//printf("2 %p %s\n", pParamsTbl->pSizeXCol, pIdbName);
		int				SizeX		= (int)pParamsTbl->pSizeXCol->GetNum(ParamPos);
//printf("3 %p\n", pParamsTbl->pSizeYCol);
		int				SizeY		= (int)pParamsTbl->pSizeYCol->GetNum(ParamPos);
//printf("4 %p\n", pParamsTbl->pShiftXCol);
		int				ShiftX		= (int)pParamsTbl->pShiftXCol->GetNum(ParamPos);
//printf("5 %p\n", pParamsTbl->pTemplateCol);
		char*			pTemplate	= pParamsTbl->pTemplateCol->GetStr(ParamPos);
//printf("6 %p\n", pParamsTbl->pMultyCoefCol);
		float			MultyCoef	= pParamsTbl->pMultyCoefCol->GetNum(ParamPos);
		char			Buff[128]; 	MemSet(Buff,0,sizeof(Buff));
//printf("7 %p %s\n", pParamsTbl, pIdbName);

		CInfoDataBase* 	pIdb = !StrCmp(pIdbName,"Scan") ? 
				(CInfoDataBase*)pProg->pScanIdb :
				(CInfoDataBase*)pProg->pRegIdb;
//puts("2");
		if( SizeX>1 && SizeY>1 )
		{// ��������� ����
			pCalcParamInfo = new CProgCalcParamInfo(pIdb, SizeX, SizeY, MultyCoef);

//			CFileTbl*	pFileTbl 		= new CFileTbl(CStr(pProg->pSettings->IdbPath) + "/" + pProg->pSettings->FilePrefix + "calc/" + pCalcName + ".idb");
//			CCol*		pShortNameCol 	= pFileTbl->AddCol("ShortName", 	"", 	SQL_C_CHAR, 16, 90);
//			CCol*		pFullNameCol	= pFileTbl->AddCol("FullName", 		"",		SQL_C_CHAR, 100,250);

			for( int PosX=1; PosX<=SizeX; PosX++ )
				for( int PosY=1; PosY<=SizeY; PosY++ )
				{
/*
					if( SizeY==24 )
					{
						int  RowPos = pFileTbl->AddRow();
						char MaskX[10];		MaskX[0] = '\0'; sprintf(MaskX, "%0*d", 3, PosX);
						char MaskY[10];		MaskY[0] = '\0'; sprintf(MaskY, "%0*d", 2, PosY);
						pShortNameCol->SetStr(RowPos, MakeIdbName(pTemplate,Buff,PosX,PosY,ShiftX) );
						pFullNameCol->SetStr(RowPos, CStr("������� ���. �� ����� � ���=") + MaskX + " ��=" + MaskY);
					}
					else
*/

					pCalcParamInfo->Set(1 + (PosX-1)*SizeY + (PosY-1), (int)ConnectWithIdb(pIdb, pCalcName, MakeIdbName(pTemplate,Buff,PosX,PosY,ShiftX),bEnableMesaage));
				}

//			if( pFileTbl->GetRowCnt() )
//				pFileTbl->WriteAllFile();
		}
		else if( SizeX>1 )
		{// ���������� ����
			pCalcParamInfo = new CProgCalcParamInfo(pIdb, SizeX, 1, MultyCoef);

			for( int PosX=1; PosX<=SizeX; PosX++ )
			{
				pCalcParamInfo->Set(PosX, (int)ConnectWithIdb(pIdb, pCalcName, MakeIdbName(pTemplate,Buff,PosX,0,ShiftX),bEnableMesaage));
//if( CalcName == "SZ" )
//		puts( CStr("PosX=") + PosX + " IdbName=" + MakeIdbName(pTemplate,Buff,PosX,0,ShiftX) + " ShiftX=" + ShiftX);
			}
		}
		else
		{// ������ ���������� ���
			pCalcParamInfo = new CProgCalcParamInfo(pIdb, 1, 1, MultyCoef);
			pCalcParamInfo->Set(1, (int)ConnectWithIdb(pIdb, pCalcName, pTemplate,bEnableMesaage));
		}
		
	}
	else
	{// ��� ������������� ���� ������� ������ CProgCalcParamInfo;
		pCalcParamInfo = new CProgCalcParamInfo(NULL, 0, 0, 1.0);

		if( bEnableMesaage )
			pProg->AddEvent(IDI_EVENT_ERROR, MakeError("Calc", "0011", CStr("���������� ") + pCalcName + " ����������� � " + pParamsTbl->FileName));
	}
//printf("pCalcParamInfo=%p\n", pCalcParamInfo);
	return pCalcParamInfo;
}

void CProgCalc::Calculation(int ChangeDataFlag )
{
	CTime	StartTime = GetCurrentTime();

	CStr	ChangeDataName;

//	int		CalcMode = (int)ReadValueFromIdb("tvCalcMode");	// ������������� �������� (1 ���. ��� 15 ���.)

	if( pProg->GetFlag(FLG_UPDATE_CONSTANTS)==SV_NEEDED )
	{// 03.10.2018 ��������� ������� �� ���������� �������� �� constants.idb (�� ������ �����������)

		CStr FullFileName(CStr(pProg->pSettings->IdbPath) + "/" + pProg->pSettings->FilePrefix + "calc/constants.idb");

		if( FXFile::exists((char*)FullFileName) )
		{
			pConstTbl->SetRowCnt(0);
			CStr	StrError;

			if( pConstTbl->Load(FullFileName, &StrError) )
					pProg->AddEvent(IDI_EVENT_ERROR, MakeError("Calc", "0012", CStr("��������� ���������� �������� �� ����� ") + FullFileName));
			else	pProg->AddEvent(IDI_EVENT_ERROR, MakeError("Calc", "0001", CStr(StrError)));
		}
		else pProg->AddEvent(IDI_EVENT_ERROR, MakeError("Calc", "0013", CStr("���� ") + FullFileName + " �� ����������. ���������� �������� ��������."));


		pProg->SetFlag(FLG_UPDATE_CONSTANTS,SV_READY);
	}


	if( ChangeDataFlag==FLG_CHANGE_SCAN_DATA )
	{
		ChangeDataName = "SCAN_DATA";

		// ������ ���������������
		Calc_HH();


		// ���������� ������������ ����� ���
		Calc_IEDC();

		// ������ �������� �� ���
		if( pProg->GetFlag(FLG_CHANGE_REBUILD_DATA)!=SV_DISABLE &&
			pProg->GetFlag(FLG_CHANGE_DIF_DATA)!=SV_DISABLE )
		{
			Calc_NED();
		}

		// ������ ������� ��������� ����� ���
		Calc_ZE();

//puts("FLG_CHANGE_SCAN_DATA");

		if( pProg->pSettings->bEnableOFSTR_06 &&
			pProg->GetFlag(FLG_CHANGE_REBUILD_DATA)!=SV_DISABLE &&
			pProg->GetFlag(FLG_CHANGE_DIF_DATA)!=SV_DISABLE )
		{// ������ �O � ������� �����
			Calc_OFSTR();
		}
	}
	else if( ChangeDataFlag==FLG_CHANGE_REBUILD_DATA )
	{
		{// ������� �������� ��������� ����� �� ������ ������������ ����� (��� ������� OFSTR)
			ReadFromIdb("SZa08",	&PrevH08);
			ReadFromIdb("SZa09",	&PrevH09);
			ReadFromIdb("SZa10",	&PrevH10);
			ReadFromIdb("wNAKZ",	&PrevPOWER);

/*
			CTime CurTime = GetCurrentTime();
			puts(CStr("   "));
			puts(CStr("###") + &CurTime + " CProg::SendDataThread WritePrevValues");
			puts(CStr("      PrevH08=") + pProg->pCalc->PrevH08.v() + " PrevH09=" + pProg->pCalc->PrevH09.v() + " PrevH10=" + pProg->pCalc->PrevH10.v() + " PrevPOWER=" + pProg->pCalc->PrevPOWER.v());
*/
		}

		ChangeDataName = "CHANGE_REBUILD_DATA";

		// ���������� ���� � ������������ (��� �������������) 24x7
		WriteSimpleFieds(ChangeDataFlag);

//puts("FLG_CHANGE_BCN_MONITOR_FILE");

		// ������ �������������� �����
		Calc_AppendFields(ChangeDataFlag);

		// ������ ��2 �� ���. �� ����/����
		if( pProg->GetFlag(FLG_CHANGE_DIF_DATA)!=SV_DISABLE	)
			Calc_PZ2_QL(ChangeDataFlag);

		// ������ ��2 �� ������ �� ������� �������
		if( pProg->GetFlag(FLG_CHANGE_DIF_DATA)!=SV_DISABLE	)
			Calc_PZ2_DNBR(ChangeDataFlag);
	}
	else if( ChangeDataFlag==FLG_CHANGE_DIF_DATA )
	{
		ChangeDataName = "CHANGE_DIF_DATA";

		// ���������� ���� � ������������ (��� �������������) 24x7
		WriteSimpleFieds(ChangeDataFlag);

//puts("FLG_CHANGE_BCN_UPDATER_FILE");

		// ������ �������������� �����
		Calc_AppendFields(ChangeDataFlag);
	}


	if(  ChangeDataFlag!=FLG_CHANGE_SCAN_DATA )
	{
		CTime EndTime = GetCurrentTime();
		puts(CStr("CProgCalc::Calculation(") + ChangeDataName +") StartTime=" + &StartTime + " ����� �������=" + (int)((double)EndTime-(double)StartTime)+"msec");
	}

	// ������� ���� ������ ������� �����������
	pProg->SetFlag(ChangeDataFlag, SV_READY);
}

void CProgCalc::Calc_OFSTR()
{// ������ ������������ ������ � ������� ����� 0.6 ���.

	IdbValue	H08i;	ReadFromIdb("SZa08",	&H08i);		// ������� ��������� 8 ������
	IdbValue	H09i;	ReadFromIdb("SZa09",	&H09i);		// ������� ��������� 9 ������
	IdbValue	H10i;	ReadFromIdb("SZa10",	&H10i);		// ������� ��������� 10 ������
	IdbValue	POWERi;	ReadFromIdb("wNAKZ",	&POWERi);	// ������� �������� ��������


	IdbValue	KP;		ReadFromIdb("BE_KP",	&KP);	// ����������� ���������������� ��������
	IdbValue	KH08;	ReadFromIdb("BE_KH8",	&KH08);	// ����������� ���������������� ��������� 8 ������
	IdbValue	KH09;	ReadFromIdb("BE_KH9",	&KH09);	// ����������� ���������������� ��������� 9 ������
	IdbValue	KH10;	ReadFromIdb("BE_KH10",	&KH10);	// ����������� ���������������� ��������� 10 ������

	double		PrevOFSTR = MonRecord4[5];				// ��������� �������� �� monitor_why.write
	double		NominalPOWER = 3000.0;


	double		OFSTR = PrevOFSTR;
	OFSTR += KP.v()*(POWERi.v() - PrevPOWER.v())/NominalPOWER;
	OFSTR += KH08.v()*(H08i.v() - PrevH08.v());
	OFSTR += KH09.v()*(H09i.v() - PrevH09.v());
	OFSTR += KH10.v()*(H10i.v() - PrevH10.v());

/*
CTime CurTime = GetCurrentTime();
puts(CStr("   ") + &CurTime + " CProgCalc::Calc_OFSTR()");
puts(CStr("      KP=") + KP.v() + " POWERi=" + POWERi.v() + " PrevPOWER=" + PrevPOWER.v() );
puts(CStr("      KH08=") + KH08.v() + " H08i=" + H08i.v() + " PrevH08=" + PrevH08.v() );
puts(CStr("      KH09=") + KH09.v() + " H09i=" + H09i.v() + " PrevH09=" + PrevH09.v() );
puts(CStr("      KH10=") + KH10.v() + " H10i=" + H10i.v() + " PrevH10=" + PrevH10.v() );
puts(CStr("      PrevOFSTR=") + PrevOFSTR + "  OFSTR=" + OFSTR);
*/

	WriteToIdb("OFSTR", OFSTR, STATUS_NORMAL);
}


void CProgCalc::WriteSimpleFieds( int ChangeDataFlag )
{// ��������������� (��� �������������) ����� 24x7 � ������ � ���

	float	BuffArray7[TVS_CNT][L07_CNT];

	// ���������� ��� ����������� ��� �������
	double	Nakz	= 0.0;				// �������� �������� ��
	char	stNakz	= STATUS_NORMAL;	// ������������� �������� �������� ��


	// �������� �������� NAKZ � ������ �� Idb (�������� ���� �������������)
	ReadFromIdb("wNAKZ", &Nakz, &stNakz);

	if( ChangeDataFlag==FLG_CHANGE_REBUILD_DATA )
	{
		if( stNakz<STATUS_UNDEF )
		{
			pKV7L->WriteValue(ConvertKV24To7(MonRecord2, BuffArray7));	// KV24
			WriteToIdb("KV_UNC", MonRecord4[3], STATUS_NORMAL);
			WriteToIdb("OFSTR", MonRecord4[5], STATUS_NORMAL);
/*
			CTime CurTime = GetCurrentTime();
			puts(CStr("   "));
			puts(CStr("***") + &CurTime + " CProgCalc::WriteSimpleFieds WriteOFSTR");
			puts(CStr("      OFSTR=") + MonRecord4[5]);
*/
		}
		else
		{
			pKV7L->WriteStatus(STATUS_UNDEF);
			WriteStatusToIdb("KV_UNC", STATUS_UNDEF);
//			WriteStatusToIdb("OFSTR", STATUS_UNDEF);
		}
	}
	else if( ChangeDataFlag==FLG_CHANGE_DIF_DATA )
	{
		if( stNakz<STATUS_UNDEF && Nakz > pProg->pSettings->MinPowerDpzCalc )
		{// SED ����� ������ ���� �������� ����� 300   01.12.2017
			pRSED->WriteValue((float*)UpdRecord11,STATUS_NORMAL);	// SED ����. ���������������� ���
		}

		if( stNakz<STATUS_UNDEF )
		{
			WriteToIdb("CborK", UpdRecord12[0], STATUS_NORMAL);
			WriteToIdb("XEWH",  UpdRecord12[4], STATUS_NORMAL);
//			WriteToIdb("OFSTR", UpdRecord12[6], STATUS_NORMAL);
			WriteToIdb("BE_KP", UpdRecord12[7], STATUS_NORMAL);
			WriteToIdb("BE_KH8", UpdRecord12[10], STATUS_NORMAL);
			WriteToIdb("BE_KH9", UpdRecord12[9], STATUS_NORMAL);
			WriteToIdb("BE_KH10", UpdRecord12[8], STATUS_NORMAL);

/*
CTime CurTime = GetCurrentTime();
puts(CStr("   "));
puts(CStr("***") + &CurTime + " CProgCalc::WriteSimpleFieds WriteOFSTR");
puts(CStr("      BE_KP=") + UpdRecord12[7]);
puts(CStr("      BE_KH8=") + UpdRecord12[10]);
puts(CStr("      BE_KH9=") + UpdRecord12[9]);
puts(CStr("      BE_KH10=") + UpdRecord12[8]);
*/
			pSL->WriteValue(Convert24To7(UpdRecord5, BuffArray7));		// SL24
			pIOD->WriteValue(Convert24To7(UpdRecord7, BuffArray7));		// IOD24
			pXE->WriteValue(Convert24To7(UpdRecord8, BuffArray7));		// XE24
			pPM->WriteValue(Convert24To7(UpdRecord9, BuffArray7));		// PM24
			pSM->WriteValue(Convert24To7(UpdRecord10, BuffArray7));		// SM24
		}
		else
		{
			WriteStatusToIdb("CborK", STATUS_UNDEF);
			WriteStatusToIdb("XEWH",  STATUS_UNDEF);
//			WriteStatusToIdb("OFSTR", STATUS_UNDEF);
			WriteStatusToIdb("BE_KP", STATUS_UNDEF);
			WriteStatusToIdb("BE_KH8", STATUS_UNDEF);
			WriteStatusToIdb("BE_KH9", STATUS_UNDEF);
			WriteStatusToIdb("BE_KH10", STATUS_UNDEF);

			pSL->WriteStatus(STATUS_UNDEF);
			pIOD->WriteStatus(STATUS_UNDEF);
			pXE->WriteStatus(STATUS_UNDEF);
			pPM->WriteStatus(STATUS_UNDEF);
			pSM->WriteStatus(STATUS_UNDEF);
		}
	}
}




DPZ_DELAY::DPZ_DELAY()
{
	Init();
}

void	DPZ_DELAY::Init(void)
{
	lam1 = 0.016386;
	lam2 = 0.002662;
	sig1 = 139.0;
	sig2 = 11.0;
	alf  = 0.06;

	PP = lam1+lam2+lam1*sig1/alf/(sig1+sig2);
	QQ = lam1*lam2*(1.0f+alf)/alf;

	mm1 = -PP/2.0 + (double)sqrt(PP*PP/4.0 - QQ);
	mm2 = -PP/2.0 - (double)sqrt(PP*PP/4.0 - QQ);


	aa1 = mm2*(mm1+lam1)*(mm1+lam2)/lam1/lam2/(mm1-mm2);
	aa2 = mm1*(mm2+lam1)*(mm2+lam2)/lam1/lam2/(mm1-mm2);

	kk1 = 0;
	kk2 = 0;
	yy  = 1;
	yy1 = 1;

	Flux = 0;
	IPrev = 0;
}


//   ������� �������� ������ Flux �������������� �� ����������� Tok1 � �������� �������� ���� Tok2,
//   ff - ���������� ������ ������,
//   �1 - ��������� �������� ����� ����������� ���� � ����, h1 � gg1 - ��������� ����������

double DPZ_DELAY::Bur_2_fl(double Q2, short t1, double h1, double gg1)
{
	double ff = Flux;
	Tok1 = IPrev;
	Tok2 = Q2;			//���������������
	ddt  = t1*0.001;		//t1 - � ����
	bb1 = (double)exp(mm1*ddt/2)*aa1;
	bb2 = (double)exp(mm2*ddt/2)*aa2;

	ee1 = (double)exp(mm1*ddt);
	ee2 = (double)exp(mm2*ddt);

	kk1 = ee1*kk1+bb1*(Tok2-Tok1);
	kk2 = ee2*kk2+bb2*(Tok2-Tok1);
	fff1 = Tok2+kk1-kk2;
	if (ff == 0.0) ff = 1e-6;
	yy1  = fff1/ff;
	yy   = yy+gg1*(yy1-yy);
	Flux = ff*yy+h1*(fff1-ff*yy);
	IPrev = Q2;

	return Flux;
}
DPZ_DELAY::~DPZ_DELAY(){}


void CProgCalc::Calc_IEDC(void)
{// ���������� ������������ ����� ���

	CTime	CurTime = GetCurrentTime();
	float	IED[KNI_CNT][DPZ_CNT];				// ���� ���
	char	stIED[KNI_CNT][DPZ_CNT];			// ������� ����� ���

	float	IEDC[KNI_CNT][DPZ_CNT];				// ���� ��� � ����������
	char	stIEDC[KNI_CNT][DPZ_CNT];			// ������� ����� ��� � ����������

	double	IEDC_LO 						= pConstTbl->GetConstValue("IEDC_LO");
	double	IEDC_HI 						= pConstTbl->GetConstValue("IEDC_HI");
	int		KDQED_THRESHOLD_NED_CORRECTION 	= (int)pConstTbl->GetConstValue("KDQED_THRESHOLD_NED_CORRECTION"); // ����� ������� �������� � NED �� ���������� ����������� QED


	// �������� �������� � ������� ���� �� Idb (�������� ���� �������������)
	pIED->Read((float*)IED, (char*)stIED);
	pIED->Read((float*)IEDC, (char*)stIED);

	// ����� ����� ��������� � ����
	double dt = ((double)CurTime - (double)PrevCalcIEDCTime);

	// ���������� ����������� ����������������� �����
	int		DostIedcCnt	= 0;


	if( PrevCalcIEDCTime.IsNull() || dt<=0 || dt>5000 )
	{// � ������ ������� ��������� ������������� ���������� �������
	 // ��� ���� ��� ������ ������� ��� ����� �������� � ������ ������ ����� 5 ���

		for( int KniPos=0; KniPos<KNI_CNT; KniPos++ )
		{
			for( int DpzPos=0; DpzPos<DPZ_CNT; DpzPos++ )
			{// �������� �������� ���� ��� � �������� �������� �������������
				DpzDelay[KniPos][DpzPos].Init();
				DpzDelay[KniPos][DpzPos].Flux 	= IED[KniPos][DpzPos];
				DpzDelay[KniPos][DpzPos].IPrev	= IED[KniPos][DpzPos];
				IEDC[KniPos][DpzPos] 			= IED[KniPos][DpzPos];
				stIEDC[KniPos][DpzPos] 			= stIED[KniPos][DpzPos];
				stPrevIED[KniPos][DpzPos]		= stIED[KniPos][DpzPos];
			}
		}
	}
	else
	{
		for( int KniPos=0; KniPos<KNI_CNT; KniPos++ )
		{// ��������� ������ � ������������ ��������� �� �������������

			for( int DpzPos=0; DpzPos<DPZ_CNT; DpzPos++ )
			{
				double	Iedc	= IED[KniPos][DpzPos]; // ����� ��� ���
				char	stIedc	= STATUS_UNDEF;


				if( stPrevIED[KniPos][DpzPos]>=STATUS_UNDEF && stIED[KniPos][DpzPos]<STATUS_UNDEF )
				{// ��� �������� ���� � ����������� ��������� - ��������� �������������
					DpzDelay[KniPos][DpzPos].Init();
					DpzDelay[KniPos][DpzPos].Flux 	= IED[KniPos][DpzPos];
					DpzDelay[KniPos][DpzPos].IPrev	= IED[KniPos][DpzPos];
					stIedc 	= STATUS_NORMAL;
				}
				else if( stIED[KniPos][DpzPos] == STATUS_NORMAL )
				{// ��� ������������� ����� ��� - ��������� ������
					Iedc	= DpzDelay[KniPos][DpzPos].Bur_2_fl(Iedc,dt,0.95,0.01);
					stIedc 	= STATUS_NORMAL;
				}

				if(Iedc < IEDC_LO){ Iedc = IEDC_LO; stIedc = STATUS_UNDEF; }
				if(Iedc > IEDC_HI){ Iedc = IEDC_HI; stIedc = STATUS_UNDEF; }

				IEDC[KniPos][DpzPos]		= Iedc;
				stIEDC[KniPos][DpzPos] 		= stIedc;
				stPrevIED[KniPos][DpzPos]	= stIED[KniPos][DpzPos];

				if( stIedc == STATUS_NORMAL ) DostIedcCnt++;
			}
		}
	}

	if( !PrevCalcIEDCTime.IsNull() && DostIedcCnt < KDQED_THRESHOLD_NED_CORRECTION )
	{// ���� ���� ����� ����������� ����������������� ����� ��� - ���������� ���������� ���������� ������������
	 // 27.10.2017 �� �� ���������� ����� IEDC ����� ������������ � ������ ����� �� 40 ��� �� ���� ������� ����������� NED (>7000���)
	 // ���� ������ �������� ��������������� NED �� ����� ��� ��������� �������������.
		PrevCalcIEDCTime = 0;

		Calc_IEDC();
	}
	else
	{
		// ������� ���� IEDC reg ���
		pIEDC->Write((float*)IEDC, (char*)stIEDC);


		PrevCalcIEDCTime = CurTime;
	}
}


void CProgCalc::Calc_ZE(void)
{// ������ ������� ��������� ����� ���

	CTime	CurTime = GetCurrentTime();
	float	IED[KNI_CNT][DPZ_CNT];				// ���� ���
	char	stIED[KNI_CNT][DPZ_CNT];			// ������� ����� ���

	float	ZE[KNI_CNT][DPZ_CNT];				// ��������� ������
	char	stZE[KNI_CNT][DPZ_CNT];				// ��������� �������


	// ***** IED or IEDC
//	if(::pProg->pSettings->bUseIEDC)
//	{// �������� �������� � ������� ���� IED� (� ����������) �� Idb
//	    pIEDC->Read((float*)IED, (char*)stIED);
//	}
//	else
	{// �������� �������� � ������� ���� �� Idb (�������� ���� �������������)
		pIED->Read((float*)IED, (char*)stIED);
	}

	if( PrevCalcZETime.IsNull() )
	{// � ������ ������� ���� �������� ���� ������� � double � ����� ������������ ������ ��
		float TimeTMValue	= ReadValueFromIdb("TEFF_TM_SCAN");
		PrevCalcZETime.Sec	= *(int*)&TimeTMValue;

		CTime CurTime; CurTime.SetCurrentTime();
		double dTimeSec = CurTime.Sec - *(int*)&TimeTMValue;

		pZE_SCAN->ReadValue((float*)ZE);
		for( int KniPos=0; KniPos<KNI_CNT; KniPos++ )
			for( int DpzPos=0; DpzPos<DPZ_CNT; DpzPos++ )
				PrevZE[KniPos][DpzPos] = ZE[KniPos][DpzPos] + (bStartAutoCalc ? ((double)IED[KniPos][DpzPos]*1.E-6 * dTimeSec) : 0 );
	}
	else
	{
		char  	stNakz	= STATUS_NORMAL;
		double	Nakz	= 0.0;	ReadFromIdb("wNAKZ", &Nakz, &stNakz);

		for( int KniPos=0; KniPos<KNI_CNT; KniPos++ )
		{// ��������� ������ � ������������ ��������� �� �������������

			for( int DpzPos=0; DpzPos<DPZ_CNT; DpzPos++ )
			{
				double	Ze		= PrevZE[KniPos][DpzPos];
				char	stZe	= STATUS_NORMAL;//STATUS_UNDEF;

				if(stIED[KniPos][DpzPos]==STATUS_NORMAL && IED[KniPos][DpzPos]>=0.01 && stNakz==STATUS_NORMAL && Nakz>=pProg->pSettings->MinPowerCalc )
				{// ��� ������������� ����� ���
					Ze += (double)IED[KniPos][DpzPos]*1.E-6 * ((double)CurTime - (double)PrevCalcZETime)/1000.0;
					stZe = STATUS_NORMAL;
				}

				PrevZE[KniPos][DpzPos] 	= Ze;
				ZE[KniPos][DpzPos]	 	= Ze;
				stZE[KniPos][DpzPos] 	= stZe;
			}
		}

		// ������� ���� ZE � scan � � reg ���
		//pZE_SCAN->Write((float*)ZE, (char*)stZE);
		pZE_REG->Write((float*)ZE, (char*)stZE);
	}
	PrevCalcZETime = CurTime;
}



void CProgCalc::Calc_NED(void)
{// 3.12.6 ������ �������� �������� ��� �� ���������� ����������� ��������

//	MinPowerDpzCalc

	float	(&SED)[KNI_CNT][DPZ_CNT] = UpdRecord11;	// ����. ���������������� ���
	float	IED[KNI_CNT][DPZ_CNT];					// ���� ���
	char	stIED[KNI_CNT][DPZ_CNT];				// ������� ����� ���
	float	QED[KNI_CNT][DPZ_CNT];					// ��������������� � ������ ������������ ���
	char	stQED[KNI_CNT][DPZ_CNT];				// ������� QED
	float	KVED[KNI_CNT][DPZ_CNT];					// KV ����������� �� ���� QED

	double	Mode 	= 0.0;	 char  stMode  	= STATUS_UNDEF;	ReadFromIdb("YC00MODE",	&Mode, 	&stMode);

	int		dostQedCnt		= 0;					// ���������� ����������� QED � ������ ������������ ���
	double	sumDostQed		= 0.0;					// ����� ���� ����������� QED
	double	Ned				= 0.0;					// �������� �� ���
	char	stNed			= STATUS_UNDEF;			// ������������� �������� �� ���
	double	Nakz			= 0.0;					// �������� �������� ��
	char	stNakz			= STATUS_NORMAL;		// ������������� �������� �������� ��

	double	Ofed			= 0.0;					// ����� �� ���
	double	stOfed			= STATUS_NORMAL;

	double	sumQedL			= 0.0;					// ��������� �� ���� ���
	double	sumQedH			= 0.0;					// ��������� �� ����� ���

	// ��������� ������������ � �������
	double	QED_VVG 						= pConstTbl->GetConstValue("QED_VVG");	// ������� ������� �������� QED
	double	QED_NVG 						= pConstTbl->GetConstValue("QED_NVG");	// ������ ������� �������� QED
	double	HFUEL	 						= pConstTbl->GetConstValue("HFUEL");	// ����� ���������� ������ � �������� ����
	double	MNED 							= pConstTbl->GetConstValue("MNED");		// ���������� ��������� ��� NED
	double	NAKZ_THRESHOLD_NED_CORRECTION 	= pConstTbl->GetConstValue("NAKZ_THRESHOLD_NED_CORRECTION"); 	// ����� ������� �������� � NED �� N���
	double	KDQED_THRESHOLD_NED_CORRECTION 	= pConstTbl->GetConstValue("KDQED_THRESHOLD_NED_CORRECTION");	// ����� ������� �������� � NED �� ���������� ����������� QED
	double	SED_HI							= pConstTbl->GetConstValue("SED_HI");	// ������� ������ ���������� ������������� ����������������

	if(SED_HI<=0.01 )	SED_HI = 10;


	// ***** IED or IEDC
	if(::pProg->pSettings->bUseIEDC)
	{// �������� �������� � ������� ���� IED� (� ����������) �� Idb
	    pIEDC->Read((float*)IED, (char*)stIED);
	}
	else
	{// �������� �������� � ������� ���� �� Idb (�������� ���� �������������)
	    pIED->Read((float*)IED, (char*)stIED);
	}


	// �������� �������� NAKZ � ������ �� Idb (�������� ���� �������������)
	ReadFromIdb("wNAKZ", &Nakz, &stNakz);


//	01.12.2017 ��� �������� ����� 300 ����� ����������� ���������� ������������ �� ��� � ������ UpdRecord11 (�� ��������)
	if( stNakz<STATUS_UNDEF && Nakz < pProg->pSettings->MinPowerDpzCalc )
	{// ���� �������� ���� - �� ����� SED �� ScanIdb � ���������� � UpdRecord11
		pSSED->ReadValue((float*)UpdRecord11);
	}

	{// ������ ��������� ������

		for( int KniPos=0; KniPos<KNI_CNT; KniPos++ )
		{// ��������� QED � ������������ ��������� �� �������������

			for( int DpzPos=0; DpzPos<DPZ_CNT; DpzPos++ )
			{
				double	Sed		= SED[KniPos][DpzPos];
				double	Qed		= IED[KniPos][DpzPos] * Sed;
				char	stQed	= STATUS_NORMAL;

//puts(CStr("KniPos=") + KniPos + " DpzPos=" + DpzPos + " IED=" + IED[KniPos][DpzPos] + " SED=" + SED[KniPos][DpzPos] + " stIED=" + (int)stIED[KniPos][DpzPos]);
				if( stIED[KniPos][DpzPos] == STATUS_NORMAL && Qed > QED_NVG && Qed < QED_VVG && Sed<SED_HI )
				{// ���������� ������ ����������� QED

					dostQedCnt += 1;	// ����������� ���������� ����������� QED
					sumDostQed += Qed;	// ����������� ����� ����������� QED

					if( 	 DpzPos  < 3 )	sumQedL += Qed;
					else if( DpzPos == 3 ){ sumQedL += 0.5*Qed; sumQedH += 0.5*Qed; }
					else					sumQedH += Qed;

				}
				else
				{// ������� ���� QED
					stQed = STATUS_UNDEF;
				}

				QED[KniPos][DpzPos] 	= Qed;
				stQED[KniPos][DpzPos] 	= stQed;
				KVED[KniPos][DpzPos] 	= Qed / (Nakz / (TVS_CNT*HFUEL));
			}
		}

		// ��������� �������� NED ��� ��������� ��������
		if( dostQedCnt > KDQED_THRESHOLD_NED_CORRECTION )
			Ned = (sumDostQed/dostQedCnt) * HFUEL * TVS_CNT * MNED;

//puts(CStr("0 CProgCalc::Calc_NED Nakz=") + Nakz + " Ned=" + Ned + " dostQedCnt=" + dostQedCnt + " sumDostQed=" + sumDostQed);

		if( stNakz!=STATUS_UNDEF && Nakz > NAKZ_THRESHOLD_NED_CORRECTION && dostQedCnt > KDQED_THRESHOLD_NED_CORRECTION )
		{// ������������ �������� NED
			float	(&KV24)[TVS_CNT][L24_CNT] 	= MonRecord2;	// KV24
			double	sumDostKvDpz				= 0.0;

			for( int KniPos=0; KniPos<KNI_CNT; KniPos++ )
			{// ������ ������� ���� KV24 � ������ ����� ����� � ������������ ������� ��������� KVDPZ

				for( int DpzPos=0; DpzPos<DPZ_CNT; DpzPos++ )

				{
					if( stQED[KniPos][DpzPos]==STATUS_NORMAL )
					{// ������ ��� ����������� ���
						CNedQuotaStruct* pDpzQuota = &NedQuotaArray[KniPos][DpzPos];

						for( int PrizmPos=0; PrizmPos < pDpzQuota->CntPrDpz; PrizmPos++ )
						{
							if(		 PrizmPos==0 ) sumDostKvDpz += pDpzQuota->QuotaPr1 * KV24[pDpzQuota->NumTvs][pDpzQuota->NumPrLoDpz-1+PrizmPos];
							else if( PrizmPos==1 ) sumDostKvDpz += pDpzQuota->QuotaPr2 * KV24[pDpzQuota->NumTvs][pDpzQuota->NumPrLoDpz-1+PrizmPos];
							else if( PrizmPos==2 ) sumDostKvDpz += pDpzQuota->QuotaPr3 * KV24[pDpzQuota->NumTvs][pDpzQuota->NumPrLoDpz-1+PrizmPos];
						}
					}
				}
			}


//puts(CStr("1 CProgCalc::Calc_NED Nakz=") + Nakz + " Ned=" + Ned + " sumDostKvDpz=" + sumDostKvDpz + " k=" + (sumDostKvDpz/dostQedCnt));

			if( sumDostKvDpz!=0 )
			{// ��������� ������������� NED �� ������� KV � ������ ������������ ����������� ���
				Ned /= (sumDostKvDpz/dostQedCnt);
			}
		}
		stNed = STATUS_NORMAL;
	}

printf("NED = %g stMOde = %d, Mode = %g\n", Ned, stMode, Mode);
	if( stMode<=STATUS_UNDEF && Mode>3 )
	{// ���� �� �� �������� �� Ned = 0
		Ned = 0.0;	stNed = STATUS_NORMAL;
	}
	else if( Nakz < pProg->pSettings->MinPowerDpzCalc ) // 27.07.2017
	{// ���� �������� ����� 300 ���
		Ned = 0.0;	stNed = STATUS_UNDEF;
	}


//puts(CStr("2 CProgCalc::Calc_NED Nakz=") + Nakz + " Ned=" + Ned + " stNakz=" + (int)stNakz);

	// ������� ����� �� ���
	if( (sumQedH + sumQedL) > 0 )
		Ofed = 100.0 * (sumQedH - sumQedL) / (sumQedH + sumQedL) ;
	else stOfed = STATUS_UNDEF;

	// ������� � ��� ����� �� ���
	WriteToIdb("OFED", Ofed, stOfed);

	// ������� � ��� �������� �� ���
//	if( Round(Ned,0) > 0.0 )  01.12.2017 �.�. �� ����������� 0 � ��� (�� ��������)
	{
		WriteToIdb("RNED", Ned, stNed);
//		WriteToIdb("SNED", Ned, stNed);
	}
//	else pProg->AddEvent(IDI_EVENT_ERROR, MakeError("CALCULATIONS", NULL,"NED=0"));

	// ������� ���� QED � ���
	pQED->Write((float*)QED, (char*)stQED);

	// ������� ���� KVED � ���
	pKVED->Write((float*)KVED, (char*)stQED);

	// ����������� �������� ���������� (��� �������)
	WriteToIdb("TST_REG", ReadValueFromIdb("TST_SCAN"), STATUS_NORMAL);

//CTime	CurTIme = GetCurrentTime();
//puts(CStr("CProgCalc::Calc_NED ") + &CurTIme + " TST_GKSO_OUT_A=" + ReadValueFromIdb("TST_SCAN") + " TST_BEACON=" + ReadValueFromIdb("TST_REG"));
}

/*
void 	CProgCalc::Calc_NAKZ(void)
{// ������ �������� ��������

	double	Nakz 	= 0;	char  stNakz = STATUS_UNDEF;


	double	N1k		= 0.0;	char  stN1k  	= STATUS_UNDEF;	ReadFromIdb("sN1k", 	&N1k, 	&stN1k);
	double	N2k		= 0.0;	char  stN2k  	= STATUS_UNDEF;	ReadFromIdb("sN2k", 	&N2k, 	&stN2k);
	double	Nopvd	= 0.0;	char  stNopvd	= STATUS_UNDEF;	ReadFromIdb("sNOpvd",	&Nopvd, &stNopvd);
	double	Nik		= 0.0;	char  stNik 	= STATUS_UNDEF;	ReadFromIdb("sNik", 	&Nik, 	&stNik);
	double	Ned 	= 0.0;	char  stNed  	= STATUS_UNDEF;	ReadFromIdb("SNED",		&Ned, 	&stNed);
	double	Mode 	= 0.0;	char  stMode  	= STATUS_UNDEF;	ReadFromIdb("YC00MODE",	&Mode, 	&stMode);

//	double	ADTpt 	= 0.0;	char  stADTpt  	= STATUS_UNDEF;	ReadFromIdb("ADTpt",	&ADTpt,	&stADTpt);
//	double	GM2k 	= 0.0;	char  stGM2k  	= STATUS_UNDEF;	ReadFromIdb("GM2k",		&GM2k,	&stGM2k);
//	double	GMpvd1 	= 0.0;	char  stGMpvd1 	= STATUS_UNDEF;	ReadFromIdb("sGMpvd1",	&GMpvd1,&stGMpvd1);
//	double	GMpvd2 	= 0.0;	char  stGMpvd2 	= STATUS_UNDEF;	ReadFromIdb("sGMpvd2",	&GMpvd2,&stGMpvd2);


	double	k1 		= pConstTbl->GetConstValue("NAKZ_K1");
	double	k2 		= pConstTbl->GetConstValue("NAKZ_K2");
	double	k3 		= pConstTbl->GetConstValue("NAKZ_K3");
	double	k4 		= pConstTbl->GetConstValue("NAKZ_K4");
	double	k5 		= pConstTbl->GetConstValue("NAKZ_K5");
	double	DN1		= pConstTbl->GetConstValue("NAKZ_DN1");
	double	DN2		= pConstTbl->GetConstValue("NAKZ_DN2");
	double	N0		= pConstTbl->GetConstValue("NAKZ_N0");

	if( Mode==0 || stMode>=STATUS_UNDEF )
	{// ���� ����� ������ �������� �����������
		Nakz = 0; stNakz = STATUS_UNDEF;
	}
	else if( Mode>3 )
	{// ���� � ������ ����� ���� ������
		Nakz = 0; stNakz = STATUS_NORMAL;
	}
	else
	{
		stNakz = STATUS_NORMAL;

		// ������ �� ���
//		double	GMpvd 	= GMpvd1 + GMpvd2;

		// ����������� ������� �������� ������������� � ��������, ����.�
//		double	ADTpt_0 = (RMode==2) ?  ADTpt_2 : ( (RMode==3) ? ADTpt_3 : ADTpt_4 );

		Nakz  = k1*Nik + k2*Ned + k3*N1k + k4*N2k + k5*Nopvd;

		if( Nakz ) Nakz *= pow(k1 + k2 + k3 + k4 + k5,-1);
//puts(CStr("2 CProgCalc::Calc_NAKZ Nakz=") + Nakz);


		double	DN 		= Nakz < N0 ? DN1 : DN2;
		double	Aqv		= abs(Ned-Nakz)	  < DN && stNed	  < STATUS_UNDEF ? 1.0 : 0.0;
		double	Aik		= abs(Nik-Nakz)   < DN && stNik	  < STATUS_UNDEF ? 1.0 : 0.0;
		double	A1k		= abs(N1k-Nakz)   < DN && stN1k	  < STATUS_UNDEF ? 1.0 : 0.0;
		double	A2k		= abs(N2k-Nakz)   < DN && stN2k	  < STATUS_UNDEF ? 1.0 : 0.0;
		double	Aopvd	= abs(Nopvd-Nakz) < DN && stNopvd < STATUS_UNDEF ? 1.0 : 0.0;

		if( Aqv > 0.0 || Aik>0.0 || A1k>0.0 || A2k>0.0 || Aopvd>0.0)
		{
			Nakz  = k1*Aik*Nik + k2*Aqv*Ned + k3*A1k*N1k + k4*A2k*N2k + k5*Aopvd*Nopvd;
//puts(CStr("3 CProgCalc::Calc_NAKZ Nakz=") + Nakz);

//puts(CStr("    k1==") + k1 + " Aik=" + Aik + " Nik=" + Nik + " k1*Aik*Nik=" + (k1*Aik*Nik));
//puts(CStr("    k2==") + k2 + " Aqv=" + Aqv + " Ned=" + Ned + " k2*Aqv*Ned=" + (k2*Aqv*Ned));
//puts(CStr("    k3==") + k3 + " A1k=" + A1k + " N1k=" + N1k + " k3*A1k*N1k=" + (k3*A1k*N1k));
//puts(CStr("    k4==") + k4 + " A2k=" + A2k + " N2k=" + N2k + " k4*A2k*N2k=" + (k4*A2k*N2k));
//puts(CStr("    k5==") + k5 + " Aopvd=" + Aopvd + " Nopvd=" + Nopvd + " k5*Aopvd*Nopvd=" + (k5*Aopvd*Nopvd));

//puts(CStr("4 CProgCalc::Calc_NAKZ Nakz=") + Nakz);

			if( Nakz ) Nakz *= pow(k1*Aik + k2*Aqv + k3*A1k + k4*A2k + k5*Aopvd,-1);
//puts(CStr("5 CProgCalc::Calc_NAKZ Nakz=") + Nakz);
//puts(CStr(""));
		}

		if(Nakz < ::pProg->pSettings->MinPowerCalc)
			stNakz = STATUS_UNDEF;
	}

	// ������� � ��� ����������� ��������
	WriteToIdb("wNAKZ",	 Nakz,  stNakz);

	{// �������� ������ TEFF
		CTime	CurTime = GetCurrentTime();
		char	stTeff	= STATUS_NORMAL;

		if( PrevCalcTeffTime.IsNull() )
		{// � ������ ������� ������ � double TEFF � ����� ����� ������������ ������ ��
			float TimeTMVAlue		= ReadValueFromIdb("TEFF_TM_SCAN");
			PrevCalcTeffTime.Sec	= *(int*)&TimeTMVAlue;
			PrevTEFF				= ReadValueFromIdb("TEFF_SCAN");
//puts(CStr("CProgCalc::Calc_NAKZ *** ") + &PrevCalcTeffTime + " Nakz=" + Nakz + " stNakz=" + (int)stNakz + " FirstTEFF=" + PrevTEFF);
		}
		else
		{
			if( stNakz < STATUS_UNDEF && Nakz > 0 )
				PrevTEFF 	= PrevTEFF + (((double)CurTime - (double)PrevCalcTeffTime)/(24.0*3600.0*1000.0)) * Nakz / 3000.0;
			else	stTeff 	= STATUS_UNDEF;
		}
		PrevCalcTeffTime = CurTime;

		// ������� � ��� ����������� ��������
		WriteToIdb("TEFF_REG",	 	PrevTEFF,  stTeff);
		WriteToIdb("TEFF_TM_REG",	*(float*)&CurTime.Sec, STATUS_NORMAL);
		if(::pProg->pScanIdb->pLastChangeTeff) *(float*)&::pProg->pScanIdb->pLastChangeTeff->Value = *(float*)&CurTime.Sec;

//puts(CStr("CProgCalc::Calc_NAKZ ") + &CurTime + " Nakz=" + Nakz + " stNakz=" + (int)stNakz + " NextTEFF=" + PrevTEFF + " DELTA=" + ((((double)CurTime - (double)PrevCalcTeffTime)/(24.0*3600.0*1000.0)) * Nakz / 3000.0));

	}

}

*/



void 	CProgCalc::Calc_wNAKZ(void)
{// ������ �������� �������� (�� ����� ���������������)

	double	Nakz 	= 0;	char  stNakz = STATUS_UNDEF;


	double	N1k		= 0.0;	char  stN1k  	= STATUS_UNDEF;	ReadFromIdb("N1k", 	&N1k, 	&stN1k);
	double	N2k		= 0.0;	char  stN2k  	= STATUS_UNDEF;	ReadFromIdb("N2k", 	&N2k, 	&stN2k);
	double	Nopvd	= 0.0;	char  stNopvd	= STATUS_UNDEF;	ReadFromIdb("NOpvd",&Nopvd, &stNopvd);
	double	Nik		= 0.0;	char  stNik 	= STATUS_UNDEF;	ReadFromIdb("Nik", 	&Nik, 	&stNik);
	double	Ned 	= 0.0;	char  stNed  	= STATUS_UNDEF;	ReadFromIdb("RNED",	&Ned, 	&stNed);
	double	Mode 	= 0.0;	char  stMode  	= STATUS_UNDEF;	ReadFromIdb("YC00MODE",	&Mode, 	&stMode);

/*
	// ��������� ������ (0-���)
	double	SPT1 	= 0.0;	char  stSPT1 	= STATUS_UNDEF;	ReadFromIdb("Spt10",	&SPT1,	&stSPT1);
	double	SPT2 	= 0.0;	char  stSPT2 	= STATUS_UNDEF;	ReadFromIdb("Spt20",	&SPT2,	&stSPT2);
	double	SPT3 	= 0.0;	char  stSPT3 	= STATUS_UNDEF;	ReadFromIdb("Spt30",	&SPT3,	&stSPT3);
	double	SPT4 	= 0.0;	char  stSPT4 	= STATUS_UNDEF;	ReadFromIdb("Spt40",	&SPT4,	&stSPT4);
*/
	double	k1 		= stNik<STATUS_UNDEF ? pConstTbl->GetConstValue("NAKZ_K1") : 0;
	double	k2 		= stNed<STATUS_UNDEF ? pConstTbl->GetConstValue("NAKZ_K2") : 0;
	double	k3 		= stN1k<STATUS_UNDEF ? pConstTbl->GetConstValue("NAKZ_K3") : 0;
	double	k4 		= stN2k<STATUS_UNDEF ? pConstTbl->GetConstValue("NAKZ_K4") : 0;
	double	k5 		= stNopvd<STATUS_UNDEF ? pConstTbl->GetConstValue("NAKZ_K5") : 0;
	double	DN1		= pConstTbl->GetConstValue("NAKZ_DN1");
	double	DN2		= pConstTbl->GetConstValue("NAKZ_DN2");
	double	N0		= pConstTbl->GetConstValue("NAKZ_N0");

	// ����� ������ �������� 0-4
/*
	int		RMode	= 0;	char stRMode = STATUS_UNDEF;

	if( (int)SPT1==0 && stSPT1<STATUS_UNDEF ){	RMode+=1;  stRMode = STATUS_NORMAL; }
	if( (int)SPT2==0 && stSPT2<STATUS_UNDEF ){	RMode+=1;  stRMode = STATUS_NORMAL; }
	if( (int)SPT3==0 && stSPT3<STATUS_UNDEF ){	RMode+=1;  stRMode = STATUS_NORMAL; }
	if( (int)SPT4==0 && stSPT4<STATUS_UNDEF ){	RMode+=1;  stRMode = STATUS_NORMAL; }
*/

    printf("Nakz = %g stMOde = %d, Mode = %g\n", Nakz, stMode, Mode);
	if( Mode==0 || stMode>=STATUS_UNDEF )
	{// ���� ����� ������ �������� �����������
		Nakz = 0; stNakz = STATUS_UNDEF;
	}
	else if( Mode>3 )
	{// ���� � ������ ����� ���� ������
		Nakz = 0; stNakz = STATUS_NORMAL;
	}
	else
	{
		stNakz = STATUS_NORMAL;

		Nakz  = k1*Nik + k2*Ned + k3*N1k + k4*N2k + k5*Nopvd;

		if( Nakz ) Nakz *= pow(k1 + k2 + k3 + k4 + k5, -1);
//puts(CStr("1 CProgCalc::Calc_NAKZ Nakz=") + Nakz + " Ned=" + Ned + " Nik=" + Nik);


		double	DN 		= Nakz < N0 ? DN1 : DN2;
		double	Aqv		= abs(Ned-Nakz)	  < DN && stNed	  < STATUS_UNDEF ? 1.0 : 0.0;
		double	Aik		= abs(Nik-Nakz)   < DN && stNik	  < STATUS_UNDEF ? 1.0 : 0.0;
		double	A1k		= abs(N1k-Nakz)   < DN && stN1k	  < STATUS_UNDEF ? 1.0 : 0.0;
		double	A2k		= abs(N2k-Nakz)   < DN && stN2k	  < STATUS_UNDEF ? 1.0 : 0.0;
		double	Aopvd	= abs(Nopvd-Nakz) < DN && stNopvd < STATUS_UNDEF ? 1.0 : 0.0;

//puts(CStr("1 CProgCalc::Calc_NAKZ Aqv=") + Aqv + " Aik=" + Aik + " A1k=" + A1k + " A2k=" + A2k + " Aopvd=" + Aopvd);


		if( Aqv > 0.0 || Aik>0.0 || A1k>0.0 || A2k>0.0 || Aopvd>0.0)
		{
			Nakz  = k1*Aik*Nik + k2*Aqv*Ned + k3*A1k*N1k + k4*A2k*N2k + k5*Aopvd*Nopvd;

//puts(CStr("2 CProgCalc::Calc_NAKZ Nakz=") + Nakz);

			if( Nakz ) Nakz *= pow(k1*Aik + k2*Aqv + k3*A1k + k4*A2k + k5*Aopvd,-1);

/*
puts(CStr("3 CProgCalc::Calc_NAKZ Nakz=") + Nakz);

puts(CStr("    k1==") + k1 + " Aik=" + Aik + " Nik=" + Nik + " k1*Aik*Nik=" + (k1*Aik*Nik));
puts(CStr("    k2==") + k2 + " Aqv=" + Aqv + " Ned=" + Ned + " k2*Aqv*Ned=" + (k2*Aqv*Ned));
puts(CStr("    k3==") + k3 + " A1k=" + A1k + " N1k=" + N1k + " k3*A1k*N1k=" + (k3*A1k*N1k));
puts(CStr("    k4==") + k4 + " A2k=" + A2k + " N2k=" + N2k + " k4*A2k*N2k=" + (k4*A2k*N2k));
puts(CStr("    k5==") + k5 + " Aopvd=" + Aopvd + " Nopvd=" + Nopvd + " k5*Aopvd*Nopvd=" + (k5*Aopvd*Nopvd));


puts(CStr("4 CProgCalc::Calc_NAKZ Nakz=") + Nakz);
*/
//puts(CStr("4 CProgCalc::Calc_NAKZ Nakz=") + Nakz);
//puts(CStr(""));
		}

		if(Nakz < ::pProg->pSettings->MinPowerCalc)
			stNakz = STATUS_UNDEF;
	}


//    char stri1[1000],stri2[1000];
//    sprintf(stri1,"=========IN CALC NAKZ = %g, MODE = %d\n ", Nakz, Mode);
//    sprintf(stri2,"=========Nik = %g, Ned = %g, N1k = %g, N2k = %g, Nopvd = %g\n ", Nik, Ned, N1k, N2k, Nopvd);
//    ::pProg->AddEvent(IDI_EVENT_INFO, MakeError("[IN PrepareScanData]", NULL, stri1));
//    ::pProg->AddEvent(IDI_EVENT_INFO, MakeError("[IN PrepareScanData]", NULL, stri2));

	// ������� � ��� ����������� ��������
	WriteToIdb("wNAKZ",	 Nakz,  stNakz);

 	{// �������� ������ TEFF
		CTime	CurTime = GetCurrentTime();
		char	stTeff	= STATUS_NORMAL;

		if( PrevCalcTeffTime.IsNull() )
		{// � ������ ������� ������ � double TEFF � ����� ����� ������������ ������ ��
			float TimeTMVAlue		= ReadValueFromIdb("TEFF_TM_SCAN");
			PrevCalcTeffTime.Sec	= *(int*)&TimeTMVAlue;
			PrevTEFF				= ReadValueFromIdb("TEFF_SCAN");
//puts(CStr("CProgCalc::Calc_NAKZ *** ") + &PrevCalcTeffTime + " Nakz=" + Nakz + " stNakz=" + (int)stNakz + " FirstTEFF=" + PrevTEFF);
		}
		else
		{
			if( stNakz < STATUS_UNDEF )
//			if( stNakz < STATUS_UNDEF && Nakz > 0 ) // 24.11.2017 �� ������ ��������, �.�. ��� �������� � ����� 5 TEFF ����� �������������
				PrevTEFF 	= PrevTEFF + (((double)CurTime - (double)PrevCalcTeffTime)/(24.0*3600.0*1000.0)) * Nakz / 3000.0;
			else	stTeff 	= STATUS_UNDEF;
		}
		PrevCalcTeffTime = CurTime;

		// ������� � ��� ����������� ��������
		WriteToIdb("TEFF_REG",	 	PrevTEFF,  stTeff);
		WriteToIdb("TEFF_TM_REG",	*(float*)&CurTime.Sec, STATUS_NORMAL);
		if(::pProg->pScanIdb->pLastChangeTeff) *(float*)&::pProg->pScanIdb->pLastChangeTeff->Value = *(float*)&CurTime.Sec;
//puts(CStr("CProgCalc::Calc_NAKZ ") + &CurTime + " Nakz=" + Nakz + " stNakz=" + (int)stNakz + " NextTEFF=" + PrevTEFF + " DELTA=" + ((((double)CurTime - (double)PrevCalcTeffTime)/(24.0*3600.0*1000.0)) * Nakz / 3000.0));
	}

//CTime	CurTIme = GetCurrentTime();
//puts(CStr("CProgCalc::Calc_wNAKZ ") + &CurTIme + " Nakz=" + Nakz + " stNakz=" + (int)stNakz);
}



void 	CProgCalc::Calc_MODE(void)
{// �.3.13.2 ������ ������ ������ ��

	double		Mode = 0;	char  stMode = STATUS_NORMAL;

	IdbValue	Nik;		ReadFromIdb("Nik",		&Nik);		// �������� �������� �� ���������� ����
    IdbValue	Ntg;		ReadFromIdb("Ntg",		&Ntg);		// �������� ���������������, ���
	IdbValue	ADTpt;		ReadFromIdb("ADTpt",	&ADTpt);	// ��.  DT �  ������  1�, ����
	IdbValue	ADTtpzn;	ReadFromIdb("ADTtpzn",	&ADTtpzn);	// ��.  DT � ��� (�� ��), ����
	IdbValue	ATgn;		ReadFromIdb("ATgn",		&ATgn);		// ��.  T  ���. �����, ����
	IdbValue	AThn;		ReadFromIdb("AThn",		&AThn);		// ��.  T  ���. �����, ����
	IdbValue	ATtpzn;		ReadFromIdb("ATtpzn",	&ATtpzn);	// ��. T �� ������ ��� (�� ��), ����

	double		NIK_OST 	= pConstTbl->GetConstValue("NIK_OST");
	double		NTG_OST 	= pConstTbl->GetConstValue("NTG_OST");
	double		DT_OST 		= pConstTbl->GetConstValue("DT_OST");
	double		T_HLAD 		= pConstTbl->GetConstValue("T_HLAD");
	double		T_GOR_OST 	= pConstTbl->GetConstValue("T_GOR_OST");



	// ��������� ������ (0-���)
	double	SPT1 	= 0.0;	char  stSPT1 	= STATUS_UNDEF;	ReadFromIdb("Spt10",	&SPT1,	&stSPT1);
	double	SPT2 	= 0.0;	char  stSPT2 	= STATUS_UNDEF;	ReadFromIdb("Spt20",	&SPT2,	&stSPT2);
	double	SPT3 	= 0.0;	char  stSPT3 	= STATUS_UNDEF;	ReadFromIdb("Spt30",	&SPT3,	&stSPT3);
	double	SPT4 	= 0.0;	char  stSPT4 	= STATUS_UNDEF;	ReadFromIdb("Spt40",	&SPT4,	&stSPT4);


/*
	double	SPT1 	= 0.0;	char  stSPT1 	= STATUS_UNDEF;	ReadFromIdb("SPT1",		&SPT1,	&stSPT1);
	double	SPT2 	= 0.0;	char  stSPT2 	= STATUS_UNDEF;	ReadFromIdb("SPT2",		&SPT2,	&stSPT2);
	double	SPT3 	= 0.0;	char  stSPT3 	= STATUS_UNDEF;	ReadFromIdb("SPT3",		&SPT3,	&stSPT3);
	double	SPT4 	= 0.0;	char  stSPT4 	= STATUS_UNDEF;	ReadFromIdb("SPT4",		&SPT4,	&stSPT4);
*/

	// ���������� ���������� ������
	int		KGcn	= 0;	char stKGcn = STATUS_UNDEF;

	if( (int)SPT1==0 && stSPT1<STATUS_UNDEF ){	KGcn+=1;  stKGcn = STATUS_NORMAL; }
	if( (int)SPT2==0 && stSPT2<STATUS_UNDEF ){	KGcn+=1;  stKGcn = STATUS_NORMAL; }
	if( (int)SPT3==0 && stSPT3<STATUS_UNDEF ){	KGcn+=1;  stKGcn = STATUS_NORMAL; }
	if( (int)SPT4==0 && stSPT4<STATUS_UNDEF ){	KGcn+=1;  stKGcn = STATUS_NORMAL; }


	int		DostParamCnt 	= 0;	// ���������� ����������� ����������
	int		OnParamCnt 		= 0;	// ���������� ����������� � ���������� ����������
	int		PowerState 		= 0;	// ������ �� ��������

	// �������� ���������� ����������� � ���������� ����������
	if( Nik.ok() ){			DostParamCnt++;   if(Nik.v() 		>= NIK_OST) OnParamCnt++; }
	if( Ntg.ok() ){			DostParamCnt++;   if(Ntg.v() 		>= NTG_OST) OnParamCnt++; }
	if( ADTpt.ok() ){		DostParamCnt++;   if(ADTpt.v() 		>= DT_OST) 	OnParamCnt++; }
	if( ADTtpzn.ok() ){		DostParamCnt++;   if(ADTtpzn.v() 	>= DT_OST) 	OnParamCnt++; }


	// � ������������� � �������� 38 �������� ��������������
	if( 	 DostParamCnt==4 && OnParamCnt>=3 )	PowerState = 1;
	else if( DostParamCnt>=2 && OnParamCnt>=2 )	PowerState = 1;
	else if( DostParamCnt==1 && OnParamCnt>=1 )	PowerState = 1;


//puts(CStr("CProgCalc::Calc_MODE ") + &CurTIme + " DostParamCnt=" + DostParamCnt + " OnParamCnt=" + OnParamCnt + " KGcn=" + KGcn);

	double	Temperature 	= 0.0;
	int		stTemperature	= STATUS_NORMAL;

	// �������� ����������� �����������
	if( 	 AThn.ok() ) 		Temperature = AThn.v();
	else if( ATgn.ok() ) 		Temperature = ATgn.v();
	else if( ATtpzn.ok() ) 		Temperature = ATtpzn.v();
	else						stTemperature = STATUS_UNDEF;

	if( PowerState==1 && stKGcn < STATUS_UNDEF && stTemperature < STATUS_UNDEF && Temperature >= T_GOR_OST )
	{// ���� �� �������� - ���������� �� ������� �������� ��� ��� ����������� ����� �������� ��������
		if( KGcn==4 ) Mode = 1; // ����� ������ �� ��������, 4 �����
		if( KGcn==3 ) Mode = 2; // ����� ������ �� ��������, 3 �����
		if( KGcn==2 ) Mode = 3; // ����� ������ �� ��������, 2 �����
	}
	else
	{// ���� �� ����������� - ���������� ����� ������ �������

		if( stTemperature<STATUS_UNDEF )
		{// �� ����������� ���������� ����� ������ ��

			if( 	  Temperature>=T_GOR_OST )	Mode = 4; // ������� �������
			else  if( Temperature>=T_HLAD )		Mode = 5; // ����������� �������
			else  								Mode = 6; // �������� �������
		}
	}

	// ������� � ��� ����������� ��������
	WriteToIdb("YC00MODE",	Mode,  stMode);

//CTime	CurTIme = GetCurrentTime();
puts(CStr("CProgCalc::Calc_MODE ") + " Mode=" + Mode + " stMode=" + (int)stMode);
}





double	CProgCalc::CalcConstUQL(double TvelSL, bool bTveg)
{// �������� ������ (��������������) �������� ������

	if( TvelSL < 0.0 ) TvelSL = 0.0;

	double	ConstU		= 0.0;
	LST*	pQlRangeLst	= bTveg ? pTvegQlRangeLst : pTvelQlRangeLst;

	if( pQlRangeLst->GetCnt()>1 )
	{
		CQlRangeStruct*	pPrevRange = (CQlRangeStruct*)pQlRangeLst->GetPtr(1);
		for( int RangePos=2; RangePos<=pQlRangeLst->GetCnt(); RangePos++ )
		{
			CQlRangeStruct*	pNextRange = (CQlRangeStruct*)pQlRangeLst->GetPtr(RangePos);

			if(TvelSL>=pPrevRange->Sl && TvelSL<pNextRange->Sl )
			{// ��������� ������� � ������ ������������
				ConstU = pPrevRange->QlRange + (pNextRange->QlRange - pPrevRange->QlRange) * (TvelSL - pPrevRange->Sl)/(pNextRange->Sl - pPrevRange->Sl);
//puts( CStr("CProgCalc::CalcConstUQL bTveg=") + (int)bTveg + " TvelSL=" + TvelSL + " RangePos=" +RangePos + " pPrevRange->QlRange=" + pPrevRange->QlRange + " pNextRange->QlRange=" + pNextRange->QlRange);

				break;
			}
			else if( RangePos==pQlRangeLst->GetCnt() )
			{
				ConstU = pNextRange->QlRange;
			}

			pPrevRange = pNextRange;
		}
	}

	return ConstU;
}


void CProgCalc::Calc_PZ2_QL(int ChangeDataFlag )
{// ������ ��2 �� ��������� ��������������� �� ���� � ����

	float	(&KK)[TVS_CNT][PIN_CNT][L24_CNT] 	= UpdRecord4;	// KK24x331
	float	(&KV)[TVS_CNT][L24_CNT] 			= ChangeDataFlag==FLG_CHANGE_REBUILD_DATA ? MonRecord2 : UpdRecord2;	// KV24
	float	(&SL)[TVS_CNT][PIN_CNT] 			= UpdRecord6;	// SL163x331 ������� ��������� ����/����
	float	QVMF[TVS_CNT][L24_CNT];								// ���� ����������� ����������� �� � ������
	char	stQVMF[TVS_CNT][L24_CNT];							// ������� ��� ���� QVMF
	float	usQVMF[TVS_CNT][L24_CNT];							// ���� ������� ����������� ����������� �� � ������

	float	MINZQLCAS[TVS_CNT];									// ������� ���� ���������� ������� �� �� � ����
	float	SLTCAS[TVS_CNT];									// ������� ���� ������� ��������� �� ���� ��� ���� � ���������� ������� �� ��
	float	SLNTCAS[TVS_CNT];									// ������ ���� �� ���� SLTCAS

	// �������� �������� ��������� � SLNTCAS
	for( int TvsPos=0; TvsPos<TVS_CNT; TvsPos++ ) SLNTCAS[TvsPos] = -1;

	double	AvgTvelQL		= 0;				// ������� �������� �� �� ����/���� (������� �������)

	double	Nakz			= 0.0;				// �������� �������� ��
	char	stNakz			= STATUS_NORMAL;	// ������������� �������� �������� ��

	double	MinZQL			= 1000;				// ����������� ����� �� �������
	char	stMinZQL		= STATUS_UNDEF;		// ������ MinZQL

	double	Pz2_QL			= 0;				// ������ PZ2 �� ��������� �� ����/����
	char	stPz2_QL		= STATUS_UNDEF;		// ������ Pz2_QL

	double	TvsMinZQL		= 0;				// ����� ��� � ����������� ������� �� �������
	char	stTvsMinZQL		= STATUS_UNDEF;		// ������ TvsMinZQL

	double	LayerMinZQL		= 0;				// ����� ���� � ����������� ������� �� �������
	char	stLayerMinZQL	= STATUS_UNDEF;		// ������ LayerMinZQL

	double	PinMinZQL		= 0;				// ����� ����/���� � ����������� ������� �� �������
	char	stPinMinZQL		= STATUS_UNDEF;		// ������ PinMinZQL

	double	QlMinZQL		= 0;				// �������� ��������� �� ������� ����/���� � ���������� ������� �� �������
	char	stQlMinZQL		= STATUS_UNDEF;		// ������ QlMinZQL

	double	UstQlMinZQL		= 0;				// �������� ������� �� ��������� �� ������� ����/���� � ���������� ������� �� �������
	char	stUstQlMinZQL	= STATUS_UNDEF;		// ������ stUstQlMinZQL


	// ��������� ������������ � �������
	double	HFUEL 			= pConstTbl->GetConstValue("HFUEL");	// ����� ���������� ������ � �������� ���� 3.55�
	double	MSL 			= pConstTbl->GetConstValue("MSL");		// ���������� ��������� ��� ���� ��������� SL
	double	MQVMF			= pConstTbl->GetConstValue("MQVMF");	// ���������� ��������� ��� QVMF

	// �������� �������� NAKZ � ������ �� Idb (�������� ���� �������������)
	ReadFromIdb("wNAKZ", &Nakz, &stNakz);

//	CTime	CurTime = GetCurrentTime();


	if( stNakz<STATUS_UNDEF )
	{

		// ����������� ������� �������� �� �� ����/����, ��� ��������� Nakz � �� � ����� ����/���� � ��� (��/��)
		AvgTvelQL	= ((double)Nakz*1000000.0)/(HFUEL*TVS_CNT*100.0*TVEL_CNT);


		for( int L24Pos=0; L24Pos<L24_CNT; L24Pos++ )
		{
			for( int TvsPos=0; TvsPos<TVS_CNT; TvsPos++ )
			{
				CTvsFuelStruct*	pTvsStruct = &TvsFuelArray[TvsPos];

				// ����������� ����� �� ������� � ������
				double			MinPrizmZQL = 1000;
				double			MaxPrizmQL 	= 0;
				double			MaxPrizmUQL = 0;

				for( int PinPos=0; PinPos<PIN_CNT; PinPos++ )
				{
					if( pTvsStruct->PinTypes[PinPos]!=pTvsStruct->ChanType )
					{// ��������� ������ �� ��������

						// ��������� ������� ����
						bool	bTveg 		= pTvsStruct->PinTypes[PinPos]==pTvsStruct->TvegType ? true : false;

						// �������� ������� ��������� ����/����, � ���*���/�� U
						double TvelSL = SL[TvsPos][PinPos] * MSL;

//if( !StrCmp(pTvsStruct->FuelName,"22AU") && PinPos==0 )

						// �������� ������ (��������������) �������� ������
						double ConstUQL = CalcConstUQL(TvelSL, bTveg);

//if( TvelSL < 0.0)
//	puts(CStr("TvsPos=") + TvsPos + " PinPos=" + PinPos + "LayerPos=" + L24Pos + " FuelName=" + pTvsStruct->FuelName + " TvelSL=" + TvelSL + " ConstUQL=" + ConstUQL);


						// ������� ���� QL ������� �� ����/���� � �������� ������
						double LineUQL = 0.0;

						// ��������� ����� ����������� �������� ������ � % �� ���� ���
						double PointX = 100.0 - (ConstUQL - 448.0 + 440.0/3.0)/(88.0/30.0);
						if(PointX>100.0) PointX = 100.0;

						// �������� �������� ��������������� ������� �� ����/����, � ��/��
						double LineQL = MQVMF * KV[TvsPos][L24Pos] * KK[TvsPos][PinPos][L24Pos] * AvgTvelQL;

//LineQL *= 1.5;

						// ��� ���� � % �� ���� ���
						double LayerL = (double)L24Pos * (100.0/24.0);

						// ���� ���� � % �� ���� ���
						double LayerH = (double)(L24Pos+1) * (100.0/24.0);

//if(TvsPos==0 && L24Pos==0 && PinPos==0 )
//	puts(CStr("Tvs=") + TvsPos + " L=" + L24Pos + " PinPos=" + PinPos + " AvgTvelQL=" + AvgTvelQL + " LineQL=" + LineQL + " PointX=" + PointX + " TvelSL=" + TvelSL + " ConstUQL=" + ConstUQL);


						if( LayerH < PointX )
						{// ���� � ��� ���� ���� ����� X - ������� ����� Const
							LineUQL = ConstUQL;
						}
						else if( LayerL > PointX )
						{// ���� � ��� ���� ���� ����� X - ������� ����� �������� ����� ���������� ������� ��� ����� � ���� ����
							double LayerLU = 448.0 + (440.0/3.0) - 88.0/30.0*LayerL;
							double LayerHU = 448.0 + (440.0/3.0) - 88.0/30.0*LayerH;

							LineUQL = (LayerLU + LayerHU)/2;
						}
						else
						{// ����� X ����� ������ ���� - ������ ������������� � ������������ � ������ �� ������ �������� ������

							double ShareL	= (PointX-LayerL)/(LayerH-LayerL);
							double ShareH	= 1.0 - ShareL;

							double LayerXU	= 448.0 + (440.0/3.0) - 88.0/30.0*PointX;
							double LayerHU	= 448.0 + (440.0/3.0) - 88.0/30.0*LayerH;

							LineUQL = ShareL*ConstUQL + ShareH*(LayerXU + LayerHU)/2;
						}

						// ����� �� ������� � ������� ����/���� � �������� ������
						double LineZQL = LineUQL - LineQL;

						if( TvelSL>=0 && ((int)SLNTCAS[TvsPos]==-1 || LineZQL < MINZQLCAS[TvsPos]) )
						{// ���� ����������� ����� �� ���� � ���
							MINZQLCAS[TvsPos] 	= LineZQL;
							SLTCAS[TvsPos] 		= TvelSL;
							SLNTCAS[TvsPos]		= PinPos+1;
						}

//if(TvsPos==0 && /*L24Pos==0 &&*/ PinPos==0 )
//	puts(CStr("Tvs=") + TvsPos + " L=" + L24Pos + " PinPos=" + PinPos + " LineUQL=" + LineUQL + " LineQL=" + LineQL + " LineZQL=" + LineZQL + " KK=" + KK[TvsPos][PinPos][L24Pos] + " KV=" + KV[TvsPos][L24Pos]);
						if( TvelSL>=0 && LineZQL < MinPrizmZQL )
						{// ��������� �������� ����������� ������� � ������
							MinPrizmZQL = LineZQL;
							MaxPrizmQL	= LineQL;
							MaxPrizmUQL = LineUQL;
						}

						if( TvelSL>=0  && LineZQL < MinZQL )
						{// ��������� ��������� ��� ������������ ������ �� ��
							stPz2_QL = STATUS_NORMAL;
							if( LineZQL<0 )  Pz2_QL = 1.0;

							QlMinZQL	= LineQL;		stQlMinZQL 		= STATUS_NORMAL;
							UstQlMinZQL	= LineUQL;		stUstQlMinZQL 	= STATUS_NORMAL;
							MinZQL 		= LineZQL;		stMinZQL 		= STATUS_NORMAL;
							TvsMinZQL 	= TvsPos+1;		stTvsMinZQL 	= STATUS_NORMAL;
							LayerMinZQL	= L24Pos+1;		stLayerMinZQL 	= STATUS_NORMAL;
							PinMinZQL	= PinPos+1;		stPinMinZQL 	= STATUS_NORMAL;
						}
					}
				}

				// �������� ���� QL �������� ������� � ������� � ����� ������
				QVMF[TvsPos][L24Pos] 	= MaxPrizmQL;
				usQVMF[TvsPos][L24Pos] 	= MaxPrizmUQL;
				stQVMF[TvsPos][L24Pos] 	= MaxPrizmQL > MaxPrizmUQL ? STATUS_VAG : STATUS_NORMAL;
			}
		}

		// ������� � ��� ����������� ��������
		WriteToIdb("MIN_ZQL", 		MinZQL, 	stMinZQL);
		WriteToIdb("PZ2_ZQL", 		Pz2_QL, 	stPz2_QL);
		WriteToIdb("TVS_MIN_ZQL", 	TvsMinZQL, 	stTvsMinZQL);
		WriteToIdb("LAYER_MIN_ZQL", LayerMinZQL,stLayerMinZQL);
		WriteToIdb("PIN_MIN_ZQL", 	PinMinZQL,	stPinMinZQL);
		WriteToIdb("QL_MIN_ZQL", 	QlMinZQL, 	stQlMinZQL);
		WriteToIdb("UST_QL_MIN_ZQL",UstQlMinZQL,stUstQlMinZQL);

		pQVMF->Write((float*)QVMF,(char*)stQVMF, true);
		pUsQVMF->WriteValue((float*)usQVMF,STATUS_NORMAL, true);
		pSLTCAS->WriteValue((float*)SLTCAS);
		pSLNTCAS->WriteValue((float*)SLNTCAS);
//CTime	CurTime = GetCurrentTime();
//puts(CStr("CProgCalc::Calc_PZ2_QL ") + &CurTime + " WriteQVMF MIN_ZQL=" + MinZQL);
	}
	else
	{
		WriteStatusToIdb("MIN_ZQL",STATUS_UNDEF);
		WriteStatusToIdb("PZ2_ZQL",STATUS_UNDEF);
		WriteStatusToIdb("TVS_MIN_ZQL",STATUS_UNDEF);
		WriteStatusToIdb("LAYER_MIN_ZQL",STATUS_UNDEF);
		WriteStatusToIdb("PIN_MIN_ZQL",STATUS_UNDEF);
		WriteStatusToIdb("QL_MIN_ZQL",STATUS_UNDEF);
		WriteStatusToIdb("UST_QL_MIN_ZQL",STATUS_UNDEF);

		pQVMF->WriteStatus(STATUS_UNDEF);
		pUsQVMF->WriteStatus(STATUS_UNDEF);
		pSLTCAS->WriteStatus(STATUS_UNDEF);
		pSLNTCAS->WriteStatus(STATUS_UNDEF);
	}

//	else
//		puts(CStr("CProgCalc::Calc_PZ2_QL else ") + &CurTime + " stNakz=" + (int)stNakz);

//	puts(CStr("CProgCalc::Calc_PZ2_QL ") + &CurTime);
}

void CProgCalc::Calc_PZ2_DNBR(int ChangeDataFlag )
{// ������ ��2 �� ������ �� �������

	float	(&KK)[TVS_CNT][PIN_CNT][L24_CNT] 	= UpdRecord4;	// KK24x331
	float	(&KV)[TVS_CNT][L24_CNT] 			= ChangeDataFlag==FLG_CHANGE_REBUILD_DATA ? MonRecord2 : UpdRecord2;	// KV24
	float	ENT[TVS_CNT][L24_CNT];								// ��������������� ���� ���������
	float	DNBR[TVS_CNT][L24_CNT];								// ���� ������� �� ������� �����������
	float	DNBT[TVS_CNT];										// ���� ������� �� ������� �������
	float	YCTOUT[TVS_CNT];									// ���� ���������� �� ������ ���
	float	YCTDT[TVS_CNT];										// ���� ���������� �� ���


	// ��������� ������������ � �������
	double	HFUEL		= pConstTbl->GetConstValue("HFUEL");	// ����� ���������� ������ � �������� ���� 3.55�
	double	FKAS 		= pConstTbl->GetConstValue("FKAS");		// ��������� ������� ������� 0.0246�2
	double	DTVEL 		= pConstTbl->GetConstValue("DTVEL");	// ������� ������� �������� ���� 0.0091�
	double	KPROT 		= pConstTbl->GetConstValue("KPROT");	// ����������� �������� ������������� ���� ��� 0.97
	double	DNBRMIN		= pConstTbl->GetConstValue("DNBRMIN");	// ����������� �������� ������������� ���� ��� 1.215
	double	MDNBR		= pConstTbl->GetConstValue("MDNBR");	// ���������� ��������� ��� DNBR
//puts(CStr("3 Calc_PZ2_DNBR "));
	double	DELTA_QKR	= pConstTbl->GetConstValue("DELTA_QKR");// ����������� ������� ������� Qkr, %
	double	KQING		= pConstTbl->GetConstValue("KQING");	// ���������� ����������� �� ��������� ������
	double	KHING		= pConstTbl->GetConstValue("KHING");	// ���������� ����������� �� ���������
	double	PKR			= pConstTbl->GetConstValue("PKR");		// ����������� ��������, ���
	double	KVHOD		= pConstTbl->GetConstValue("KVHOD");	// ����������� ��������������� �������� �� ����� ���
	double	DGM1K		= pConstTbl->GetConstValue("DGM1K");	// �������� � ��������� ������� ������� �������, �/�
	double	DTIN		= pConstTbl->GetConstValue("DTIN");		// �������� � ��������� ������� ������� �������, ���� �
	double	DNAKZ		= pConstTbl->GetConstValue("DNAKZ");	// �������� �� �������� ��������, ���


	// ���������� ��� ����������� ��� �������
	double	Nakz	= (ReadValueFromIdb("wNAKZ") + DNAKZ)*1000000.0;		// �������� �������� �� � �� � ���������� ��������� 120 ���

//  14.01.2018 - �������� �� ���� ����������� ����������
//	double	YCP_OUT	= ReadValueFromIdb("YC70P_A")*0.0980665;	// ������� �������� ��� ���, ���
	double	YCP_OUT	= ReadValueFromIdb("APzn")*0.0980665;	// ������� �������� ��� ���, ���

	double	YCDP	= ReadValueFromIdb("YC14P03")*0.0980665;	// ������� �������� ��� ���, ���

//  14.01.2018 - �������� �� ���� ����������� ����������
//	double	YAT_IN	= ReadValueFromIdb("YA02T_A");				// ������� ����������� �� ����� � ���
	double	YAT_IN	= ReadValueFromIdb("AThn");					// ������� ����������� �� ����� � ���


// 14.01.2018 - �������� �� ���� ����������� ����������
//	double	YA10F	= ReadValueFromIdb("YA10F");				// ������ �� ����� 1
//	double	YA20F	= ReadValueFromIdb("YA20F");				// ������ �� ����� 2
//	double	YA30F	= ReadValueFromIdb("YA30F");				// ������ �� ����� 3
//	double	YA40F	= ReadValueFromIdb("YA40F");				// ������ �� ����� 4
	double	YA10F	= ReadValueFromIdb("GMpt10");				// ������ �� ����� 1
	double	YA20F	= ReadValueFromIdb("GMpt20");				// ������ �� ����� 2
	double	YA30F	= ReadValueFromIdb("GMpt30");				// ������ �� ����� 3
	double	YA40F	= ReadValueFromIdb("GMpt40");				// ������ �� ����� 4


	double	MinZDNBR			= 1000;				// ����������� ����� �� ������� �� DNBR
	char	stMinZDNBR			= STATUS_UNDEF;		// ������ MinZDNBR

	double	Pz2_DNBR			= 0;				// ������ PZ2 �� ������ �� �������
	char	stPz2_DNBR			= STATUS_UNDEF;		// ������ Pz2_DNBR

	double	TvsMinZDNBR			= 0;				// ����� ��� � ����������� ������� �� ������� �� DNBR
	char	stTvsMinZDNBR		= STATUS_UNDEF;		// ������ TvsMinZDNBR

	double	LayerMinZDNBR		= 0;				// ����� ���� � ����������� ������� �� ������� �� DNBR
	char	stLayerMinZDNBR		= STATUS_UNDEF;		// ������ LayerMinZDNBR

	double	PinMinZDNBR			= 0;				// ����� ����/���� � ����������� ������� �� �������
	char	stPinMinZDNBR		= STATUS_UNDEF;		// ������ PinMinZDNBR

	double	DnbrMinZDNBR		= 0;				// �������� ������ �� ������� ������� ����/���� � ���������� ������� �� ������� �� DNBR
	char	stDnbrMinZDNBR		= STATUS_UNDEF;		// ������ DnbrMinZDNBR

	double	UstDnbrMinZDNBR		= 0;				// �������� ������� �� ������ �� ������� ������� ����/���� � ���������� ������� �� ������� �� DNBR
	char	stUstDnbrMinZDNBR	= STATUS_UNDEF;		// ������ stUstDnbrMinZDNBR


	if( ReadStatusFromIdb("wNAKZ") < STATUS_UNDEF )
	{
		// ������ � 24 �����
		int		LayersCnt	= L24_CNT;

		// �������� �� ����� � ���
		double 	YcpIn		= YCP_OUT + YCDP;

		// ������� ������ ����� ������� � ��/c (��� ���������� ��������)
		double 	TvsG_c 		= (YA10F + YA20F + YA30F + YA40F) * 1000000.0/3600.0 * KPROT / TVS_CNT;

		// ������� ������ ����� ������� � ��/c (� ���������� ��������� � ������������� ��������������� �������� �� ����� � ���)
		double 	TvsG 		= (YA10F + YA20F + YA30F + YA40F + DGM1K/1000.0) * 1000000.0/3600.0 * KPROT / TVS_CNT * KVHOD;

//puts(CStr("(YA10F + YA20F + YA30F + YA40F)=") + (YA10F + YA20F + YA30F + YA40F) + " TvsG="+ TvsG);
		// �������� ������������ � ��� ��/�2*�
		double	SpeedW			=  TvsG/FKAS;

		// ������� ����������� ������� ����� ����������� � ������
		double TvelS			= PI * DTVEL * HFUEL / LayersCnt;

		// ��������� �� ����� � ������� ���/�� �� �. 3.19.1 � ���������
		double 	EntIn 			= WaterSteamProperty::getWaterEnthalpy(YcpIn, YAT_IN + DTIN);

		// ��������� �� ����� � ������� ���/�� �� �. 3.19.1 ��� ��������
		double 	EntIn_c			= WaterSteamProperty::getWaterEnthalpy(YcpIn, YAT_IN);

		// ����������� ������� �� �������� �� ������ �� ��� (�. 3.19.10)
		double	TemperatureOutS	= WaterSteamProperty::getWaterTemperatureS(YCP_OUT);

		// ������� �������� � ������, ��
		double	MidQvPrizm	= Nakz / (TVS_CNT*L24_CNT);

		for( int LayerPos=0; LayerPos<LayersCnt; LayerPos++ )
		{
			// �������� � ������ ����
			double LayerP 		= YcpIn - YCDP/(LayersCnt*2) - YCDP*LayerPos/LayersCnt;

			// ��������� ��������� � ���� �� �. 3.19.6
			double LayerEntS 	= WaterSteamProperty::getWaterEnthalpyS(LayerP);

			// ������� ��������������� � ���� �. 3.19.7
			double LayerR 		= WaterSteamProperty::getWaterBoilingHeatS(LayerP);

			for( int TvsPos=0; TvsPos<TVS_CNT; TvsPos++ )
			{
				CTvsFuelStruct*	pTvsStruct = &TvsFuelArray[TvsPos];


				// ����������� ����� �� ������� � ������
				double PrizmMinDNBR		   = 999.0;

				// ��������� ������
				double PrizmEnt 			= 0;

				if( LayerPos==0 )
				{// ��������� � ������ ���� ����� ����� ��������� ����� � ��������
				 // ��������� �������� ������ � ������� ����� �������, �������� � ���/��
					PrizmEnt  = EntIn;
					PrizmEnt += KV[TvsPos][LayerPos]*MidQvPrizm/1000.0/TvsG/2;
				}
				else
				{// ��������� ��������� ����� ����� ��������� ����������, ����� ��������
				 // ��������� �������� ���������� ������ � ������� ����� ������� ����
				 // �������� ��������� �������� ������� ������ � ������� ����� �������
				 // , �������� � ���/��
					PrizmEnt  = ENT[TvsPos][LayerPos-1];
					PrizmEnt += KV[TvsPos][LayerPos-1]*MidQvPrizm/1000.0/TvsG/2;
					PrizmEnt += KV[TvsPos][LayerPos]*MidQvPrizm/1000.0/TvsG/2;
				}

				// ��������� � ������
				ENT[TvsPos][LayerPos] = PrizmEnt;

				// ����������� ���� � ������ �� ��������� �������� � ��������� �� �. 3.19.2
				//double PrizmT = WaterSteamProperty::getWaterTemperature(LayerP, PrizmEnt);

				// ������������� ��������������
				double PrismX = (PrizmEnt*KHING - LayerEntS)/LayerR;

				// ������ ������������ ��������� ������
				double PrizmQkr = 0.795 * pow(1.0 - PrismX, 0.105*LayerP - 0.5) * pow(SpeedW, 0.311*(1-PrismX)-0.127) * (1.0 - 0.0185*LayerP);

				for( int PinPos=0; PinPos<PIN_CNT; PinPos++ )
				{// �������� ������� �������� ������ � ������� ������� ����� � �������� ������

					if( pTvsStruct->PinTypes[PinPos]!=pTvsStruct->ChanType )
					{// ��������� ������ �� ���

						// ������� �������� ����� ���/�2
						double	LineQ 	= ( (Nakz/1000000.0) * KV[TvsPos][LayerPos] * KK[TvsPos][PinPos][LayerPos]) / (TVS_CNT * LayersCnt * TVEL_CNT * TvelS);

						// �������� �������� ����� ��� ������� ����-������� F
						LINEQ[TvsPos][PinPos][LayerPos] = LineQ;


						// ����� �������� ������� ���� ����������� �����
						double	SumLineQ = 0;
						for( int i=LayerPos; i>=0 && i>=LayerPos-3; i-- )
							SumLineQ += LINEQ[TvsPos][PinPos][i];

						// ���� ������ ����������� ��������������� �������� ����
						double	F		= pow( SumLineQ / (4 * LineQ), 3.79 - 19.61*LayerP/PKR + 17.86*pow(LayerP/PKR,2));

						// ����� �� ������� � ������� ����
						double	LineDNBR	= MDNBR * PrizmQkr * (1 - DELTA_QKR/100) * F / (LineQ * KQING);

//if(TvsPos==0 && LayerPos==0 && PinPos==0 )
//	puts(CStr("**** F = ") + F + " PrismX=" + PrismX + " LayerR=" + LayerR + " PrizmQkr=" + PrizmQkr + " LayerP=" + LayerP + " SpeedW=" + SpeedW);
						// ����� �� �������
						double 	LineZDNBR 	= LineDNBR - DNBRMIN;

						// �������� ����������� ����� �� ������� � ������
						if( LineDNBR < PrizmMinDNBR )
							PrizmMinDNBR = LineDNBR;

						if( LineZDNBR < MinZDNBR )
						{// ��������� ��������� ��� ������������ ������ �� DNBR

							stPz2_DNBR = STATUS_NORMAL;
							if( LineZDNBR<0 )  Pz2_DNBR = 1.0;

							DnbrMinZDNBR	= LineDNBR;		stDnbrMinZDNBR 		= STATUS_NORMAL;
							UstDnbrMinZDNBR	= DNBRMIN;		stUstDnbrMinZDNBR 	= STATUS_NORMAL;
							MinZDNBR 		= LineZDNBR;	stMinZDNBR	 		= STATUS_NORMAL;
							TvsMinZDNBR 	= TvsPos+1;		stTvsMinZDNBR	 	= STATUS_NORMAL;
							LayerMinZDNBR	= LayerPos+1;	stLayerMinZDNBR 	= STATUS_NORMAL;
							PinMinZDNBR		= PinPos+1;		stPinMinZDNBR 		= STATUS_NORMAL;
						}
					}
				}

				// ��������� ���� ������� �� ������� �� DNBR ��� ������ ������
				DNBR[TvsPos][LayerPos] = PrizmMinDNBR;


				if( LayerPos==LayersCnt-1 )
				{// � ��������� ���� �������� ������ ���� DNBT

					// ��������� �� ������ �� �������
					double  EnthalpyOut = EntIn_c + QVCAS[TvsPos]*1000.0/TvsG_c;

					// ����������� ���� �� ������ �������
					double  TemperatureOutW = WaterSteamProperty::getWaterTemperature(LayerP, EnthalpyOut);

					// ��������� � DNBT
					DNBT[TvsPos] 	= TemperatureOutS - TemperatureOutW;

					// �������� ����������� �� ������ ���
					YCTOUT[TvsPos] 	= TemperatureOutW;

					// �������� �������� �� ���
					YCTDT[TvsPos] 	= TemperatureOutW - YAT_IN;
				}
			}
		}

		// ������� � ��� ����������� ��������
		WriteToIdb("MIN_ZDNBR", 		MinZDNBR, 		stMinZDNBR);
		WriteToIdb("PZ2_ZDNBR", 		Pz2_DNBR, 		stPz2_DNBR);
		WriteToIdb("TVS_MIN_ZDNBR", 	TvsMinZDNBR,	stTvsMinZDNBR);
		WriteToIdb("LAYER_MIN_ZDNBR", 	LayerMinZDNBR,	stLayerMinZDNBR);
		WriteToIdb("PIN_MIN_ZDNBR", 	PinMinZDNBR,	stPinMinZDNBR);
		WriteToIdb("DNBR_MIN_ZDNBR", 	DnbrMinZDNBR, 	stDnbrMinZDNBR);
		WriteToIdb("UST_DNBR_MIN_ZDNBR",UstDnbrMinZDNBR,stUstDnbrMinZDNBR);

		pDNBR->WriteValue((float*)DNBR);
		pDNBT->WriteValue((float*)DNBT);
		pYCTOUT->WriteValue((float*)YCTOUT);
		pYCTDT->WriteValue((float*)YCTDT);
	}

//CTime	CurTime = GetCurrentTime();
//puts(CStr("CProgCalc::Calc_PZ2_DNBR ") + &CurTime);
}




void CProgCalc::Calc_AppendFields(int ChangeDataFlag)
{// ������������� ������ ������� �����

	if( ChangeDataFlag==FLG_CHANGE_DIF_DATA )
	{
		float	(&SL24)[TVS_CNT][L24_CNT] 			= UpdRecord5;	// SL24
		float	(&KK24)[TVS_CNT][PIN_CNT][L24_CNT] 	= UpdRecord4;	// KK24
		float	KC24[TVS_CNT][L24_CNT];
		float	SLCAS[TVS_CNT];					// ������� ���������

		// ���������� ��� ����������� ��� �������
		double	Nakz	= 0.0;				// �������� �������� ��
		char	stNakz	= STATUS_NORMAL;	// ������������� �������� �������� ��


		// �������� �������� NAKZ � ������ �� Idb (�������� ���� �������������)
		ReadFromIdb("wNAKZ", &Nakz, &stNakz);

		if( stNakz<STATUS_UNDEF )
		{
			for( int TvsPos=0; TvsPos<TVS_CNT; TvsPos++ )
			{
				double	sumSlTvs 	= 0.0;

				for(int L24Pos=0; L24Pos<L24_CNT; L24Pos++ )
				{
					sumSlTvs 	+= SL24[TvsPos][L24Pos];

					double		sumKkPrizm 	= 0.0;

					sumKkPrizm 	+= KK24[TvsPos][145-1][L24Pos];
					sumKkPrizm 	+= KK24[TvsPos][146-1][L24Pos];
					sumKkPrizm 	+= KK24[TvsPos][165-1][L24Pos];
					sumKkPrizm 	+= KK24[TvsPos][167-1][L24Pos];
					sumKkPrizm 	+= KK24[TvsPos][186-1][L24Pos];
					sumKkPrizm 	+= KK24[TvsPos][187-1][L24Pos];

					KC24[TvsPos][L24Pos] = sumKkPrizm/6.0;
				}
				SLCAS[TvsPos]	= sumSlTvs/L24_CNT;
			}

			float	BuffArray7[TVS_CNT][L07_CNT];
			pSLCAS->WriteValue((float*)SLCAS);
			pKC->WriteValue(Convert24To7(KC24, BuffArray7));
		}
		else
		{
			pSLCAS->WriteStatus(STATUS_UNDEF);
			pKC->WriteStatus(STATUS_UNDEF);
		}
	}
	else if( ChangeDataFlag==FLG_CHANGE_REBUILD_DATA )
	{
		float	(&KV24)[TVS_CNT][L24_CNT] 			= MonRecord2;
		float	KQ[TVS_CNT];
		float	OFSTCAS[TVS_CNT];				// ���� �������
		float	KV7L[TVS_CNT][L07_CNT];

		// ���������� ��� ����������� ��� �������
		double	Nakz	= 0.0;				// �������� �������� ��
		char	stNakz	= STATUS_NORMAL;	// ������������� �������� �������� ��

		// �������� �������� NAKZ � ������ �� Idb (�������� ���� �������������)
		ReadFromIdb("wNAKZ", &Nakz, &stNakz);

		pKV7L->ReadValue((float*)KV7L);

		if( stNakz<STATUS_UNDEF )
		{
			// ������� �������� �������
			double	AvgTvsQL = Nakz/TVS_CNT;

			// ��������� KV ���� ���
			double	sumKvL	 = 0.0;

			// ��������� KV ����� ���
			double	sumKvH	 = 0.0;

			// ��������� KQ
//			double sumKQ = 0;

			float	Hj[L24_CNT] = {15.3279,16.1025,14.6113,14.6113,14.6113,14.6113,14.6113,14.6113,14.6113,14.6113,14.6113,14.6113,14.6113,14.6113,14.6113,14.6113,14.6113,14.6113,14.6113,14.6113,14.6113,14.6113,16.1025,15.3279};

/*
			for( int TvsPos=0; TvsPos<TVS_CNT; TvsPos++ )
			{// ���������� KQ �� ���� KV7L
				double	sumKvTvs 	= 0.0;

				for(int L07Pos=0; L07Pos<L07_CNT; L07Pos++ )
					sumKvTvs += KV7L[TvsPos][L07Pos];

				KQ[TvsPos] 			= sumKvTvs/L07_CNT;
//				sumKQ			   += sumKvTvs/L07_CNT;
			}

����, ������. ���������� ������� (��������� ������� - �� 15-00 �������) �������� � �������� ������� KQ � GetebcnCalc.
����� ��������� ������� ������������ �� 2-� ������  ����� �����������.
����� ��������� ������� : KQi=SUM(wj*NPij)
��� NPij -  24-������� �������� KV  �� ������ (2-� ������  ����� �����������)
Wj- ������������ �������������� ��������� �������  (�������������� 1 ���, ��� ������������� ������������� � ������ ��������� ����� )
Wj=hj/sum(hj)
��� �������� - ����� wj ������ ��������� �������.
Hj ����� ��������������:
15.3279
16.1025
14.6113 (20 ���)
16.1025
15.3279

*/
			double sumHJ = 0;
			for( int ArrPos=0; ArrPos<L24_CNT; ArrPos++ )
				sumHJ += Hj[ArrPos];


			for( int TvsPos=0; TvsPos<TVS_CNT; TvsPos++ )
			{
				double	sumKvTvs 	= 0.0;
				double	sumKvTvsL 	= 0.0;
				double	sumKvTvsH 	= 0.0;


				for(int L24Pos=0; L24Pos<L24_CNT; L24Pos++ )
				{
					double	KvPrizm	= KV24[TvsPos][L24Pos];

					sumKvTvs 	+= KvPrizm*Hj[L24Pos]/sumHJ;

					if(L24Pos < L24_CNT/2)	sumKvTvsL += KvPrizm;
					else 					sumKvTvsH += KvPrizm;
				}

				OFSTCAS[TvsPos]	= 100.0 * (sumKvTvsH - sumKvTvsL) / (sumKvTvsH + sumKvTvsL);

				sumKvL	+= sumKvTvsL;
				sumKvH	+= sumKvTvsH;

				KQ[TvsPos] 		= sumKvTvs;///L24_CNT;
				QVCAS[TvsPos] 	= KQ[TvsPos]*AvgTvsQL;
			}

			double	Ofst	= 0.0;				// ����� �� ���������������� ����
			char	stOfst	= STATUS_NORMAL;	//

			if( (sumKvH + sumKvL) > 0  ) 	Ofst = 100.0 * (sumKvH - sumKvL) / (sumKvH + sumKvL);
			else 							stOfst = STATUS_UNDEF;

//puts(CStr("sumKQ=") + sumKQ);

			WriteToIdb("OFST",	Ofst, stOfst);
			pKQ->WriteValue((float*)KQ);
			pQVCAS->WriteValue((float*)QVCAS);
			pOFSTCAS->WriteValue((float*)OFSTCAS);
		}
		else
		{
			WriteStatusToIdb("OFST",STATUS_UNDEF);
			pKQ->WriteStatus(STATUS_UNDEF);
			pQVCAS->WriteStatus(STATUS_UNDEF);
			pOFSTCAS->WriteStatus(STATUS_UNDEF);
		}
	}
}

void CProgCalc::CalcStartTEFF()
{
    float TimeTMValue = ReadValueFromIdb("TEFF_TM_SCAN");
    CTime CurTime; CurTime.SetCurrentTime();
    double dTimeSec = CurTime.Sec - *(int*)&TimeTMValue;    
    float NakzValue = ReadValueFromIdb("wNAKZ");
    float TeffValue = ReadValueFromIdb("TEFF_SCAN");
    TeffValue += (NakzValue/3000) * (dTimeSec/86400);
    WriteValueToIdb("TEFF_SCAN", TeffValue);
    WriteValueToIdb("TEFF_REG", TeffValue);
}


void CProgCalc::Calc_Tn(IdbValue* pResult, IdbValue* pTs, IdbValue* pTp1, IdbValue* pTp2, double kTs, double kTp1, double kTp2)
{// 3.10.1 ���ޣ� ���������� ������������� ������� ������� �� �� � ��

//puts(CStr("CProgCalc::Calc_Tn pTs->ok()=") + (int)pTs->ok() + " pTs->v()=" + pTs->v() + " pTp1->ok()=" + (int)pTp1->ok() + " pTp1->v()=" + pTp1->v() + " pTp2->ok()=" + (int)pTp2->ok() + " pTp2->v()=" + pTp2->v());

	pResult->cStatus = STATUS_NORMAL;
	if( pTs->ok() && pTp1->ok() && pTp2->ok() )	pResult->dValue = (kTs*pTs->v() + kTp1*pTp1->v() + kTp2*pTp2->v()) / (kTs + kTp1 + kTp2);
	else if( pTs->ok() && pTp1->ok() )			pResult->dValue = (kTs*pTs->v() + kTp1*pTp1->v()) / (kTs + kTp1);
	else if( pTs->ok() && pTp2->ok() )			pResult->dValue = (kTs*pTs->v() + kTp2*pTp2->v()) / (kTs + kTp2);
	else if( pTp1->ok() && pTp2->ok() )			pResult->dValue = (kTp1*pTp1->v() + kTp2*pTp2->v()) / (kTp1 + kTp2);
	else if( pTs->ok() )						pResult->dValue = pTs->v();
	else if( pTp1->ok() )
	{
		pResult->dValue = pTp1->v();
//puts(CStr("CProgCalc::Calc_Tn pTs->ok()=") + (int)pTs->ok() + " pTs->v()=" + pTs->v() + " pTp1->ok()=" + (int)pTp1->ok() + " pTp1->v()=" + pTp1->v());// + " pTp2->ok()=" + (int)pTp2->ok() + " pTp2->v()=" + pTp2->v());

	}
	else if( pTp2->ok() )						pResult->dValue = pTp2->v();
	else
	{
		pResult->dValue = (kTs*pTs->v() + kTp1*pTp1->v() + kTp2*pTp2->v()) / (kTs + kTp1 + kTp2);
		pResult->cStatus = STATUS_UNDEF;

//puts(CStr("***** CProgCalc::Calc_Tn pTs->ok()=") + (int)pTs->ok() + " pTs->v()=" + pTs->v() + " pTp1->ok()=" + (int)pTp1->ok() + " pTp1->v()=" + pTp1->v());// + " pTp2->ok()=" + (int)pTp2->ok() + " pTp2->v()=" + pTp2->v());
	}
}

void CProgCalc::Calc_DtPt(IdbValue* pResult, IdbValue* pThn,  IdbValue* pTgn)
{// 3.10.8 ������ ���������� ������������� � ������ ������� �������
	pResult->dValue 	= pTgn->v() - pThn->v();
	pResult->cStatus 	= pTgn->ok() && pThn->ok() ? STATUS_NORMAL : STATUS_UNDEF;
}

void CProgCalc::Calc_ThnTgnDt(int Step)
{// 3.10.1 ���ޣ� ���������� ������������� ������� �������

	// �������� ����������������� � ��
	IdbValue	Ttshn10;	ReadFromIdb("Ttshn10",&Ttshn10); //� � 1-�� �� 1� (�� ��) YA12T32
	IdbValue	Ttshn20;	ReadFromIdb("Ttshn20",&Ttshn20); //� � 2-�� �� 1� (�� ��) YA22T32
	IdbValue	Ttshn30;	ReadFromIdb("Ttshn30",&Ttshn30); //� � 3-�� �� 1� (�� ��) YA32T32
	IdbValue	Ttshn40;	ReadFromIdb("Ttshn40",&Ttshn40); //� � 4-�� �� 1� (�� ��) YA42T32

	// �������� ������ ������� � ��
	IdbValue	Ttp1hn10;	ReadFromIdb("Ttp1hn10",&Ttp1hn10); //� � 1-�� �� 1� (�� ��) YA12T01
	IdbValue	Ttp1hn20;	ReadFromIdb("Ttp1hn20",&Ttp1hn20); //� � 2-�� �� 1� (�� ��) YA22T01
	IdbValue	Ttp1hn30;	ReadFromIdb("Ttp1hn30",&Ttp1hn30); //� � 3-�� �� 1� (�� ��) YA32T01
	IdbValue	Ttp1hn40;	ReadFromIdb("Ttp1hn40",&Ttp1hn40); //� � 4-�� �� 1� (�� ��) YA42T01

	// �������� ������ ������� � ��
	IdbValue	Ttp2hn10;	ReadFromIdb("Ttp2hn10",&Ttp2hn10); //� � 1-�� �� 1� (�� ��) YA12T05
	IdbValue	Ttp2hn20;	ReadFromIdb("Ttp2hn20",&Ttp2hn20); //� � 2-�� �� 1� (�� ��) YA22T05
	IdbValue	Ttp2hn30;	ReadFromIdb("Ttp2hn30",&Ttp2hn30); //� � 3-�� �� 1� (�� ��) YA32T05
	IdbValue	Ttp2hn40;	ReadFromIdb("Ttp2hn40",&Ttp2hn40); //� � 4-�� �� 1� (�� ��) YA42T05

	// ��������� ������
	IdbValue	Spt10;		ReadFromIdb("Spt10",  &Spt10);  // ��������� 1-�� ����� 1�, ���/����/--/�������  YA10ST
	IdbValue	Spt20;		ReadFromIdb("Spt20",  &Spt20);  // ��������� 2-�� ����� 1�, ���/����/--/�������  YA20ST
	IdbValue	Spt30;		ReadFromIdb("Spt30",  &Spt30);  // ��������� 3-�� ����� 1�, ���/����/--/�������  YA30ST
	IdbValue	Spt40;		ReadFromIdb("Spt40",  &Spt40);  // ��������� 4-�� ����� 1�, ���/����/--/�������  YA40ST

	// �������� ����������������� � ��
	IdbValue	Ttsgn10;	ReadFromIdb("Ttsgn10",&Ttsgn10); //� � 1-�� �� 1� (�� ��) YA11T31
	IdbValue	Ttsgn20;	ReadFromIdb("Ttsgn20",&Ttsgn20); //� � 2-�� �� 1� (�� ��) YA21T31
	IdbValue	Ttsgn30;	ReadFromIdb("Ttsgn30",&Ttsgn30); //� � 3-�� �� 1� (�� ��) YA31T31
	IdbValue	Ttsgn40;	ReadFromIdb("Ttsgn40",&Ttsgn40); //� � 4-�� �� 1� (�� ��) YA41T31

	// �������� ������ ������� � ��
	IdbValue	Ttp1gn10;	ReadFromIdb("Ttp1gn10",&Ttp1gn10); //� � 1-�� �� 1� (�� ��) YA11T32
	IdbValue	Ttp1gn20;	ReadFromIdb("Ttp1gn20",&Ttp1gn20); //� � 2-�� �� 1� (�� ��) YA21T32
	IdbValue	Ttp1gn30;	ReadFromIdb("Ttp1gn30",&Ttp1gn30); //� � 3-�� �� 1� (�� ��) YA31T32
	IdbValue	Ttp1gn40;	ReadFromIdb("Ttp1gn40",&Ttp1gn40); //� � 4-�� �� 1� (�� ��) YA41T32

	// �������� ������ ������� � ��
	IdbValue	Ttp2gn10;	ReadFromIdb("Ttp2gn10",&Ttp2gn10); //� � 1-�� �� 1� (�� ��) YA11T27
	IdbValue	Ttp2gn20;	ReadFromIdb("Ttp2gn20",&Ttp2gn20); //� � 1-�� �� 1� (�� ��) YA21T27
	IdbValue	Ttp2gn30;	ReadFromIdb("Ttp2gn30",&Ttp2gn30); //� � 1-�� �� 1� (�� ��) YA31T27
	IdbValue	Ttp2gn40;	ReadFromIdb("Ttp2gn40",&Ttp2gn40); //� � 1-�� �� 1� (�� ��) YA41T27


	// �������������� �������� ���������� � ���������� � ������
	IdbValue	Thn10;  	// T � 1-�� �� 1�, ���� YA12T
	IdbValue	Thn20;  	// T � 2-�� �� 1�, ���� YA22T
	IdbValue	Thn30;  	// T � 3-�� �� 1�, ���� YA32T
	IdbValue	Thn40;  	// T � 4-�� �� 1�, ���� YA42T

	IdbValue	Tgn10;  	// T � 1-�� �� 1�, ����	YA11T
	IdbValue	Tgn20;  	// T � 2-�� �� 1�, ����	YA21T
	IdbValue	Tgn30;  	// T � 3-�� �� 1�, ����	YA31T
	IdbValue	Tgn40;  	// T � 4-�� �� 1�, ����	YA41T

	IdbValue	DTpt10;  	// DT � ����� 1, ���� YA10T_DT
	IdbValue	DTpt20;  	// DT � ����� 2, ���� YA20T_DT
	IdbValue	DTpt30;  	// DT � ����� 3, ���� YA30T_DT
	IdbValue	DTpt40;  	// DT � ����� 4, ���� YA40T_DT


	// ������� ������������ �� � ��
	double	Ktshn10		= pConstTbl->GetConstValue("KTSHN10");
	double	Ktshn20		= pConstTbl->GetConstValue("KTSHN20");
	double	Ktshn30		= pConstTbl->GetConstValue("KTSHN30");
	double	Ktshn40		= pConstTbl->GetConstValue("KTSHN40");

	double	Ktsgn10		= pConstTbl->GetConstValue("KTSGN10");
	double	Ktsgn20		= pConstTbl->GetConstValue("KTSGN20");
	double	Ktsgn30		= pConstTbl->GetConstValue("KTSGN30");
	double	Ktsgn40		= pConstTbl->GetConstValue("KTSGN40");

	double	Ktp1hn10	= pConstTbl->GetConstValue("KTP1HN10");
	double	Ktp1hn20	= pConstTbl->GetConstValue("KTP1HN20");
	double	Ktp1hn30	= pConstTbl->GetConstValue("KTP1HN30");
	double	Ktp1hn40	= pConstTbl->GetConstValue("KTP1HN40");

	double	Ktp2hn10	= pConstTbl->GetConstValue("KTP2HN10");
	double	Ktp2hn20	= pConstTbl->GetConstValue("KTP2HN20");
	double	Ktp2hn30	= pConstTbl->GetConstValue("KTP2HN30");
	double	Ktp2hn40	= pConstTbl->GetConstValue("KTP2HN40");

	double	Ktp1gn10	= pConstTbl->GetConstValue("KTP1GN10");
	double	Ktp1gn20	= pConstTbl->GetConstValue("KTP1GN20");
	double	Ktp1gn30	= pConstTbl->GetConstValue("KTP1GN30");
	double	Ktp1gn40	= pConstTbl->GetConstValue("KTP1GN40");

	double	Ktp2gn10	= pConstTbl->GetConstValue("KTP2GN10");
	double	Ktp2gn20	= pConstTbl->GetConstValue("KTP2GN20");
	double	Ktp2gn30	= pConstTbl->GetConstValue("KTP2GN30");
	double	Ktp2gn40	= pConstTbl->GetConstValue("KTP2GN40");

	double	EpsHn10		= pConstTbl->GetConstValue("EPSHN10");
	double	EpsHn20		= pConstTbl->GetConstValue("EPSHN20");
	double	EpsHn30		= pConstTbl->GetConstValue("EPSHN30");
	double	EpsHn40		= pConstTbl->GetConstValue("EPSHN40");

	double	EpsGn10		= pConstTbl->GetConstValue("EPSGN10");
	double	EpsGn20		= pConstTbl->GetConstValue("EPSGN20");
	double	EpsGn30		= pConstTbl->GetConstValue("EPSGN30");
	double	EpsGn40		= pConstTbl->GetConstValue("EPSGN40");

	if( Step==2 )
	{// ����� ����������� ��������� ������

		int	GoodPtCnt = 0;
		if( Spt10.ok() && Spt10.v()==STPT_ON ) GoodPtCnt++;
		if( Spt20.ok() && Spt20.v()==STPT_ON ) GoodPtCnt++;
		if( Spt30.ok() && Spt30.v()==STPT_ON ) GoodPtCnt++;
		if( Spt40.ok() && Spt40.v()==STPT_ON ) GoodPtCnt++;



		// �������� ������� ����������� �� ��� ���������� ������
		double	sumThn 	= 0.0;
		double	sumDhn 	= 0.0;

		if( ((Spt10.ok() && Spt10.v()==STPT_ON) || !GoodPtCnt) && (Ttshn10.ok() || Ttp1hn10.ok() || Ttp2hn10.ok() ) )
		{// ���� 1 ����� �������� � ���� �� ���� ������ �����
			sumThn	+= Ttshn10.ok()?Ttshn10.v():0.0 + Ttp1hn10.ok()?Ttp1hn10.v():0.0 + Ttp2hn10.ok()?Ttp2hn10.v():0.0;
			sumDhn 	+= Ttshn10.ok()?1.0:0.0 + Ttp1hn10.ok()?1.0:0.0 + Ttp2hn10.ok()?1.0:0.0;
		}

		if( ((Spt20.ok() && Spt20.v()==STPT_ON) || !GoodPtCnt) && (Ttshn20.ok() || Ttp1hn20.ok() || Ttp2hn20.ok() ) )
		{// ���� 2 ����� �������� � ���� �� ���� ������ �����
			sumThn	+= Ttshn20.ok()?Ttshn20.v():0.0 + Ttp1hn20.ok()?Ttp1hn20.v():0.0 + Ttp2hn20.ok()?Ttp2hn20.v():0.0;
			sumDhn 	+= Ttshn20.ok()?1.0:0.0 + Ttp1hn20.ok()?1.0:0.0 + Ttp2hn20.ok()?1.0:0.0;
		}

		if( ((Spt30.ok() && Spt30.v()==STPT_ON) || !GoodPtCnt) && (Ttshn30.ok() || Ttp1hn30.ok() || Ttp2hn30.ok() ) )
		{// ���� 3 ����� �������� � ���� �� ���� ������ �����
			sumThn	+= Ttshn30.ok()?Ttshn30.v():0.0 + Ttp1hn30.ok()?Ttp1hn30.v():0.0 + Ttp2hn30.ok()?Ttp2hn30.v():0.0;
			sumDhn 	+= Ttshn30.ok()?1.0:0.0 + Ttp1hn30.ok()?1.0:0.0 + Ttp2hn30.ok()?1.0:0.0;
		}

		if( ((Spt40.ok() && Spt40.v()==STPT_ON) || !GoodPtCnt) && (Ttshn40.ok() || Ttp1hn40.ok() || Ttp2hn40.ok() ) )
		{// ���� 4 ����� �������� � ���� �� ���� ������ �����
			sumThn	+= Ttshn40.ok()?Ttshn40.v():0.0 + Ttp1hn40.ok()?Ttp1hn40.v():0.0 + Ttp2hn40.ok()?Ttp2hn40.v():0.0;
			sumDhn 	+= Ttshn40.ok()?1.0:0.0 + Ttp1hn40.ok()?1.0:0.0 + Ttp2hn40.ok()?1.0:0.0;
		}

		// ������� ����������� � �� �� ���������� �������� ��� ����� ������� ������������� (����);
		double 	AvThn = sumDhn>0 ? sumThn/sumDhn : 0.0;



		// �������� ������� ����������� �� ��� ���������� ������
		double	sumTgn	= 0.0;
		double	sumDgn 	= 0.0;

		if( ((Spt10.ok() && Spt10.v()==STPT_ON) || !GoodPtCnt) && (Ttsgn10.ok() || Ttp1gn10.ok() || Ttp2gn10.ok() ) )
		{// ���� 1 ����� �������� � ���� �� ���� ������ �����
			sumTgn	+= Ttsgn10.ok()?Ttsgn10.v():0.0 + Ttp1gn10.ok()?Ttp1gn10.v():0.0 + Ttp2gn10.ok()?Ttp2gn10.v():0.0;
			sumDgn 	+= Ttsgn10.ok()?1.0:0.0 + Ttp1gn10.ok()?1.0:0.0 + Ttp2gn10.ok()?1.0:0.0;
		}

		if( ((Spt20.ok() && Spt20.v()==STPT_ON) || !GoodPtCnt) && (Ttsgn20.ok() || Ttp1gn20.ok() || Ttp2gn20.ok() ) )
		{// ���� 2 ����� �������� � ���� �� ���� ������ �����
			sumTgn	+= Ttsgn20.ok()?Ttsgn20.v():0.0 + Ttp1gn20.ok()?Ttp1gn20.v():0.0 + Ttp2gn20.ok()?Ttp2gn20.v():0.0;
			sumDgn 	+= Ttsgn20.ok()?1.0:0.0 + Ttp1gn20.ok()?1.0:0.0 + Ttp2gn20.ok()?1.0:0.0;
		}

		if( ((Spt30.ok() && Spt30.v()==STPT_ON) || !GoodPtCnt) && (Ttsgn30.ok() || Ttp1gn30.ok() || Ttp2gn30.ok() ) )
		{// ���� 3 ����� �������� � ���� �� ���� ������ �����
			sumTgn	+= Ttsgn30.ok()?Ttsgn30.v():0.0 + Ttp1gn30.ok()?Ttp1gn30.v():0.0 + Ttp2gn30.ok()?Ttp2gn30.v():0.0;
			sumDgn 	+= Ttsgn30.ok()?1.0:0.0 + Ttp1gn30.ok()?1.0:0.0 + Ttp2gn30.ok()?1.0:0.0;
		}

		if( ((Spt40.ok() && Spt40.v()==STPT_ON) || !GoodPtCnt) && (Ttsgn40.ok() || Ttp1gn40.ok() || Ttp2gn40.ok() ) )
		{// ���� 4 ����� �������� � ���� �� ���� ������ �����
			sumTgn	+= Ttsgn40.ok()?Ttsgn40.v():0.0 + Ttp1gn40.ok()?Ttp1gn40.v():0.0 + Ttp2gn40.ok()?Ttp2gn40.v():0.0;
			sumDgn 	+= Ttsgn40.ok()?1.0:0.0 + Ttp1gn40.ok()?1.0:0.0 + Ttp2gn40.ok()?1.0:0.0;
		}

		// ������� ����������� � �� �� ���������� �������� ��� ����� ������� ������������� (����);
		double 	AvTgn = sumDgn>0 ? sumTgn/sumDgn : 0.0;

		// ��������������� ������� �� �� , ��� ������� �� ����������� ������� At-t<eps
		if( Ttshn10.ok() && FXABS(AvThn - Ttshn10.v()) 	 >= EpsHn10  )	Ttshn10.cStatus = STATUS_UNDEF;
		if( Ttp1hn10.ok() && FXABS(AvThn - Ttp1hn10.v()) >= EpsHn10  )	Ttp1hn10.cStatus = STATUS_UNDEF;
		if( Ttp2hn10.ok() && FXABS(AvThn - Ttp2hn10.v()) >= EpsHn10  )	Ttp2hn10.cStatus = STATUS_UNDEF;

		if( Ttshn20.ok() && FXABS(AvThn - Ttshn20.v()) 	 >= EpsHn20  )	Ttshn20.cStatus = STATUS_UNDEF;
		if( Ttp1hn20.ok() && FXABS(AvThn - Ttp1hn20.v()) >= EpsHn20  )	Ttp1hn20.cStatus = STATUS_UNDEF;
		if( Ttp2hn20.ok() && FXABS(AvThn - Ttp2hn20.v()) >= EpsHn20  )	Ttp2hn20.cStatus = STATUS_UNDEF;

		if( Ttshn30.ok() && FXABS(AvThn - Ttshn30.v()) 	 >= EpsHn30  )	Ttshn30.cStatus = STATUS_UNDEF;
		if( Ttp1hn30.ok() && FXABS(AvThn - Ttp1hn30.v()) >= EpsHn30  )	Ttp1hn30.cStatus = STATUS_UNDEF;
		if( Ttp2hn30.ok() && FXABS(AvThn - Ttp2hn30.v()) >= EpsHn30  )	Ttp2hn30.cStatus = STATUS_UNDEF;

		if( Ttshn40.ok() && FXABS(AvThn - Ttshn40.v()) 	 >= EpsHn40  )	Ttshn40.cStatus = STATUS_UNDEF;
		if( Ttp1hn40.ok() && FXABS(AvThn - Ttp1hn40.v()) >= EpsHn40  )	Ttp1hn40.cStatus = STATUS_UNDEF;
		if( Ttp2hn40.ok() && FXABS(AvThn - Ttp2hn40.v()) >= EpsHn40  )	Ttp2hn40.cStatus = STATUS_UNDEF;

		// ��������������� ������� �� �� , ��� ������� �� ����������� ������� At-t<eps
		if( Ttsgn10.ok() && FXABS(AvTgn - Ttsgn10.v())   >= EpsGn10  )	Ttsgn10.cStatus = STATUS_UNDEF;
		if( Ttp1gn10.ok() && FXABS(AvTgn - Ttp1gn10.v()) >= EpsGn10  )	Ttp1gn10.cStatus = STATUS_UNDEF;
		if( Ttp2gn10.ok() && FXABS(AvTgn - Ttp2gn10.v()) >= EpsGn10  )	Ttp2gn10.cStatus = STATUS_UNDEF;

		if( Ttsgn20.ok() && FXABS(AvTgn - Ttsgn20.v()) 	 >= EpsGn20  )	Ttsgn20.cStatus = STATUS_UNDEF;
		if( Ttp1gn20.ok() && FXABS(AvTgn - Ttp1gn20.v()) >= EpsGn20  )	Ttp1gn20.cStatus = STATUS_UNDEF;
		if( Ttp2gn20.ok() && FXABS(AvTgn - Ttp2gn20.v()) >= EpsGn20  )	Ttp2gn20.cStatus = STATUS_UNDEF;

		if( Ttsgn30.ok() && FXABS(AvTgn - Ttsgn30.v()) 	 >= EpsGn30  )	Ttsgn30.cStatus = STATUS_UNDEF;
		if( Ttp1gn30.ok() && FXABS(AvTgn - Ttp1gn30.v()) >= EpsGn30  )	Ttp1gn30.cStatus = STATUS_UNDEF;
		if( Ttp2gn30.ok() && FXABS(AvTgn - Ttp2gn30.v()) >= EpsGn30  )	Ttp2gn30.cStatus = STATUS_UNDEF;

		if( Ttsgn40.ok() && FXABS(AvTgn - Ttsgn40.v()) 	 >= EpsGn40  )	Ttsgn40.cStatus = STATUS_UNDEF;
		if( Ttp1gn40.ok() && FXABS(AvTgn - Ttp1gn40.v()) >= EpsGn40  )	Ttp1gn40.cStatus = STATUS_UNDEF;
		if( Ttp2gn40.ok() && FXABS(AvTgn - Ttp2gn40.v()) >= EpsGn40  )	Ttp2gn40.cStatus = STATUS_UNDEF;

		// ������� �� ��������� ������ ���������� � ������ � ����������
	}

	// ����������� ��
	Calc_Tn(&Thn10, &Ttshn10, &Ttp1hn10, &Ttp2hn10, Ktshn10, Ktp1hn10, Ktp2hn10);
	Calc_Tn(&Thn20, &Ttshn20, &Ttp1hn20, &Ttp2hn20, Ktshn20, Ktp1hn20, Ktp2hn20);
	Calc_Tn(&Thn30, &Ttshn30, &Ttp1hn30, &Ttp2hn30, Ktshn30, Ktp1hn30, Ktp2hn30);
	Calc_Tn(&Thn40, &Ttshn40, &Ttp1hn40, &Ttp2hn40, Ktshn40, Ktp1hn40, Ktp2hn40);

	// ����������� ��
	Calc_Tn(&Tgn10, &Ttsgn10, &Ttp1gn10, &Ttp2gn10, Ktsgn10, Ktp1gn10, Ktp2gn10);
	Calc_Tn(&Tgn20, &Ttsgn20, &Ttp1gn20, &Ttp2gn20, Ktsgn20, Ktp1gn20, Ktp2gn20);
	Calc_Tn(&Tgn30, &Ttsgn30, &Ttp1gn30, &Ttp2gn30, Ktsgn30, Ktp1gn30, Ktp2gn30);
	Calc_Tn(&Tgn40, &Ttsgn40, &Ttp1gn40, &Ttp2gn40, Ktsgn40, Ktp1gn40, Ktp2gn40);

	// ��������� �� �����
	Calc_DtPt(&DTpt10, &Thn10,  &Tgn10);
	Calc_DtPt(&DTpt20, &Thn20,  &Tgn20);
	Calc_DtPt(&DTpt30, &Thn30,  &Tgn30);
	Calc_DtPt(&DTpt40, &Thn40,  &Tgn40);

	// ������� ���������� � ���
	WriteToIdb("Thn10",&Thn10); 	// T � 1-�� �� 1�, ����
	WriteToIdb("Tgn10",&Tgn10); 	// T � 1-�� �� 1�, ����
	WriteToIdb("DTpt10",&DTpt10); 	// DT � ����� 1, ����

	WriteToIdb("Thn20",&Thn20); 	// T � 2-�� �� 1�, ����
	WriteToIdb("Tgn20",&Tgn20); 	// T � 2-�� �� 1�, ����
	WriteToIdb("DTpt20",&DTpt20); 	// DT � ����� 2, ����

	WriteToIdb("Thn30",&Thn30); 	// T � 3-�� �� 1�, ����
	WriteToIdb("Tgn30",&Tgn30); 	// T � 3-�� �� 1�, ����
	WriteToIdb("DTpt30",&DTpt30); 	// DT � ����� 3, ����

	WriteToIdb("Thn40",&Thn40); 	// T � 4-�� �� 1�, ����
	WriteToIdb("Tgn40",&Tgn40); 	// T � 4-�� �� 1�, ����
	WriteToIdb("DTpt40",&DTpt40); 	// DT � ����� 4, ����
}

void CProgCalc::Calc_StPt(IdbValue* pResult, IdbValue* pOn,  IdbValue* pOff, IdbValue* pDt, IdbValue* pDp, double DT0, double DP0)
{// 3.13.1 ����������� ��������� ������ ������� �������

	int	Sgcn 	= 0; 	// �������������� ���������  ���
	int Spt	 	= -1;

	if( pOn->ok() && pOff->ok() )
	{
		if( 	 pOn->v()==1 && pOff->v()==0 ) 	Sgcn = 1; // ���������� ��������� ���
		else if( pOn->v()==0 && pOff->v()==1 ) 	Sgcn = 2; // ����������� ��������� ���
	}

	if( Sgcn==1 )
	{// ���������� ��������� ���
		if( 	 pDt->ok() && pDt->v()>DT0 )
		{
			Spt = STPT_ON;
		}
		else if( pDt->ok() && pDt->v()<=DT0 )
		{
			if( 	 pDp->ok() && pDp->v()>DP0 ) 	Spt = STPT_ON;
			else if( pDp->ok() && pDp->v()<=DP0 ) 	Spt = STPT_OFF;
			else if( !pDp->ok() ) 					Spt = STPT_UNDEF;
		}
		else if( !pDt->ok() )
		{
			if( 	 pDp->ok() && pDp->v()>DP0 ) 	Spt = STPT_ON;
			else if( pDp->ok() && pDp->v()<=DP0 ) 	Spt = STPT_UNDEF;
			else if( !pDp->ok() ) 					Spt = STPT_ON;
		}
	}
	else if( Sgcn==2 )
	{// ����������� ��������� ���
		if( 	 pDt->ok() && pDt->v()>DT0 )
		{
			if( 	 pDp->ok() && pDp->v()>DP0 ) 	Spt = STPT_ON;
			else if( pDp->ok() && pDp->v()<=DP0 ) 	Spt = STPT_OFF;
			else if( !pDp->ok() ) 					Spt = STPT_UNDEF;
		}
		else if( pDt->ok() && pDt->v()<=DT0 )
		{
			Spt = STPT_OFF;
		}
		else if( !pDt->ok() )
		{
			if( 	 pDp->ok() && pDp->v()>DP0 ) 	Spt = STPT_UNDEF;
			else if( pDp->ok() && pDp->v()<=DP0 ) 	Spt = STPT_OFF;
			else if( !pDp->ok() ) 					Spt = STPT_OFF;
		}
	}
	else
	{// �������������� ���������  ���
		if( 	 pDt->ok() && pDt->v()>DT0 )
		{
			if( 	 pDp->ok() && pDp->v()>DP0 ) 	Spt = STPT_ON;
			else if( pDp->ok() && pDp->v()<=DP0 ) 	Spt = STPT_UNDEF;
			else if( !pDp->ok() ) 					Spt = STPT_ON;
		}
		else if( pDt->ok() && pDt->v()<=DT0 )
		{
			if( 	 pDp->ok() && pDp->v()>DP0 ) 	Spt = STPT_ON;
			else if( pDp->ok() && pDp->v()<=DP0 ) 	Spt = STPT_OFF;
			else if( !pDp->ok() ) 					Spt = STPT_UNDEF;
		}
		else if( !pDt->ok() )
		{
			if( 	 pDp->ok() && pDp->v()>DP0 ) 	Spt = STPT_ON;
			else if( pDp->ok() && pDp->v()<=DP0 ) 	Spt = STPT_OFF;
			else if( !pDp->ok() ) 					Spt = STPT_UNDEF;
		}
	}

	if( Spt==-1 )
	{// �� ������ ���������� ���������
		pResult->dValue 	= 3;
		pResult->cStatus 	= STATUS_UNDEF;
	}
	else
	{
		pResult->dValue 	= Spt;
		pResult->cStatus 	= STATUS_NORMAL;
	}
}




void CProgCalc::Calc_StPt()
{// 3.13.1 ����������� ��������� ������ ������� �������

	// �������� ��������
	IdbValue	GcnOn10;	ReadFromIdb("GcnOn10",	&GcnOn10);	// ��������� ���-1, ���/��  YD10D01B1
	IdbValue	GcnOn20;	ReadFromIdb("GcnOn20",	&GcnOn20);	// ��������� ���-2, ���/��  YD20D01B1
	IdbValue	GcnOn30;	ReadFromIdb("GcnOn30",	&GcnOn30);	// ��������� ���-3, ���/��  YD30D01B1
	IdbValue	GcnOn40;	ReadFromIdb("GcnOn40",	&GcnOn40);	// ��������� ���-4, ���/��  YD40D01B1

	IdbValue	GcnOff10;	ReadFromIdb("GcnOff10",	&GcnOff10);	// ���������� ���-1, ���/�� YD10D01B2
	IdbValue	GcnOff20;	ReadFromIdb("GcnOff20",	&GcnOff20);	// ���������� ���-1, ���/�� YD20D01B2
	IdbValue	GcnOff30;	ReadFromIdb("GcnOff30",	&GcnOff30);	// ���������� ���-1, ���/�� YD30D01B2
	IdbValue	GcnOff40;	ReadFromIdb("GcnOff40",	&GcnOff40);	// ���������� ���-1, ���/�� YD40D01B2

	IdbValue	DTpt10;		ReadFromIdb("DTpt10",	&DTpt10);	// DT � ����� 1, ���� YA10T_DT
	IdbValue	DTpt20;		ReadFromIdb("DTpt20",	&DTpt20);	// DT � ����� 2, ���� YA20T_DT
	IdbValue	DTpt30;		ReadFromIdb("DTpt30",	&DTpt30);	// DT � ����� 3, ���� YA30T_DT
	IdbValue	DTpt40;		ReadFromIdb("DTpt40",	&DTpt40);	// DT � ����� 4, ���� YA40T_DT

	IdbValue	DPgcn10;	ReadFromIdb("DPgcn10", &DPgcn10);	// DP �� ���-1, ��/��2 YA10P18
	IdbValue	DPgcn20;	ReadFromIdb("DPgcn20", &DPgcn20);	// DP �� ���-2, ��/��2 YA20P18
	IdbValue	DPgcn30;	ReadFromIdb("DPgcn30", &DPgcn30);	// DP �� ���-3, ��/��2 YA30P18
	IdbValue	DPgcn40;	ReadFromIdb("DPgcn40", &DPgcn40);	// DP �� ���-4, ��/��2 YA40P18

	// �������������� �������� ��������� ������
	IdbValue	Spt10;	// ��������� 1-�� ����� 1�, ���/����/--/�������  YA10ST
	IdbValue	Spt20;	// ��������� 2-�� ����� 1�, ���/����/--/�������  YA20ST
	IdbValue	Spt30;	// ��������� 3-�� ����� 1�, ���/����/--/�������  YA30ST
	IdbValue	Spt40;	// ��������� 4-�� ����� 1�, ���/����/--/�������  YA40ST

	// ���������
	double	MINDTPT0	= pConstTbl->GetConstValue("MINDTPT0");	// ����������� �������� ��������� � �����
	double	MINDPPT0	= pConstTbl->GetConstValue("MINDPPT0");	// ����������� �������� �������� �������� �� ���
	//??? ���� ���������� �������� DT0 � DP0

	Calc_StPt(&Spt10, &GcnOn10, &GcnOff10, &DTpt10, &DPgcn10, MINDTPT0, MINDPPT0);
	Calc_StPt(&Spt20, &GcnOn20, &GcnOff20, &DTpt20, &DPgcn20, MINDTPT0, MINDPPT0);
	Calc_StPt(&Spt30, &GcnOn30, &GcnOff30, &DTpt30, &DPgcn30, MINDTPT0, MINDPPT0);
	Calc_StPt(&Spt40, &GcnOn40, &GcnOff40, &DTpt40, &DPgcn40, MINDTPT0, MINDPPT0);

	// ������� ���������� � ���
	WriteToIdb("Spt10",&Spt10);
	WriteToIdb("Spt20",&Spt20);
	WriteToIdb("Spt30",&Spt30);
	WriteToIdb("Spt40",&Spt40);
}


void CProgCalc::Calc_Avg(IdbValue* pResult, LONG_LST*	pIdbValueLst, LONG_LST*	pStPtValueLst, bool bAllDost)
{
	bool	bUseStPt 	= pStPtValueLst && pIdbValueLst->GetCnt()==pStPtValueLst->GetCnt() ? true : false;
	int		SumDostCnt	= 0;
	double	SumValue	= 0.0;
	double	AvgValue	= 0.0;
	char	stAvgValue	= STATUS_NORMAL;

	if( bUseStPt )
	{// ��������� ��������� ������
		int		OnPtCnt = 0;

		// ���������� ���������� ���������� ������
		for( int ValPos=1; ValPos<=pStPtValueLst->GetCnt(); ValPos++ )
			if( ((IdbValue*)pStPtValueLst->Get(ValPos))->ok() && ((IdbValue*)pStPtValueLst->Get(ValPos))->v()==STPT_ON )
				OnPtCnt	+= 1;

		if( OnPtCnt )
		{// ���� ���� ���������� ����� - �� ��������� ������ ����������� ��������� �� ���������� ������

			for( int ValPos=1; ValPos<=pIdbValueLst->GetCnt(); ValPos++ )
				if( ((IdbValue*)pIdbValueLst->Get(ValPos))->ok() && ((IdbValue*)pStPtValueLst->Get(ValPos))->ok() && ((IdbValue*)pStPtValueLst->Get(ValPos))->v()==STPT_ON )
				{
					SumValue 	+= ((IdbValue*)pIdbValueLst->Get(ValPos))->v();
					SumDostCnt++;
				}
		}
		else
		{// ���� ��� ���������� ������ - ��������� ��� �����������
			for( int ValPos=1; ValPos<=pIdbValueLst->GetCnt(); ValPos++ )
				if( ((IdbValue*)pIdbValueLst->Get(ValPos))->ok() )
				{
					SumValue 	+= ((IdbValue*)pIdbValueLst->Get(ValPos))->v();
					SumDostCnt++;
				}
		}

		if( SumDostCnt>0 )
		{
			AvgValue	= SumValue/(double)SumDostCnt;
		}
		else
		{// �� ������� ��������� �������
			stAvgValue	= STATUS_UNDEF;
		}
	}
	else
	{// ��� ����� ��������� ������

		// ���������� ���������� ����������� ��������
		for( int ValPos=1; ValPos<=pIdbValueLst->GetCnt(); ValPos++ )
			if( ((IdbValue*)pIdbValueLst->Get(ValPos))->ok() )
				SumDostCnt 	+= 1;


		if( (bAllDost && SumDostCnt==pIdbValueLst->GetCnt() ) || (!bAllDost && SumDostCnt>0 ) )
		{// ���� ���� ����������� ��������, �� ������ ������ �� �����������
			for( int ValPos=1; ValPos<=pIdbValueLst->GetCnt(); ValPos++ )
				if( ((IdbValue*)pIdbValueLst->Get(ValPos))->ok() )
					SumValue 	+= ((IdbValue*)pIdbValueLst->Get(ValPos))->v();
			AvgValue	= SumValue/(double)SumDostCnt;
		}
		else
		{// ���� ����������� ��� �� ������� �� ���� ���������, �� ������ ����� �������������
			for( int ValPos=1; ValPos<=pIdbValueLst->GetCnt(); ValPos++ )
				SumValue 	+= ((IdbValue*)pIdbValueLst->Get(ValPos))->v();
			AvgValue	= SumValue/(double)pIdbValueLst->GetCnt();
			stAvgValue	= STATUS_UNDEF;
		}
	}

	// ��������
	pResult->dValue  = AvgValue;
	pResult->cStatus = stAvgValue;
}



void CProgCalc::Calc_Sum(IdbValue* pResult, LONG_LST*	pIdbValueLst, bool bAllDost)
{
	int		SumDostCnt	= 0;
	double	SumValue	= 0.0;
	char	stSumValue	= STATUS_NORMAL;

	// ���������� ���������� ����������� ��������
	for( int ValPos=1; ValPos<=pIdbValueLst->GetCnt(); ValPos++ )
		if( ((IdbValue*)pIdbValueLst->Get(ValPos))->ok() )
			SumDostCnt 	+= 1;


	if( (bAllDost && SumDostCnt==pIdbValueLst->GetCnt() ) || (!bAllDost && SumDostCnt>0 ) )
	{// ���� ���� ����������� ��������, �� ������ ������ �� �����������
		for( int ValPos=1; ValPos<=pIdbValueLst->GetCnt(); ValPos++ )
			if( ((IdbValue*)pIdbValueLst->Get(ValPos))->ok() )
				SumValue 	+= ((IdbValue*)pIdbValueLst->Get(ValPos))->v();
	}
	else
	{// ���� ����������� ��� �� ������� �� ���� ���������, �� ������ ����� �������������
		for( int ValPos=1; ValPos<=pIdbValueLst->GetCnt(); ValPos++ )
			SumValue 	+= ((IdbValue*)pIdbValueLst->Get(ValPos))->v();
		stSumValue	= STATUS_UNDEF;
	}

	// ��������
	pResult->dValue  = SumValue;
	pResult->cStatus = stSumValue;
}



void CProgCalc::Calc_Avg()
{// 3.9 ����������� ������� �������� ��������� �������� ����������� � �������� � �� � ����������� � ������� ������ ��������

	LONG_LST	PznLst;
	IdbValue	Pzn_1;		ReadFromIdb("Pzn1",	&Pzn_1);	PznLst.Add((int)&Pzn_1);	// P ��� ���, ��/��2 YC70P04
	IdbValue	Pzn_2;		ReadFromIdb("Pzn2",	&Pzn_2);	PznLst.Add((int)&Pzn_2);	// P ��� ���, ��/��2 YC30P05
	IdbValue	Pzn_3;		ReadFromIdb("Pzn3",	&Pzn_3);	PznLst.Add((int)&Pzn_3);	// P ��� ���, ��/��2 YC30P06

	LONG_LST	PpgLst;
	IdbValue	Ppg10;		ReadFromIdb("Ppg10",	&Ppg10);	PpgLst.Add((int)&Ppg10);	// P ���� � ��-1, ��/��2 YB10P10
	IdbValue	Ppg20;		ReadFromIdb("Ppg20",	&Ppg20);	PpgLst.Add((int)&Ppg20);	// P ���� � ��-2, ��/��2 YB20P10
	IdbValue	Ppg30;		ReadFromIdb("Ppg30",	&Ppg30);	PpgLst.Add((int)&Ppg30);	// P ���� � ��-3, ��/��2 YB30P10
	IdbValue	Ppg40;		ReadFromIdb("Ppg40",	&Ppg40);	PpgLst.Add((int)&Ppg40);	// P ���� � ��-4, ��/��2 YB40P10

	LONG_LST	TpwLst;
	IdbValue	Tpw10;		ReadFromIdb("Tpw10",	&Tpw10);	TpwLst.Add((int)&Tpw10);	// T �� ����� ��-1, ���� RL71T01
	IdbValue	Tpw20;		ReadFromIdb("Tpw20",	&Tpw20);	TpwLst.Add((int)&Tpw20);	// T �� ����� ��-2, ���� RL72T01
	IdbValue	Tpw30;		ReadFromIdb("Tpw30",	&Tpw30);	TpwLst.Add((int)&Tpw30);	// T �� ����� ��-3, ���� RL73T01
	IdbValue	Tpw40;		ReadFromIdb("Tpw40",	&Tpw40);	TpwLst.Add((int)&Tpw40);	// T �� ����� ��-4, ���� RL74T01

	LONG_LST	SptLst;
	IdbValue	Spt10;  	ReadFromIdb("Spt10",	&Spt10);	SptLst.Add((int)&Spt10);	// ��������� 1-�� ����� 1�, ���/����/--/������� YA10ST
	IdbValue	Spt20;  	ReadFromIdb("Spt20",	&Spt20);	SptLst.Add((int)&Spt20);	// ��������� 1-�� ����� 1�, ���/����/--/������� YA20ST
	IdbValue	Spt30;  	ReadFromIdb("Spt30",	&Spt30);	SptLst.Add((int)&Spt30);	// ��������� 1-�� ����� 1�, ���/����/--/������� YA30ST
	IdbValue	Spt40;  	ReadFromIdb("Spt40",	&Spt40);	SptLst.Add((int)&Spt40);	// ��������� 1-�� ����� 1�, ���/����/--/������� YA40ST

	// �������������� ��������
	IdbValue	APzn;  		// ��. P ��� ���, ��/��2 		YC70P_A
	IdbValue	APpg;	  	// ��. P ���� � ��, ��/��2 		YBP_A
	IdbValue	ATpw;  		// ������� � ���. ����, ���� 	RLT_A

	Calc_Avg(&APzn, &PznLst, NULL);
	Calc_Avg(&APpg, &PpgLst, &SptLst);
	Calc_Avg(&ATpw, &TpwLst, &SptLst);

	// ������� ���������� � ���
	// ??? ���� ���������� ��� � RegIdb
	WriteToIdb("APzn",&APzn);		// ��. P ��� ���, ��/��2
	WriteToIdb("APpg",&APpg);		// ��. P ���� � ��, ��/��2
	WriteToIdb("ATpw",&ATpw);		// ������� � ���. ����, ����
}

void CProgCalc::Calc_Gpt(IdbValue* pGVpt, IdbValue* pGMpt, IdbValue* pDpt, IdbValue* pStPt, IdbValue* pFpt, IdbValue* pDPpt, IdbValue* pThn, double YcpIn, double A0,  double A1,  double A2, double KCIO, double HOBI, double Density0, double Frequency_0, double GcnRes)
{
	double	GV = 0.0;	char stGV = STATUS_UNDEF;
	double	Freequency	= pFpt->ok() ? pFpt->v() : Frequency_0;
	double	Density 	= 1000.0*WaterSteamProperty::getWaterDensity(YcpIn, pThn->v()); // ��/�3
	double	DPgcn		= GcnRes + pDPpt->v();///1.033227453; // ���




//puts(CStr("CProgCalc::Calc_Gpt Density=") + Density + " YcpIn=" + YcpIn + " pThn->v()=" + pThn->v());


	if( pStPt->ok() && pStPt->v()==STPT_ON && pDPpt->ok() )
	{// ���� ����� �������� � ���������� �������
		double K	= 10000.0*DPgcn/Density * pow(Frequency_0/Freequency, 2);
		GV 			= (Freequency/Frequency_0) * (A0 + A1*K + A2*K*K)/* / 1000.0*/;

/*
puts(CStr("CProgCalc::Calc_Gpt A0=") + A0 + " A1=" + A1 + " A2=" + A2);
puts(CStr("                  F=") + Freequency + " F0=" + Frequency_0 + " R=" + Density + " Ro=" + Density0);
puts(CStr("                  Dp=") + pDPpt->v() + "(���/��2) Dp=" + (pDPpt->v()/1.033227453) + "(���) " );
puts(CStr("                  K=") + K + " A1*K=" + (A1*K) + " A2*K*K=" + (A2*K*K));
puts(CStr("                  GV=") + GV);


puts(CStr("CProgCalc::Calc_Gpt A0=") + A0 + " A1=" + A1 + " A2=" + A2 + " Freequency=" + Freequency + " Density=" + Density + " K=" + K + " GV=" + GV);
*/
		stGV 		= STATUS_NORMAL;
	}
	else if( pStPt->ok() && pStPt->v()==STPT_OFF )
	{// ���� ����� ���������

		if( pDPpt->ok() )
		{// ������� ����������
			GV 	 = -sqrt(FXABS( (Density0*DPgcn) / (Density*KCIO) ));/* / 1000.0*/;
			stGV = STATUS_NORMAL;
//puts(CStr("CProgCalc::Calc_Gpt GV=") + GV + " pDPpt=" + pDPpt->v() + " KCIO=" + KCIO + " Density0=" + Density0 + " Density=" + Density);
		}
		else
		{// ������������� �������
			GV = -1000.0*HOBI/Density;
		}
	}

	// ���������
	pGVpt->dValue 	= GV/**1000.0*/;			pGVpt->cStatus 	= stGV;
	pGMpt->dValue 	= GV*Density/1000000.0;	pGMpt->cStatus 	= stGV;

	pDpt->dValue 	= Density;	pDpt->cStatus 	= pThn->cStatus;
//puts(CStr("CProgCalc::Calc_Gpt GM=") + pGMpt->dValue + " GV=" + pGVpt->v());

}



void CProgCalc::Calc_Gpt()
{// 3.11.1 ������� ����� ����� ������� ������� �� �������� �������� �� ���
 //	3.11.2 ������ ������������� � 1 ������� �� �������� �������� �� ���

	IdbValue	Fgcn10;		ReadFromIdb("Fgcn10",	&Fgcn10);	// ������� �� ������ ������� ���-1, ��  YD10H01
	IdbValue	Fgcn20;		ReadFromIdb("Fgcn20",	&Fgcn20);	// ������� �� ������ ������� ���-2, ��  YD20H01
	IdbValue	Fgcn30;		ReadFromIdb("Fgcn30",	&Fgcn30);	// ������� �� ������ ������� ���-3, ��  YD30H01
	IdbValue	Fgcn40;		ReadFromIdb("Fgcn40",	&Fgcn40);	// ������� �� ������ ������� ���-4, ��  YD40H01

	IdbValue	DPgcn10;	ReadFromIdb("DPgcn10",	&DPgcn10);	// DP �� ���-1, ��/��2 YA10P18
	IdbValue	DPgcn20;	ReadFromIdb("DPgcn20",	&DPgcn20);	// DP �� ���-2, ��/��2 YA20P18
	IdbValue	DPgcn30;	ReadFromIdb("DPgcn30",	&DPgcn30);	// DP �� ���-3, ��/��2 YA30P18
	IdbValue	DPgcn40;	ReadFromIdb("DPgcn40",	&DPgcn40);	// DP �� ���-4, ��/��2 YA40P18

	IdbValue	Spt10;  	ReadFromIdb("Spt10",	&Spt10);	// ��������� 1-�� ����� 1�, ���/����/--/������� YA10ST
	IdbValue	Spt20;  	ReadFromIdb("Spt20",	&Spt20);	// ��������� 1-�� ����� 1�, ���/����/--/������� YA20ST
	IdbValue	Spt30;  	ReadFromIdb("Spt30",	&Spt30);	// ��������� 1-�� ����� 1�, ���/����/--/������� YA30ST
	IdbValue	Spt40;  	ReadFromIdb("Spt40",	&Spt40);	// ��������� 1-�� ����� 1�, ���/����/--/������� YA40ST

	IdbValue	Thn10;		ReadFromIdb("Thn10", 	&Thn10); 	// T � 1-�� �� 1�, ���� YA12T
	IdbValue	Thn20;		ReadFromIdb("Thn20", 	&Thn20);	// T � 2-�� �� 1�, ���� YA22T
	IdbValue	Thn30;		ReadFromIdb("Thn30", 	&Thn30);	// T � 3-�� �� 1�, ���� YA32T
	IdbValue	Thn40;		ReadFromIdb("Thn40", 	&Thn40);	// T � 4-�� �� 1�, ���� YA42T


	double		YCP_OUT	= ReadValueFromIdb("APzn")*0.0980665;		// ������� �������� ��� ���, ���
	double		YCDP	= ReadValueFromIdb("YC14P03")*0.0980665;	// ������� �������� ��� ���, ���

//	double		YAT_IN	= ReadValueFromIdb("ATzn");					// ������� ����������� �� ����� � ���

	double		A0_10		= pConstTbl->GetConstValue("A0_10");	// ��������� ��� ������� ������� �� ����� 1
	double		A1_10		= pConstTbl->GetConstValue("A1_10");
	double		A2_10		= pConstTbl->GetConstValue("A2_10");
	double		KCIO_10		= pConstTbl->GetConstValue("KCIO_10");

	double		A0_20		= pConstTbl->GetConstValue("A0_20");	// ��������� ��� ������� ������� �� ����� 2
	double		A1_20		= pConstTbl->GetConstValue("A1_20");
	double		A2_20		= pConstTbl->GetConstValue("A2_20");
	double		KCIO_20		= pConstTbl->GetConstValue("KCIO_20");

	double		A0_30		= pConstTbl->GetConstValue("A0_30");	// ��������� ��� ������� ������� �� ����� 3
	double		A1_30		= pConstTbl->GetConstValue("A1_30");
	double		A2_30		= pConstTbl->GetConstValue("A2_30");
	double		KCIO_30		= pConstTbl->GetConstValue("KCIO_30");

	double		A0_40		= pConstTbl->GetConstValue("A0_40");	// ��������� ��� ������� ������� �� ����� 4
	double		A1_40		= pConstTbl->GetConstValue("A1_40");
	double		A2_40		= pConstTbl->GetConstValue("A2_40");
	double		KCIO_40		= pConstTbl->GetConstValue("KCIO_40");

	// ����������� �������� ��������� ���� ��� (�3/���) � ����������� �� ����� ���������� ������, ������������ �� ����� ���
	double		HOBI_1		= pConstTbl->GetConstValue("HOBI_1");
	double		HOBI_2		= pConstTbl->GetConstValue("HOBI_2");
	double		HOBI_3		= pConstTbl->GetConstValue("HOBI_3");
	double		HOBI_4		= pConstTbl->GetConstValue("HOBI_4");

	// �������� �� ������� �������� �� ��� ��� ������� ��������� ������� �� ����� 1
	double		GCNRES_1	= pConstTbl->GetConstValue("GCNRES1");
	double		GCNRES_2	= pConstTbl->GetConstValue("GCNRES2");
	double		GCNRES_3	= pConstTbl->GetConstValue("GCNRES3");
	double		GCNRES_4	= pConstTbl->GetConstValue("GCNRES4");


	// ��������� ������������� � '��������' �����, � ������� ��������� ��������-�������� ��������������, (��/�3)
	double		Density_0 	= pConstTbl->GetConstValue("DGCN0");

	// ����������� ������� ������� ���
	double 		Frequency_0 = pConstTbl->GetConstValue("FGCN0");

	// �������������� ��������
	LONG_LST	GMptLst;
	IdbValue	GMpt10;	GMptLst.Add((int)&GMpt10);	// F ������-�� � 1-�� �����1�, ��/� YA10F
	IdbValue	GMpt20;	GMptLst.Add((int)&GMpt20);	// F ������-�� � 2-�� �����1�, ��/� YA20F
	IdbValue	GMpt30;	GMptLst.Add((int)&GMpt30);	// F ������-�� � 3-�� �����1�, ��/� YA30F
	IdbValue	GMpt40;	GMptLst.Add((int)&GMpt40);	// F ������-�� � 4-�� �����1�, ��/� YA40F

	IdbValue	GM1k;	// C���. F �������-�� � ������ 1�, ��/�  YAF_S

	IdbValue	Dhn10;	// ��������� �������� ����� ����� 1, ��/�3
	IdbValue	Dhn20;	// ��������� �������� ����� ����� 2, ��/�3
	IdbValue	Dhn30;	// ��������� �������� ����� ����� 3, ��/�3
	IdbValue	Dhn40;	// ��������� �������� ����� ����� 4, ��/�3

	IdbValue	GVpt10;	// F (��������) ������-�� � 1-�� ����� 1�, �3/�  YA10F_O
	IdbValue	GVpt20;	// F (��������) ������-�� � 2-�� ����� 1�, �3/�  YA20F_O
	IdbValue	GVpt30;	// F (��������) ������-�� � 3-�� ����� 1�, �3/�  YA30F_O
	IdbValue	GVpt40;	// F (��������) ������-�� � 4-�� ����� 1�, �3/�  YA40F_O

	// ���������� ���������� ���������� ������
	int		OnPtCnt	= 0;
	OnPtCnt  += Spt10.ok() && Spt10.v()==STPT_ON ? 1 : 0;
	OnPtCnt  += Spt20.ok() && Spt20.v()==STPT_ON ? 1 : 0;
	OnPtCnt  += Spt30.ok() && Spt30.v()==STPT_ON ? 1 : 0;
	OnPtCnt  += Spt40.ok() && Spt40.v()==STPT_ON ? 1 : 0;

	// ������� ����������� HOBI � ��������� �� ����� ���������� ���
	double	HOBI	= 0.0;
	if(		 OnPtCnt==1 ) HOBI = HOBI_1;
	else if( OnPtCnt==2 ) HOBI = HOBI_2;
	else if( OnPtCnt==3 ) HOBI = HOBI_3;
	else if( OnPtCnt==4 ) HOBI = HOBI_4;


	// �������� �� ����� � ���
	double 	YcpIn		= YCP_OUT + YCDP;

	Calc_Gpt(&GVpt10, &GMpt10, &Dhn10, &Spt10, &Fgcn10, &DPgcn10, &Thn10, YcpIn, A0_10,  A1_10,  A2_10, KCIO_10, HOBI, Density_0, Frequency_0, GCNRES_1);
	Calc_Gpt(&GVpt20, &GMpt20, &Dhn20, &Spt20, &Fgcn20, &DPgcn20, &Thn20, YcpIn, A0_20,  A1_20,  A2_20, KCIO_20, HOBI, Density_0, Frequency_0, GCNRES_2);
	Calc_Gpt(&GVpt30, &GMpt30, &Dhn30, &Spt30, &Fgcn30, &DPgcn30, &Thn30, YcpIn, A0_30,  A1_30,  A2_30, KCIO_30, HOBI, Density_0, Frequency_0, GCNRES_3);
	Calc_Gpt(&GVpt40, &GMpt40, &Dhn40, &Spt40, &Fgcn40, &DPgcn40, &Thn40, YcpIn, A0_40,  A1_40,  A2_40, KCIO_40, HOBI, Density_0, Frequency_0, GCNRES_4);

	Calc_Sum(&GM1k,&GMptLst,true);

	// ������� ���������� � ���
	WriteToIdb("GMpt10",&GMpt10);
	WriteToIdb("GMpt20",&GMpt20);
	WriteToIdb("GMpt30",&GMpt30);
	WriteToIdb("GMpt40",&GMpt40);
	WriteToIdb("GM1k",&GM1k);

	WriteToIdb("GVpt10",&GVpt10);
	WriteToIdb("GVpt20",&GVpt20);
	WriteToIdb("GVpt30",&GVpt30);
	WriteToIdb("GVpt40",&GVpt40);

	WriteToIdb("Dhn10",&Dhn10);
	WriteToIdb("Dhn20",&Dhn20);
	WriteToIdb("Dhn30",&Dhn30);
	WriteToIdb("Dhn40",&Dhn40);
}


void CProgCalc::Calc_Gpg(IdbValue* pGMpg, IdbValue* pGVpg, IdbValue* pDpg, IdbValue* pTpg, IdbValue* pATpg, IdbValue* pPpg, IdbValue* pAPpg, double	Density_0)
{// 3.11.4 ������ ��������� ������� ����������� ���� � ���������������

	double	T = (pTpg->ok()? 1.0 : 0.0) * pTpg->v() + (1.0 - pTpg->ok()? 1.0 : 0.0) * (pATpg->ok()? 1.0 : 0.0) * pATpg->v();
	double	P = (pPpg->ok()? 1.0 : 0.0) * pPpg->v() + (1.0 - pPpg->ok()? 1.0 : 0.0) * (pAPpg->ok()? 1.0 : 0.0) * pAPpg->v();

	if( T>0.0 && P>0.0 )
	{
		double	Density 	= 1000.0*WaterSteamProperty::getWaterDensity(P*0.0980665,T);
		pGMpg->dValue		= pGVpg->v()*sqrt(Density * Density_0) / 1000.0 / 1000;
		pGMpg->cStatus 		= pGVpg->s();
		pDpg->dValue 		= Density;
		pDpg->cStatus 		= STATUS_NORMAL;

//puts(CStr("Calc_Gpg T=") + T + " P=" + P + " Ro=" + Density+"/" + Density_0 + " sqrt=" + sqrt(Density*Density_0) + " GV=" + pGVpg->v() + " GM=" + pGMpg->dValue);
	}
	else
	{// ��� ������������
		pGMpg->dValue 	= 0.0;  pGMpg->cStatus = STATUS_UNDEF;
		pDpg->dValue	= 0.0;	pDpg->cStatus = STATUS_UNDEF;
	}
}

void CProgCalc::Calc_Gpg()
{// 3.11.4 ������ ��������� ������� ����������� ���� � ���������������
 // 3.11.5 ������ ������� ����������� ���� �� 2 �������

	IdbValue	GVpg10;	ReadFromIdb("GVpg10",	&GVpg10);	// F �� � ��-1, �/�  RL71F01
	IdbValue	GVpg20;	ReadFromIdb("GVpg20",	&GVpg20);	// F �� � ��-2, �/�  RL72F01
	IdbValue	GVpg30;	ReadFromIdb("GVpg30",	&GVpg30);	// F �� � ��-3, �/�  RL73F01
	IdbValue	GVpg40;	ReadFromIdb("GVpg40",	&GVpg40);	// F �� � ��-4, �/�  RL74F01

	IdbValue	Tpw10;	ReadFromIdb("Tpw10",	&Tpw10);	// T �� ����� ��-1, ���� RL71T01
	IdbValue	Tpw20;	ReadFromIdb("Tpw20",	&Tpw20);	// T �� ����� ��-2, ���� RL72T01
	IdbValue	Tpw30;	ReadFromIdb("Tpw30",	&Tpw30);	// T �� ����� ��-3, ���� RL73T01
	IdbValue	Tpw40;	ReadFromIdb("Tpw40",	&Tpw40);	// T �� ����� ��-4, ���� RL74T01

	IdbValue	Ppw10;	ReadFromIdb("Ppw10",	&Ppw10);	// P �� � ��-1, ��/��2 RL71P01
	IdbValue	Ppw20;	ReadFromIdb("Ppw20",	&Ppw20);	// P �� � ��-2, ��/��2 RL72P01
	IdbValue	Ppw30;	ReadFromIdb("Ppw30",	&Ppw30);	// P �� � ��-3, ��/��2 RL73P01
	IdbValue	Ppw40;	ReadFromIdb("Ppw40",	&Ppw40);	// P �� � ��-4, ��/��2 RL74P01

	IdbValue	APpg;	ReadFromIdb("APpg",		&APpg);		// ��. P ���� � ��, ��/��2
	IdbValue	ATpw;	ReadFromIdb("ATpw",		&ATpw);		// ������� � ���. ����, ����

	double		Density_0 	= pConstTbl->GetConstValue("DPW0");
	int			GV2kAsGM2k	= (int)pConstTbl->GetConstValue("GV2kAsGM2k");

	// �������������� ��������
	LONG_LST	GMpgLst;
	IdbValue	GMpg10;  GMpgLst.Add((int)&GMpg10);  // F �� � ��-1 (��������), ��/� RL71F
	IdbValue	GMpg20;  GMpgLst.Add((int)&GMpg20);  // F �� � ��-2 (��������), ��/� RL72F
	IdbValue	GMpg30;  GMpgLst.Add((int)&GMpg30);  // F �� � ��-3 (��������), ��/� RL73F
	IdbValue	GMpg40;  GMpgLst.Add((int)&GMpg40);  // F �� � ��-4 (��������), ��/� RL74F

	IdbValue	Dpg10; // ��������� ����������� ���� ��1 ��/�3
	IdbValue	Dpg20; // ��������� ����������� ���� ��2 ��/�3
	IdbValue	Dpg30; // ��������� ����������� ���� ��3 ��/�3
	IdbValue	Dpg40; // ��������� ����������� ���� ��4 ��/�3


	IdbValue	GM2k;  // ���. F �� � ��, ��/�
	IdbValue	AGMpg;  // ��. F �� � ��, ��/�

	if( GV2kAsGM2k!=1 )
	{// ����������� �������� ������� �� ��������
		Calc_Gpg(&GMpg10, &GVpg10, &Dpg10, &Tpw10, &ATpw, &Ppw10, &APpg, Density_0);
		Calc_Gpg(&GMpg20, &GVpg20, &Dpg20, &Tpw20, &ATpw, &Ppw20, &APpg, Density_0);
		Calc_Gpg(&GMpg30, &GVpg30, &Dpg30, &Tpw30, &ATpw, &Ppw30, &APpg, Density_0);
		Calc_Gpg(&GMpg40, &GVpg40, &Dpg40, &Tpw40, &ATpw, &Ppw40, &APpg, Density_0);

		WriteToIdb("Dpg10",&Dpg10);
		WriteToIdb("Dpg20",&Dpg20);
		WriteToIdb("Dpg30",&Dpg30);
		WriteToIdb("Dpg40",&Dpg40);
	}
	else
	{// ������� ��� � �������� �������� ������ �������� �������
		GMpg10.dValue = GVpg10.dValue/1000; 	GMpg10.cStatus = GVpg10.cStatus;
		GMpg20.dValue = GVpg20.dValue/1000; 	GMpg20.cStatus = GVpg20.cStatus;
		GMpg30.dValue = GVpg30.dValue/1000; 	GMpg30.cStatus = GVpg30.cStatus;
		GMpg40.dValue = GVpg40.dValue/1000; 	GMpg40.cStatus = GVpg40.cStatus;
	}
	Calc_Avg(&AGMpg, &GMpgLst, NULL);
	Calc_Sum(&GM2k,  &GMpgLst, NULL);

	// ������� ���������� � ���
	WriteToIdb("GMpg10",&GMpg10);
	WriteToIdb("GMpg20",&GMpg20);
	WriteToIdb("GMpg30",&GMpg30);
	WriteToIdb("GMpg40",&GMpg40);

	WriteToIdb("AGMpg",&AGMpg);
	WriteToIdb("GM2k",&GM2k);
}


void CProgCalc::Calc_Gpvd(IdbValue* pGMpvd, IdbValue* pGVpvd, IdbValue* pDpvd, IdbValue* pTpvd, IdbValue* pPpvd, double	Density_0)
{// 3.11.6 ������ ������� ����������� ���� � ���

	double	Density 	= 1000.0 * WaterSteamProperty::getWaterDensity(pPpvd->v()*0.0980665, (pTpvd->ok()?pTpvd->v():170));
	pGMpvd->dValue		= pGVpvd->v()*sqrt(Density * Density_0)/1000000.0;

//puts(CStr("") + " pGVpvd=" + pGVpvd->v() + " Density=" + Density + " Density_0=" + Density_0 + " pTpvd=" + pTpvd->v() + " pPvd=" + pPpvd->v());
//puts(CStr("") + " pGMpvd->dValue=" + pGMpvd->dValue);

	// 03.01.2019 �� ������ ��������
	//pGMpvd->cStatus		= pGVpvd->ok() && pTpvd->ok() && pPpvd->ok() ? STATUS_NORMAL : STATUS_UNDEF;
	pGMpvd->cStatus		= pGVpvd->ok() && pPpvd->ok() ? STATUS_NORMAL : STATUS_UNDEF;

	pDpvd->dValue		= Density;
	// 03.01.2019 �� ������ ��������
	//pDpvd->cStatus		= pPpvd->ok() && pTpvd->ok() ? STATUS_NORMAL : STATUS_UNDEF;
	pDpvd->cStatus		= pPpvd->ok() ? STATUS_NORMAL : STATUS_UNDEF;
}



void CProgCalc::Calc_Gpvd()
{// 3.11.6 ������ ������� ����������� ���� � ���

	IdbValue	GVpvd1;	ReadFromIdb("GVpvd1",	&GVpvd1);	// F �� � 1-�� ����� ���, �/�  RL61F01
	IdbValue	GVpvd2;	ReadFromIdb("GVpvd2",	&GVpvd2);	// F �� � 1-�� ����� ���, �/�  RL62F01
	IdbValue	Tpvd1;	ReadFromIdb("Tpvd1_D",	&Tpvd1);	// T �� ����� 1-�� ����� ���, ����  RL61T02
	IdbValue	Tpvd2;	ReadFromIdb("Tpvd2_D",	&Tpvd2);	// T �� ����� 1-�� ����� ���, ����  RL62T02
//	IdbValue	Tpvd1;	ReadFromIdb("Tpvd1",	&Tpvd1);	// T �� ����� 1-�� ����� ���, ����  RL74T02
//	IdbValue	Tpvd2;	ReadFromIdb("Tpvd2",	&Tpvd2);	// T �� ����� 1-�� ����� ���, ����  RL74T03
	IdbValue	Ppvd;	ReadFromIdb("Ppvd",		&Ppvd);		// P �� ����� ���, ��/��2  RL74P02

	// �������������� ��������
	IdbValue	GMpvd1; // F �� � 1-�� ����� ���, ��/�
	IdbValue	GMpvd2;	// F �� � 2-�� ����� ���, ��/�

	IdbValue	Dpvd1; // ��������� ����������� ���� ���1, ��/�3
	IdbValue	Dpvd2; // ��������� ����������� ���� ���2, ��/�3

	double		Density_0 	= pConstTbl->GetConstValue("DPVD0");
	int			GV2kAsGM2k	= (int)pConstTbl->GetConstValue("GV2kAsGM2k");

	if( GV2kAsGM2k!=1 )
	{
		Calc_Gpvd(&GMpvd1, &GVpvd1, &Dpvd1, &Tpvd1, &Ppvd, Density_0);
		Calc_Gpvd(&GMpvd2, &GVpvd2, &Dpvd2,	&Tpvd2, &Ppvd, Density_0);

		WriteToIdb("Dpvd1",&Dpvd1);
		WriteToIdb("Dpvd2",&Dpvd2);
	}
	else
	{
		GMpvd1.dValue = GVpvd1.dValue/1000; 	GMpvd1.cStatus = GVpvd1.cStatus;
		GMpvd2.dValue = GVpvd2.dValue/1000; 	GMpvd2.cStatus = GVpvd2.cStatus;
	}

	WriteToIdb("GMpvd1",&GMpvd1);
	WriteToIdb("GMpvd2",&GMpvd2);

//puts(CStr("GMpvd1=") + GMpvd1.v());
//puts(CStr("GMpvd2=") + GMpvd2.v() + " GVpvd2=" + GVpvd2.v() + " Tpvd2=" + Tpvd2.v() + " Ppvd=" + Ppvd.v());
}


void CProgCalc::Calc_ATn(IdbValue* pATn, LONG_LST* pTnLst, LONG_LST* pGMptLst, LONG_LST* pSptLst, int Kgcnm, int DostTnCnt, int GMPtDostCnt )
{
	double	SumUp	= 0.0;
	double	SumDown	= 0.0;



	if( Kgcnm==DostTnCnt && Kgcnm==GMPtDostCnt )
	{// �� ���� ���������� ������ ���������� ����������� � �������

		for( int PtPos=1; PtPos<=pSptLst->GetCnt(); PtPos++ )
		{
			IdbValue* pSpt = (IdbValue*)pSptLst->Get(PtPos);
			if( pSpt->ok() && pSpt->v()==STPT_ON )
			{
				SumUp  += ((IdbValue*)pTnLst->Get(PtPos))->v() * ((IdbValue*)pGMptLst->Get(PtPos))->v();
				SumDown += ((IdbValue*)pGMptLst->Get(PtPos))->v();
			}
		}
	}
	else if( Kgcnm==DostTnCnt )
	{// �� ���� ���������� ������ ���������� �����������

		for( int PtPos=1; PtPos<=pSptLst->GetCnt(); PtPos++ )
		{
			IdbValue* pSpt = (IdbValue*)pSptLst->Get(PtPos);
			if( pSpt->ok() && pSpt->v()==STPT_ON )
			{
				SumUp += ((IdbValue*)pTnLst->Get(PtPos))->v();
				SumDown += 1;
			}
		}
	}

	if( SumUp>0.0 && SumDown>0.0 ){ pATn->dValue = SumUp/SumDown;	pATn->cStatus = STATUS_NORMAL; }
	else						  { pATn->dValue = 0;				pATn->cStatus = STATUS_UNDEF; }
}



void CProgCalc::Calc_AThnAtgnADTpt()
{// 3.10.2 ������ ������� ����������� � �������� (�������) ������ ������� ������� � ���������
 // 3.10.3 ������ ������� ����������� � ��������

	LONG_LST	ThnLst;
	IdbValue	Thn10;	ReadFromIdb("Thn10", &Thn10);	ThnLst.Add((int)&Thn10); // T � 1-�� �� 1�, ���� YA12T
	IdbValue	Thn20;	ReadFromIdb("Thn20", &Thn20);	ThnLst.Add((int)&Thn20); // T � 2-�� �� 1�, ���� YA22T
	IdbValue	Thn30;	ReadFromIdb("Thn30", &Thn30);	ThnLst.Add((int)&Thn30); // T � 3-�� �� 1�, ���� YA32T
	IdbValue	Thn40;	ReadFromIdb("Thn40", &Thn40);	ThnLst.Add((int)&Thn40); // T � 4-�� �� 1�, ���� YA42T

	LONG_LST	TgnLst;
	IdbValue	Tgn10;	ReadFromIdb("Tgn10",	&Tgn10);	TgnLst.Add((int)&Tgn10); // T � 1-�� �� 1�, ���� YA11T
	IdbValue	Tgn20;	ReadFromIdb("Tgn20",	&Tgn20);	TgnLst.Add((int)&Tgn20); // T � 2-�� �� 1�, ���� YA21T
	IdbValue	Tgn30;	ReadFromIdb("Tgn30",	&Tgn30);	TgnLst.Add((int)&Tgn30); // T � 3-�� �� 1�, ���� YA31T
	IdbValue	Tgn40;	ReadFromIdb("Tgn40",	&Tgn40);	TgnLst.Add((int)&Tgn40); // T � 4-�� �� 1�, ���� YA41T

	LONG_LST	SptLst;
	IdbValue	Spt10;  	ReadFromIdb("Spt10",	&Spt10);	SptLst.Add((int)&Spt10);	// ��������� 1-�� ����� 1�, ���/����/--/������� YA10ST
	IdbValue	Spt20;  	ReadFromIdb("Spt20",	&Spt20);	SptLst.Add((int)&Spt20);	// ��������� 1-�� ����� 1�, ���/����/--/������� YA20ST
	IdbValue	Spt30;  	ReadFromIdb("Spt30",	&Spt30);	SptLst.Add((int)&Spt30);	// ��������� 1-�� ����� 1�, ���/����/--/������� YA30ST
	IdbValue	Spt40;  	ReadFromIdb("Spt40",	&Spt40);	SptLst.Add((int)&Spt40);	// ��������� 1-�� ����� 1�, ���/����/--/������� YA40ST

	LONG_LST	GMptLst;
	IdbValue	GMpt10;		ReadFromIdb("GMpt10",	&GMpt10);	GMptLst.Add((int)&GMpt10);	// F ������-�� � 1-�� �����1�, ��/� YA10F
	IdbValue	GMpt20;		ReadFromIdb("GMpt20",	&GMpt20);	GMptLst.Add((int)&GMpt20);	// F ������-�� � 2-�� �����1�, ��/� YA20F
	IdbValue	GMpt30;		ReadFromIdb("GMpt30",	&GMpt30);	GMptLst.Add((int)&GMpt30);	// F ������-�� � 3-�� �����1�, ��/� YA30F
	IdbValue	GMpt40;		ReadFromIdb("GMpt40",	&GMpt40);	GMptLst.Add((int)&GMpt40);	// F ������-�� � 4-�� �����1�, ��/� YA40F

	// �������������� ��������
	IdbValue	AThn; 		   	// ��.  T  ���. �����, ���� YA02T_A
	IdbValue	ATgn;   	 	// ��.  T  ���. �����, ���� YA01T_A
	IdbValue	ADTpt; 		   	// ��.  DT �  ������  1�, ����  YA00T_DT_A

	// ���������� ���������� ������
	int			Kgcn		= 0;
	int			HnDostCnt	= 0;
	int			GnDostCnt	= 0;
	int			GMPtDostCnt	= 0;

	for( int PtPos=1; PtPos<=SptLst.GetCnt(); PtPos++ )
	{// ���������� ���������� ���������� ������
		IdbValue* pStPt = (IdbValue*)SptLst.Get(PtPos);
		if( pStPt->ok() && pStPt->v()==STPT_ON )
		{
			Kgcn++;
			HnDostCnt 		+= ((IdbValue*)ThnLst.Get(PtPos))->ok() ? 1 : 0;
			GnDostCnt 		+= ((IdbValue*)TgnLst.Get(PtPos))->ok() ? 1 : 0;
			GMPtDostCnt 	+= ((IdbValue*)GMptLst.Get(PtPos))->ok() ? 1 : 0;
		}
	}

	if( Kgcn )
	{
		Calc_ATn(&AThn, &ThnLst, &GMptLst, &SptLst, Kgcn, HnDostCnt, GMPtDostCnt);
		Calc_ATn(&ATgn, &TgnLst, &GMptLst, &SptLst, Kgcn, GnDostCnt, GMPtDostCnt);
	}
	else
	{
		Calc_Avg(&AThn, &ThnLst, NULL);
		Calc_Avg(&ATgn, &TgnLst, NULL);
	}

	ADTpt.dValue 	= ATgn.v() - AThn.v();
	ADTpt.cStatus 	= (AThn.s()!=STATUS_UNDEF && ATgn.s()!=STATUS_UNDEF) ? STATUS_NORMAL :  STATUS_UNDEF;

	// ������� ���������� � ���
	WriteToIdb("AThn",&AThn);
	WriteToIdb("ATgn",&ATgn);
	WriteToIdb("ADTpt",&ADTpt);
//puts(CStr("AThn=") + AThn.v() + " ATgn=" + ATgn.v() + " ADTpt=" + ADTpt.v());
}

void CProgCalc::Calc_DTtpADTtp()
{// ������ ���������� � ��� �� �� � ��������� ��������� �� ��

	IdbValue	AThn; ReadFromIdb("AThn", &AThn);  // ��.  T  ���. �����, ���� YA02T_A

	float		YC00T_TP[TVS_TP_CNT];				// ����������� �� ������ ��� �� ��
	char		stYC00T_TP[TVS_TP_CNT];				// �������  ���������� �� ������ ��� �� ��
	float		YC00DT_TP[TVS_TP_CNT];				// ��������� �� ������ ��� �� ��
	char		stYC00DT_TP[TVS_TP_CNT];			// �������  ���������� �� ������ ��� �� ��

	IdbValue	ATtpzn;		// ��. T �� ������ ��� (�� ��), ���� YC00T_A
	IdbValue	ADTtpzn;	// ��.  DT � ��� (�� ��)� ���� YC00T_DT_A


	// �������� �������� � ������� ���� �� Idb (�������� ���� �������������)
	pYC00T_TP->Read((float*)YC00T_TP, (char*)stYC00T_TP);


	int		DostTpCnt 	= 0;
	double	SumT		= 0.0;
	double	SumDT		= 0.0;

	for( int TpPos=0; TpPos<TVS_TP_CNT; TpPos++ )
	{
		if( stYC00T_TP[TpPos]<STATUS_UNDEF )
		{
			YC00DT_TP[TpPos] = YC00T_TP[TpPos] - AThn.v();

			DostTpCnt++;
			SumT  += YC00T_TP[TpPos];
			SumDT += YC00DT_TP[TpPos];

			stYC00DT_TP[TpPos] = STATUS_NORMAL;
		}
		else
		{
			YC00DT_TP[TpPos]   = 0.0;
			stYC00DT_TP[TpPos] = STATUS_UNDEF;
		}

		if( AThn.s()>=STATUS_UNDEF ) stYC00DT_TP[TpPos] = STATUS_UNDEF;
	}

	if( DostTpCnt>0 )
	{
		ATtpzn.dValue = SumT/DostTpCnt;		ATtpzn.cStatus = STATUS_NORMAL;
		ADTtpzn.dValue = SumDT/DostTpCnt;	ADTtpzn.cStatus = AThn.s()<STATUS_UNDEF ? STATUS_NORMAL : STATUS_UNDEF;
	}
	else
	{
		ATtpzn.cStatus = STATUS_UNDEF;
		ADTtpzn.cStatus = STATUS_UNDEF;
	}

	// ������� ���� YC00DT_TP reg ���
	pYC00DT_TP->Write((float*)YC00DT_TP, (char*)stYC00DT_TP);
	WriteToIdb("ATtpzn",&ATtpzn);
	WriteToIdb("ADTtpzn",&ADTtpzn);
}

void CProgCalc::Calc_V(IdbValue* pNewV, IdbValue* pPrevV, IdbValue* pNewParam, IdbValue* pPrevParam, int N, int K)
{// ������ ��������� �� ����������

	if( pPrevParam->s()==STATUS_EMPTY )
	{// � ������ �������
		pPrevParam->dValue 	= pNewParam->v();
		pPrevParam->cStatus = pNewParam->s();
	}

	if( pPrevV->s()==STATUS_EMPTY )
	{// � ������ �������
		pPrevV->dValue 	= 0.0;//pScanV->v();
		pPrevV->cStatus = STATUS_NORMAL;
	}

	if( pPrevParam->ok() && pNewParam->ok() )
	{// ���� ���������� ��� ���������������� �������� ����������� ��������

		double A = 2.0/((double)N+1);

		pNewV->dValue 	= A*(pNewParam->v()-pPrevParam->v()) + (1.0-A)*pPrevV->v();

//puts(CStr("CProgCalc::Calc_V NewParam=") + pNewParam->v() + "/" + (int)pNewParam->s() + " PrevParam=" + pPrevParam->v() + "/" + (int)pPrevParam->s() + " PrevV=" + pPrevV->v() + "/" + (int)pPrevV->s());
//puts(CStr("           V=") + pNewV->dValue + " A=" + A + " 1=" + A*(pNewParam->v()-pPrevParam->v()) + " 2=" + (1-A)*pPrevV->v() + " Prev=" + pPrevV->v());
		pNewV->cStatus 	= STATUS_NORMAL;
	}
	else
	{// ���� ���� ���� ������������ - �������� ������������ � �� ���������������
		pNewV->dValue 	= pPrevV->v();
		pNewV->cStatus 	= STATUS_UNDEF;
	}

	// �������� ���������� ��������
	pPrevParam->dValue 	= pNewParam->v();
	pPrevParam->cStatus = pNewParam->s();

	pPrevV->dValue 	= pNewV->v();
	pPrevV->cStatus = pNewV->s();

	pNewV->dValue  *= K;
}



void CProgCalc::Calc_V()
{// ������ ��������� �� ����������

	IdbValue	APzn;	ReadFromIdb("APzn", &APzn);  // ��. P ��� ���, ��/��2

	IdbValue	Tzn1;	ReadFromIdb("Tzn1",	&Tzn1); // T � ����� ������ �� ���.�� ��� (��� �������), ���� YC00T01
	IdbValue	Tzn2;	ReadFromIdb("Tzn2",	&Tzn2); // T � ����� ������ �� ���.�� ��� (��� �������), ���� YC00T02
	IdbValue	Tzn3;	ReadFromIdb("Tzn3",	&Tzn3); // T � ����� ������ �� ���.�� ��� (��� �������), ���� YC00T03

	IdbValue	Ppg10;	ReadFromIdb("Ppg10", &Ppg10);// P ���� � ��-1, ��/��2 YB10P10
	IdbValue	Ppg20;	ReadFromIdb("Ppg20", &Ppg20);// P ���� � ��-2, ��/��2 YB20P10
	IdbValue	Ppg30;	ReadFromIdb("Ppg30", &Ppg30);// P ���� � ��-3, ��/��2 YB30P10
	IdbValue	Ppg40;	ReadFromIdb("Ppg40", &Ppg40);// P ���� � ��-4, ��/��2 YB40P10

	IdbValue	Thn10;  ReadFromIdb("Thn10", &Thn10);// T � 1-�� �� 1�, ���� YA12T
	IdbValue	Thn20;  ReadFromIdb("Thn20", &Thn20);// T � 2-�� �� 1�, ���� YA22T
	IdbValue	Thn30;  ReadFromIdb("Thn30", &Thn30);// T � 3-�� �� 1�, ���� YA32T
	IdbValue	Thn40;  ReadFromIdb("Thn40", &Thn40);// T � 4-�� �� 1�, ���� YA42T

	IdbValue	Tgn10;  ReadFromIdb("Tgn10", &Tgn10);// T � 1-�� �� 1�, ���� YA11T
	IdbValue	Tgn20;  ReadFromIdb("Tgn20", &Tgn20);// T � 2-�� �� 1�, ���� YA21T
	IdbValue	Tgn30;  ReadFromIdb("Tgn30", &Tgn30);// T � 3-�� �� 1�, ���� YA31T
	IdbValue	Tgn40;  ReadFromIdb("Tgn40", &Tgn40);// T � 4-�� �� 1�, ���� YA41T

	int	VT_N	= pConstTbl->GetConstValue("VT_N");	// ������ ����������� ��� ���������� (���������� ����� ������ ���������� � �������� �������)
	int	VP_N	= pConstTbl->GetConstValue("VP_N"); // ������ ����������� ��� �������� (���������� ����� ������ ���������� � �������� �������)

	// �������������� ��������
	IdbValue	VPzn; 	 // V ��������� � ��� ���, ��/��2*��� YC70P_V
	IdbValue	VTzn1;  // V ��������� T ��� ��� (��� �������), ����/� YC00T01_V
	IdbValue	VTzn2;  // V ��������� T ��� ��� (��� �������), ����/� YC00T01_V
	IdbValue	VTzn3;  // V ��������� T ��� ��� (��� �������), ����/� YC00T01_V

	IdbValue	VPpg10; // V ��������� � � ��-1, ��/��2*��� YB10P10_V
	IdbValue	VPpg20; // V ��������� � � ��-2, ��/��2*��� YB20P10_V
	IdbValue	VPpg30; // V ��������� � � ��-3, ��/��2*��� YB30P10_V
	IdbValue	VPpg40; // V ��������� � � ��-4, ��/��2*��� YB40P10_V

	IdbValue	VTgn10; // V ��������� T � ��-1, ����/� YA11T_V
	IdbValue	VTgn20; // V ��������� T � ��-2, ����/� YA21T_V
	IdbValue	VTgn30; // V ��������� T � ��-3, ����/� YA31T_V
	IdbValue	VTgn40; // V ��������� T � ��-4, ����/� YA41T_V

	IdbValue	VThn10; // V ��������� T � ��-1, ����/� YA12T_V
	IdbValue	VThn20; // V ��������� T � ��-2, ����/� YA22T_V
	IdbValue	VThn30; // V ��������� T � ��-3, ����/� YA32T_V
	IdbValue	VThn40; // V ��������� T � ��-4, ����/� YA42T_V


	Calc_V(&VPzn, &PrevVPzn, &APzn, &PrevPzn, VP_N, 100);
//puts(CStr("CProgCalc::Calc_V VPzn=") + VPzn.v() + "/" + (int)VPzn.s() + " sVPzn=" + sVPzn.v() + "/" + (int)sVPzn.s() + " APzn=" + APzn.v() + "/" + (int)APzn.s() + " PrevPzn=" + PrevPzn.v() +  "/" + (int)PrevPzn.s() + " N=" + VP_N);


	Calc_V(&VTzn1, &PrevVTzn1, &Tzn1, &PrevTzn1, VT_N, 6000);
	Calc_V(&VTzn2, &PrevVTzn2, &Tzn2, &PrevTzn2, VT_N, 6000);
	Calc_V(&VTzn3, &PrevVTzn3, &Tzn3, &PrevTzn3, VT_N, 6000);

	Calc_V(&VPpg10, &PrevVPpg10, &Ppg10, &PrevPpg10, VP_N, 100);
	Calc_V(&VPpg20, &PrevVPpg20, &Ppg20, &PrevPpg20, VP_N, 100);
	Calc_V(&VPpg30, &PrevVPpg30, &Ppg30, &PrevPpg30, VP_N, 100);
	Calc_V(&VPpg40, &PrevVPpg40, &Ppg40, &PrevPpg40, VP_N, 100);

	Calc_V(&VTgn10, &PrevVTgn10, &Tgn10, &PrevTgn10, VT_N, 6000);
	Calc_V(&VTgn20, &PrevVTgn20, &Tgn20, &PrevTgn20, VT_N, 6000);
	Calc_V(&VTgn30, &PrevVTgn30, &Tgn30, &PrevTgn30, VT_N, 6000);
	Calc_V(&VTgn40, &PrevVTgn40, &Tgn40, &PrevTgn40, VT_N, 6000);

	Calc_V(&VThn10, &PrevVThn10, &Thn10, &PrevThn10, VT_N, 6000);
	Calc_V(&VThn20, &PrevVThn20, &Thn20, &PrevThn20, VT_N, 6000);
	Calc_V(&VThn30, &PrevVThn30, &Thn30, &PrevThn30, VT_N, 6000);
	Calc_V(&VThn40, &PrevVThn40, &Thn40, &PrevThn40, VT_N, 6000);

	// ����� � ����
	WriteToIdb("VPzn",&VPzn);

	WriteToIdb("VTzn1",&VTzn1);
	WriteToIdb("VTzn2",&VTzn2);
	WriteToIdb("VTzn3",&VTzn3);

	WriteToIdb("VPpg10",&VPpg10);
	WriteToIdb("VPpg20",&VPpg20);
	WriteToIdb("VPpg30",&VPpg30);
	WriteToIdb("VPpg40",&VPpg40);

	WriteToIdb("VTgn10",&VTgn10);
	WriteToIdb("VTgn20",&VTgn20);
	WriteToIdb("VTgn30",&VTgn30);
	WriteToIdb("VTgn40",&VTgn40);

	WriteToIdb("VThn10",&VThn10);
	WriteToIdb("VThn20",&VThn20);
	WriteToIdb("VThn30",&VThn30);
	WriteToIdb("VThn40",&VThn40);
}

void CProgCalc::Calc_Npt(IdbValue* pNpt, IdbValue* pGMpt, IdbValue* pThn, IdbValue* pIhn, IdbValue* pTgn, IdbValue* pIgn, double Pzn, double DPzn)
{
	double	Ign	  = WaterSteamProperty::getWaterEnthalpy(Pzn,pTgn->v());
	double	Ihn	  = WaterSteamProperty::getWaterEnthalpy(Pzn+DPzn,pThn->v());

	pNpt->dValue  = (1000.0/3600.0) * pGMpt->v() * (Ign - Ihn);
	pIhn->dValue  = Ihn;
	pIgn->dValue  = Ign;
//puts(CStr("CProgCalc::Calc_Npt=") + pNpt->dValue + " pGMpt->v()=" + pGMpt->v() + " DPzn=" + DPzn + " pTgn->v()=" + pTgn->v() + " pThn->v()=" + pThn->v() );

	if( pGMpt->ok() && pThn->ok() && pTgn->ok() )
	{
		 pNpt->cStatus = STATUS_NORMAL;
		 pIhn->cStatus = STATUS_NORMAL;
		 pIgn->cStatus = STATUS_NORMAL;
	}
	else pNpt->cStatus = STATUS_UNDEF;
}

void CProgCalc::Calc_N1k()
{ // 3.12.2 ������ �������� �������� ��� �� ���������� � �������� �������������, ������������ �� �������� �������� �� ���  � ������ ������� �������

	IdbValue	GMpt10;	ReadFromIdb("GMpt10", &GMpt10);	// F ������-�� � 1-�� �����1�, ��/� YA10F
	IdbValue	GMpt20;	ReadFromIdb("GMpt20", &GMpt20);	// F ������-�� � 2-�� �����1�, ��/� YA20F
	IdbValue	GMpt30;	ReadFromIdb("GMpt30", &GMpt30);	// F ������-�� � 3-�� �����1�, ��/� YA30F
	IdbValue	GMpt40;	ReadFromIdb("GMpt40", &GMpt40);	// F ������-�� � 4-�� �����1�, ��/� YA40F

	IdbValue	Thn10;  ReadFromIdb("Thn10", &Thn10);// T � 1-�� �� 1�, ���� YA12T
	IdbValue	Thn20;  ReadFromIdb("Thn20", &Thn20);// T � 2-�� �� 1�, ���� YA22T
	IdbValue	Thn30;  ReadFromIdb("Thn30", &Thn30);// T � 3-�� �� 1�, ���� YA32T
	IdbValue	Thn40;  ReadFromIdb("Thn40", &Thn40);// T � 4-�� �� 1�, ���� YA42T

	IdbValue	Tgn10;  ReadFromIdb("Tgn10", &Tgn10);// T � 1-�� �� 1�, ���� YA11T
	IdbValue	Tgn20;  ReadFromIdb("Tgn20", &Tgn20);// T � 2-�� �� 1�, ���� YA21T
	IdbValue	Tgn30;  ReadFromIdb("Tgn30", &Tgn30);// T � 3-�� �� 1�, ���� YA31T
	IdbValue	Tgn40;  ReadFromIdb("Tgn40", &Tgn40);// T � 4-�� �� 1�, ���� YA41T

	double		YCP_OUT	= ReadValueFromIdb("APzn")*0.0980665;		// ������� �������� ��� ���, ���
	double		YCDP	= ReadValueFromIdb("YC14P03")*0.0980665;	// ������� �������� ��� ���, ���

	double	    Pot1k	= pConstTbl->GetConstValue("POT1K");	// ������ ����� � 1 �������, ���, ������������ �� ����� ���


	// �������������� ��������
	IdbValue	Npt10; // N ����� 1, ��� YA10N
	IdbValue	Npt20; // N ����� 2, ��� YA20N
	IdbValue	Npt30; // N ����� 3, ��� YA30N
	IdbValue	Npt40; // N ����� 4, ��� YA40N

	IdbValue	Ihn10; // ��������� �������� ����� ����� 1
	IdbValue	Ihn20; // ��������� �������� ����� ����� 2
	IdbValue	Ihn30; // ��������� �������� ����� ����� 3
	IdbValue	Ihn40; // ��������� �������� ����� ����� 4

	IdbValue	Ign10; // ��������� ������� ����� ����� ����� 1
	IdbValue	Ign20; // ��������� ������� ����� ����� ����� 1
	IdbValue	Ign30; // ��������� ������� ����� ����� ����� 1
	IdbValue	Ign40; // ��������� ������� ����� ����� ����� 1

	IdbValue	N1k;   // �������� �� 1�,  ��� YAN_S

	Calc_Npt(&Npt10, &GMpt10, &Thn10, &Ihn10, &Tgn10, &Ign10, YCP_OUT, YCDP );
	Calc_Npt(&Npt20, &GMpt20, &Thn20, &Ihn20, &Tgn20, &Ign20, YCP_OUT, YCDP );
	Calc_Npt(&Npt30, &GMpt30, &Thn30, &Ihn30, &Tgn30, &Ign30, YCP_OUT, YCDP );
	Calc_Npt(&Npt40, &GMpt40, &Thn40, &Ihn40, &Tgn40, &Ign40, YCP_OUT, YCDP );

	N1k.dValue = Npt10.v() + Npt20.v() + Npt30.v() + Npt40.v() + Pot1k;

	if( Npt10.ok() && Npt20.ok() && Npt30.ok() && Npt40.ok())
		 N1k.cStatus = STATUS_NORMAL;
	else N1k.cStatus = STATUS_UNDEF;

	// ������ � ���
	WriteToIdb("Npt10",&Npt10);
	WriteToIdb("Npt20",&Npt20);
	WriteToIdb("Npt30",&Npt30);
	WriteToIdb("Npt40",&Npt40);
	WriteToIdb("N1k",&N1k);

	WriteToIdb("Ihn10",&Ihn10);
	WriteToIdb("Ihn20",&Ihn20);
	WriteToIdb("Ihn30",&Ihn30);
	WriteToIdb("Ihn40",&Ihn40);

	WriteToIdb("Ign10",&Ign10);
	WriteToIdb("Ign20",&Ign20);
	WriteToIdb("Ign30",&Ign30);
	WriteToIdb("Ign40",&Ign40);
}


void CProgCalc::Calc_Npg(IdbValue* pNpg, IdbValue* pGMpg, IdbValue* pPpg, IdbValue* pISpg, IdbValue* pAPpg, IdbValue* pPpw, IdbValue* pIpg, IdbValue* pTpw, IdbValue* pAPpw, IdbValue* pATpw, IdbValue* pNgcn, IdbValue* pGppgl, IdbValue* pGppol, IdbValue* pGnpgl, IdbValue* pGnpol)
{// 3.12.3 ������ �������� �������� ��� �� ������� ����������� ���� � ��

	// ������� �������� ����� ��� (�� �������� � �� ������� ������)
	double	Gprod 	= 0;
	double 	Ppg		= 0.0980665 * (pPpg->ok() ? pPpg->v() : pAPpg->v());
	double	Ppw		= 0.0980665 * (pPpw->ok() ? pPpw->v() : pAPpw->v());
	double	Tpw		= pTpw->ok() ? pTpw->v() : pATpw->v();

	// ��������� ������ ����������� ������� ��������
	if( pGppgl->ok() )	Gprod += pGppgl->v();
	if( pGppol->ok() )	Gprod += pGppol->v();
	if( pGnpgl->ok() )	Gprod += pGnpgl->v();
	if( pGnpol->ok() )	Gprod += pGnpol->v();

//Gprod = 22.5;

	double	ISpg	= WaterSteamProperty::getSteamEnthalpyS(Ppg);
	double	Ipg		= WaterSteamProperty::getWaterEnthalpy(Ppw,Tpw);

	double	Nprod	= (1000.0/3600.0) * (Gprod/1000.0) * (ISpg - Ipg);

	pNpg->dValue	= (1000.0/3600.0) * pGMpg->v() * (WaterSteamProperty::getSteamEnthalpyS(Ppg) - WaterSteamProperty::getWaterEnthalpy(Ppw,Tpw));


//puts(CStr("GMpg=") + pGMpg->v() + " Ppg=" + Ppg + " Ppw=" + Ppw + " Tpw=" + Tpw + " Npg=" + pNpg->dValue + " EntPg=" + WaterSteamProperty::getWaterEnthalpyS(Ppg) + " EntPw=" + WaterSteamProperty::getWaterEnthalpy(Ppw,Tpw) + " Nprod=" + Nprod);


//puts(CStr("") + "1 Npg=" + pNpg->dValue + " Nprod=" + Nprod + " Gprod=" + Gprod);

	pNpg->dValue   -= pNgcn->v();
	pNpg->dValue   -= Nprod;
	pISpg->dValue = ISpg;
	pIpg->dValue = Ipg;

	// �������� �� ����� ���������� ������ ���� ���������� ��� ��������� ����������� � ������� (����� �������� ��)
	if( pGMpg->ok() && (pPpg->ok()||pAPpg->ok()) && (pPpw->ok()||pAPpw->ok()) && (pTpw->ok()||pATpw->ok()) && pNgcn->ok() )
	{
		 pNpg->cStatus = STATUS_NORMAL;
		 pISpg->cStatus = STATUS_NORMAL;
		 pIpg->cStatus = STATUS_NORMAL;
	}
	else
	{
		pNpg->cStatus = STATUS_UNDEF;
		pISpg->cStatus = STATUS_UNDEF;
		pIpg->cStatus = STATUS_UNDEF;
	}
//puts(CStr("") + "2 Npg=" + pNpg->dValue);
}

void CProgCalc::Calc_N2k()
{// 3.12.3 ������ �������� �������� ��� �� ������� ����������� ���� � ��

	IdbValue	GMpg10;  ReadFromIdb("GMpg10", &GMpg10);  // F �� � ��-1 (��������), ��/� RL71F
	IdbValue	GMpg20;  ReadFromIdb("GMpg20", &GMpg20);  // F �� � ��-2 (��������), ��/� RL72F
	IdbValue	GMpg30;  ReadFromIdb("GMpg30", &GMpg30);  // F �� � ��-3 (��������), ��/� RL73F
	IdbValue	GMpg40;  ReadFromIdb("GMpg40", &GMpg40);  // F �� � ��-4 (��������), ��/� RL74F

	IdbValue	Ngcn10;  ReadFromIdb("Ngcn10", &Ngcn10);  // �������� ���-1, ��� YD10N10
	IdbValue	Ngcn20;  ReadFromIdb("Ngcn20", &Ngcn20);  // �������� ���-2, ��� YD20N10
	IdbValue	Ngcn30;  ReadFromIdb("Ngcn30", &Ngcn30);  // �������� ���-3, ��� YD30N10
	IdbValue	Ngcn40;  ReadFromIdb("Ngcn40", &Ngcn40);  // �������� ���-4, ��� YD40N10

	IdbValue	Gppgl10; ReadFromIdb("Gppgl10", &Gppgl10);  // F ������. �������� ��.����� ��-1, �/� RY21F01
	IdbValue	Gppgl20; ReadFromIdb("Gppgl20", &Gppgl20);  // F ������. �������� ��.����� ��-2, �/� RY22F01
	IdbValue	Gppgl30; ReadFromIdb("Gppgl30", &Gppgl30);  // F ������. �������� ��.����� ��-3, �/� RY23F01
	IdbValue	Gppgl40; ReadFromIdb("Gppgl40", &Gppgl40);  // F ������. �������� ��.����� ��-4, �/� RY24F01

	IdbValue	Gppol10; ReadFromIdb("Gppol10", &Gppol10);  // F ������. �������� �����.����� ��-1, �/� RY21F02
	IdbValue	Gppol20; ReadFromIdb("Gppol20", &Gppol20);  // F ������. �������� �����.����� ��-2, �/� RY22F02
	IdbValue	Gppol30; ReadFromIdb("Gppol30", &Gppol30);  // F ������. �������� �����.����� ��-3, �/� RY23F02
	IdbValue	Gppol40; ReadFromIdb("Gppol40", &Gppol40);  // F ������. �������� �����.����� ��-4, �/� RY24F02

	IdbValue	Gnpgl10; ReadFromIdb("Gnpgl10", &Gnpgl10);  // F ����. �������� ��.����� ��-1, �/� RY11F01
	IdbValue	Gnpgl20; ReadFromIdb("Gnpgl20", &Gnpgl20);  // F ����. �������� ��.����� ��-2, �/� RY12F01
	IdbValue	Gnpgl30; ReadFromIdb("Gnpgl30", &Gnpgl30);  // F ����. �������� ��.����� ��-3, �/� RY13F01
	IdbValue	Gnpgl40; ReadFromIdb("Gnpgl40", &Gnpgl40);  // F ����. �������� ��.����� ��-4, �/� RY14F01

	IdbValue	Gnpol10; ReadFromIdb("Gnpol10", &Gnpol10);  // F ����. �������� �����.����� ��-1, �/� RY11F02
	IdbValue	Gnpol20; ReadFromIdb("Gnpol20", &Gnpol20);  // F ����. �������� �����.����� ��-2, �/� RY12F02
	IdbValue	Gnpol30; ReadFromIdb("Gnpol30", &Gnpol30);  // F ����. �������� �����.����� ��-3, �/� RY13F02
	IdbValue	Gnpol40; ReadFromIdb("Gnpol40", &Gnpol40);  // F ����. �������� �����.����� ��-4, �/� RY14F02

	IdbValue	Ppg10;	 ReadFromIdb("Ppg10",	&Ppg10);	// P ���� � ��-1, ��/��2 YB10P10
	IdbValue	Ppg20;	 ReadFromIdb("Ppg20",	&Ppg20);	// P ���� � ��-2, ��/��2 YB20P10
	IdbValue	Ppg30;	 ReadFromIdb("Ppg30",	&Ppg30);	// P ���� � ��-3, ��/��2 YB30P10
	IdbValue	Ppg40;	 ReadFromIdb("Ppg40",	&Ppg40);	// P ���� � ��-4, ��/��2 YB40P10
	IdbValue	APpg;	 ReadFromIdb("APpg",	&APpg);		// ��. P ���� � ��, ��/��2 		YBP_A

	LONG_LST	PpwLst;
	IdbValue	Ppw10;	ReadFromIdb("Ppw10",	&Ppw10); PpwLst.Add((int)&Ppw10);	// P �� � ��-1, ��/��2 RL71P01
	IdbValue	Ppw20;	ReadFromIdb("Ppw20",	&Ppw20); PpwLst.Add((int)&Ppw20);	// P �� � ��-2, ��/��2 RL72P01
	IdbValue	Ppw30;	ReadFromIdb("Ppw30",	&Ppw30); PpwLst.Add((int)&Ppw30);	// P �� � ��-3, ��/��2 RL73P01
	IdbValue	Ppw40;	ReadFromIdb("Ppw40",	&Ppw40); PpwLst.Add((int)&Ppw40);	// P �� � ��-4, ��/��2 RL74P01
	IdbValue	APpw;										// ������� P ��, ��/��2

	IdbValue	Tpw10;	ReadFromIdb("Tpw10",	&Tpw10);	// T �� ����� ��-1, ���� RL71T01
	IdbValue	Tpw20;	ReadFromIdb("Tpw20",	&Tpw20);	// T �� ����� ��-2, ���� RL72T01
	IdbValue	Tpw30;	ReadFromIdb("Tpw30",	&Tpw30);	// T �� ����� ��-3, ���� RL73T01
	IdbValue	Tpw40;	ReadFromIdb("Tpw40",	&Tpw40);	// T �� ����� ��-4, ���� RL74T01
	IdbValue	ATpw;  	ReadFromIdb("ATpw",		&ATpw);		// ������� � ���. ����, ���� 	RLT_A

	LONG_LST	SptLst;
	IdbValue	Spt10;		ReadFromIdb("Spt10",  &Spt10);  SptLst.Add((int)&Spt10); // ��������� 1-�� ����� 1�, ���/����/--/�������  YA10ST
	IdbValue	Spt20;		ReadFromIdb("Spt20",  &Spt20);  SptLst.Add((int)&Spt20); // ��������� 2-�� ����� 1�, ���/����/--/�������  YA20ST
	IdbValue	Spt30;		ReadFromIdb("Spt30",  &Spt30);  SptLst.Add((int)&Spt30); // ��������� 3-�� ����� 1�, ���/����/--/�������  YA30ST
	IdbValue	Spt40;		ReadFromIdb("Spt40",  &Spt40);  SptLst.Add((int)&Spt40); // ��������� 4-�� ����� 1�, ���/����/--/�������  YA40ST

	IdbValue	YCMode;		ReadFromIdb("YC00MODE",  &YCMode);

	double	    Pot1k	= pConstTbl->GetConstValue("POT1K");	// ������ ����� � 1 �������, ���, ������������ �� ����� ���
	double	    Pot2k	= pConstTbl->GetConstValue("POT2K");	// ������ ����� �� 2 �������, ���, ������������ �� ����� ���

	// ����������
	IdbValue	Npg10;	// N ��-1, ��� YB10N
	IdbValue	Npg20;	// N ��-2, ��� YB20N
	IdbValue	Npg30;	// N ��-3, ��� YB30N
	IdbValue	Npg40;	// N ��-4, ��� YB40N
	IdbValue	N2k;	// ����. N ��, ���  YBN_S

	IdbValue	ISpg10; // ��������� ���� ��1
	IdbValue	ISpg20; // ��������� ���� ��2
	IdbValue	ISpg30; // ��������� ���� ��3
	IdbValue	ISpg40; // ��������� ���� ��4

	IdbValue	Ipg10; 	// ��������� ����������� ���� ��1
	IdbValue	Ipg20; 	// ��������� ����������� ���� ��2
	IdbValue	Ipg30; 	// ��������� ����������� ���� ��3
	IdbValue	Ipg40; 	// ��������� ����������� ���� ��4

	Calc_Avg(&APpw, &PpwLst, &SptLst);

	Calc_Npg(&Npg10, &GMpg10,&Ppg10, &ISpg10, &APpg, &Ppw10, &Ipg10,  &Tpw10, &APpw, &ATpw, &Ngcn10, &Gppgl10, &Gppol10, &Gnpgl10, &Gnpol10);
	Calc_Npg(&Npg20, &GMpg20,&Ppg20, &ISpg20, &APpg, &Ppw20, &Ipg20, &Tpw20, &APpw, &ATpw, &Ngcn20, &Gppgl20, &Gppol20, &Gnpgl20, &Gnpol20);
	Calc_Npg(&Npg30, &GMpg30,&Ppg30, &ISpg30, &APpg, &Ppw30, &Ipg30, &Tpw30, &APpw, &ATpw, &Ngcn30, &Gppgl30, &Gppol30, &Gnpgl30, &Gnpol30);
	Calc_Npg(&Npg40, &GMpg40,&Ppg40, &ISpg40, &APpg, &Ppw40, &Ipg40, &Tpw40, &APpw, &ATpw, &Ngcn40, &Gppgl40, &Gppol40, &Gnpgl40, &Gnpol40);

	N2k.cStatus = YCMode.v()<=3 ? STATUS_NORMAL : STATUS_UNDEF;

	// ����������� ����������� �������� �� �� ���������� ������
	if( Spt10.ok() && Spt10.v()==STPT_ON ){ if( Npg10.ok() && YCMode.v()<=3 ) N2k.dValue += Npg10.v(); else N2k.cStatus = STATUS_UNDEF; }
	if( Spt20.ok() && Spt20.v()==STPT_ON ){ if( Npg20.ok() && YCMode.v()<=3 ) N2k.dValue += Npg20.v(); else N2k.cStatus = STATUS_UNDEF; }
	if( Spt30.ok() && Spt30.v()==STPT_ON ){ if( Npg30.ok() && YCMode.v()<=3 ) N2k.dValue += Npg30.v(); else N2k.cStatus = STATUS_UNDEF; }
	if( Spt40.ok() && Spt40.v()==STPT_ON ){ if( Npg40.ok() && YCMode.v()<=3 ) N2k.dValue += Npg40.v(); else N2k.cStatus = STATUS_UNDEF; }

	// ����� ������ �����
	N2k.dValue += Pot1k + Pot2k;


	// ������ � ���
	WriteToIdb("Npg10",&Npg10);
	WriteToIdb("Npg20",&Npg20);
	WriteToIdb("Npg30",&Npg30);
	WriteToIdb("Npg40",&Npg40);

	WriteToIdb("ISpg10",&ISpg10);
	WriteToIdb("ISpg20",&ISpg20);
	WriteToIdb("ISpg30",&ISpg30);
	WriteToIdb("ISpg40",&ISpg40);

	WriteToIdb("Ipg10",&Ipg10);
	WriteToIdb("Ipg20",&Ipg20);
	WriteToIdb("Ipg30",&Ipg30);
	WriteToIdb("Ipg40",&Ipg40);

	WriteToIdb("N2k",&N2k);
}


void CProgCalc::Calc_Npvd(IdbValue* pNpvd, IdbValue* pGMpvd, IdbValue* pAPpg, IdbValue* pISpvd, IdbValue* pPpvd, IdbValue* pIpvd, IdbValue* pTpvd)
{// 3.12.4 ������ �������� �������� ��� �� ������� ����������� ���� �� ���

	double 	APpg	= 0.0980665 * pAPpg->v();
	double	Ppvd	= 0.0980665 * pPpvd->v();
	double	Tpvd	= pTpvd->v();
	double	ISpvd	= WaterSteamProperty::getSteamEnthalpyS(APpg);
	double	Ipvd	= WaterSteamProperty::getWaterEnthalpy(Ppvd,Tpvd);


	pNpvd->dValue	= (1000.0/3600.0) * pGMpvd->v() * (ISpvd - Ipvd);
	pISpvd->dValue 	= ISpvd;
	pIpvd->dValue 	= Ipvd;


//puts(CStr("GMpvd=") + pGMpvd->v() + " APpg=" + APpg + " Ppvd=" + Ppvd + " Tpvd=" + Tpvd + " ");
	if( pGMpvd->ok() && pAPpg->ok() && pPpvd->ok() && pTpvd->ok() )
	{
		 pNpvd->cStatus = STATUS_NORMAL;
		 pISpvd->cStatus = STATUS_NORMAL;
		 pIpvd->cStatus = STATUS_NORMAL;
	}
	else
	{
		pNpvd->cStatus = STATUS_UNDEF;
		pISpvd->cStatus = STATUS_UNDEF;
		pIpvd->cStatus = STATUS_UNDEF;
	}
}


void CProgCalc::Calc_Npvd()
{// 3.12.4 ������ �������� �������� ��� �� ������� ����������� ���� �� ���
	LONG_LST	NgcnLst;
	IdbValue	Ngcn10; 	ReadFromIdb("Ngcn10", &Ngcn10);  NgcnLst.Add((int)&Ngcn10); // �������� ���-1, ��� YD10N10
	IdbValue	Ngcn20;  	ReadFromIdb("Ngcn20", &Ngcn20);  NgcnLst.Add((int)&Ngcn20); // �������� ���-2, ��� YD20N10
	IdbValue	Ngcn30;  	ReadFromIdb("Ngcn30", &Ngcn30);  NgcnLst.Add((int)&Ngcn30); // �������� ���-3, ��� YD30N10
	IdbValue	Ngcn40;  	ReadFromIdb("Ngcn40", &Ngcn40);  NgcnLst.Add((int)&Ngcn40); // �������� ���-4, ��� YD40N10

	IdbValue	Tpvd1;		ReadFromIdb("Tpvd1",	&Tpvd1);	// T �� ����� 1-�� ����� ���, ����  RL74T02
	IdbValue	Tpvd2;		ReadFromIdb("Tpvd2",	&Tpvd2);	// T �� ����� 2-�� ����� ���, ����  RL74T03
	IdbValue	Ppvd;		ReadFromIdb("Ppvd",		&Ppvd);		// P �� ����� ���  RL74P02

	IdbValue	APpg;	  	ReadFromIdb("APpg",		&APpg);		// ��. P ���� � ��, ��/��2 		YBP_A
//	IdbValue	APpg;	  	ReadFromIdb("Ppvd",		&APpg);		// P �� ����� ���, ��/��2  RL74P02

	IdbValue	GMpvd1;  	ReadFromIdb("GMpvd1",&GMpvd1); // F �� � 1-�� ����� ���, ��/�
	IdbValue	GMpvd2;  	ReadFromIdb("GMpvd2",&GMpvd2); // F �� � 2-�� ����� ���, ��/�

	IdbValue	YCMode;  	ReadFromIdb("YC00MODE",&YCMode); // ����� ������ ��

	double	    Pot1k		= pConstTbl->GetConstValue("POT1K");		// ������ ����� � 1 �������, ���, ������������ �� ����� ���
	double	    Pot2k		= pConstTbl->GetConstValue("POT2K");		// ������ ����� �� 2 �������, ���, ������������ �� ����� ���

	bool		bEnableKgtn	= pConstTbl->GetConstValue("ENABLE_KGTN")==1 ? true : false;	// ��������� ���� ����

	// ����������
	IdbValue	Npvd1;	// N 1-�� ����� ���, ��� RL61N
	IdbValue	Npvd2;	// N 2-�� ����� ���, ��� RL62N
	IdbValue	NOpvd;	// ����. N  ���, ��� RLN_S

	IdbValue	ISpvd1;	// ��������� ���� ���1
	IdbValue	ISpvd2;	// ��������� ���� ���2

	IdbValue	Ipvd1;	// ��������� ����������� ���� ���1
	IdbValue	Ipvd2;	// ��������� ����������� ���� ���2

	IdbValue	NSgcn;	Calc_Sum(&NSgcn, &NgcnLst, true);
	IdbValue	Nkgtn;	// N ����

	Calc_Npvd(&Npvd1, &GMpvd1, &APpg, &ISpvd1, &Ppvd, &Ipvd1, &Tpvd1);
	Calc_Npvd(&Npvd2, &GMpvd2, &APpg, &ISpvd2, &Ppvd, &Ipvd2, &Tpvd2);

//	NOpvd.dValue  += Npvd1.v();
//	NOpvd.dValue  += Npvd2.v();

	// 24.07.2018 �� ������ �������� ���������� ������ ����������� Npvd
	if( Npvd1.ok() ) NOpvd.dValue  += Npvd1.v();
	if( Npvd2.ok() ) NOpvd.dValue  += Npvd2.v();

	// ����� ������ �����
	NOpvd.dValue  += Pot1k + Pot2k;
	NOpvd.dValue  -= NSgcn.v();

	if( bEnableKgtn  )
	{
		IdbValue	Kgtn1;  	ReadFromIdb("Kgtn1",&Kgtn1); // ��������� ������ �������� �� ������ ����
		IdbValue	Kgtn2;  	ReadFromIdb("Kgtn2",&Kgtn2); // ��������� ������ �������� �� ������ ����

		IdbValue	Tkcpp; 		ReadFromIdb("Tkcpp", &Tkcpp); 	// � �����.���� �� ���.���� �� ��, ����
		IdbValue	Gkcpp; 		ReadFromIdb("Gkcpp", &Gkcpp); 	// ���������� ������ �������� ���� �� ���, �/���

		double	    K_PPBO		= pConstTbl->GetConstValue("K_PPBO");		// �����. �������� ��� ��������� ��� ���
		double	    K_IPBO		= pConstTbl->GetConstValue("K_IPBO");		// ����������� ��������� ��� ��� = 854,107
		double	    K_TPBO		= pConstTbl->GetConstValue("K_TPBO");		// �����. ����-� ��� ��������� ��� ��� = 200 ����
		double	    K_PPBD7		= pConstTbl->GetConstValue("K_PPBD7");		// ������� �������� �����.���� ����� ���-7 = 80 ��/��2
		double		K_L1		= pConstTbl->GetConstValue("K_L1");			// �-� L1 ��� ������� ��������� ��� ��� = 0,00525539412
		double		K_L2		= pConstTbl->GetConstValue("K_L2");			// �-� L1 ��� ������� ��������� ��� ��� = 0,00000379889719
		double		K_L3		= pConstTbl->GetConstValue("K_L3");			// �-� L1 ��� ������� ��������� ��� ��� = 0,0000565607

		if( Kgtn1.v()==1 || Kgtn2.v()==1 )
		{
			double	IPAPS	= WaterSteamProperty::getSteamEnthalpyS(APpg.ok() ? APpg.v() : K_PPBO); // ��������� ���� � ��
			double	IKCPP 	= 0; // ��������� ���������� �������� ���� ���

			IKCPP = K_IPBO * (1 + K_L1*(Tkcpp.v()  - K_TPBO) + K_L2*(Tkcpp.v()  - K_TPBO)*(Tkcpp.v()  - K_TPBO) + K_L3*(K_PPBD7 - K_PPBO));
			Nkgtn.dValue = Gkcpp.v() * (IPAPS - IKCPP) / 3600;
			Nkgtn.cStatus = STATUS_NORMAL;
		}
	}

	if( Nkgtn.ok() )	NOpvd.dValue += Nkgtn.v();


//puts(CStr("Calc_Npvd Npvd1.v()=") + Npvd1.v() + " Npvd2.v()=" + Npvd2.v());

	if( Npvd1.ok() && Npvd2.ok() && YCMode.v()<=3 )
	{
		 NOpvd.cStatus = STATUS_NORMAL;
	}
	else
	{
		NOpvd.dValue  = 0.0;
		NOpvd.cStatus = STATUS_UNDEF;
	}

	WriteToIdb("Npvd1",&Npvd1);
	WriteToIdb("Npvd2",&Npvd2);

	WriteToIdb("ISpvd1",&ISpvd1);
	WriteToIdb("ISpvd2",&ISpvd2);

	WriteToIdb("Ipvd1",&Ipvd1);
	WriteToIdb("Ipvd2",&Ipvd2);

	WriteToIdb("NOpvd",&NOpvd);
}

void CProgCalc::Calc_Nik()
{// 3.12.7 ������ �������� �������� ��� �� ���������� ������������� �����

	LONG_LST	FikLst;
	LONG_LST	NikLst;
	DOUBLE_LST	VesNikLst;
	bool		bEnableMsg = false;

	// �������������� (�������) ��������
	IdbValue	Fik11_E;	ReadFromIdb("Fik1_E",  &Fik11_E,bEnableMsg);		FikLst.Add((int)&Fik11_E); 	// ���� ��������� ������ ������ ���-1 � ��
	IdbValue	Nik11_E;	ReadFromIdb("Nik11_E", &Nik11_E,bEnableMsg);		NikLst.Add((int)&Nik11_E);	// �������� �� ������ ������ ���-1 � ��, ���

	IdbValue	Fik12_E;	ReadFromIdb("Fik1_E",  &Fik12_E,bEnableMsg);		FikLst.Add((int)&Fik12_E); 	// ���� ��������� ������ ������ ���-1 � ��
	IdbValue	Nik12_E;	ReadFromIdb("Nik12_E", &Nik12_E,bEnableMsg);		NikLst.Add((int)&Nik12_E);	// �������� �� ������ ������ ���-1 � ��, ���

	IdbValue	Fik21_E;	ReadFromIdb("Fik2_E",  &Fik21_E,bEnableMsg);		FikLst.Add((int)&Fik21_E); 	// ���� ��������� ������ ������ ���-2 � ��
	IdbValue	Nik21_E;	ReadFromIdb("Nik21_E", &Nik21_E,bEnableMsg);		NikLst.Add((int)&Nik21_E);	// �������� �� ������ ������ ���-2 � ��, ���

	IdbValue	Fik22_E;	ReadFromIdb("Fik2_E",  &Fik22_E,bEnableMsg);		FikLst.Add((int)&Fik22_E); 	// ���� ��������� ������ ������ ���-2 � ��
	IdbValue	Nik22_E;	ReadFromIdb("Nik22_E", &Nik22_E,bEnableMsg);		NikLst.Add((int)&Nik22_E);	// �������� �� ������ ������ ���-2 � ��, ���

	IdbValue	Fik31_E;	ReadFromIdb("Fik3_E",  &Fik31_E,bEnableMsg);		FikLst.Add((int)&Fik31_E); 	// ���� ��������� ������ ������ ���-3 � ��
	IdbValue	Nik31_E;	ReadFromIdb("Nik31_E", &Nik31_E,bEnableMsg);		NikLst.Add((int)&Nik31_E);	// �������� �� ������ ������ ���-3 � ��, ���

	IdbValue	Fik32_E;	ReadFromIdb("Fik3_E",  &Fik32_E,bEnableMsg);		FikLst.Add((int)&Fik32_E); 	// ���� ��������� ������ ������ ���-3 � ��
	IdbValue	Nik32_E;	ReadFromIdb("Nik32_E", &Nik32_E,bEnableMsg);		NikLst.Add((int)&Nik32_E);	// �������� �� ������ ������ ���-3 � ��, ���

	IdbValue	Fik41_E;	ReadFromIdb("Fik4_E",  &Fik41_E,bEnableMsg);		FikLst.Add((int)&Fik41_E); 	// ���� ��������� ������ ������ ���-4 � ��
	IdbValue	Nik41_E;	ReadFromIdb("Nik41_E", &Nik41_E,bEnableMsg);		NikLst.Add((int)&Nik41_E);	// �������� �� ������ ������ ���-4 � ��, ���

	IdbValue	Fik42_E;	ReadFromIdb("Fik4_E",  &Fik42_E,bEnableMsg);		FikLst.Add((int)&Fik42_E); 	// ���� ��������� ������ ������ ���-4 � ��
	IdbValue	Nik42_E;	ReadFromIdb("Nik42_E", &Nik42_E,bEnableMsg);		NikLst.Add((int)&Nik42_E);	// �������� �� ������ ������ ���-4 � ��, ���

	IdbValue	Fik51_E;	ReadFromIdb("Fik5_E",  &Fik51_E,bEnableMsg);		FikLst.Add((int)&Fik51_E); 	// ���� ��������� ������ ������ ���-5 � ��
	IdbValue	Nik51_E;	ReadFromIdb("Nik51_E", &Nik51_E,bEnableMsg);		NikLst.Add((int)&Nik51_E);	// �������� �� ������ ������ ���-5 � ��, ���

	IdbValue	Fik52_E;	ReadFromIdb("Fik5_E",  &Fik52_E,bEnableMsg);		FikLst.Add((int)&Fik52_E); 	// ���� ��������� ������ ������ ���-5 � ��
	IdbValue	Nik52_E;	ReadFromIdb("Nik52_E", &Nik52_E,bEnableMsg);		NikLst.Add((int)&Nik52_E);	// �������� �� ������ ������ ���-5 � ��, ���

	IdbValue	Fik61_E;	ReadFromIdb("Fik6_E",  &Fik61_E,bEnableMsg);		FikLst.Add((int)&Fik61_E); 	// ���� ��������� ������ ������ ���-6 � ��
	IdbValue	Nik61_E;	ReadFromIdb("Nik61_E", &Nik61_E,bEnableMsg);		NikLst.Add((int)&Nik61_E);	// �������� �� ������ ������ ���-6 � ��, ���

	IdbValue	Fik62_E;	ReadFromIdb("Fik6_E",  &Fik62_E,bEnableMsg);		FikLst.Add((int)&Fik62_E); 	// ���� ��������� ������ ������ ���-6 � ��
	IdbValue	Nik62_E;	ReadFromIdb("Nik62_E", &Nik62_E,bEnableMsg);		NikLst.Add((int)&Nik62_E);	// �������� �� ������ ������ ���-6 � ��, ���

	// ������������� (��������) ��������
	IdbValue	Fik1_P;	ReadFromIdb("Fik1_P", &Fik1_P,bEnableMsg);		FikLst.Add((int)&Fik1_P); 	// ���� ��������� ������ ���-1 � ��
	IdbValue	Nik1_P;	ReadFromIdb("Nik1_P", &Nik1_P,bEnableMsg);		NikLst.Add((int)&Nik1_P);	// �������� �� ������ ���-1 � ��, ���

	IdbValue	Fik2_P;	ReadFromIdb("Fik2_P", &Fik2_P,bEnableMsg);		FikLst.Add((int)&Fik2_P); 	// ���� ��������� ������ ���-2 � ��
	IdbValue	Nik2_P;	ReadFromIdb("Nik2_P", &Nik2_P,bEnableMsg);		NikLst.Add((int)&Nik2_P);	// �������� �� ������ ���-2 � ��, ���

	IdbValue	Fik3_P;	ReadFromIdb("Fik3_P", &Fik3_P,bEnableMsg);		FikLst.Add((int)&Fik3_P); 	// ���� ��������� ������ ���-3 � ��
	IdbValue	Nik3_P;	ReadFromIdb("Nik3_P", &Nik3_P,bEnableMsg);		NikLst.Add((int)&Nik3_P);	// �������� �� ������ ���-3 � ��, ���

	IdbValue	Fik4_P;	ReadFromIdb("Fik4_P", &Fik4_P,bEnableMsg);		FikLst.Add((int)&Fik4_P); 	// ���� ��������� ������ ���-4 � ��
	IdbValue	Nik4_P;	ReadFromIdb("Fik4_P", &Nik4_P,bEnableMsg);		NikLst.Add((int)&Nik4_P);	// �������� �� ������ ���-4 � ��, ���

	IdbValue	Fik5_P;	ReadFromIdb("Fik5_P", &Fik5_P,bEnableMsg);		FikLst.Add((int)&Fik5_P); 	// ���� ��������� ������ ���-5 � ��
	IdbValue	Nik5_P;	ReadFromIdb("Fik4_P", &Nik5_P,bEnableMsg);		NikLst.Add((int)&Nik5_P);	// �������� �� ������ ���-5 � ��, ���

	IdbValue	Fik6_P;	ReadFromIdb("Fik6_P", &Fik6_P,bEnableMsg);		FikLst.Add((int)&Fik6_P); 	// ���� ��������� ������ ���-6 � ��
	IdbValue	Nik6_P;	ReadFromIdb("Fik4_P", &Nik6_P,bEnableMsg);		NikLst.Add((int)&Nik6_P);	// �������� �� ������ ���-6 � ��, ���

	// ��������� ��������
	IdbValue	Fik1_I;	ReadFromIdb("Fik1_I", &Fik1_I,bEnableMsg);		FikLst.Add((int)&Fik1_I); 	// ���� ��������� ������ ���-1 � ��
	IdbValue	Nik1_I;	ReadFromIdb("Nik1_I", &Nik1_I,bEnableMsg);		NikLst.Add((int)&Nik1_I);	// �������� �� ������ ���-1 � ��, ���

	IdbValue	Fik2_I;	ReadFromIdb("Fik2_I", &Fik2_I,bEnableMsg);		FikLst.Add((int)&Fik2_I); 	// ���� ��������� ������ ���-2 � ��
	IdbValue	Nik2_I;	ReadFromIdb("Nik2_I", &Nik2_I,bEnableMsg);		NikLst.Add((int)&Nik2_I);	// �������� �� ������ ���-2 � ��, ���

	IdbValue	Fik3_I;	ReadFromIdb("Fik3_I", &Fik3_I,bEnableMsg);		FikLst.Add((int)&Fik3_I); 	// ���� ��������� ������ ���-3 � ��
	IdbValue	Nik3_I;	ReadFromIdb("Nik3_I", &Nik3_I,bEnableMsg);		NikLst.Add((int)&Nik3_I);	// �������� �� ������ ���-3 � ��, ���

	IdbValue	Fik4_I;	ReadFromIdb("Fik4_I", &Fik4_I,bEnableMsg);		FikLst.Add((int)&Fik4_I); 	// ���� ��������� ������ ���-4 � ��
	IdbValue	Nik4_I;	ReadFromIdb("Nik4_I", &Nik4_I,bEnableMsg);		NikLst.Add((int)&Nik4_I);	// �������� �� ������ ���-4 � ��, ���

	IdbValue	Fik5_I;	ReadFromIdb("Fik5_I", &Fik5_I,bEnableMsg);		FikLst.Add((int)&Fik5_I); 	// ���� ��������� ������ ���-5 � ��
	IdbValue	Nik5_I;	ReadFromIdb("Nik5_I", &Nik5_I,bEnableMsg);		NikLst.Add((int)&Nik5_I);	// �������� �� ������ ���-5 � ��, ���

	IdbValue	Fik6_I;	ReadFromIdb("Fik6_I", &Fik6_I,bEnableMsg);		FikLst.Add((int)&Fik6_P); 	// ���� ��������� ������ ���-6 � ��
	IdbValue	Nik6_I;	ReadFromIdb("Nik6_I", &Nik6_I,bEnableMsg);		NikLst.Add((int)&Nik6_I);	// �������� �� ������ ���-6 � ��, ���

	// ��������� (����) �������������� (�������) ��������
	double	    VesNik11_E = pConstTbl->GetConstValue("VesNik11_E");	VesNikLst.AddPtr(&VesNik11_E); //��� ��� ������ ������ ���-1 � ��
	double	    VesNik12_E = pConstTbl->GetConstValue("VesNik12_E");	VesNikLst.AddPtr(&VesNik12_E); //��� ��� ������ ������ ���-1 � ��

	double	    VesNik21_E = pConstTbl->GetConstValue("VesNik21_E");	VesNikLst.AddPtr(&VesNik21_E); //��� ��� ������ ������ ���-2 � ��
	double	    VesNik22_E = pConstTbl->GetConstValue("VesNik22_E");	VesNikLst.AddPtr(&VesNik22_E); //��� ��� ������ ������ ���-2 � ��

	double	    VesNik31_E = pConstTbl->GetConstValue("VesNik31_E");	VesNikLst.AddPtr(&VesNik31_E); //��� ��� ������ ������ ���-3 � ��
	double	    VesNik32_E = pConstTbl->GetConstValue("VesNik32_E");	VesNikLst.AddPtr(&VesNik32_E); //��� ��� ������ ������ ���-3 � ��

	double	    VesNik41_E = pConstTbl->GetConstValue("VesNik41_E");	VesNikLst.AddPtr(&VesNik41_E); //��� ��� ������ ������ ���-4 � ��
	double	    VesNik42_E = pConstTbl->GetConstValue("VesNik42_E");	VesNikLst.AddPtr(&VesNik42_E); //��� ��� ������ ������ ���-4 � ��

	double	    VesNik51_E = pConstTbl->GetConstValue("VesNik51_E");	VesNikLst.AddPtr(&VesNik51_E); //��� ��� ������ ������ ���-5 � ��
	double	    VesNik52_E = pConstTbl->GetConstValue("VesNik52_E");	VesNikLst.AddPtr(&VesNik52_E); //��� ��� ������ ������ ���-5 � ��

	double	    VesNik61_E = pConstTbl->GetConstValue("VesNik61_E");	VesNikLst.AddPtr(&VesNik61_E); //��� ��� ������ ������ ���-6 � ��
	double	    VesNik62_E = pConstTbl->GetConstValue("VesNik62_E");	VesNikLst.AddPtr(&VesNik62_E); //��� ��� ������ ������ ���-6 � ��

	// ��������� (����) ������������� (��������) ��������
	double	    VesNik1_P = pConstTbl->GetConstValue("VesNik1_P");	VesNikLst.AddPtr(&VesNik1_P); //��� ��� ������ ���-1 � ��
	double	    VesNik2_P = pConstTbl->GetConstValue("VesNik2_P");	VesNikLst.AddPtr(&VesNik2_P); //��� ��� ������ ���-2 � ��
	double	    VesNik3_P = pConstTbl->GetConstValue("VesNik3_P");	VesNikLst.AddPtr(&VesNik3_P); //��� ��� ������ ���-3 � ��
	double	    VesNik4_P = pConstTbl->GetConstValue("VesNik4_P");	VesNikLst.AddPtr(&VesNik4_P); //��� ��� ������ ���-4 � ��
	double	    VesNik5_P = pConstTbl->GetConstValue("VesNik5_P");	VesNikLst.AddPtr(&VesNik5_P); //��� ��� ������ ���-5 � ��
	double	    VesNik6_P = pConstTbl->GetConstValue("VesNik6_P");	VesNikLst.AddPtr(&VesNik6_P); //��� ��� ������ ���-6 � ��

	// ��������� (����) �������� ���������
	double	    VesNik1_I = pConstTbl->GetConstValue("VesNik1_I");	VesNikLst.AddPtr(&VesNik1_I); //��� ��� ������ ���-1 � ��
	double	    VesNik2_I = pConstTbl->GetConstValue("VesNik2_I");	VesNikLst.AddPtr(&VesNik2_I); //��� ��� ������ ���-2 � ��
	double	    VesNik3_I = pConstTbl->GetConstValue("VesNik3_I");	VesNikLst.AddPtr(&VesNik3_I); //��� ��� ������ ���-3 � ��
	double	    VesNik4_I = pConstTbl->GetConstValue("VesNik4_I");	VesNikLst.AddPtr(&VesNik4_I); //��� ��� ������ ���-4 � ��
	double	    VesNik5_I = pConstTbl->GetConstValue("VesNik5_I");	VesNikLst.AddPtr(&VesNik5_I); //��� ��� ������ ���-5 � ��
	double	    VesNik6_I = pConstTbl->GetConstValue("VesNik6_I");	VesNikLst.AddPtr(&VesNik6_I); //��� ��� ������ ���-6 � ��

	// ����������
	IdbValue	Nik;	// N �� ��, ��� wYC00N_NIK

	double	UpSum 	= 0.0;
	double	DownSum = 0.0;
	for( int NikPos=1; NikPos<=NikLst.GetCnt(); NikPos++ )
	{
		IdbValue*	pFik 	= (IdbValue*)FikLst.Get(NikPos);
		IdbValue*	pNik 	= (IdbValue*)NikLst.Get(NikPos);
		double		VesNik 	= *(double*)VesNikLst.GetPtr(NikPos);

		if( pFik->ok() && (int)pFik->v()==1 && pNik->ok() && (int)pNik->v()>0 && VesNik>0 )
		{
			UpSum 	+= pNik->v()*VesNik;
			DownSum += VesNik;

//if( pFik->pCalcName && pNik->pCalcName )
//	puts(CStr("NIK Pos=") + NikPos + " " + pFik->pCalcName + "=" + pFik->v() + " " + pNik->pCalcName + "=" + pNik->v());
		}
	}

	if( DownSum>0.0 )
	{
		Nik.dValue 		= (UpSum/DownSum);
		Nik.cStatus 	= STATUS_NORMAL;
//puts(CStr("NIK =") + Nik.v());
	}
	else
	{
		Nik.dValue 		= 0.0;
		Nik.cStatus 	= STATUS_UNDEF;
	}

	WriteToIdb("Nik",&Nik);
}



void CProgCalc::Calc_TSgn()
{// ������ ����������� ��������� 1� � ������� �� ��������� � ������� ������ DTS10

	IdbValue	Pzna; 	ReadFromIdb("APzn",&Pzna);		// ������� �������� ��� ���, ���/��2
	IdbValue	Tgn10; 	ReadFromIdb("Tgn10",&Tgn10);		// T � 1-�� �� 1�, ����
	IdbValue	Tgn20; 	ReadFromIdb("Tgn20",&Tgn20);		// T � 2-�� �� 1�, ����
	IdbValue	Tgn30; 	ReadFromIdb("Tgn30",&Tgn30);		// T � 3-�� �� 1�, ����
	IdbValue	Tgn40; 	ReadFromIdb("Tgn40",&Tgn40);		// T � 4-�� �� 1�, ����

	// ����������
	IdbValue	TS1k;		// T ��������� 1�, ����
	IdbValue	DTSgn10;	// DTS  1� � ��-1, ����
	IdbValue	DTSgn20;	// DTS  1� � ��-2, ����
	IdbValue	DTSgn30;	// DTS  1� � ��-3, ����
	IdbValue	DTSgn40;	// DTS  1� � ��-4, ����


	// ����������� ������� �� �������� �� ������ �� ��� (�. 3.19.10)
	TS1k.dValue 	= WaterSteamProperty::getWaterTemperatureS(Pzna.v()*0.0980665);
	TS1k.cStatus	= Pzna.ok() ? STATUS_NORMAL : STATUS_UNDEF;

	DTSgn10.dValue	= TS1k.v()-Tgn10.v();
	DTSgn10.cStatus	= TS1k.ok() && Tgn10.ok() ? STATUS_NORMAL : STATUS_UNDEF;

	DTSgn20.dValue	= TS1k.v()-Tgn20.v();
	DTSgn20.cStatus	= TS1k.ok() && Tgn20.ok() ? STATUS_NORMAL : STATUS_UNDEF;

	DTSgn30.dValue	= TS1k.v()-Tgn30.v();
	DTSgn30.cStatus	= TS1k.ok() && Tgn30.ok() ? STATUS_NORMAL : STATUS_UNDEF;

	DTSgn40.dValue	= TS1k.v()-Tgn40.v();
	DTSgn40.cStatus	= TS1k.ok() && Tgn40.ok() ? STATUS_NORMAL : STATUS_UNDEF;

	// ������ ����������
	WriteToIdb("TS1k",&TS1k);
	WriteToIdb("DTSgn10",&DTSgn10);
	WriteToIdb("DTSgn20",&DTSgn20);
	WriteToIdb("DTSgn30",&DTSgn30);
	WriteToIdb("DTSgn40",&DTSgn40);
}


void CProgCalc::Calc_TSpg()
{// ������ ����������� ��������� � �� � DTS75

	IdbValue	TS1k;		ReadFromIdb("TS1k",	&TS1k);			// T ��������� 1�, ����
	IdbValue	Ppg10;		ReadFromIdb("Ppg10",	&Ppg10);	// P ���� � ��-1, ��/��2 YB10P10
	IdbValue	Ppg20;		ReadFromIdb("Ppg20",	&Ppg20);	// P ���� � ��-2, ��/��2 YB20P10
	IdbValue	Ppg30;		ReadFromIdb("Ppg30",	&Ppg30);	// P ���� � ��-3, ��/��2 YB30P10
	IdbValue	Ppg40;		ReadFromIdb("Ppg40",	&Ppg40);	// P ���� � ��-4, ��/��2 YB40P10


	// ����������
	IdbValue	TSpg10;	// T ��������� � ��-1, ����
	IdbValue	TSpg20;	// T ��������� � ��-2, ����
	IdbValue	TSpg30;	// T ��������� � ��-3, ����
	IdbValue	TSpg40;	// T ��������� � ��-4, ����

	IdbValue	DTSpg10;	// DTS 1� � ��-1, ����
	IdbValue	DTSpg20;	// DTS 1� � ��-2, ����
	IdbValue	DTSpg30;	// DTS 1� � ��-3, ����
	IdbValue	DTSpg40;	// DTS 1� � ��-4, ����


	// ����������� ������� �� �������� � ����������� (�. 3.19.10)
	TSpg10.dValue 	= WaterSteamProperty::getWaterTemperatureS(Ppg10.v()*0.0980665);
	TSpg10.cStatus	= Ppg10.ok() ? STATUS_NORMAL : STATUS_UNDEF;

	TSpg20.dValue 	= WaterSteamProperty::getWaterTemperatureS(Ppg20.v()*0.0980665);
	TSpg20.cStatus	= Ppg20.ok() ? STATUS_NORMAL : STATUS_UNDEF;

	TSpg30.dValue 	= WaterSteamProperty::getWaterTemperatureS(Ppg30.v()*0.0980665);
	TSpg30.cStatus	= Ppg30.ok() ? STATUS_NORMAL : STATUS_UNDEF;

	TSpg40.dValue 	= WaterSteamProperty::getWaterTemperatureS(Ppg40.v()*0.0980665);
	TSpg40.cStatus	= Ppg40.ok() ? STATUS_NORMAL : STATUS_UNDEF;

	DTSpg10.dValue	= TS1k.v()-TSpg10.v();
	DTSpg10.cStatus	= TS1k.ok() && TSpg10.ok() ? STATUS_NORMAL : STATUS_UNDEF;

	DTSpg20.dValue	= TS1k.v()-TSpg20.v();
	DTSpg20.cStatus	= TS1k.ok() && TSpg20.ok() ? STATUS_NORMAL : STATUS_UNDEF;

	DTSpg30.dValue	= TS1k.v()-TSpg30.v();
	DTSpg30.cStatus	= TS1k.ok() && TSpg30.ok() ? STATUS_NORMAL : STATUS_UNDEF;

	DTSpg40.dValue	= TS1k.v()-TSpg40.v();
	DTSpg40.cStatus	= TS1k.ok() && TSpg40.ok() ? STATUS_NORMAL : STATUS_UNDEF;

	// ������ ����������
	WriteToIdb("TSpg10",&TSpg10);
	WriteToIdb("TSpg20",&TSpg20);
	WriteToIdb("TSpg30",&TSpg30);
	WriteToIdb("TSpg40",&TSpg40);
	WriteToIdb("DTSpg10",&DTSpg10);
	WriteToIdb("DTSpg20",&DTSpg20);
	WriteToIdb("DTSpg30",&DTSpg30);
	WriteToIdb("DTSpg40",&DTSpg40);
}

void CProgCalc::Calc_SZa(float*	pSuz, char* pStSuz, int* pSuzGr, IdbValue* pSZa, int GrNum)
{
	LONG_LST	GroupSuzPosLst(10);

	// �������� ������� �� ��� � ������
	for(int SuzPos=0; SuzPos<SUZ_CNT; SuzPos++ )
		if( *(pSuzGr+SuzPos) == GrNum )
		{
			GroupSuzPosLst.Add(SuzPos);
//puts(CStr("1 Calc_SZa GrNum=") + GrNum + " SuzPos=" + SuzPos);
		}


	double	Sum 		= 0.0;
	int	 	DostCnt 	= 0;

	for( int Pos=1; Pos<=GroupSuzPosLst.GetCnt(); Pos++ )
	{
		int SuzPos = GroupSuzPosLst.Get(Pos);
		if( *(pStSuz+SuzPos)<STATUS_UNDEF )
		{
			Sum		+= *(pSuz+SuzPos);
			DostCnt += 1;
//puts(CStr("2 Calc_SZa GrNum=") + GrNum + " SuzPos=" + SuzPos + " DostCnt=" + DostCnt + " Value=" + *(pSuz+SuzPos));
		}
	}


	if( DostCnt )
	{
		pSZa->dValue 	= Sum/DostCnt;
		pSZa->cStatus 	= STATUS_NORMAL;
	}
	else pSZa->cStatus 	= STATUS_UNDEF;
}



void CProgCalc::Calc_SZa()
{// 3.16 ������ �������� ��������� �� ��� � ������

	float		SZ[SUZ_CNT];		// �� ��� � ���������
	char		stSZ[SUZ_CNT];		// ������� �� ���

	// �������� �������� � ������� ���� �� Idb
	pSZ->Read((float*)SZ, (char*)stSZ);

	// ���������
	int			SZgr[SUZ_CNT];

	for( int SuzPos=0; SuzPos<SUZ_CNT; SuzPos++ )
	{// ��������� �� �������� ������ ����� ��� ������� �� ���
		CStr	ConstName( CStr("Suz") + (SuzPos+1) + "gr");
		SZgr[SuzPos]  = pConstTbl->GetConstValue((char*)ConstName);
//puts(CStr("SuzPos=") + (SuzPos)  + " ConstName=" + ConstName + " Group="  + SZgr[SuzPos] + " Value=" + SZ[SuzPos]);
	}

	// ����������
	IdbValue	SZa01;	// ��. �����. �� ��� � ��. 1, %
	IdbValue	SZa02;	// ��. �����. �� ��� � ��. 2, %
	IdbValue	SZa03;	// ��. �����. �� ��� � ��. 3, %
	IdbValue	SZa04;	// ��. �����. �� ��� � ��. 4, %
	IdbValue	SZa05;	// ��. �����. �� ��� � ��. 5, %
	IdbValue	SZa06;	// ��. �����. �� ��� � ��. 6, %
	IdbValue	SZa07;	// ��. �����. �� ��� � ��. 7, %
	IdbValue	SZa08;	// ��. �����. �� ��� � ��. 8, %
	IdbValue	SZa09;	// ��. �����. �� ��� � ��. 9, %
	IdbValue	SZa10;	// ��. �����. �� ��� � ��. 10, %

	Calc_SZa((float*)SZ, (char*)stSZ, (int*)SZgr, &SZa01 ,1);
	Calc_SZa((float*)SZ, (char*)stSZ, (int*)SZgr, &SZa02, 2);
	Calc_SZa((float*)SZ, (char*)stSZ, (int*)SZgr, &SZa03, 3);
	Calc_SZa((float*)SZ, (char*)stSZ, (int*)SZgr, &SZa04, 4);
	Calc_SZa((float*)SZ, (char*)stSZ, (int*)SZgr, &SZa05, 5);
	Calc_SZa((float*)SZ, (char*)stSZ, (int*)SZgr, &SZa06, 6);
	Calc_SZa((float*)SZ, (char*)stSZ, (int*)SZgr, &SZa07, 7);
	Calc_SZa((float*)SZ, (char*)stSZ, (int*)SZgr, &SZa08, 8);
	Calc_SZa((float*)SZ, (char*)stSZ, (int*)SZgr, &SZa09, 9);
	Calc_SZa((float*)SZ, (char*)stSZ, (int*)SZgr, &SZa10, 10);

/*
	CTime CurTime = GetCurrentTime();
	puts(CStr("---") + &CurTime + " CProgCalc::Calc_SZa()");
	puts(CStr("      SZa08=") + SZa08.v() + " SZa09=" + SZa09.v() + " SZa10=" + SZa10.v() );
*/

	// ������ ����������
	WriteToIdb("SZa01",&SZa01);
	WriteToIdb("SZa02",&SZa02);
	WriteToIdb("SZa03",&SZa03);
	WriteToIdb("SZa04",&SZa04);
	WriteToIdb("SZa05",&SZa05);
	WriteToIdb("SZa06",&SZa06);
	WriteToIdb("SZa07",&SZa07);
	WriteToIdb("SZa08",&SZa08);
	WriteToIdb("SZa09",&SZa09);
	WriteToIdb("SZa10",&SZa10);

}


void CProgCalc::Calc_Cbor()
{// ������ ������������ ������ ������� �� ���������� ��������

	IdbValue	Cbor1;		ReadFromIdb("Cbor1",	&Cbor1);	// ������������ ������ �������, ����/��3	TV30Q01
	IdbValue	Cbor2;		ReadFromIdb("Cbor2",	&Cbor2);	// ������������ ������ �������, ����/��3	TV40Q01

	// ���������
	double	    VesCbor1 = pConstTbl->GetConstValue("VesCbor1"); // ��� ������� ������� ������������ ������ �������

	// ����������
	IdbValue	Cbor; // ������������ ������ ������� � 1�

	if( Cbor1.ok() && Cbor2.ok() )
	{
		Cbor.dValue 	= VesCbor1*Cbor1.v() + (1-VesCbor1)*Cbor2.v();
		Cbor.cStatus 	= STATUS_NORMAL;
	}
	else if( Cbor1.ok()  )
	{
		Cbor.dValue 	= Cbor1.v();
		Cbor.cStatus 	= STATUS_NORMAL;
	}
	else if( Cbor2.ok()  )
	{
		Cbor.dValue 	= Cbor2.v();
		Cbor.cStatus 	= STATUS_NORMAL;
	}

	// ������ ����������
	WriteToIdb("Cbor",&Cbor);
}


void CProgCalc::Calc_Kpd()
{// 3.13.5 ������ ����������� ��������� ��������

	IdbValue	Nru;		ReadFromIdb("wNAKZ",&Nru);	// �������� �������� ��, ���
	IdbValue	Ntg;		ReadFromIdb("Ntg",	&Ntg);	// �������� ���������������, ���

	// ����������
	IdbValue	Kpd; // ����������� ��������� �������� �����������

	if( Nru.v()>0 )
	{
		Kpd.dValue 	= 100*Ntg.v()/Nru.v();
		Kpd.cStatus = Ntg.ok() && Nru.ok() ? STATUS_NORMAL : STATUS_UNDEF;
	}
	else
	{
		Kpd.dValue 	= 0.0;
		Kpd.cStatus = STATUS_UNDEF;
	}

//puts(CStr("Nru=") + Nru.v() +  " Ntg=" + Ntg.v() + " Kpd=" + Kpd.v());


	// ������ ����������
	WriteToIdb("Kpd",&Kpd);
}

void CProgCalc::Calc_HH()
{// heathydraulics - ������������������� ������

//	CTime CurTime = GetCurrentTime();
//puts(CStr("CProgCalc::Calc_HH() ") + &CurTime);

	// ��������� ����������� ��, �� � ���������� ��� ����� ��������� ������
	Calc_ThnTgnDt(1);

	// ����������� ��������� ������ ������� �������
	Calc_StPt();

	// �������� ��������� ����������� ��, �� � ��������� � ������ ��������� ������
	Calc_ThnTgnDt(2);


	// ������� ��������� ����������� � �������� � �� � ����������� � ������� ������ ��������
	Calc_Avg();

	// ������� ����� ����� ������� ������� �� �������� �������� �� ���
	Calc_Gpt();

	//������ ��������� ������� ����������� ���� � ���������������
	Calc_Gpg();

	// ������ ������� ����������� ���� � ���
	Calc_Gpvd();

	// ������ ������� ����������� � �������� (�������) ������ ������� ������� � ��. ���������
	Calc_AThnAtgnADTpt();

	// ������ ���������� � ��� �� �� � ��������� ��������� �� ��
	Calc_DTtpADTtp();

	// ������ ��������� ���������
	Calc_V();

	// ������ �������� 1 �������
	Calc_N1k();

	// ������ �������� �������� ��� �� ������� ����������� ���� � ��
	Calc_N2k();

	// ������ �������� �������� ��� �� ������� ����������� ���� �� ���
	Calc_Npvd();

	// ������ �������� �������� ��� �� ���������� ������������� �����
	Calc_Nik();

	// ������ ����������� ��������� 1� � ������� �� ��������� � ������� ������ DTS10
	Calc_TSgn();

	// ������ ����������� ��������� � �� � DTS75
	Calc_TSpg();

	// ������ �������� ��������� �� ��� � ������
	Calc_SZa();

	// ������ ������������ ������ ������� �� ���������� ��������
	Calc_Cbor();

	// ������ ����������� ��������� ��������
	Calc_Kpd();

	// ������ ������ ������ ��
	Calc_MODE();

	// ������ �������� �������� ��
	Calc_wNAKZ();
}


