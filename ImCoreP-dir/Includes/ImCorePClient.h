#ifndef _IMCORE_P_CLIENT_H
#define _IMCORE_P_CLIENT_H
#include "ImCorePMain.h"


class FORECAST;

// ���� �������� (RequestType)
#define	PT_CUR		0
#define	PT_REQ		1
#define	PT_GEN		2
#define	PT_MKU		3

// ���� ������� (AnswerType)
#define	AT_POINT	0
#define	AT_MSG		1
#define	AT_STOP		2

// ���� �������� (ValueType)
#define	VT_FLOAT			0
#define	VT_STRING			1
#define	VT_FLOAT_ARRAY		2

#pragma pack(1)

struct	CPredictRequestHeader
{
	int		PackageSize;				// ���������� ������ ������
	char	RequestName[32];			// ������������ �������
	int		RequestType;				// ��� ����������� �������
	CTime	StartRequestTime;			// ��������� ����� �������
	int		EtapCounter;				// ���������� ������
};

struct	CPredictEtapHeader
{
	int		EtapSize;					// ������ ����� ������ � �������
	CTime	Duration;					// ������������ �����
	int		ValuesCounter;				// ���������� �������� � �����
};

struct	CPredictValueHeader
{
	int		ValueSize;					// ������ �������� ������ � �������
	int		ValueType;					// ��� ��������
	char	ValueName[20];				// ������������ ��������
	int		SizeX;						// ����������� �� �
	int		SizeY;						// ����������� �� Y
};

struct CPredictAnswerHeader
{
	int		PackageSize;
	int		AnswerType;
	CTime	StartTime;
	CTime	PointTime;
	int		ValuesCounter;
};

#pragma pack()

//////////////////////////////////////////////////////////////////////
// class CPredictClient
class CPredictClient
{
public:
	CStr				ClientHost;
	int					ClientPort;
	int					NetSocket;

	int					MutexID;

	// �������� ������ ���������
	CStr				ThreadName;
	int					ThreadPid;
	int					ThreadFlgStart;
	int					ThreadFlgRun;
	int					ThreadFlgStop;
	CStr				ThreadError;


    FORECAST*           pForecast;

public:
	CPredictClient(CStr Host, int Port);
	~CPredictClient();

	void	StartThread();
	bool	ClientThread();
	bool 	Lock(){	  return MutexID ? ::MutexLock(MutexID)	  : false; }
	bool	Unlock(){ return MutexID ? ::MutexUnlock(MutexID) : false; }
	bool	IsLock(){ return MutexID ? ::MutexIsLock(MutexID) : false; }
	void	Disconnect();
	void	PrepareClientRequest(unsigned char* pPackage, CPredictRequestHeader* pHeader, LST* pEtapLst);
	long	GetOffsetInStruct(const char* pValueName);
};



#endif // _IMCORE_P_CLIENT_H
